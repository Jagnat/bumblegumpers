#include "primitives.h"

Vec2 Vec2_Normalize(Vec2 v)
{
	float l = sqrt(v.x * v.x + v.y * v.y);
	if (l == 1)
		return v;
	Vec2 r = {0};
	r.x = v.x / l;
	r.y = v.y / l;
	return r;
}

float Vec2_Dot(Vec2 v1, Vec2 v2)
{
	return v1.x * v2.x + v1.y * v2.y;
}

Vec2 Vec2_Lerp(Vec2 v1, Vec2 v2, float x)
{
	Vec2 r;
	r.x = lerp(v1.x, v2.x, x);
	r.y = lerp(v1.y, v2.y, x);
	return r;
}

float Vec2_DistSq(Vec2 v1, Vec2 v2)
{
	return (v1.x - v2.x) * (v1.x - v2.x) + (v1.y - v2.y) * (v1.y - v2.y);
}

float Vec2_Dist(Vec2 v1, Vec2 v2)
{
	return sqrt(Vec2_DistSq(v1, v2));
}

Vec2 operator+(Vec2 v1, Vec2 v2)
{
	Vec2 r = {0};
	r.x = v1.x + v2.x;
	r.y = v1.y + v2.y;
	return r;
}

Vec2 operator-(Vec2 v1, Vec2 v2)
{
	Vec2 r = {0};
	r.x = v1.x - v2.x;
	r.y = v1.y - v2.y;
	return r;
}

Vec2 operator*(Vec2 v, float f)
{
	return CreateVec(v.x * f, v.y * f);
}

Vec2 operator-(Vec2 v)
{
	return CreateVec(-v.x, -v.y);
}

Vec3 Vec3_Normalize(Vec3 v)
{
	float l = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	if (l == 1)
		return v;
	if (l == 0)
		return v;
	Vec3 r = {0};
	r.x = v.x / l;
	r.y = v.y / l;
	r.z = v.z / l;
	return r;
}

float Vec3_Dot(Vec3 v1, Vec3 v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

Vec3 Vec3_Cross(Vec3 v1, Vec3 v2)
{
	Vec3 r = {0};
	r.x = v1.y * v2.z - v1.z * v2.y;
	r.y = v1.z * v2.x - v1.x * v2.z;
	r.z = v1.x * v2.y - v1.y * v2.x;
	return r;
}

Vec3 Vec3_Lerp(Vec3 v1, Vec3 v2, float x)
{
	Vec3 r;
	r.x = lerp(v1.x, v2.x, x);
	r.y = lerp(v1.y, v2.y, x);
	r.z = lerp(v1.z, v2.z, x);
	return r;
}

Vec3 operator+(Vec3 v1, Vec3 v2)
{
	Vec3 r = {0};
	r.x = v1.x + v2.x;
	r.y = v1.y + v2.y;
	r.z = v1.z + v2.z;
	return r;
}

Vec3 operator-(Vec3 v1, Vec3 v2)
{
	Vec3 r = {0};
	r.x = v1.x - v2.x;
	r.y = v1.y - v2.y;
	r.z = v1.z - v2.z;
	return r;
}

Vec4 Vec4_Normalize(Vec4 v)
{
	float l = sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
	if (l == 1)
		return v;
	Vec4 r = {0};
	r.x = v.x / l;
	r.y = v.y / l;
	r.z = v.z / l;
	r.w = v.w / l;
	return r;
}

float Vec4_Dot(Vec4 v1, Vec4 v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
}

Vec4 operator+(Vec4 v1, Vec4 v2)
{
	Vec4 r = {0};
	r.x = v1.x + v2.x;
	r.y = v1.y + v2.y;
	r.z = v1.z + v2.z;
	r.w = v1.w + v2.w;
	return r;
}

Vec4 operator-(Vec4 v1, Vec4 v2)
{
	Vec4 r = {0};
	r.x = v1.x - v2.x;
	r.y = v1.y - v2.y;
	r.z = v1.z - v2.z;
	r.w = v1.w - v2.w;
	return r;
}

Vec4 Vec4_MMult(Vec4 v, Mat4 m)
{
	Vec4 r;
	r.x = Vec4_Dot(v, m.col0);
	r.y = Vec4_Dot(v, m.col1);
	r.z = Vec4_Dot(v, m.col2);
	r.w = Vec4_Dot(v, m.col3);
	return r;
}

Mat4 operator*(Mat4 a, Mat4 b)
{
	Mat4 r = {0};
	Vec4 row0 = {a.m00, a.m01, a.m02, a.m03};
	Vec4 row1 = {a.m10, a.m11, a.m12, a.m13};
	Vec4 row2 = {a.m20, a.m21, a.m22, a.m23};
	Vec4 row3 = {a.m30, a.m31, a.m32, a.m33};
	r.m00 = Vec4_Dot(row0, b.col0);
	r.m01 = Vec4_Dot(row0, b.col1);
	r.m02 = Vec4_Dot(row0, b.col2);
	r.m03 = Vec4_Dot(row0, b.col3);

	r.m10 = Vec4_Dot(row1, b.col0);
	r.m11 = Vec4_Dot(row1, b.col1);
	r.m12 = Vec4_Dot(row1, b.col2);
	r.m13 = Vec4_Dot(row1, b.col3);

	r.m20 = Vec4_Dot(row2, b.col0);
	r.m21 = Vec4_Dot(row2, b.col1);
	r.m22 = Vec4_Dot(row2, b.col2);
	r.m23 = Vec4_Dot(row2, b.col3);

	r.m30 = Vec4_Dot(row3, b.col0);
	r.m31 = Vec4_Dot(row3, b.col1);
	r.m32 = Vec4_Dot(row3, b.col2);
	r.m33 = Vec4_Dot(row3, b.col3);
	return r;
}

Mat4 Mat4_Ortho(float l, float r, float b, float t, float n, float f)
{
	Mat4 m = Mat4_Identity();
	m.m00 = 2 / (r - l);
	m.m11 = 2 / (t - b);
	m.m22 = -2 / (f - n);

	m.m03 = -((r + l)/(r - l));

	m.m13 = -((t + b)/(t - b));

	m.m23 = -((f + n)/(f - n));
	m.m33 = 1;
	return m;
}

Mat4 Mat4_PerspectiveFOV(float fov, float aspect, float n, float f)
{
	float tanfovover2 = tan(fov / 2.0f);
	Mat4 m = {};
	m.m00 = 1.0f / (aspect * tanfovover2);
	m.m11 = 1.0f / tanfovover2;
	m.m22 = - (f + n) / (f - n);
	m.m23 = - (2.0f * f * n) / (f - n);
	m.m32 = -1;
	return m;
}

Mat4 Mat4_Identity()
{
	Mat4 m = {0};
	m.m00 = 1;
	m.m11 = 1;
	m.m22 = 1;
	m.m33 = 1;
	return m;
}

Mat4 Mat4_Translate(float x, float y, float z)
{
	Mat4 r = Mat4_Identity();
	r.m03 = x;
	r.m13 = y;	r.m23 = z;
	return r;
}

Mat4 Mat4_Translate(Vec3 direction)
{
	return Mat4_Translate(direction.x, direction.y, direction.z);
}

Mat4 Mat4_Scale(float x, float y, float z)
{
	Mat4 r = {0};
	r.m00 = x;
	r.m11 = y;
	r.m22 = z;
	r.m33 = 1;
	return r;
}

Mat4 Mat4_Scale(Vec3 factor)
{
	return Mat4_Scale(factor.x, factor.y, factor.z);
}

bool Mat4_Invert(Mat4 in, Mat4 *out)
{
	float *m = in.flat;
	float inv[16], det;
	int i;

	inv[ 0] =  m[5] * m[10] * m[15] - m[5] * m[11] * m[14] - m[9] * m[6] * m[15] + m[9] * m[7] * m[14] + m[13] * m[6] * m[11] - m[13] * m[7] * m[10];
	inv[ 4] = -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] + m[8] * m[6] * m[15] - m[8] * m[7] * m[14] - m[12] * m[6] * m[11] + m[12] * m[7] * m[10];
	inv[ 8] =  m[4] * m[ 9] * m[15] - m[4] * m[11] * m[13] - m[8] * m[5] * m[15] + m[8] * m[7] * m[13] + m[12] * m[5] * m[11] - m[12] * m[7] * m[ 9];
	inv[12] = -m[4] * m[ 9] * m[14] + m[4] * m[10] * m[13] + m[8] * m[5] * m[14] - m[8] * m[6] * m[13] - m[12] * m[5] * m[10] + m[12] * m[6] * m[ 9];
	inv[ 1] = -m[1] * m[10] * m[15] + m[1] * m[11] * m[14] + m[9] * m[2] * m[15] - m[9] * m[3] * m[14] - m[13] * m[2] * m[11] + m[13] * m[3] * m[10];
	inv[ 5] =  m[0] * m[10] * m[15] - m[0] * m[11] * m[14] - m[8] * m[2] * m[15] + m[8] * m[3] * m[14] + m[12] * m[2] * m[11] - m[12] * m[3] * m[10];
	inv[ 9] = -m[0] * m[ 9] * m[15] + m[0] * m[11] * m[13] + m[8] * m[1] * m[15] - m[8] * m[3] * m[13] - m[12] * m[1] * m[11] + m[12] * m[3] * m[ 9];
	inv[13] =  m[0] * m[ 9] * m[14] - m[0] * m[10] * m[13] - m[8] * m[1] * m[14] + m[8] * m[2] * m[13] + m[12] * m[1] * m[10] - m[12] * m[2] * m[ 9];
	inv[ 2] =  m[1] * m[ 6] * m[15] - m[1] * m[ 7] * m[14] - m[5] * m[2] * m[15] + m[5] * m[3] * m[14] + m[13] * m[2] * m[ 7] - m[13] * m[3] * m[ 6];
	inv[ 6] = -m[0] * m[ 6] * m[15] + m[0] * m[ 7] * m[14] + m[4] * m[2] * m[15] - m[4] * m[3] * m[14] - m[12] * m[2] * m[ 7] + m[12] * m[3] * m[ 6];
	inv[10] =  m[0] * m[ 5] * m[15] - m[0] * m[ 7] * m[13] - m[4] * m[1] * m[15] + m[4] * m[3] * m[13] + m[12] * m[1] * m[ 7] - m[12] * m[3] * m[ 5];
	inv[14] = -m[0] * m[ 5] * m[14] + m[0] * m[ 6] * m[13] + m[4] * m[1] * m[14] - m[4] * m[2] * m[13] - m[12] * m[1] * m[ 6] + m[12] * m[2] * m[ 5];
	inv[ 3] = -m[1] * m[ 6] * m[11] + m[1] * m[ 7] * m[10] + m[5] * m[2] * m[11] - m[5] * m[3] * m[10] - m[ 9] * m[2] * m[ 7] + m[ 9] * m[3] * m[ 6];
	inv[ 7] =  m[0] * m[ 6] * m[11] - m[0] * m[ 7] * m[10] - m[4] * m[2] * m[11] + m[4] * m[3] * m[10] + m[ 8] * m[2] * m[ 7] - m[ 8] * m[3] * m[ 6];
	inv[11] = -m[0] * m[ 5] * m[11] + m[0] * m[ 7] * m[ 9] + m[4] * m[1] * m[11] - m[4] * m[3] * m[ 9] - m[ 8] * m[1] * m[ 7] + m[ 8] * m[3] * m[ 5];
	inv[15] =  m[0] * m[ 5] * m[10] - m[0] * m[ 6] * m[ 9] - m[4] * m[1] * m[10] + m[4] * m[2] * m[ 9] + m[ 8] * m[1] * m[ 6] - m[ 8] * m[2] * m[ 5];

	det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

	if(det == 0)
		return false;

	det = 1.f / det;

	for(i = 0; i < 16; i++)
		out->flat[i] = inv[i] * det;

	return true;
}

static Mat4 construct_x_y_z(Vec3 x, Vec3 y, Vec3 z, Vec3 eye)
{
	Mat4 m = Mat4_Identity();
	m.m00 = x.x;
	m.m01 = x.y;
	m.m02 = x.z;

	m.m10 = y.x;
	m.m11 = y.y;
	m.m12 = y.z;

	m.m20 = z.x;
	m.m21 = z.y;
	m.m22 = z.z;

	m.m03 = -Vec3_Dot(eye, x);
	m.m13 = -Vec3_Dot(eye, y);
	m.m23 = -Vec3_Dot(eye, z);
	return m;
}

Mat4 Mat4_LookAt(Vec3 target, Vec3 eye, Vec3 up)
{
	Vec3 z = Vec3_Normalize((eye - target));
	Vec3 x = Vec3_Normalize(Vec3_Cross(up, z));
	Vec3 y = Vec3_Normalize(Vec3_Cross(z, x));

	return construct_x_y_z(x, y, z, eye);
}

Mat4 Mat4_FPSCam(Vec3 eye, float rotLR, float rotUD)
{
	float cosPitch = cos(rotUD);
	float sinPitch = sin(rotUD);
	float cosYaw = cos(rotLR);
	float sinYaw = sin(rotLR);

	Vec3 x = {cosYaw, 0, -sinYaw};
	Vec3 y = {sinYaw * sinPitch, cosPitch, cosYaw * sinPitch};
	Vec3 z = {sinYaw * cosPitch, -sinPitch, cosPitch * cosYaw};

	return construct_x_y_z(x, y, z, eye);
}
