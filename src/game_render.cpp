#include "bg_render.h"
#include "bg.h"

#include <string.h>

static Renderer *renderer;

static GLuint createProgram(const char *vertShaderPath, const char *fragShaderPath);
static char *loadShaderCode(const char *filepath);
static GLuint createShader(const char *shaderCode, GLenum shaderType);
static GLuint loadTexture(uint8 *textureData, int w, int h, int bytesPerPixel);
static void initSpriteBatch();
static void initFrameBuffer();

void beginRender()
{
	glBindFramebuffer(GL_FRAMEBUFFER, renderer->framebuffer.framebufferId);
	glUseProgram(renderer->programId);
	glClearColor(0.094f, 0.098f, 0.129f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void endRender()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(renderer->framebuffer.framebufferProgramId);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindTexture(GL_TEXTURE_2D, renderer->framebuffer.framebufferTexture);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, renderer->framebuffer.frameVboId);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

// SpriteBatch rendering functions
void initSpriteBatch()
{
	MGLSpriteBatch *batch = &renderer->spriteBatch;
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

	int stride = VERTEX_SIZE;
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, stride, (void*)(3 * sizeof(float)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, stride, (void*)(4 * sizeof(float)));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void beginSpriteBatch(int textureIndex, CameraMode mode)
{
	switch (mode)
	{
		case CAMERA_MODE_GAME:
		{
			Mat4 transform = renderer->projMatrix * renderer->viewMatrix;
			glUniformMatrix4fv(renderer->transformUniformLoc, 1, false, transform.flat);
		}
		break;
		case CAMERA_MODE_SCREEN:
		default:
		{
			Mat4 transform = renderer->pixelMatrix;
			glUniformMatrix4fv(renderer->transformUniformLoc, 1, false, transform.flat);
		}
		break;
	}
	glBindTexture(GL_TEXTURE_2D, renderer->textureIds[textureIndex]);
	MGLSpriteBatch *batch = &renderer->spriteBatch;
	batch->z = 0;
	batch->col = COLOR_WHITE;
	batch->populatedVertices = 0;
	glBindBuffer(GL_ARRAY_BUFFER, batch->vboId);
	batch->vertices = (Vertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	if (!batch->vertices)
		log_exit("CRITICAL: failed to retrieve memory from GPU");
}

void setZ(float z) { renderer->spriteBatch.z = z; }
void setColor(Color32 color) { renderer->spriteBatch.col = color; }

void addSprite(AABB quad, AABB tex)
{
	MGLSpriteBatch *batch = &renderer->spriteBatch;
	uint index = batch->populatedVertices;

	batch->vertices[index + 0].position = Vec3_Create(quad.x, quad.y, batch->z);
	batch->vertices[index + 0].color = batch->col;
	batch->vertices[index + 0].texel = Vec2_Create(tex.x, tex.y + tex.h);

	batch->vertices[index + 1].position = Vec3_Create(quad.x, quad.y + quad.h, batch->z);
	batch->vertices[index + 1].color = batch->col;
	batch->vertices[index + 1].texel = Vec2_Create(tex.x, tex.y);

	batch->vertices[index + 2].position = Vec3_Create(quad.x + quad.w, quad.y + quad.h, batch->z);
	batch->vertices[index + 2].color = batch->col;
	batch->vertices[index + 2].texel = Vec2_Create(tex.x + tex.w, tex.y);

	batch->vertices[index + 3].position = Vec3_Create(quad.x + quad.w, quad.y, batch->z);
	batch->vertices[index + 3].color = batch->col;
	batch->vertices[index + 3].texel = Vec2_Create(tex.x + tex.w, tex.y + tex.h);

	batch->populatedVertices += 4;
}

void add14ptText(Vec2 startPos, const char *string, ...)
{
	va_list li;
	char buffer[256];
	va_start(li, string);
	int res = vsprintf(buffer, string, li);
	va_end(li);
	if (res < 0)
		log_error("String vsprintf failed!");

	Vec2 temp = startPos;
	AABB pos, bounds;
	for (int i = 0; buffer[i] != 0; ++i)
	{
		char c = buffer[i];
		if (buffer[i] < 32 || buffer[i] > 127)
			c = ' ';
		getCharCoords(c, &pos, &bounds, &temp);
		addSprite(pos, bounds);
	}
}

void endSpriteBatch()
{
	MGLSpriteBatch *batch = &renderer->spriteBatch;

	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	batch->vertices = 0;

	// DO actual render
	uint numIndices = (batch->populatedVertices * 3) / 2;
	glBindVertexArray(batch->vaoId);
	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, (void*)0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
// End render batch functions

void initRenderer(Renderer *rend, int w, int h)
{
	renderer = rend;
	if (!renderer->initialized)
	{
		renderer->width = w;
		renderer->height = h;

		renderer->viewMatrix = Mat4_Identity();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_TEXTURE_2D);
		glDepthFunc(GL_LEQUAL);

		renderer->programId = createProgram("res/vertex.glsl", "res/fragment.glsl");

		renderer->transformUniformLoc =
			glGetUniformLocation(renderer->programId, "transform");
		if (renderer->transformUniformLoc == -1)
			log_error("Failed to retrieve transform matrix uniform location!");
		renderer->textureUniformLoc = 
			glGetUniformLocation(renderer->programId, "texture");
		if (renderer->textureUniformLoc == -1)
			log_error("Failed to retrieve texture uniform location!");
		log_info("Completed uniform location checks!");

		initFrameBuffer();

		initSpriteBatch();

		renderer->initialized = true;

		log_info("Successfully initialized renderer!");
	}
}

void initFrameBuffer()
{
	// Framebuffer code.
	MGLFramebufferData *fb = &renderer->framebuffer;

	glGenTextures(1, &fb->framebufferTexture);
	glBindTexture(GL_TEXTURE_2D, fb->framebufferTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, renderer->width, renderer->height,
		0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenRenderbuffers(1, &fb->renderbufferId);
	glBindRenderbuffer(GL_RENDERBUFFER, fb->renderbufferId);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16,
		renderer->width, renderer->height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glGenFramebuffers(1, &fb->framebufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, fb->framebufferId);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, fb->framebufferTexture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_RENDERBUFFER, fb->renderbufferId);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		log_exit("Failed to complete framebuffer!");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	const float fbVerts[8] = {-1, -1,
							1, -1,
							-1, 1,
							1, 1};
	glGenBuffers(1, &fb->frameVboId);
	glBindBuffer(GL_ARRAY_BUFFER, fb->frameVboId);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float),
		fbVerts, GL_STATIC_DRAW);

	fb->framebufferProgramId =
		createProgram("res/fbVertex.glsl", "res/fbFragment.glsl");


	// Find sampler2d uniform location
	GLint fboTexLoc, fboResLoc;
	fboTexLoc = glGetUniformLocation(fb->framebufferProgramId, "fboTexture");
	if (fboTexLoc == -1)
		log_exit("Failed to retrieve post shader uniforms!");
	fb->uniformTexLoc = fboTexLoc;

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(renderer->framebuffer.uniformTexLoc, GL_TEXTURE0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	log_info("Completed framebuffer initialization!");
}


void resizeRenderer(uint w, uint h)
{
	renderer->width = w;
	renderer->height = h;
	glViewport(0, 0, w, h);
	const float pix = (float)TILE_PIXELS;
	//renderer->projMatrix = Mat4_Ortho(0, (float)w / pix, 0, (float)h / pix, -1, 1);
	renderer->projMatrix = Mat4_Ortho(-(float)w / pix / 2, (float)w / pix / 2,
		-(float)h / pix / 2, (float)h / pix / 2, -1, 1);
	renderer->pixelMatrix = Mat4_Ortho(0, w, 0, h, -1, 1);
	// renderer->projMatrix = Mat4_Identity();
	if (renderer->initialized)
	{
		glBindTexture(GL_TEXTURE_2D, renderer->framebuffer.framebufferTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glBindRenderbuffer(GL_RENDERBUFFER, renderer->framebuffer.renderbufferId);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, w, h);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glUseProgram(renderer->framebuffer.framebufferProgramId);
	}
	log_info("Resized window to W:%u, H:%u", w, h);
}

GLuint loadTexture(uint8 *textureData, int w, int h, int bytesPerPixel)
{
	GLuint id;
	GLenum format = GL_RGBA;
	if (bytesPerPixel == 4)
		format = GL_RGBA;
	else if (bytesPerPixel == 3)
		format = GL_RGB;
	else if (bytesPerPixel == 1)
		format = GL_ALPHA;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0,
		format, GL_UNSIGNED_BYTE, textureData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
	return id;
}

void setTextureAtIndex(int index, Texture *texture)
{
	renderer->textureIds[index] =loadTexture(texture->pixels,
		texture->width, texture->height, texture->bytesPerPixel);
}

// TODO: Rewrite function to accept memory buffer instead of filepath.
// TODO: Handle errors more gracefully... switch render modes
GLuint createProgram(const char *vertShaderPath, const char *fragShaderPath)
{
	char *vertCode = loadShaderCode(vertShaderPath);
	if (!vertCode)
		log_exit("Failed to load vertex shader code!");
	char *fragCode = loadShaderCode(fragShaderPath);
	if (!fragCode)
		log_exit("Failed to load fragment shader code!");

	GLuint vertexId = createShader(vertCode, GL_VERTEX_SHADER);
	GLuint fragmentId = createShader(fragCode, GL_FRAGMENT_SHADER);
	if (!vertexId || !fragmentId)
		log_exit("Failed to create shader!");
	GLuint programId = glCreateProgram();
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

GLuint createShader(const char *shaderCode, GLenum shaderType)
{
	GLuint id = 0;
	id = glCreateShader(shaderType);
	if (id == 0)
		log_exit("glCreateShader failed!");
	glShaderSource(id, 1, &shaderCode, 0);
	glCompileShader(id);

	int status = 0;
	glGetShaderiv(id, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
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
