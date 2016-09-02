#include "bg.h"

#include <SDL.h>
#include <glew.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <assert.h>
#include <string.h>

#include "log.h"

#include "bg_util.h"
#include "primitives.h"
#include "bg_render.h"
#include "bg_world.h"
#include "bg_editor.h"

Platform *platform;
Game *game;

void initPlatform();
void initGame();
void handleEvents();
double elapsedMs();

void progressPlayerState(Player *player, World *world)
{
	TileCollision under = getCollision(world, player->x, player->y - 1);
	if (under == TC_SOLID || under == TC_LADDER)
		player->state = PLAYER_ONGROUND;
	if (getCollision(world, player->x, player->y) == TC_LADDER)
		player->state = PLAYER_CLIMBING;
}

void fallPlayer(Player *player, World *world)
{
	while (true)
	{
		TileCollision at = getCollision(world, player->x, player->y);
		TileCollision under = getCollision(world, player->x, player->y - 1);
		if (under == TC_LADDER || under == TC_SOLID || at == TC_LADDER)
			break;
		player->y--;
	}
}

bool doPlayerLR(Input *input, Player *player, World *world)
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

bool doPlayerUp(Input *input, Player *player, World *world)
{
	if (input->up.pressed &&
		getCollision(world, player->x, player->y + 1) != TC_SOLID)
	{
		player->y++;
		return true;
	}
	return false;
}

void updatePlayer(Input *input, Player *player, World *world)
{
	switch(player->state)
	{
		case PLAYER_ONGROUND:
		{
			if (input->down.pressed &&
				getCollision(world, player->x, player->y - 1) == TC_LADDER)
				player->y--;
			if (doPlayerUp(input, player, world))
				player->state = PLAYER_JUMPING;
			else
			{
				doPlayerLR(input, player, world);
				player->state = PLAYER_AIRWALKING;
			}
			progressPlayerState(player, world);
		}
		break;
		case PLAYER_AIRWALKING:
		{
			if (input->down.pressed)
				fallPlayer(player, world);
			else if (doPlayerLR(input, player, world))
				fallPlayer(player, world);
			progressPlayerState(player, world);
		}
		break;
		case PLAYER_JUMPING:
		{
			if (input->down.pressed)
				fallPlayer(player, world);
			else if (doPlayerUp(input, player, world))
				fallPlayer(player, world);
			else if (doPlayerLR(input, player, world))
				fallPlayer(player, world);
			progressPlayerState(player, world);
		}
		break;
		case PLAYER_CLIMBING:
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
				player->state = PLAYER_AIRWALKING;
			progressPlayerState(player, world);
		}
		break;
		default:
		break;
	}
}

void update()
{
	if (game->input.toggleEdit.pressed)
	{
		game->inEditor = !game->inEditor;
		log_info("Set editor mode to %s!", game->inEditor ? "true" : "false");
		if (game->inEditor)
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

	if (!game->inEditor)
		updatePlayer(&game->input, &game->player, &game->world);
	else
		editorUpdate(&game->input);

	// reset player input
	for (int i = 0; i < NUM_BUTTONS; ++i)
	{
		game->input.buttons[i].pressed = false;
		game->input.buttons[i].released = false;
	}
}

const float tileRegion = 1.f / 32.f;

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
			t--;
			Rect texPos = CreateRect((t % 32) * tileRegion, (t / 32) * tileRegion,
				tileRegion, tileRegion);
			addSprite(CreateRect(x, y, 1, 1), texPos);
		}
	}
}

void render(double interval)
{
	World *world = &game->world;

	beginRender();

	if (game->inEditor)
	{
		editorRender(interval);
	}
	else
	{
		beginSpriteBatch(1, CAM_GAME);
		{
			renderLayer(world->width, world->height, world->bTiles, 0.49f, 1.f);
			renderLayer(world->width, world->height, world->mTiles, 0.50f, 1.f);
			renderLayer(world->width, world->height, world->fTiles, 0.51f, 1.f);
			addSprite(CreateRect(game->player.x, game->player.y, 1, 1),
				CreateRect(0, 31 * tileRegion, tileRegion, tileRegion));
		}
		endSpriteBatch();
	}

	endRender();
}

int main(int argc, char **argv)
{
	initPlatform();

	initGame();

	double prevTime = elapsedMs();
	double elapsedUpdate = 0, elapsedRender = 0;
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
			SDL_GL_SwapWindow(platform->window);
			elapsedRender = 0;
		}
	}

	return 0;
}

void initGame()
{

	game->inEditor = false;
	game->player.x = 1;
	game->player.y = 1;
	game->player.state = PLAYER_ONGROUND;
	
	createTestWorld(&game->world);
}

void processButtonPress(Button *button, uint8 state)
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
				if (!e.key.repeat)
					switch(e.key.keysym.sym)
					{
						case SDLK_LEFT:
							processButtonPress(&in->left, e.key.state); break;
						case SDLK_RIGHT:
							processButtonPress(&in->right, e.key.state); break;
						case SDLK_UP:
							processButtonPress(&in->up, e.key.state); break;
						case SDLK_DOWN:
							processButtonPress(&in->down, e.key.state); break;
						case SDLK_z:
							processButtonPress(&in->attack, e.key.state); break;
						case SDLK_x:
							processButtonPress(&in->use, e.key.state); break;
						case SDLK_c:
							processButtonPress(&in->shield, e.key.state); break;
						case SDLK_e:
							processButtonPress(&in->toggleEdit, e.key.state); break;
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
						processButtonPress(&in->leftMouse, e.button.state); break;
					case SDL_BUTTON_RIGHT:
						processButtonPress(&in->rightMouse, e.button.state); break;
					case SDL_BUTTON_MIDDLE:
						processButtonPress(&in->middleMouse, e.button.state); break;
				}
			}
			break;
			case SDL_MOUSEMOTION:
			{
				in->mouseX = e.motion.x;
				in->mouseY = platform->height - (e.motion.y + 1);
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
					}
					break;
				}
			}
			break;
		}
	}
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
	game = &platform->game;

	platform->targetUpdateDelta = 1000.L / 120.L;
	platform->targetRenderDelta = 1000.L / 60.L;
	platform->running = true;
	platform->width = 768;
	platform->height = 576;

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		log_exit("Failed to init SDL!");

	platform->window = SDL_CreateWindow("?????",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		platform->width, platform->height,
		SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	if (!platform->window)
		log_exit("Couldn't create window!");

	platform->glContext = SDL_GL_CreateContext(platform->window);
	if (!platform->glContext)
		log_exit("Failed to create opengl context!");

	if (glewInit() != GLEW_OK)
		log_exit("Failed to init glew!");

	SDL_GL_SetSwapInterval(1);

	platform->timerResolution = (double)SDL_GetPerformanceFrequency() / (double)1000.L;

	initRenderer(&platform->renderer, platform->width, platform->height);

	editorInit(&platform->editor);
	log_info("platform init'd");
}

#include "log.cpp"
#include "primitives.cpp"
#include "bg_render.cpp"
#include "bg_world.cpp"
#include "bg_editor.cpp"
