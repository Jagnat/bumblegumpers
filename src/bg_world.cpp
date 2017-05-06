#include "bg_world.h"

void createWorld(World *world, int w, int h)
{
	assert(world != 0);
	world->width = w;
	world->height = h;
	if (!world->collision)
	{
		uint8 *tmp = (uint8*)calloc(1, w * h * sizeof(TileCollision) +
			3 * w * h * sizeof(uint16));
		world->collision = (TileCollision*)tmp;
		world->bTiles = (uint16*)&tmp[w * h * sizeof(TileCollision)];
		world->mTiles = &world->bTiles[w * h];
		world->fTiles = &world->mTiles[w * h];
	}
}

void freeWorld(World *world)
{
	if (world && world->collision)
	{
		free(world->collision);
		world->collision = 0;
		world->bTiles = 0;
		world->mTiles = 0;
		world->fTiles = 0;
	}
}

int resizeWorld(World *wld, int leftAmt, int rightAmt, int upAmt, int downAmt)
{
	if (!wld->collision || !wld->bTiles || !wld->mTiles || !wld->fTiles)
		return 1;
	// No resize necessary
	if (!leftAmt && !rightAmt && !upAmt && !downAmt)
		return 0;
	int w = wld->width + leftAmt + rightAmt;
	int h = wld->height + upAmt + downAmt;
	if (w <= 0 || h <= 0)
		return 1;

	uint8 *tmp = (uint8*)calloc(1, (w * h * sizeof(TileCollision)) +
		(3 * w * h * sizeof(uint16)));

	TileCollision *tc = (TileCollision*)tmp;
	uint16 *bt = (uint16*)&tmp[w * h * sizeof(TileCollision)];
	uint16 *mt = &bt[w * h];
	uint16 *ft = &mt[w * h];

	int cpyW = wld->width;
	if (leftAmt < 0)
		cpyW += leftAmt;
	if (rightAmt < 0)
		cpyW += rightAmt;
	int cpyH = wld->height;
	if (upAmt < 0)
		cpyH += upAmt;
	if (downAmt < 0)
		cpyH += downAmt;

	log_info("cpyW: %d, cpyH: %d", cpyW, cpyH);

	// Source and Dest Offsets - how far forward or backward
	// to index from the beginning of the array during the copy
	int srcWO = leftAmt < 0 ? -leftAmt : 0;
	int dstWO = leftAmt >= 0 ? leftAmt : 0;

	int srcHO = upAmt < 0 ? -upAmt : 0;
	int dstHO = upAmt >= 0 ? upAmt : 0;

	for (int x = 0; x < cpyW; ++x)
	{
		for (int y = 0; y < cpyH; ++y)
		{
			// original width for 2d indexing
			int oW = wld->width;
			tc[((y + dstHO) * w) + x + dstWO] = wld->collision[((y + srcHO) * oW) + x + srcWO];
			bt[((y + dstHO) * w) +  x + dstWO] = wld->bTiles[((y + srcHO) * oW) + x + srcWO];
			mt[((y + dstHO) * w) +  x + dstWO] = wld->mTiles[((y + srcHO) * oW) + x + srcWO];
			ft[((y + dstHO) * w) +  x + dstWO] = wld->fTiles[((y + srcHO) * oW) + x + srcWO];
		}
	}

	free(wld->collision);

	wld->height = h;
	wld->width = w;
	wld->collision = tc;
	wld->bTiles = bt;
	wld->mTiles = mt;
	wld->fTiles = ft;

	return 0;
}

bool loadWorldDev(World *world, const char *filename)
{
}

void saveWorldDev(World *world, const char *filename)
{
	FILE *wf = fopen(filename, "r");
	if (!wf)
	{
		return;
	}
}

bool loadWorld(World *world, const char *filename)
{
	FILE *wf = fopen(filename, "rb");
	if (!wf)
	{
		// TODO: handle gracefully
		log_warning("Wasn't able to open worldfile: %s!", filename);
		return false;
	}

	fread(&world->width, sizeof(uint), 1, wf);
	fread(&world->height, sizeof(uint), 1, wf);

	createWorld(world, world->width, world->height);

	fread(world->collision, sizeof(TileCollision) * world->width * world->height, 1, wf);

	fread(world->bTiles, sizeof(uint16) * world->width * world->height, 1, wf);
	fread(world->mTiles, sizeof(uint16) * world->width * world->height, 1, wf);
	fread(world->fTiles, sizeof(uint16) * world->width * world->height, 1, wf);
	fclose(wf);
	return true;
}

void saveWorld(World *world, const char *filename)
{
	assert(world);
	FILE *wf = fopen(filename, "wb");
	if (!wf)
	{
		log_warning("Wasn't able to create world file: %s!", filename);
		return;
	}

	fwrite(&world->width, sizeof(uint), 1, wf);
	fwrite(&world->height, sizeof(uint), 1, wf);
	
	fwrite(world->collision, sizeof(TileCollision) * world->width * world->height, 1, wf);
	log_info("sizeof(tilecollision): %d", sizeof(TileCollision));

	fwrite(world->bTiles, sizeof(uint16) * world->width * world->height, 1, wf);
	fwrite(world->mTiles, sizeof(uint16) * world->width * world->height, 1, wf);
	fwrite(world->fTiles, sizeof(uint16) * world->width * world->height, 1, wf);
	fclose(wf);
}

void setCollision(World *world, int x, int y, TileCollision col)
{
	if (x >= world->width || x < 0 || y >= world->height || y < 0)
		return;
	world->collision[y * world->width + x] = col;
}

TileCollision getCollision(World *world, int x, int y)
{
	if (x >= world->width || x < 0 || y >= world->height || y < 0)
		return TC_SOLID;
	return world->collision[y * world->width + x];
}

void setB(World *world, int x, int y, uint16 val)
{
	if (x >= world->width || x < 0 || y >= world->height || y < 0)
		return;
	world->bTiles[y * world->width + x] = val;
}
void setM(World *world, int x, int y, uint16 val)
{
	if (x >= world->width || x < 0 || y >= world->height || y < 0)
		return;
	world->mTiles[y * world->width + x] = val;
}
void setF(World *world, int x, int y, uint16 val)
{
	if (x >= world->width || x < 0 || y >= world->height || y < 0)
		return;
	world->fTiles[y * world->width + x] = val;
}

uint16 getB(World *world, int x, int y)
{
	if (x >= world->width || x < 0 || y >= world->height || y < 0)
		return 0;
	return world->bTiles[y * world->width + x];
}
uint16 getM(World *world, int x, int y)
{
	if (x >= world->width || x < 0 || y >= world->height || y < 0)
		return 0;
	return world->mTiles[y * world->width + x];
}
uint16 getF(World *world, int x, int y)
{
	if (x >= world->width || x < 0 || y >= world->height || y < 0)
		return 0;
	return world->fTiles[y * world->width + x];
}

void createTestWorld(World *world)
{
	loadWorld(world, "testworld.bgw");
	/*createWorld(world, 16, 12);

	for (int x = 0; x < world->width; x++)
	{
		for (int y = 0; y < world->height; y++)
		{
			setCollision(world, 0, y, TC_SOLID);
			setCollision(world, x, 0, TC_SOLID);
			setCollision(world, world->width - 1, y, TC_SOLID);
			setCollision(world, x, world->height - 1, TC_SOLID);
		}
	}

	setCollision(world, 3, 1, TC_SOLID);

	setCollision(world, 7, 1, TC_SOLID);
	setCollision(world, 8, 2, TC_SOLID);
	setCollision(world, 9, 3, TC_SOLID);
	for (int i = 0; i < 3; ++i)
	{
		setCollision(world, 11 + i, 3, TC_SOLID);
		setCollision(world, 6 + i, 7, TC_SOLID);
	}

	for (int i = 0; i < 3; ++i)
		setCollision(world, 14, 1 + i, TC_LADDER);
	setCollision(world, 9, 6, TC_LADDER);
	setCollision(world, 9, 7, TC_LADDER);

	setCollision(world, 14, 10, TC_PLATFORM);
	setCollision(world, 13, 10, TC_CORNER);

	saveWorld(world, "testworld.bgw");*/
}
