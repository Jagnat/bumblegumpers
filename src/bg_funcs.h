#pragma once
#ifndef _BG_FUNCS_H_
#define _BG_FUNCS_H_

#include "bg_data.h"

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
Rect getTileRect(uint16 id);
Rect getPlayerRect();
void imguiInit();
void imguiResize(int w, int h);
void imguiRender();

// World API
void createWorld(World *world, int w, int h);
void createTestWorld(World *world);
bool loadWorld(World *world, const char *filename);
void saveWorld(World *world, const char *filename);
int resizeWorld(World *wld, int leftAmt, int rightAmt, int upAmt, int downAmt);
void setCollision(World *world, int x, int y, TileCollision col);
TileCollision getCollision(World *world, int x, int y);
void setB(World *world, int x, int y, uint16 val);
void setM(World *world, int x, int y, uint16 val);
void setF(World *world, int x, int y, uint16 val);
uint16 getB(World *world, int x, int y);
uint16 getM(World *world, int x, int y);
uint16 getF(World *world, int x, int y);

// Editor API
void editorInit(Editor *e);
void editorSetWorld(World *world);
void editorUpdate(Input *input);
void editorRender();
void editorResize(int w, int h);

#endif

