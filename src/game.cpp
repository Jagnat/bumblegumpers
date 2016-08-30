#include "game.h"

#include <SDL.h>
#include <glew.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <assert.h>
#include <string.h>

#include "log.h"

#include "game_util.h"
#include "game_render.h"
#include "game_world.h"

#include "primitives.h"

#define DO_TESTS

Engine *engine = 0;
Game *game = 0;

// BEGIN ENTITY SYSTEM

void handleHorizontalAccel(Entity *e)
{
	/*float *vX = &e->velocity.x;
	if (e->moveDir.x != 0)
	{
		if (e->moveDir.x == 1)
		{
			*vX += e->acceleration;
			if(*vX > e->topVelocity.x)
				*vX = e->topVelocity.x;
		}
		else
		{
			*vX -= e->acceleration;
			if (*vX < -e->topVelocity.x)
				*vX = -e->topVelocity.x;
		}
	}
	else if (*vX != 0)
	{
		if (*vX > 0 && *vX - e->acceleration > 0)
			*vX -= e->acceleration;
		else if (*vX < 0 && *vX + e->acceleration < 0)
			*vX += e->acceleration;
		else
			*vX = 0;
	}*/
}

void updateEntities()
{
	memcpy(game->pastEntities, game->entities, sizeof(Entity) * NUM_ENTITIES);

	for (int i = 0; i < NUM_ENTITIES; ++i)
	{
		Entity *e = &game->entities[i];
		switch (e->type)
		{
			case ENTITYTYPE_PLAYER:
			{
				float moveX = 0;
				if (engine->input.leftBtn.endedDown)
					moveX -= 1;
				if (engine->input.rightBtn.endedDown)
					moveX += 1;
				e->moveDir.x = moveX;
				handleHorizontalAccel(e);
				e->pos.x += e->velocity.x;
			}
			break;

			case ENTITYTYPE_EMPTY:
			default:
			break;
		}
	}
}

Entity *createEntity()
{
	return 0;
}

// END ENTITY SYSTEM

void updateGame()
{
	Button left = engine->input.leftBtn;
	Button right = engine->input.rightBtn;
	Button jump = engine->input.jumpBtn;
	Button down = engine->input.runBtn;
	Vec2 moveDir = {0};
	if (right.endedDown)
		moveDir.x += 1;
	if (left.endedDown)
		moveDir.x -= 1;
	if (jump.endedDown)
		moveDir.y += 1;
	if (down.endedDown)
		moveDir.y -= 1;

	game->camPos.y += .1 * moveDir.y;
	game->camPos.x += .1 * moveDir.x;

	updateEntities();
}

void renderGame(float interval)

{	Vec2 iCamPos = {0};
	iCamPos.x = lerp(game->camPosPrev.x, game->camPos.x, interval);
	iCamPos.y = lerp(game->camPosPrev.y, game->camPos.y, interval);
	// engine->renderer->viewMatrix = Mat4_Translate(-game->camPos.x, -game->camPos.y, 0);
	engine->renderer->viewMatrix = Mat4_Translate(-iCamPos.x, -iCamPos.y, 0);
	game->camPosPrev = game->camPos;
	beginRender();

	WorldChunk *chunk = &game->chunk;
	beginSpriteBatch(1, CAMERA_MODE_GAME);
	{
		// Render world
		int xOff = chunk->relativeX * WORLDCHUNK_SIZE;
		int yOff = chunk->relativeY * WORLDCHUNK_SIZE;
		for (int x = 0; x < WORLDCHUNK_SIZE; ++x)
		{
			for (int y = 0; y < WORLDCHUNK_SIZE; ++y)
			{
				if (chunk->tiles[x][y] == 255)
				{
					addSprite(AABB_Create(x + xOff, y + yOff, 1.0, 1.0),
						engine->assets->tileRegion);
				}
			}
		}

		// Render all entities
		for (int i = 0; i < NUM_ENTITIES; ++i)
		{
			if (game->entities[i].type != ENTITYTYPE_EMPTY)
			{
				Entity c = game->entities[i];
				Entity p = game->pastEntities[i];
				AABB interp =
				{
					lerp(p.pos.x, c.pos.x, interval),
					lerp(p.pos.y, c.pos.y, interval),
					lerp(p.bounds.x, c.bounds.x, interval),
					lerp(p.bounds.y, c.bounds.y, interval)
				};
				addSprite(interp, engine->assets->playerRegion);
			}
		}
	}
	endSpriteBatch();

	beginSpriteBatch(2, CAMERA_MODE_SCREEN);
	{
		add14ptText(Vec2_Create(10, 10), "pos: %f",
			game->entities[0].pos.x);
	}
	endSpriteBatch();

	endRender();
}

void initEngine()
{
	log_init();


	// Allocate one big buffer to hold everything
	// TODO: Once config system is up, tune this at runtime

	uint bufSize = sizeof(Engine) + sizeof(Game) +
		sizeof(Renderer) + sizeof(GameAssets);
	uint8 *mem = (uint8*)calloc(1, bufSize);
	engine = (Engine*)mem;
	game = (Game*)(mem + sizeof(Engine));
	engine->renderer = (Renderer*)(mem + sizeof(Engine) + sizeof(Game));
	engine->assets = (GameAssets*)(mem + sizeof(Engine) + sizeof(Game) + sizeof(Renderer));
	
	game->running = true;
	engine->targetUpdateDelta = 1000.L / 120.L;
	engine->targetRenderDelta = 1000.L / 60.L;
	engine->width = DEFAULT_W;
	engine->height = DEFAULT_H;

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		log_exit("Failed to initialize SDL!");
	engine->window = SDL_CreateWindow("Game",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		engine->width, engine->height,
		SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (!engine->window) log_exit("Failed to create SDL_Window!");

	engine->glContext = SDL_GL_CreateContext(engine->window);
	if (!engine->glContext) log_exit("Failed to create context!");

	if (glewInit() != GLEW_OK)
		log_exit("Failed to init glew!");

	// TODO: switch to legacy renderer
	if (!GLEW_VERSION_3_3)
		log_exit("This computer doesn't support OpenGL 3.3!");

	SDL_GL_SetSwapInterval(1);

	engine->renderer->initialized = false;
	initRenderer(engine->renderer);
	resizeRenderer(engine->width, engine->height);

	loadAssets(engine->assets);

	engine->timerResolution = (double)SDL_GetPerformanceFrequency() / (double)1000.L;
	log_info("Timer resolution: %f", engine->timerResolution);
	log_info("Successfully initialized engine!");
}

void initGame()
{

	WorldChunk *chunk = &game->chunk;
	for (int x = 0; x < WORLDCHUNK_SIZE; ++x)
	{
		chunk->tiles[x][0] = 255;
		chunk->tiles[x][WORLDCHUNK_SIZE - 1] = 255;
	}
	for (int y = 0; y < WORLDCHUNK_SIZE; ++y)
	{
		chunk->tiles[0][y] = 255;
		chunk->tiles[WORLDCHUNK_SIZE - 1][y] = 255;
	}
	chunk->tiles[5][5] = 255;

	const float playerAccelFrames = 1.0;

	Entity *playerEntity = &game->entities[0];
	playerEntity->type = ENTITYTYPE_PLAYER;
	playerEntity->collision = {2.0, 2.6, 1.0, 1.0};
	playerEntity->topVelocity = {.1f, .5f};
	playerEntity->acceleration = playerEntity->topVelocity.x / playerAccelFrames;
	playerEntity->gravityForce = playerEntity->topVelocity.y / 20.f;
}

int main(int argc, char **argv)
{
	initEngine();

	#ifdef DO_TESTS
	extern void doTests();
	doTests();
	#endif

	initGame();

	double prevTime = elapsedMs();
	double elapsedTime;
	double elapsedUpdate = 0, elapsedRender = 0;

	while (game->running)
	{
		handleEvents(&engine->input);

		double currentTime = elapsedMs();
		elapsedTime = currentTime - prevTime;
		if (elapsedTime > 500)
		{
			log_warning("Long delay happened! Window resize...?");
			elapsedTime = 500;
		}
		prevTime = currentTime;

		elapsedUpdate += elapsedTime;
		elapsedRender += elapsedTime;

		while (elapsedUpdate >= engine->targetUpdateDelta)
		{
			updateGame();

			// Reset events
			for (int i = 0; i < NUM_BUTTONS; ++i)
				engine->input.buttons[i].numStateTransitions = 0;
			elapsedUpdate -= engine->targetUpdateDelta;
		}

		if (elapsedRender >= engine->targetRenderDelta)
		{
			// Temporary lock
			renderGame(elapsedUpdate / engine->targetUpdateDelta);
			SDL_GL_SwapWindow(engine->window);
			elapsedRender = 0;
		}

	}
	return 0;
}

void processKeyPress(Input *input, int index, uint8 state)
{
	input->buttons[index].numStateTransitions++;
	input->buttons[index].endedDown = (state == SDL_PRESSED ? true : false);
}

void handleEvents(Input *input)
{
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		switch (e.type)
		{
			case SDL_QUIT: game->running = false; break;
			case SDL_KEYDOWN:
			case SDL_KEYUP:
			{
				if (!e.key.repeat)
					switch (e.key.keysym.sym)
					{
						case SDLK_a: case SDLK_LEFT:
							processKeyPress(input, 0, e.key.state); break;
						case SDLK_d: case SDLK_RIGHT:
							processKeyPress(input, 1, e.key.state); break;
						case SDLK_LSHIFT: case SDLK_s:
							processKeyPress(input, 2, e.key.state); break;
						case SDLK_SPACE: case SDLK_w:
							processKeyPress(input, 3, e.key.state); break;
						case SDLK_ESCAPE: game->running = false; break;
					}
			}
			break;
			case SDL_WINDOWEVENT:
			{
				switch (e.window.event)
				{
					case SDL_WINDOWEVENT_RESIZED:
					{
						engine->width = e.window.data1;
						engine->height = e.window.data2;
						resizeRenderer(engine->width, engine->height);
					}
				}
			}
			break;
		}
	}
}

double elapsedMs()
{
	double ticks = (double)SDL_GetPerformanceCounter();
	return ticks / engine->timerResolution;
}

// Src includes
#include "log.cpp"
#include "game_render.cpp"
#include "game_assets.cpp"
#include "game_world.cpp"
#include "primitives.cpp"

#ifdef DO_TESTS
#include "tests.cpp"
#endif
