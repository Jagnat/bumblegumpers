#include "bg_editor.h"

Editor *editor;

#define ECOLOR_MAIN 0x6e8b9fCC
#define ECOLOR_SCROLL_BG 0x262626CC
#define ECOLOR_SCROLL_THUMB 0xC93EF0FF

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

	beginSpriteBatch(0, CAM_GAME);
	{
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
