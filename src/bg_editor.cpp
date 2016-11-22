#include "bg_editor.h"

Editor *editor;

void editorSetWorld(World *world)
{
	editor->world = world;
}

void editorResize(int w, int h)
{
	editor->screenW = w;
	editor->screenH = h;
}

void editorInit(Editor *e)
{
	editor = e;
	editor->camPos = CreateVec(0, 0);
	editor->editMode = MODE_FOREGROUND;
	editor->editText = &blockText[0];
}

static void editorDrawLayer(uint16 *layer, int w, int h, float z = 0.5f)
{
	setZ(z);
	for (int x = 0; x < w; x++)
	{
		for (int y = 0; y < h; ++y)
		{
			uint16 t = layer[y * w + x];
			if (t == 0)
				continue;
			Rect texPos = getTileRect(t);
			addSprite(CreateRect(x, y, 1, 1), texPos);
		}
	}
}

static void editorChooseTileColor(int index)
{
	if (editor->editMode > index)
		setColor(0xAAAAAA33);
	else if (editor->editMode < index)
		setColor(0x44444400);
	else
		setColor(0xFFFFFFFF);
}

static void editorDrawGrid(int w, int h)
{
	// draw grid
	setZ(0.6f);
	setColor(WithAlpha(COLOR_WHITE, 0.2f));
	for (int x = 0; x <= w; ++x)
		addSprite(CreateRect(x - .02f, 0, .04f, h));
	for (int y = 0; y <= h; ++y)
		addSprite(CreateRect(0, y - .02f, w, .04f));
}

void editorDrawCollision(World *world, float x, float y, TileCollision c)
{
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
		{
			setColor(0xec877cFF);
			addSprite(CreateRect(x, y, 1, 1));
		}
		break;
		default:
		break;
	}
}

void editorDrawCollisionMap(World *world)
{
	setZ(0.52f);
	for (int x = 0; x < world->width; ++x)
	{
		for (int y = 0; y < world->height; ++y)
		{
			TileCollision c = getCollision(world, x, y);
			if (c == TC_EMPTY)
				continue;
			editorDrawCollision(world, x, y, c);
		}
	}
}

void editorUpdate(Input *input)
{

	World *world = editor->world;

	ImGuiIO *io = &ImGui::GetIO();
	io->MousePos = ImVec2(input->mouseX, input->mouseY);
	io->MouseDown[0] = input->leftMouse.down;
	io->MouseDown[1] = input->rightMouse.down;
	io->MouseDown[2] = input->middleMouse.down;
	ImGui::NewFrame();

	ImGuiStyle *style = &ImGui::GetStyle();
	style->Colors[ImGuiCol_WindowBg] = ImVec4(.9, .9, .9, .8);
	style->Colors[ImGuiCol_Text] = ImVec4(0, 0, 0, 1);
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(.6, .6, .6, .8);
	style->Colors[ImGuiCol_TitleBg] = ImVec4(.5, .5, .5, .8);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(.5, .5, .5, .6);
	style->Colors[ImGuiCol_CheckMark] = ImVec4(.2, .2, .2, 1);
	style->Colors[ImGuiCol_FrameBg] = ImVec4(.6, .6, .6, 1);
	style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(.5, .5, .5, 1);
	style->Colors[ImGuiCol_FrameBgActive] = ImVec4(.3, .7, .3, 1);
	style->Colors[ImGuiCol_ResizeGrip] = ImVec4(.3, .3, .3, 1);
	style->Colors[ImGuiCol_PopupBg] = ImVec4(.6, .6, .6, 1);
	style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(.1, .1, .1, .8);
	style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(.7, .7, .7, .8);
	style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(.5, .5, .7, 1);
	style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(.7, .7, 1, 1);

	ImGui::Begin("Editor");
	EditMode pastMode = editor->editMode;

	ImGui::Text("World controls");
	if (ImGui::Button("Save As"))
	{
		char buff[128];
		// saveWorld(editor->world, "savedworld.wld");
		// ImGui::InputText("labeL", buff, 128);
		ImGui::OpenPopup("save_as_popup");
	}
	if (ImGui::BeginPopup("save_as_popup"))
	{
		// TODO: Fix text input for imgui so this works
		char buff[128];
		ImGui::InputText("LabeL", buff, 128);
		ImGui::EndPopup();
	}
	ImGui::SameLine();
	if (ImGui::Button("Load"))
	{
		// resizeWorld(editor->world, -1, -1, -1, -1);
	}
	ImGui::SameLine();
	static bool showResizeWindow = true;
	if (ImGui::Button("Show/Hide Resize"))
	{
		showResizeWindow = !showResizeWindow;
	}

	if (showResizeWindow)
	{
		ImGui::Begin("Resize...");
		ImGui::End();
	}

	ImGui::Text("Edit Mode");
	ImGui::RadioButton("background", (int*)&editor->editMode, (int)MODE_BACKGROUND);
	ImGui::SameLine();
	ImGui::RadioButton("main", (int*)&editor->editMode, (int)MODE_MAIN);
	ImGui::SameLine();
	ImGui::RadioButton("foreground", (int*)&editor->editMode, (int)MODE_FOREGROUND);
	ImGui::SameLine();
	ImGui::RadioButton("collision", (int*)&editor->editMode, (int)MODE_COLLISION);
	if (pastMode != editor->editMode)
		editor->editId = 0;

	if (editor->editMode == MODE_COLLISION)
		editor->editText = &collisionText[0];
	else
		editor->editText = &blockText[0];

	if (ImGui::CollapsingHeader(editor->editText))
	{
		if (editor->editMode != MODE_COLLISION)
		{
			ImDrawList *dl = ImGui::GetWindowDrawList();
			ImVec2 p = ImGui::GetCursorScreenPos();
			float w = ImGui::GetWindowWidth();

			int numRows = 0;
			int numColumns = 0;
			int yo = 10;
			for (int i = 0; i < NUM_TILES + 1;)
			{
				for (int xo = 0; 34 + (xo * 34) < w; xo++)
				{
					if (i >= NUM_TILES + 1)
						break;
					ImVec2 po = ImVec2(p.x + xo * 34, p.y + yo);
					Rect uv = getTileRect(i);

					if (i == 0)
						dl->AddRectFilled(po, ImVec2(po.x + 24, po.y + 24), ImU32(0xec877cFF));
					else
						dl->AddImage((void*)&renderer->textures[1].glId, po, ImVec2(po.x + 24, po.y + 24),
							ImVec2(uv.x, uv.y), ImVec2(uv.x + uv.w, uv.y + uv.h));
					i++;

					if (xo > numColumns)
						numColumns = xo;
				}
				numRows++;
				yo += 34;
			}
			numColumns++;
			ImGui::InvisibleButton("sButn", ImVec2(w, numRows * 34 + 10));
			if (ImGui::IsItemClicked())
			{
				ImVec2 mPos = ImVec2(input->mouseX - p.x, input->mouseY - p.y);
				mPos.y -= 10;
				log_info("nc: %d nr: %d", numColumns, numRows);	
				log_info("x:%.3f, y:%.3f", mPos.x, mPos.y);

				int xO = (int)floor(mPos.x) / 34;
				int yO = (int)floor(mPos.y) / 34;
				int xR = (int)floor(mPos.x) % 34;
				int yR = (int)floor(mPos.y) % 34;
				if (xR < 24 && yR < 24)
					editor->editId = yO * numColumns + xO;
			}
		}
		else
		{
			ImGui::RadioButton("Erase", (int*)&editor->editId, 0); ImGui::SameLine();
			ImGui::RadioButton("Solid", (int*)&editor->editId, 1);
			ImGui::RadioButton("Ladder", (int*)&editor->editId, 2); ImGui::SameLine();
			ImGui::RadioButton("Corner", (int*)&editor->editId, 3);
			ImGui::RadioButton("Platform", (int*)&editor->editId, 4);
		}
	}

	if (ImGui::CollapsingHeader("Test"))
	{
		ImGui::Button("This is a test");
	}

	float cS = .2f;
	if (input->left.down)
		editor->camPos.x -= cS;
	if (input->right.down)
		editor->camPos.x += cS;
	if (input->up.down)
		editor->camPos.y += cS;
	if (input->down.down)
		editor->camPos.y -= cS;
	setCamPos(editor->camPos);

	Vec4 res = {-20000, -20000};

	ImVec2 winMin = ImGui::GetWindowPos();
	ImVec2 bnds = ImGui::GetWindowSize();
	ImVec2 winMax = ImVec2(bnds.x + winMin.x + 200, bnds.y + winMin.y + 200);

	ImGui::End();

	if (!ImGui::IsMouseHoveringAnyWindow())
	{
		float mx = input->mouseX;
		float my = input->mouseY;
		Mat4 mV = renderer->viewMatrix;
		Mat4 mP = renderer->projMatrix;

		Mat4 inv = Mat4_Identity();
		bool s = Mat4_Invert((mP * mV), &inv);

		if (s)
		{
			float xT = (float)mx / (float)editor->screenW;
			float yT = (float)my / (float)editor->screenH;
			xT = (xT * 2) - 1;
			yT = -((yT * 2) - 1);
			Vec4 ssP = CreateVec(xT, yT, 1, 1);
			res = Vec4_MMult(ssP, inv);

			res.x += editor->camPos.x;
			res.y += editor->camPos.y;
		}
		else
			log_info("inverse failed");

		if (input->leftMouse.down &&
			res.x < world->width && res.x > 0 &&
			res.y < world->height && res.y > 0)
		{
			if (editor->editMode == MODE_COLLISION)
			{
				setCollision(world, res.x, res.y, (TileCollision)editor->editId);
			}
			else
			{
				uint16 *arr[3] = { world->bTiles, world->mTiles, world->fTiles };
				(arr[editor->editMode])[(int)floor(res.y) * world->width + (int)floor(res.x)] = editor->editId;
			}
		}
	}
	editor->cursorPos = CreateVec(res.x, res.y);

	ImGui::EndFrame();
}

void editorRender()
{
	World *world = editor->world;
	
	beginRender();

	Vec2 cp = editor->cursorPos;
	if (cp.x < world->width && cp.x > 0 &&
		cp.y < world->height && cp.y > 0)
	{
		// draw preview
		if (editor->editMode == MODE_COLLISION || editor->editId == 0)
		{
			beginSpriteBatch(0, CAM_GAME);
			setZ(0.8f);
			editorDrawCollision(world, floor(cp.x), floor(cp.y), (TileCollision)editor->editId);
			endSpriteBatch();
		}
		else
		{
			beginSpriteBatch(1, CAM_GAME);
			setZ(0.8f);
			Rect texPos = getTileRect(editor->editId);
			addSprite(CreateRect(floor(cp.x), floor(cp.y), 1, 1), texPos);
			endSpriteBatch();
		}
	}

	beginSpriteBatch(1, CAM_GAME);
	{
		editorChooseTileColor(0);
		editorDrawLayer(world->bTiles, world->width, world->height, 0.49f);
		editorChooseTileColor(1);
		editorDrawLayer(world->mTiles, world->width, world->height, 0.5f);
		editorChooseTileColor(2);
		editorDrawLayer(world->fTiles, world->width, world->height, 0.51f);
	}
	endSpriteBatch();

	beginSpriteBatch(0, CAM_GAME);
	{
		if (editor->editMode == MODE_COLLISION)
			editorDrawCollisionMap(editor->world);
		editorDrawGrid(world->width, world->height);
	}
	endSpriteBatch();

	imguiRender();
	endRender();
}
