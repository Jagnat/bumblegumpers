#pragma once
#ifndef _BG_RENDER_H_
#define _BG_RENDER_H_

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
static const Color32 COLOR_RED = {255, 0, 0, 255};
static const Color32 COLOR_GREEN = {0, 255, 0, 255};
static const Color32 COLOR_BLUE = {0, 0, 255, 255};
static const Color32 COLOR_BLACK = {0, 0, 0, 255};

enum CameraMode
{
	CAM_GAME,
	CAM_SCREEN
};

struct Texture
{
	uint glId;
	int width, height;
	int bytesPerPixel;
	bool linearBlend;
	uint8 *pixels;
};

union Vertex;
struct SpriteBatch;

struct SpriteBatch
{
	union
	{
		struct
		{
			uint vboId;
			uint iboId;
			uint vaoId;
		};
		uint ids[3];
	};
	uint totalVertices;
	uint totalIndices;
	uint populatedVertices;
	Vertex *vertices;
	float z;
	Color32 col;
};

struct Renderer
{
	bool initialized;
	uint width, height;

	Mat4 projMatrix;
	Mat4 viewMatrix;
	Mat4 screenMatrix;

	float tilePixels;
	float zoom;
#define TEXTURE_EMPTY 0
#define TEXTURE_TILESHEET 1
#define TEXTURE_FONT 2
	Texture textures[16];

	uint programId;
	int transformLocation;
	int textureLocation;
	SpriteBatch spritebatch;
};

union Vertex
{
	struct
	{
		float x, y, z;
		uint8 r, g, b, a;
		float u, v;
	};
	struct
	{
		Vec3 pos;
		Color32 col;
		Vec2 tex;
	};
};

// Rendering API
void initRenderer(Renderer *r, int w, int h);
void resizeRenderer(int w, int h);

void beginRender();
void endRender();

void setCamPos(Vec2 pos);

void beginSpriteBatch(int textureIndex, CameraMode mode);
void endSpriteBatch();
void setColor(Color32 color);
void setColor(float r, float g, float b, float a);
void setColor(uint hex);
void setAlpha(float a);
void setZ(float z);
void addSprite(Rect quad, Rect tex);
void addSprite(Rect quad);
void addText(Vec2 pos, bool centerX, bool centerY, const char *text, ...);
Vec2 getTextBounds(const char *text);

struct Input;

void imguiInit();
void imguiResize(int w, int h);
void imguiInput(Input *input);
void imguiRender();

#endif
