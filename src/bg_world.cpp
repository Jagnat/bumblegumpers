#include "bg_world.h"

void createWorld(World *world, int w, int h)
{
	assert(world != 0);
	world->width = w;
	world->height = h;
	world->collision = (TileCollision*)calloc(1, w * h * sizeof(TileCollision));
	world->bTiles = (uint16*)calloc(1, w * h * sizeof(uint16));
	world->mTiles = (uint16*)calloc(1, w * h * sizeof(uint16));
	world->fTiles = (uint16*)calloc(1, w * h * sizeof(uint16));
}

bool loadWorld(World *world, const char *filename)
{
	FILE *wf = fopen(filename, "rb");
	if (!wf)
	{
		log_warning("Wasn't able to open worldfile: %s!", filename);
		return false;
	}

	fread(&world->width, sizeof(uint), 1, wf);
	fread(&world->height, sizeof(uint), 1, wf);
	log_info("Loading world %s, %dx%d", filename, world->width, world->height);

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
