#pragma once
#ifndef _BG_EDITOR_H_
#define _BG_EDITOR_H_

#include "bg_world.h"

enum EditMode
{
	MODE_BACKGROUND,
	MODE_MAIN,
	MODE_FOREGROUND,
	MODE_COLLISION
};

static const char blockName[] = "Blocks";
static const char collisionName[] = "Collision";
struct BlockPanel
{
	bool extended;
	bool showCollision;

	int height;
};

struct Editor
{
	World *world;
	Vec2 camPrev;
	Vec2 camPos;
	int width, height;

	EditMode editMode;
	uint16 editId;

	BlockPanel blockPanel;

	bool scrollEnabled;
	int thumbPos;	
	int scrollOffset;
	int totalContentHeight;
};

struct Input;

void editorInit(Editor *e);
void editorSetWorld(World *world);

void editorUpdate(Input *input);
void editorRender(double interval);
void editorResize(int w, int h);

#endif // _BG_EDITOR_H_
