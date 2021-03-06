#pragma once
#ifndef _BG_DATA_H_
#define _BG_DATA_H_

#endif

#include "primitives.h"

union Color32
{
	struct
	{
		uint8 r, g, b, a;
	};
	struct
	{
		uint8 elements[4];
	};
	uint element;
};

inline Color32 WithAlpha(Color32 c, float alpha)
{
	Color32 r = {c.r, c.g, c.b, (uint8)(alpha * 255)};
	return r;
}

static const Color32 COLOR_WHITE = {255, 255, 255, 255};
static const Color32 COLOR_BLACK = {0, 0, 0, 255};
static const Color32 COLOR_RED = {255, 0, 0, 255};
static const Color32 COLOR_GREEN = {0, 255, 0, 255};
static const Color32 COLOR_BLUE = {0, 0, 255, 255};

enum CameraMode
{
	CAM_GAME,
	CAM_SCREEN
};

struct Texture;

struct Sprite
{
	Rect region;
	Texture *texture;
};

struct Button
{
	bool pressed;
	bool released;
	bool down;
};

union ModState
{
	struct
	{
		bool control;
		bool shift;
		bool alt;
	};
	bool mods[3];
};

struct Input
{
	union
	{
		struct
		{
			// Gameplay buttons
			Button left, right, up, down;
			Button attack, use, shield;

			// Editor buttons
			Button toggleEdit;

			// Mouse
			Button leftMouse, rightMouse, middleMouse;
		};
		#define NUM_BUTTONS 16
		Button buttons[NUM_BUTTONS];
	};
	ModState modState;

	// NOTE: for imgui
	// TODO: remove or change
	bool keysDown[512];

	// TODO: expand this if we overflow
	#define INPUT_TEXT_BUFFER_LENGTH 256
	char textBuffer[INPUT_TEXT_BUFFER_LENGTH];

	int mouseX, mouseY;
};

enum EntityType
{
	EMPTY,
	PLAYER,
	CHANDELIER,
	LEVER
};

enum MovementMode
{
	ONGROUND,
	AIRWALKING,
	JUMPING,
	CLIMBING,
	FALLING
};

struct Entity
{
	uint id;
	EntityType type;

	// TODO: put defines here
	int flags;

	union
	{
		struct { int x, y; };
		int pos[2];
	};

	MovementMode movement;

	bool noTexture;
	int textureId;
	Rect uv;
};

enum TileCollision : uint8
{
	TC_EMPTY = 0,
	TC_SOLID = 1,
	TC_LADDER = 2,
	TC_CORNER = 3,
	TC_PLATFORM = 4
};

struct World
{
	char id[64];
	uint width, height;
	TileCollision *collision;
	uint16 *bTiles;
	uint16 *mTiles;
	uint16 *fTiles;
};

#define NUM_ENTITIES 32
struct Game
{
	World world;

	Entity entities[NUM_ENTITIES];

	Input input;
};

struct Platform
{
	SDL_Window *window;
	SDL_GLContext glContext;
	double targetUpdateDelta;
	double targetRenderDelta;
	double timerResolution;
	uint width, height;
	bool running;
	bool editorEnabled;
};

