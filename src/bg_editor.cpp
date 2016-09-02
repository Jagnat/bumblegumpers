#include "bg_editor.h"

Editor *editor;

#define ECOLOR_MAIN 0xBCC8CCCC
#define ECOLOR_SCROLL_BG 0x262626CC
#define ECOLOR_SCROLL_THUMB 0xEDEDEDFF

#define DEPTH_BLAYER 0.49f
#define DEPTH_MLAYER 0.50f
#define DEPTH_FLAYER 0.51f
#define DEPTH_CLAYER 0.52f
#define DEPTH_GRID 0.53f

#define DEPTH_GUI_BG 0.75f
#define DEPTH_GUI_OVERLAY 0.76f
#define DEPTH_GUI_MAIN 0.77f
#define DEPTH_GUI_TEXT 0.78f

void editorInit(Editor *e)
{
	editor = e;
	editor->camPos = CreateVec(0, 0);
	editor->editMode = MODE_COLLISION;
}

void editorResize(int w, int h)
{
	editor->width = w;
	editor->height = h;
}

void editorSetWorld(World *world)
{
	editor->world = world;
}

void editorUpdate(Input *input)
{
	editor->camPrev = editor->camPos;
	if (input->left.down)
		editor->camPos.x -= 0.1f;
	if (input->right.down)
		editor->camPos.x += 0.1f;
	if (input->up.down)
		editor->camPos.y += 0.1f;
	if (input->down.down)
		editor->camPos.y -= 0.1f;
}

void editorDrawGrid(int w, int h)
{
	// draw grid
	setZ(DEPTH_GRID);
	setColor(WithAlpha(COLOR_WHITE, 0.2f));
	for (int x = 0; x <= w; ++x)
		addSprite(CreateRect(x - .02f, 0, .04f, h));
	for (int y = 0; y <= h; ++y)
		addSprite(CreateRect(0, y - .02f, w, .04f));
}

static void editorChooseTileColor(int index)
{
	if (editor->editMode > index)
		setColor(0xCCCCCCAA);
	else if (editor->editMode < index)
		setColor(0x444444AA);
	else
		setColor(0xFFFFFFFF);
}

static void editorDrawLayer(uint16 *layer, int w, int h)
{
	const float tr = 1.f / 32.f;
	for (int x = 0; x < w; x++)
	{
		for (int y = 0; y < h; ++y)
		{
			uint16 t = layer[y * w + x];
			if (t == 0)
				continue;
			t--;
			Rect texPos = CreateRect((t % 32) * tr, (t / 32) * tr, tr, tr);
			addSprite(CreateRect(x, y, 1, 1), texPos);
		}
	}
}

void editorDrawTiles(World *world)
{
	editorChooseTileColor(0);
	setZ(DEPTH_BLAYER);
	editorDrawLayer(world->bTiles, world->width, world->height);
	editorChooseTileColor(1);
	setZ(DEPTH_MLAYER);
	editorDrawLayer(world->mTiles, world->width, world->height);
	editorChooseTileColor(2);
	setZ(DEPTH_FLAYER);
	editorDrawLayer(world->fTiles, world->width, world->height);
}

void editorDrawCollision(World *world)
{
	setZ(DEPTH_CLAYER);
	for (int x = 0; x < world->width; ++x)
	{
		for (int y = 0; y < world->height; ++y)
		{
			TileCollision c = getCollision(world, x, y);
			switch(c)
			{
				case TC_SOLID:
				{
					setColor(0x77889980);
					addSprite(CreateRect(x, y, 1, 1));
				}
				break;
				case TC_LADDER:
				{
					setColor(0x69F9F980);
					addSprite(CreateRect(x + .1f, y, 0.1f, 1));
					addSprite(CreateRect(x + 0.8, y, 0.1f, 1));
				}
				break;
				case TC_CORNER:
				{
					setColor(0xB9696980);
					addSprite(CreateRect(x, y, .2f, .2f));
					addSprite(CreateRect(x + .8f, y, .2f, .2f));
					addSprite(CreateRect(x, y + .8f, .2f, .2f));
					addSprite(CreateRect(x + .8f, y + .8f, .2f, .2f));
				}
				break;
				case TC_PLATFORM:
				{
					setColor(0x69B96980);
					addSprite(CreateRect(x, y + .7f, 1, .3f));
				}
				break;
				case TC_EMPTY:
				default:
				break;
			}
		}
	}
}

void editorDrawEditMode()
{

}

void editorDrawScrollbar()
{
	setZ(DEPTH_GUI_BG);
	setColor(ECOLOR_SCROLL_BG);
	addSprite(CreateRect(252, 0, 16, editor->height));
	setZ(DEPTH_GUI_OVERLAY);
	setColor(ECOLOR_SCROLL_THUMB);
	addSprite(CreateRect(252, 0, 16, 64));
}

void editorRender(double interval)
{
	setCamPos(Vec2_Lerp(editor->camPrev, editor->camPos, interval));

	beginSpriteBatch(1, CAM_GAME);
	{
		editorDrawTiles(editor->world);
	}
	endSpriteBatch();

	beginSpriteBatch(0, CAM_GAME);
	{
		if (editor->editMode == MODE_COLLISION)
			editorDrawCollision(editor->world);
		editorDrawGrid(editor->world->width, editor->world->height);
	}
	endSpriteBatch();

	beginSpriteBatch(0, CAM_SCREEN);
	{
		// background
		setColor(ECOLOR_MAIN);
		setZ(DEPTH_GUI_BG);
		addSprite(CreateRect(0, 0, 252, editor->height));
		editorDrawScrollbar();
	}
	endSpriteBatch();
}
