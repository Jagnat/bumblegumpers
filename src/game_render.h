#pragma once
#ifndef _GAME_RENDER_H_
#define _GAME_RENDER_H_

#include <glew.h>
#include "primitives.h"
#include "game_assets.h"

union Color32
{
	struct
	{
		uint8 r, g, b, a;
	};
	uint8 elements[4];
	int element;
};

struct MGLFramebufferData
{
	GLuint framebufferProgramId;
	GLuint framebufferId;
	GLuint renderbufferId;
	GLuint frameVboId;
	GLuint framebufferTexture;

	GLint uniformTexLoc;
};

struct Vertex;

enum CameraMode
{
	CAMERA_MODE_GAME,
	CAMERA_MODE_SCREEN
};

struct MGLSpriteBatch
{
	union
	{
		struct
		{
			GLuint vboId;
			GLuint iboId;
			GLuint vaoId;
		};
		GLuint ids[3];
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
	GLuint programId;
	MGLFramebufferData framebuffer;
	
	GLuint textureIds[16];

	Mat4 projMatrix;
	Mat4 viewMatrix;
	Mat4 pixelMatrix;
	GLint transformUniformLoc;
	GLint textureUniformLoc;
	MGLSpriteBatch spriteBatch;

};

static const Color32 COLOR_WHITE = {255, 255, 255, 255};
static const Color32 COLOR_RED = {255, 0, 0, 255};
static const Color32 COLOR_GREEN = {0, 255, 0, 255};
static const Color32 COLOR_BLUE = {0, 0, 255, 255};
static const Color32 COLOR_BLACK = {0, 0, 0, 255};

struct Vertex
{
	union
	{
		struct
		{
			float x, y, z;
			uint8 r, g, b, a;
			float u, v;
		};
		struct
		{
			Vec3 position;
			Color32 color;
			Vec2 texel;
		};
	};
};
#define VERTEX_SIZE (sizeof(Vertex))

// Rendering api
void initRenderer(Renderer *rend, int w, int h);
void resizeRenderer(uint w, uint h);

void setTextureAtIndex(int index, Texture *texture);

void beginRender();
void endRender();

void beginSpriteBatch(int textureIndex, CameraMode mode);
void setColor(Color32 color);
void setZ(float z);
void addSprite(AABB quad, AABB tex);
void add14ptText(Vec2 startPos, const char *string, ...);
void endSpriteBatch();
#endif
