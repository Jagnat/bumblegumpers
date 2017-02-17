#pragma once
#ifndef _BG_H_
#define _BG_H_

#include "bg_util.h"
#include "bg_render.h"
#include "bg_world.h"
#include "bg_editor.h"
#include <SDL.h>

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

	// TODO: expand this if we overflow
	#define INPUT_TEXT_BUFFER_LENGTH 256
	char textBuffer[INPUT_TEXT_BUFFER_LENGTH];

	int mouseX, mouseY;
};

enum PlayerState
{
	PLAYER_ONGROUND,
	PLAYER_AIRWALKING,
	PLAYER_JUMPING,
	PLAYER_CLIMBING,
	PLAYER_FALLING
};

struct Player
{
	int x, y;
	PlayerState state;
};

struct Game
{
	bool inEditor;

	World world;
	Player player;

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

	Renderer renderer;
	Game game;
	Editor editor;
};

#endif
