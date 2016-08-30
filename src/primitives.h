#ifndef _PRIMITIVES_H_
#define _PRIMITIVES_H_

#include <math.h>
#include <stdio.h>

union Vec2
{
	struct
	{
		float x, y;
	};
	struct
	{
		float u, v;
	};
	float element[2];
};

union Vec3
{
	struct
	{
		float x, y, z;
	};
	struct
	{
		float u, v, w;
	};
	struct
	{
		float r, g, b;
	};
	float element[3];
};

union Vec4
{
	struct
	{
		float x, y, z, w;
	};
	struct
	{
		float r, g, b, a;
	};
	struct
	{
		Vec3 xyz;
		float unused_w1;
	};
	struct
	{
		Vec3 rgb;
		float unused_a1;
	};
	float element[4];
};

union Mat4
{
	float flat[16];
	float square[4][4];
	struct
	{
		Vec4 col0;
		Vec4 col1;
		Vec4 col2;
		Vec4 col3;
	};
	struct
	{
		float m00;
		float m10;
		float m20;
		float m30;
		float m01;
		float m11;
		float m21;
		float m31;
		float m02;
		float m12;
		float m22;
		float m32;
		float m03;
		float m13;
		float m23;
		float m33;
	};
};

union Rect
{
	struct
	{
		Vec2 pos;
		Vec2 bounds;
	};
	struct
	{
		float x, y, w, h;
	};
	float element[4];
};

inline Rect CreateRect(float x, float y, float w, float h)
{
	Rect rect = {x, y, w, h};
	return rect;
}

inline Vec2 CreateVec(float x, float y)
{
	Vec2 v = {x, y};
	return v;
}

Vec2 Vec2_Normalize(Vec2 v);
float Vec2_Dot(Vec2 v1, Vec2 v2);
Vec2 Vec2_Lerp(Vec2 v1, Vec2 v2, float x);
float Vec2_Dist(Vec2 v1, Vec2 v2);
Vec2 operator+(Vec2 v1, Vec2 v2);
Vec2 operator-(Vec2 v1, Vec2 v2);
Vec2 operator*(Vec2 v, float f);
Vec2 operator-(Vec2 v);

inline Vec3 CreateVec(float x, float y, float z)
{
	Vec3 v = {x, y, z};
	return v;
}
Vec3 Vec3_Normalize(Vec3 v);
float Vec3_Dot(Vec3 v1, Vec3 v2);
Vec3 Vec3_Cross(Vec3 v1, Vec3 v2);
Vec3 Vec3_Lerp(Vec3 v1, Vec3 v2, float x);
Vec3 operator+(Vec3 v1, Vec3 v2);
Vec3 operator-(Vec3 v1, Vec3 v2);

Vec4 Vec4_Normalize(Vec4 v);
float Vec4_Dot(Vec4 v1, Vec4 v2);
Vec4 operator+(Vec4 v1, Vec4 v2);
Vec4 operator-(Vec4 v1, Vec4 v2);

Mat4 operator*(Mat4 a, Mat4 b);
Mat4 Mat4_Ortho(float l, float r, float b, float t, float n, float f);
Mat4 Mat4_PerspectiveFOV(float fov, float aspect, float near, float far);
Mat4 Mat4_Identity();
Mat4 Mat4_Translate(float x, float y, float z);
Mat4 Mat4_Translate(Vec3 direction);
Mat4 Mat4_Scale(float x, float y, float z);
Mat4 Mat4_Scale(Vec3 factor);
bool Mat4_Invert(Mat4 in, Mat4 *out);
Mat4 Mat4_LookAt(Vec3 target, Vec3 eye, Vec3 up);
Mat4 Mat4_FPSCam(Vec3 pos, float rotLR, float rotUD);

#endif