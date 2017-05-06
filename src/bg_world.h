#pragma once
#ifndef _BG_WORLD_H_
#define _BG_WORLD_H_

enum TileCollision : uint8
{
	TC_EMPTY = 0,
	TC_SOLID = 1,
	TC_LADDER = 2,
	TC_CORNER = 3,
	TC_PLATFORM = 4
};

#define NUM_TILES 64

struct World
{
	char id[64];
	uint width, height;
	TileCollision *collision;
	uint16 *bTiles;
	uint16 *mTiles;
	uint16 *fTiles;
};

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

#endif

