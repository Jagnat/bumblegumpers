#include <stdio.h>
#include <stdint.h>
#include <assert.h>

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int64_t int64;

typedef struct
{
	uint8 id;
	uint8 x, y, w, h;
	int8 xo, yo;
	int8 a;
} GlyphData;

GlyphData data[190];

int err(char *c);

int main (int argc, char **argv)
{
	FILE *src = fopen("questrial.fnt", "rb");
	if (!src)
		err("Can't load file!");

	fseek(src, 0, SEEK_END);
	int64 size = ftell(src);

	// seek to beginning of char data
	fseek(src, 0x4A, SEEK_SET);
	int i = 0;
	while (ftell(src) <= size)
	{
		int temp;
		// fread((void*)(&data[i]), 18, 1, src);
		fread((void*)&temp, 4, 1, src);
		data[i].id = (uint8)temp;

		fread((void*)&temp, 2, 1, src);
		data[i].x = (uint8)temp;
		fread((void*)&temp, 2, 1, src);
		data[i].y = (uint8)temp;
		fread((void*)&temp, 2, 1, src);
		data[i].w = (uint8)temp;
		fread((void*)&temp, 2, 1, src);
		data[i].h = (uint8)temp;

		fread((void*)&temp, 2, 1, src);
		data[i].xo = (int8)temp;
		fread((void*)&temp, 2, 1, src);
		data[i].yo = (int8)temp;
		fread((void*)&temp, 2, 1, src);
		data[i].a = (int8)temp;

		fseek(src, 2, SEEK_CUR);
		i++;
	}

	FILE *dest = fopen("output.cpp", "w");

	// write struct definition;
	fprintf(dest, "\n\nstruct GlyphData\n{\n\tuint8 id, x, y, w, h;\n\tint8 xo, yo, a;\n};\n\n");

	fprintf(dest, "const GlyphData glyphData[189] = \n{");

	for (int i = 0; i < 189; ++i)
	{
		if (i != 0)
			fprintf(dest, ",");
		GlyphData d = data[i];
		fprintf(dest, "\n\t{ %d, %d, %d, %d, %d, %d, %d, %d }",
		 	d.id, d.x, d.y, d.w, d.h, d.xo, d.yo, d.a);
	}
	fprintf(dest, "\n};\n");

	return 0;
}

int err(char *c)
{
	printf("%s\n", c);
	exit(1);
}
