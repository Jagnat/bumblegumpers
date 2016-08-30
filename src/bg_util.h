#pragma once
#ifndef _BG_UTIL_H_
#define _BG_UTIL_H_

#include <stdint.h>

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int64_t int64;

inline int m2to1(int x, int y, int w)
{
	return y * w + x;
}

inline float lerp(float a, float b, float x)
{
	return (1 - x) * a + x * b;
}

#define AABB_UNION union \
	{ \
		struct {float x, y, w, h; }; \
		struct { Vec2 pos, bounds; }; \
		AABB aabb; \
	};

#endif