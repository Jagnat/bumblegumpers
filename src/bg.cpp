#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <assert.h>
#include <string.h>

#include "log.h"

#include "bg_util.h"
#include "primitives.h"
#include "bg_data.h"
#include "bg_funcs.h"

#include "thirdparty/imgui.h"

Platform *platform;
Game *game;

void initPlatform();
void initGame();
void handleEvents();
double elapsedMs();

void progressPlayerState(Entity *player, World *world)
{
	TileCollision under = getCollision(world, player->x, player->y - 1);
	TileCollision at = getCollision(world, player->x, player->y);
	if (under == TC_SOLID || under == TC_LADDER || under == TC_PLATFORM)
		player->movement = ONGROUND;
	if (at == TC_LADDER)
		player->movement = CLIMBING;
	if (at == TC_CORNER)
		player->movement = AIRWALKING;
}

void fallPlayer(Entity *player, World *world)
{
	while (true)
	{
		TileCollision at = getCollision(world, player->x, player->y);
		TileCollision under = getCollision(world, player->x, player->y - 1);
		if (under == TC_LADDER || under == TC_SOLID || at == TC_LADDER || at == TC_CORNER)
			break;
		player->y--;
	}
}

bool doPlayerLR(Input *input, Entity *player, World *world)
{
	if (input->left.pressed && !input->right.pressed &&
		getCollision(world, player->x - 1, player->y) != TC_SOLID)
	{
		player->x--;
		return true;
	}
	else if (input->right.pressed && !input->left.pressed &&
		getCollision(world, player->x + 1, player->y) != TC_SOLID)
	{
		player->x++;
		return true;
	}
	return false;
}

bool doPlayerUp(Input *input, Entity *player, World *world)
{
	if (input->up.pressed &&
		getCollision(world, player->x, player->y + 1) != TC_SOLID)
	{
		player->y++;
		return true;
	}
	return false;
}

void updatePlayer(Input *input, Entity *player, World *world)
{
	switch(player->movement)
	{
		case ONGROUND:
		{
			if (input->down.pressed &&
				getCollision(world, player->x, player->y - 1) == TC_LADDER)
				player->y--;
			if (doPlayerUp(input, player, world))
				player->movement = JUMPING;
			else
			{
				doPlayerLR(input, player, world);
				player->movement = AIRWALKING;
			}
			progressPlayerState(player, world);
		} break;

		case AIRWALKING:
		{
			if (input->up.pressed &&
				getCollision(world, player->x, player->y) == TC_CORNER)
			{
				doPlayerUp(input, player, world);
				fallPlayer(player, world);
			}
			if (input->down.pressed)
				fallPlayer(player, world);
			else if (doPlayerLR(input, player, world))
				fallPlayer(player, world);
			progressPlayerState(player, world);
		} break;

		case JUMPING:
		{
			if (input->down.pressed)
				fallPlayer(player, world);
			else if (doPlayerUp(input, player, world))
				fallPlayer(player, world);
			else if (doPlayerLR(input, player, world))
				fallPlayer(player, world);
			progressPlayerState(player, world);
		} break;

		case CLIMBING:
		{
			if (input->down.pressed)
			{
				if (getCollision(world, player->x, player->y - 1) != TC_SOLID)
					player->y--;
				fallPlayer(player, world);
			}
			else if (doPlayerUp(input, player, world))
				fallPlayer(player, world);
			else if (doPlayerLR(input, player, world))
				player->movement = AIRWALKING;
			progressPlayerState(player, world);
		} break;

		default:
		break;
	}
}

void updateEntities(Entity *entities, int length)
{
	for (int i = 0; i < length; i++)
	{
		Entity *entity = &entities[i];
		switch (entity->type)
		{
			case PLAYER:
			{
				updatePlayer(&game->input, entity, &game->world);
			} break;

			case CHANDELIER:
			{

			} break;

			case LEVER:
			{

			} break;
			
			case EMPTY:
			default:
				continue;
		}
	}
}

void update()
{
	if (game->input.toggleEdit.pressed && game->input.modState.control)
	{
		// TODO: Replace all platform->editor.enabled calls with something nice!
		// This feels pretty gross
		platform->editorEnabled = !platform->editorEnabled;

		log_info("Set editor mode to %s!", platform->editorEnabled ? "true" : "false");
		if (platform->editorEnabled)
		{
			SDL_SetWindowSize(platform->window, 1400, 900);
			SDL_SetWindowPosition(platform->window,
				SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
			editorSetWorld(&game->world);
			editorResize(1400, 900);
		}
		else
		{
			SDL_SetWindowSize(platform->window, 768, 576);
			SDL_SetWindowPosition(platform->window,
				SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
			setCamPos(CreateVec(8, 6));
		}
	}

	if (!platform->editorEnabled)
	{
		//updatePlayer(&game->input, &game->player, &game->world);
		updateEntities(game->entities, NUM_ENTITIES);
	}
	else
	{
		editorUpdate(&game->input);
	}

	// reset player input
	for (int i = 0; i < NUM_BUTTONS; ++i)
	{
		game->input.buttons[i].pressed = false;
		game->input.buttons[i].released = false;
	}
	memset(game->input.textBuffer, 0, INPUT_TEXT_BUFFER_LENGTH * sizeof(char));
}

void renderLayer(int w, int h, uint16 *layer, float z, float opacity)
{
	setZ(z);
	setColor(WithAlpha(COLOR_WHITE, opacity));

	for (int x = 0; x < w; x++)
	{
		for (int y = 0; y < h; ++y)
		{
			uint16 t = layer[y * w + x];
			if (t == 0)
				continue;
			if (t > 32 * 32)
				continue;
			Rect texPos = getTileRect(t);
			addSprite(CreateRect(x, y, 1, 1), texPos);
		}
	}
}

void renderEntities(Entity *entities)
{
	for (int i = 0; i < NUM_ENTITIES; i++)
	{
		switch(entities[i].type)
		{
			case PLAYER:
			{
				addSprite(CreateRect(game->entities[i].x, game->entities[i].y, 1, 1),
					getPlayerRect());
			} break;

			case LEVER:
			{

			} break;

			case EMPTY:
			default:
				continue;
		}
	}
}

void render(double interval)
{
	World *world = &game->world;

	if (platform->editorEnabled)
	{
		editorRender();
	}
	else
	{
		beginRender();
		beginSpriteBatch(1, CAM_GAME);
		{
			renderLayer(world->width, world->height, world->bTiles, 0.49f, 1.f);
			renderLayer(world->width, world->height, world->mTiles, 0.50f, 1.f);
			renderLayer(world->width, world->height, world->fTiles, 0.51f, 1.f);
			renderEntities(game->entities);
		}
		endSpriteBatch();
		endRender();
	}
	SDL_GL_SwapWindow(platform->window);
}

void initGame()
{
	//TODO: Get rid of me! We don't manually set entities anymore
	game = (Game*)calloc(1, sizeof(Game));
	memset(&game->entities[0], 0, sizeof(Entity) * NUM_ENTITIES);

	game->entities[0].type = PLAYER;
	game->entities[0].x = 1;
	game->entities[0].y = 4;
	game->entities[0].movement = ONGROUND;

	game->entities[1].type = LEVER;
	game->entities[1].x = 3;
	game->entities[1].y = 4;
	game->entities[1].textureId = 1;
	
	createTestWorld(&game->world);
}

void event_ProcessButtonPress(Button *button, uint8 state)
{
	if (state == SDL_PRESSED)
	{
		button->down = true;
		button->pressed = true;
	}
	if (state == SDL_RELEASED)
	{
		button->down = false;
		button->released = true;
	}
}

void event_UpdateModState(ModState *ms)
{
	memset(ms, 0, sizeof(ModState));
	SDL_Keymod state = SDL_GetModState();
	if (state & KMOD_LCTRL || state & KMOD_RCTRL)
		ms->control = true;
	if (state & KMOD_LSHIFT || state & KMOD_RSHIFT)
		ms->shift = true;
	if (state & KMOD_RALT || state & KMOD_LALT)
		ms->alt = true;
}

void handleEvents()
{
	SDL_Event e;
	Input *in = &game->input;
	while (SDL_PollEvent(&e))
	{
		switch (e.type)
		{
			case SDL_QUIT:
			{
				// TODO: graceful exit
				platform->running = false;
			}
			break;
			case SDL_KEYDOWN:
			case SDL_KEYUP:
			{
				// store in array for imgui
				int key = e.key.keysym.sym & ~SDLK_SCANCODE_MASK;
				in->keysDown[key] = (e.type == SDL_KEYDOWN);

				if (!e.key.repeat)
					switch(e.key.keysym.sym)
					{
						case SDLK_LEFT:
							event_ProcessButtonPress(&in->left, e.key.state); break;
						case SDLK_RIGHT:
							event_ProcessButtonPress(&in->right, e.key.state); break;
						case SDLK_UP:
							event_ProcessButtonPress(&in->up, e.key.state); break;
						case SDLK_DOWN:
							event_ProcessButtonPress(&in->down, e.key.state); break;
						case SDLK_z:
							event_ProcessButtonPress(&in->attack, e.key.state); break;
						case SDLK_x:
							event_ProcessButtonPress(&in->use, e.key.state); break;
						case SDLK_c:
							event_ProcessButtonPress(&in->shield, e.key.state); break;
						case SDLK_e:
							event_ProcessButtonPress(&in->toggleEdit, e.key.state); break;
						// TODO: graceful exit
						case SDLK_ESCAPE:
						platform->running = false; break;
					}
			}
			break;
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
			{
				switch(e.button.button)
				{
					case SDL_BUTTON_LEFT:
						event_ProcessButtonPress(&in->leftMouse, e.button.state); break;
					case SDL_BUTTON_RIGHT:
						event_ProcessButtonPress(&in->rightMouse, e.button.state); break;
					case SDL_BUTTON_MIDDLE:
						event_ProcessButtonPress(&in->middleMouse, e.button.state); break;
				}
			}
			break;
			case SDL_MOUSEMOTION:
			{
				in->mouseX = e.motion.x;
				in->mouseY = e.motion.y;
			}
			break;
			case SDL_WINDOWEVENT:
			{
				switch(e.window.event)
				{
					// NOTE: this handles both user/system resize
					case SDL_WINDOWEVENT_SIZE_CHANGED:
					{
						resizeRenderer(e.window.data1,
							e.window.data2);
						platform->width = e.window.data1;
						platform->height = e.window.data2;
					}
					break;
				}
			}
			break;
			case SDL_TEXTINPUT:
			{
				assert(strlen(in->textBuffer) + 1 < INPUT_TEXT_BUFFER_LENGTH);
				strcat(in->textBuffer, e.text.text);
			}
			break;
		}
	}
	event_UpdateModState(&in->modState);
}

double elapsedMs()
{
	double ticks = (double)SDL_GetPerformanceCounter();
	return ticks / platform->timerResolution;
}

void initPlatform()
{
	log_init();
	platform = (Platform*)calloc(1, sizeof(Platform));

	platform->targetUpdateDelta = 1000.L / 60.L;
	platform->targetRenderDelta = 1000.L / 60.L;
	platform->running = true;
	platform->width = 768;
	platform->height = 576;

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		log_exit("Failed to init SDL!");

	if (SDL_GL_LoadLibrary(0))
		log_exit("Failed to load GL lib!");

	platform->window = SDL_CreateWindow("?????",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		platform->width, platform->height,
		SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	if (!platform->window)
		log_exit("Couldn't create window!");

	platform->glContext = SDL_GL_CreateContext(platform->window);
	if (!platform->glContext)
		log_exit("Failed to create opengl context!");

	SDL_GL_SetSwapInterval(1);

	platform->timerResolution = (double)SDL_GetPerformanceFrequency() / (double)1000.L;

	initRenderer(platform->width, platform->height);

	editorInit();
	log_info("platform init'd");
}

int main(int argc, char **argv)
{
	initPlatform();
	initGame();

	double prevTime = elapsedMs();
	double elapsedUpdate = 0, elapsedRender = 0;
	log_info("Entering loop...");
	while (platform->running)
	{
		handleEvents();

		double currentTime = elapsedMs();
		double elapsedTime = currentTime - prevTime;
		prevTime = currentTime;
		if (elapsedTime > 500)
		{
			log_warning("Long delay happened! window resize...?");
			elapsedTime = 500;
		}

		elapsedUpdate += elapsedTime;
		elapsedRender += elapsedTime;

		while (elapsedUpdate >= platform->targetUpdateDelta)
		{
			update();
			elapsedUpdate -= platform->targetUpdateDelta;
		}

		if (elapsedRender >= platform->targetRenderDelta)
		{
			render(elapsedUpdate / platform->targetUpdateDelta);
			elapsedRender = 0;
		}
	}

	log_info("exit complete!");

	return 0;
}

#include "log.cpp"
#include "primitives.cpp"
#include "bg_render.cpp"
#include "bg_world.cpp"
#include "bg_editor.cpp"

#include "thirdparty/imgui.cpp"
#include "thirdparty/imgui_draw.cpp"
#include "thirdparty/imgui_demo.cpp"
