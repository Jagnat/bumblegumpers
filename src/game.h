#pragma once
#ifndef _GAME_H_
#define _GAME_H_

#include <SDL.h>
#include "game_util.h"
#include "game_render.h"
#include "game_assets.h"
#include "game_world.h"

struct Button
{
	int numStateTransitions;
	bool endedDown;
};

#define NUM_BUTTONS 4
struct Input
{
	union
	{
		struct
		{
			Button leftBtn;
			Button rightBtn;
			Button runBtn;
			Button jumpBtn;
		};
		Button buttons[NUM_BUTTONS];
	};
};

#define DEFAULT_W 1280
#define DEFAULT_H 720

#define TILE_PIXELS 32

struct Engine
{
	SDL_Window *window;
	SDL_GLContext glContext;
	double targetUpdateDelta;
	double targetRenderDelta;
	double timerResolution;
	uint width, height;

	Input input;
	Renderer *renderer;
	GameAssets *assets;
};

enum EntityType
{
	ENTITYTYPE_EMPTY,
	ENTITYTYPE_PLAYER,
	ENTITYTYPE_ENEMY
};

struct Entity
{
	EntityType type;

	union
	{
		AABB collision;
		struct { Vec2 pos, bounds; };
	};

	bool hasGravity;

	Vec2 velocity;
	Vec2 topVelocity;
	float acceleration;
	float gravityForce;

	bool onGround;

	AABB renderBounds;
};

#define NUM_ENTITIES 32

struct Game
{
	bool running;
	bool paused;

	WorldChunk chunk;

	Entity entities[NUM_ENTITIES];
	Entity pastEntities[NUM_ENTITIES];

	Vec2 camPosPrev;
	Vec2 camPos;
};

void initEngine();
void handleEvents(Input *Input);
void updateEntities();
double elapsedMs();

#endif
