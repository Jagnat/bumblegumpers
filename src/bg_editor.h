#pragma once
#ifndef _BG_EDITOR_H_
#define _BG_EDITOR_H_

#include "bg_world.h"

enum EditMode : int
{
	MODE_BACKGROUND = 0,
	MODE_MAIN = 1,
	MODE_FOREGROUND = 2,
	MODE_COLLISION = 3
};

// In ImGui, ###label allows both text modes to have same ID
char blockText[] = "Picker: Block Mode###selector";
char collisionText[] = "Picker: Collision Mode###selector";
struct Editor
{
	World *world;
	Vec2 camPos;
	Vec2 cursorPos;

	EditMode editMode;
	char *editText;
	uint16 editId;

	uint screenW, screenH;
};

struct Input;

void editorInit(Editor *e);
void editorSetWorld(World *world);

void editorUpdate(Input *input);
void editorRender();
void editorResize(int w, int h);

#endif // _BG_EDITOR_H_
