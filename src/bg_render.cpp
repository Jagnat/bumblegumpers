#include "bg_render.h"

#define STB_IMAGE_IMPLEMENTATION
#include "thirdparty/stb_image.h"

#include "thirdparty/imgui.h"

struct GlyphData
{
	uint8 id, x, y, w, h;
	int8 xo, yo, a;
};

#include "questrial.h"

Renderer *renderer;

static uint createProgram(const char *vertShaderPath, const char *fragShaderPath);
static char *loadShaderCode(const char *filepath);
static uint createShader(const char *shaderCode, GLenum shaderType);
static void initSpritebatch();
static void initTextures();

void initRenderer(Renderer *r, int w, int h)
{
	renderer = r;

	if (!renderer->initialized)
	{
		renderer->width = w;
		renderer->height = h;

		// renderer->viewMatrix = Mat4_Identity();
		renderer->viewMatrix = Mat4_Translate(CreateVec(-8, -6, 0));

		renderer->tilePixels = 12;
		renderer->zoom = 4;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		// glBlendEquation(GL_FUNC_ADD);
		// glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_TEXTURE_2D);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LEQUAL);

		renderer->programId = createProgram("res/vertex.glsl", "res/fragment.glsl");
		glUseProgram(renderer->programId);

		renderer->transformLocation =
			glGetUniformLocation(renderer->programId, "transform");
		if (renderer->transformLocation == -1)
			log_error("Failed to retrieve transform matrix uniform location!");
		renderer->textureLocation = 
			glGetUniformLocation(renderer->programId, "texture");
		if (renderer->textureLocation == -1)
			log_error("Failed to retrieve texture uniform location!");

		initSpritebatch();
		initTextures();

		imguiInit();

		resizeRenderer(w, h);

		renderer->initialized = true;
		log_info("init'd renderer");
	}
}

static void loadTexture(Texture *texture)
{
	uint id;
	GLenum format = GL_RGBA;
	if (texture->bytesPerPixel == 4)
		format = GL_RGBA;
	else if (texture->bytesPerPixel == 3)
		format = GL_RGB;
	else if (texture->bytesPerPixel == 1)
		format = GL_ALPHA;
	glGenTextures(1, &texture->glId);
	glBindTexture(GL_TEXTURE_2D, texture->glId);
	glTexImage2D(GL_TEXTURE_2D, 0, format, texture->width, texture->height, 0,
		format, GL_UNSIGNED_BYTE, texture->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

}

static uint8 whitePixels[16] = {255, 255, 255, 255,
	255, 255, 255, 255,
	255, 255, 255, 255,
	255, 255, 255, 255 };

void initTextures()
{
	Texture *white = &renderer->textures[0];
	white->pixels = whitePixels;
	white->width = 2;
	white->height = 2;
	white->bytesPerPixel = 4;
	loadTexture(white);

	Texture *tilesheet = &renderer->textures[1];
	tilesheet->pixels = stbi_load("res/tilesheet.png", &tilesheet->width, &tilesheet->height,
		&tilesheet->bytesPerPixel, 0);
	if (!tilesheet->pixels)
	{
		log_error("Failed to load tilesheet!");
		return;
	}
	loadTexture(tilesheet);

	Texture *font = &renderer->textures[2];
	uint8 *fontAlpha = (uint8*)calloc(1, 256 * 256 * sizeof(uint8));
	assert(fontAlpha);
	int fbpp;
	fontAlpha = stbi_load("res/questrial.png", &font->width, &font->height,
		&fbpp, 0);
	assert(fbpp == 1);
	font->pixels = (uint8*)calloc(1, font->width * font->height * 4 * sizeof(uint8));
	assert(font->pixels);

	for (int x = 0; x < font->width; ++x)
	{
		for (int y = 0; y < font->height; ++y)
		{
			font->pixels[4 * (y * font->width + x) + 0] = 255;
			font->pixels[4 * (y * font->width + x) + 1] = 255;
			font->pixels[4 * (y * font->width + x) + 2] = 255;
			font->pixels[4 * (y * font->width + x) + 3] = fontAlpha[y * font->width + x];
		}
	}
	free(fontAlpha);

	loadTexture(font);
}

void resizeRenderer(int w, int h)
{
	renderer->width = w;
	renderer->height = h;
	glViewport(0, 0, w, h);
	float scale = renderer->tilePixels * renderer->zoom;
	renderer->projMatrix = Mat4_Ortho(-(float)w / scale / 2, (float)w / scale / 2,
		-(float)h / scale / 2, (float)h / scale / 2, -1, 1);
	renderer->screenMatrix = Mat4_Ortho(0, w, h, 0, -1, 1);

	imguiResize(w, h);

	log_info("render sized to %ux%u", w, h);
}

void beginRender()
{
	// glClearColor(0.094f, 0.098f, 0.129f, 1.f);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
}

void endRender()
{
	
}

void setCamPos(Vec2 pos)
{
	renderer->viewMatrix = Mat4_Translate(-pos.x, -pos.y, 0);
}

// SPRITEBATCH FUNCS
void initSpritebatch()
{
	SpriteBatch *batch = &renderer->spritebatch;
	batch->totalVertices = 24576; // % 4, % 3

	glGenBuffers(2, &batch->ids[0]);
	glGenVertexArrays(1, &batch->vaoId);

	glBindBuffer(GL_ARRAY_BUFFER, batch->vboId);
	glBufferData(GL_ARRAY_BUFFER, batch->totalVertices * sizeof(Vertex),
		0, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// CPU-side index buffer code
	batch->totalIndices = (batch->totalVertices / 2) * 3;
	uint *ind = (uint*)malloc(batch->totalIndices * sizeof(uint));
	for (int i = 0; i < batch->totalVertices / 4; ++i)
	{
		ind[i * 6 + 0] = i * 4 + 0;
		ind[i * 6 + 1] = i * 4 + 1;
		ind[i * 6 + 2] = i * 4 + 2;
		ind[i * 6 + 3] = i * 4 + 0;
		ind[i * 6 + 4] = i * 4 + 2;
		ind[i * 6 + 5] = i * 4 + 3;
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, batch->iboId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, batch->totalIndices * sizeof(uint),
		ind, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	free(ind);

	glBindVertexArray(batch->vaoId);
	glBindBuffer(GL_ARRAY_BUFFER, batch->vboId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, batch->iboId);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	int stride = sizeof(Vertex);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, stride, (void*)(3 * sizeof(float)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, stride, (void*)(4 * sizeof(float)));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void beginSpriteBatch(int textureIndex, CameraMode mode)
{
	switch(mode)
	{
		case CAM_GAME:
		{
			Mat4 transform = renderer->projMatrix * renderer->viewMatrix;
			glUniformMatrix4fv(renderer->transformLocation, 1, false, transform.flat);
			break;
		}
		case CAM_SCREEN:
		default:
		{
			Mat4 transform = renderer->screenMatrix;
			glUniformMatrix4fv(renderer->transformLocation, 1, false, transform.flat);
			break;
		}
	}

	glBindTexture(GL_TEXTURE_2D, renderer->textures[textureIndex].glId);
	SpriteBatch *batch = &renderer->spritebatch;
	batch->z = 0;
	batch->col = COLOR_WHITE;
	batch->populatedVertices = 0;
	glBindBuffer(GL_ARRAY_BUFFER, batch->vboId);
	batch->vertices = (Vertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	if (!batch->vertices)
		log_exit("FAILED to map buffer for spritebatch!");
}

void endSpriteBatch()
{
	SpriteBatch *batch = &renderer->spritebatch;

	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	batch->vertices = (Vertex*)0;

	// do render
	uint numIndices = (batch->populatedVertices * 3) / 2;
	glBindVertexArray(batch->vaoId);
	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, (void*)0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void setColor(Color32 color) { renderer->spritebatch.col = color; }

void setColor(float r, float g, float b, float a)
{
	Color32 c = {(uint8)(r * 255.f), (uint8)(g * 255.f),
		(uint8)(b * 255.f), (uint8)(a * 255.f)};
	setColor(c);
}

void setColor(uint hex)
{
	Color32 c = {(hex >> 24) & 0xFF, (hex >> 16) & 0xFF,
		(hex >> 8) & 0xFF, hex & 0xFF };
	setColor(c);
}

void setAlpha(float a) { setColor(WithAlpha(renderer->spritebatch.col, a)); }

void setZ(float z) { renderer->spritebatch.z = z; }

void addSprite(Rect quad, Rect tex)
{
	SpriteBatch *batch = &renderer->spritebatch;
	int index = batch->populatedVertices;

	batch->vertices[index + 0].pos = CreateVec(quad.x, quad.y, batch->z);
	batch->vertices[index + 0].col = batch->col;
	batch->vertices[index + 0].tex = CreateVec(tex.x, tex.y + tex.h);

	batch->vertices[index + 1].pos = CreateVec(quad.x, quad.y + quad.h, batch->z);
	batch->vertices[index + 1].col = batch->col;
	batch->vertices[index + 1].tex = CreateVec(tex.x, tex.y);

	batch->vertices[index + 2].pos = CreateVec(quad.x + quad.w, quad.y + quad.h, batch->z);
	batch->vertices[index + 2].col = batch->col;
	batch->vertices[index + 2].tex = CreateVec(tex.x + tex.w, tex.y);

	batch->vertices[index + 3].pos = CreateVec(quad.x + quad.w, quad.y, batch->z);
	batch->vertices[index + 3].col = batch->col;
	batch->vertices[index + 3].tex = CreateVec(tex.x + tex.w, tex.y + tex.h);
	batch->populatedVertices += 4;
}

void addSprite(Rect quad) { addSprite(quad, CreateRect(0, 0, 1, 1)); }

int getCharIndex(char c)
{
	if (c > 31 && c < 127)
		return (int)c - 32;
	return 191;
}

Vec2 getTextBounds(const char *text)
{
	Vec2 r = {0, questrialLineHeight};
	int sum = 0;
	for (int k = 0; text[k] != '\0'; ++k)
	{
		int i = getCharIndex(text[k]);
		sum += questrialData[i].a;
	}
	r.x = sum;
	return r;
}

void addText(Vec2 pos, bool centerX, bool centerY, const char *text, ...)
{
	va_list li;
	char buffer[256];
	va_start(li, text);
	int res = vsprintf(buffer, text, li);
	va_end(li);
	if (res < 0)
		log_error("String vsprintf failed!");

	Vec2 temp = pos;
	temp.y += (questrialLineHeight - questrialBase);

	Vec2 bounds = getTextBounds(buffer);
	if (centerX)
		temp.x -= bounds.x / 2;
	if (centerY)
		temp.y -= bounds.y / 2;
	for (int i = 0; buffer[i] != 0; ++i)
	{
		char c = buffer[i];
		GlyphData gd = questrialData[getCharIndex(c)];
		Rect src = CreateRect((float)gd.x / 256.l, (float)gd.y / 256.l,
			(float)gd.w / 256.l, (float)gd.h / 256.l);

		Rect b = CreateRect(temp.x + gd.xo, temp.y + questrialBase - (gd.yo + gd.h), gd.w, gd.h);
		// Rect b = CreateRect(temp.x + gd.xo, temp.y, gd.w, gd.h);
		addSprite(b, src);
		temp.x += gd.a;
	}
}
// END SPRITEBATCH FUNCS

// PROGRAM/SHADER LOADING CODE
// TODO: Rewrite function to accept memory buffer instead of filepath.
// TODO: Handle errors more gracefully... switch render modes
uint createProgram(const char *vertShaderPath, const char *fragShaderPath)
{
	char *vertCode = loadShaderCode(vertShaderPath);
	if (!vertCode)
		log_exit("Failed to load vertex shader code!");
	char *fragCode = loadShaderCode(fragShaderPath);
	if (!fragCode)
		log_exit("Failed to load fragment shader code!");

	uint vertexId = createShader(vertCode, GL_VERTEX_SHADER);
	uint fragmentId = createShader(fragCode, GL_FRAGMENT_SHADER);
	if (!vertexId || !fragmentId)
		log_exit("Failed to create shader!");
	uint programId = glCreateProgram();
	glAttachShader(programId, vertexId);
	glAttachShader(programId, fragmentId);
	free(vertCode); free(fragCode);
	glLinkProgram(programId);
	int status = 0;
	glGetProgramiv(programId, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		int len = 0;
		glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &len);
		char *infolog = (char*)malloc(len);
		glGetProgramInfoLog(programId, len,  0, infolog);
		log_exit("Failed to link shader program:\n%s", infolog);
	}
	return programId;
}

uint createShader(const char *shaderCode, GLenum shaderType)
{
	uint id = 0;
	id = glCreateShader(shaderType);
	if (id == 0)
		log_exit("glCreateShader failed!");
	glShaderSource(id, 1, &shaderCode, 0);
	glCompileShader(id);

	int status = 0;
	glGetShaderiv(id, GL_COMPILE_STATUS, &status);
	if (status == false)
	{
		int len = 0;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);
		char *infolog = (char*)malloc(len);
		glGetShaderInfoLog(id, len, 0, infolog);
		log_error("Failed to compile shader:\n%s", infolog);
		free(infolog);
		glDeleteShader(id);
		return 0;
	}
	return id;
}

char *loadShaderCode(const char *filepath)
{
	FILE *fp = fopen(filepath, "rb");
	if (!fp)
	{
		log_error("Failed to load shader file: %s\n", filepath);
		return 0;
	}
	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char *text = (char*)malloc(size + 1);
	if (!text)
	{
		log_error("Failed to allocate memory!");
		return 0;
	}
	fread(text, 1, size, fp);
	text[size] = 0;
	return text;
}

// ImGui code
static void imguiRenderCallback(ImDrawData *data)
{
	ImGuiIO *io = &ImGui::GetIO();

	glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TRANSFORM_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnable(GL_TEXTURE_2D);
	glUseProgram(0);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, io->DisplaySize.x, io->DisplaySize.y, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
	for (int n = 0; n < data->CmdListsCount; ++n)
	{
		ImDrawList *cmdList = data->CmdLists[n];
		ImDrawVert *vBuffer = cmdList->VtxBuffer.Data;
		ImDrawIdx *iBuffer = cmdList->IdxBuffer.Data;
		glVertexPointer(2, GL_FLOAT, sizeof(ImDrawVert), (void*)((char*)vBuffer + OFFSETOF(ImDrawVert, pos)));
		glTexCoordPointer(2, GL_FLOAT, sizeof(ImDrawVert), (void*)((char*)vBuffer + OFFSETOF(ImDrawVert, uv)));
		glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(ImDrawVert), (void*)((char*)vBuffer + OFFSETOF(ImDrawVert, col)));

		for (int i = 0; i < cmdList->CmdBuffer.Size; ++i)
		{
			ImDrawCmd *pcmd = &cmdList->CmdBuffer[i];
			if (pcmd->UserCallback)
			{
				pcmd->UserCallback(cmdList, pcmd);
			}
			else
			{
				uint tId;
				if (pcmd->TextureId == 0)
					tId = 0;
				else
					tId = *((uint*)pcmd->TextureId);
				glBindTexture(GL_TEXTURE_2D, ((Texture*)(pcmd->TextureId))->glId);
				glScissor((int)pcmd->ClipRect.x, (int)(io->DisplaySize.y - pcmd->ClipRect.w),
					(int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
				glDrawElements(GL_TRIANGLES, pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, iBuffer);
			}
			iBuffer += pcmd->ElemCount;
		}

	}
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glBindTexture(GL_TEXTURE_2D, 0);
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glPopAttrib();

	glUseProgram(renderer->programId);
}

void imguiInit()
{
	ImGuiIO *io = &ImGui::GetIO();

	io->RenderDrawListsFn = imguiRenderCallback;

	Texture *imfnt = &renderer->textures[3];
	io->Fonts->GetTexDataAsRGBA32(&imfnt->pixels,
		&imfnt->width, &imfnt->height);	
	loadTexture(imfnt);
	io->Fonts->TexID = (void*)&imfnt->glId;
}

void imguiResize(int w, int h)
{
	ImGuiIO *io = &ImGui::GetIO();
	io->DisplaySize = ImVec2(w, h);
}

void imguiInput(Input *input)
{
	ImGuiIO *io = &ImGui::GetIO();
	io->MousePos = ImVec2(input->mouseX, input->mouseY);
	io->MouseDown[0] = input->leftMouse.down;
	io->MouseDown[1] = input->rightMouse.down;
	io->MouseDown[2] = input->middleMouse.down;
	ImGui::NewFrame();
}

void imguiRender()
{
	ImGui::Render();
}
