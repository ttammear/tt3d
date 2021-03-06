#include "shared.h"

#include <math.h>


inline IVec2 ivec2(i32 x, i32 y)
{
    IVec2 ret;
    ret.x = x;
    ret.y = y;
    return ret;
}

inline IVec4 ivec4(i32 x, i32 y, i32 z, i32 w)
{
    IVec4 ret;
    ret.x = x;
    ret.y = y;
    ret.z = z;
    ret.w = w;
    return ret;
}

inline Vec2 vec2Zero()
{
    Vec2 vec = {0.f,0.f};
    return vec;
}
inline Vec3 vec3Zero()
{
    Vec3 vec = {0.0f,0.0f,0.0f};
    return vec;
}
inline Vec4 vec4Zero()
{
    Vec4 vec = {0.0f,0.0f,0.0f,0.0f};
    return vec;
}

inline Vec2 vec2(r32 x, r32 y)
{
    Vec2 vec;
    vec.x = x;
    vec.y = y;
    return vec;
}

inline Vec3 vec3(r32 x, r32 y, r32 z)
{
    Vec3 vec;
    vec.x = x;
    vec.y = y;
    vec.z = z;
    return vec;
}

inline Vec4 vec4(r32 x, r32 y, r32 z, r32 w)
{
    Vec4 vec;
    vec.x = x;
    vec.y = y;
    vec.z = z;
    vec.w = w;
    return vec;
}

inline Vec4 vec4FromVec3AndW(Vec3 v, r32 w)
{
    Vec4 vec;
    vec.x = v.x;
    vec.y = v.y;
    vec.z = v.z;
    vec.w = w;
    return vec;
}

inline Vec3 vec3FromVec4(Vec4 v)
{
    Vec3 vec;
    vec.x = v.x;
    vec.y = v.y;
    vec.z = v.z;
    return vec;
}

inline Quaternion quaternion(r32 w, r32 x, r32 y, r32 z)
{
    Quaternion quat;
    quat.w = w;
    quat.x = x;
    quat.y = y;
    quat.z = z;
    return quat;
}

inline void vec2Sub(Vec2 *d, Vec2 *v0, Vec2 *v1) { VEC2_OP(d,v0,-,v1) }
inline void vec3Sub(Vec3 *d, Vec3 *v0, Vec3 *v1) { VEC3_OP(d,v0,-,v1) }
inline void vec4Sub(Vec4 *d, Vec4 *v0, Vec4 *v1) { VEC4_OP(d,v0,-,v1) }

inline void vec2Add(Vec2 *d, Vec2 *v0, Vec2 *v1) { VEC2_OP(d,v0,+,v1) }
inline void vec3Add(Vec3 *d, Vec3 *v0, Vec3 *v1) { VEC3_OP(d,v0,+,v1) }
inline void vec4Add(Vec4 *d, Vec4 *v0, Vec4 *v1) { VEC4_OP(d,v0,+,v1) }

inline void vec2Mul(Vec2 *d, Vec2 *v0, Vec2 *v1) { VEC2_OP(d,v0,*,v1) }
inline void vec3Mul(Vec3 *d, Vec3 *v0, Vec3 *v1) { VEC3_OP(d,v0,*,v1) }
inline void vec4Mul(Vec4 *d, Vec4 *v0, Vec4 *v1) { VEC4_OP(d,v0,*,v1) }

inline void vec3Scale (Vec3 *d, Vec3 *v, r32 s)
{
    d->x = v->x * s;
    d->y = v->y * s;
    d->z = v->z * s;
}

inline void vec4Scale (Vec4 *d, Vec4 *v, r32 s)
{
    d->x = v->x * s;
    d->y = v->y * s;
    d->z = v->z * s;
    d->w = v->w * s;
}

inline r32 clamp01(r32 v)
{
    if(v < 0.0f)
        return 0.0f;
    if(v > 1.0f)
        return 1.0f;
    return v;
}

inline r32 vec3Dot(Vec3 *v0, Vec3 *v1)
{
    return v0->x*v1->x+v0->y*v1->y+v0->z*v1->z;
}

inline r32 vec2Mag(Vec2 *v)
{
    return (r32)sqrt(v->x*v->x+v->y*v->y);
}

inline r32 vec3Mag(Vec3 *v)
{
    return (r32)sqrt(v->x*v->x+v->y*v->y+v->z*v->z);
}

inline r32 vec3Mag2(Vec3 *v)
{
    return (r32)v->x*v->x+v->y*v->y+v->z*v->z;
}

inline Vec3 vec3Normalized(Vec3 *v)
{
    Vec3 ret;
    vec3Scale(&ret, v, 1.0f / vec3Mag(v));
    return ret;
}

inline Vec3 vec3Cross(Vec3 *v0, Vec3 *v1)
{
    Vec3 ret;
    ret.x = v0->y*v1->z-v0->z*v1->y;
    ret.y = v0->z*v1->x-v0->x*v1->z;
    ret.z = v0->x*v1->y-v0->y*v1->x;
    return ret;
}

inline Quaternion quaternionFromAxisAngle(Vec3 a, r32 angle)
{
    Quaternion q;
    q.x = a.x * (float)sinf(angle/2.0f);
    q.y = a.y * (float)sinf(angle/2.0f);
    q.z = a.z * (float)sinf(angle/2.0f);
    q.w = cos(angle/2.0f);
    return q;
}

void quaternionMul(Quaternion *d, Quaternion *q0, Quaternion *q1)
{
    d->w = q0->w * q1->w - q0->x * q1->x - q0->y * q1->y - q0->z * q1->z;  // 1
    d->x = q0->w * q1->x + q0->x * q1->w + q0->y * q1->z - q0->z * q1->y;  // i
    d->y = q0->w * q1->y - q0->x * q1->z + q0->y * q1->w + q0->z * q1->x;  // j
    d->z = q0->w * q1->z + q0->x * q1->y - q0->y * q1->x + q0->z * q1->w;  // k
}

void mat4FromQuaternion(Mat4 *d, Quaternion *quat)
{
    d->m[0] = 1.0f - 2.0f*quat->y*quat->y - 2.0f*quat->z*quat->z;
    d->m[1] = 2.0f*quat->x*quat->y + 2.0f*quat->z*quat->w;
    d->m[2] = 2.0f*quat->x*quat->z - 2.0f*quat->y*quat->w;
    d->m[3] = 0.0f;

    d->m[4] = 2.0f*quat->x*quat->y - 2.0f*quat->z*quat->w;
    d->m[5] = 1.0f - 2.0f*quat->x*quat->x - 2.0f*quat->z*quat->z;
    d->m[6] = 2.0f*quat->y*quat->z + 2.0f*quat->x*quat->w;
    d->m[7] = 0.0f;

    d->m[8] = 2.0f*quat->x*quat->z + 2.0f*quat->y*quat->w;
    d->m[9] = 2.0f*quat->y*quat->z - 2.0f*quat->x*quat->w;
    d->m[10] = 1.0f - 2.0f*quat->x*quat->x - 2.0f*quat->y*quat->y;
    d->m[11] = 0.0f;

    d->m[12] = 0.0f;
    d->m[13] = 0.0f;
    d->m[14] = 0.0f;
    d->m[15] = 1.0f;
}

void mat4Mul(Mat4 *d, Mat4 *m0, Mat4 *m1)
{
    // TODO: is this intermediate step necessary?
    float i11, i12, i13, i14;
    float i21, i22, i23, i24;
    float i31, i32, i33, i34;
    float i41, i42, i43, i44;

    i11 = m0->m11 * m1->m11 + m0->m12 * m1->m21 + m0->m13 * m1->m31 + m0->m14 * m1->m41;
    i12 = m0->m11 * m1->m12 + m0->m12 * m1->m22 + m0->m13 * m1->m32 + m0->m14 * m1->m42;
    i13 = m0->m11 * m1->m13 + m0->m12 * m1->m23 + m0->m13 * m1->m33 + m0->m14 * m1->m43;
    i14 = m0->m11 * m1->m14 + m0->m12 * m1->m24 + m0->m13 * m1->m34 + m0->m14 * m1->m44;
    i21 = m0->m21 * m1->m11 + m0->m22 * m1->m21 + m0->m23 * m1->m31 + m0->m24 * m1->m41;
    i22 = m0->m21 * m1->m12 + m0->m22 * m1->m22 + m0->m23 * m1->m32 + m0->m24 * m1->m42;
    i23 = m0->m21 * m1->m13 + m0->m22 * m1->m23 + m0->m23 * m1->m33 + m0->m24 * m1->m43;
    i24 = m0->m21 * m1->m14 + m0->m22 * m1->m24 + m0->m23 * m1->m34 + m0->m24 * m1->m44;
    i31 = m0->m31 * m1->m11 + m0->m32 * m1->m21 + m0->m33 * m1->m31 + m0->m34 * m1->m41;
    i32 = m0->m31 * m1->m12 + m0->m32 * m1->m22 + m0->m33 * m1->m32 + m0->m34 * m1->m42;
    i33 = m0->m31 * m1->m13 + m0->m32 * m1->m23 + m0->m33 * m1->m33 + m0->m34 * m1->m43;
    i34 = m0->m31 * m1->m14 + m0->m32 * m1->m24 + m0->m33 * m1->m34 + m0->m34 * m1->m44;
    i41 = m0->m41 * m1->m11 + m0->m42 * m1->m21 + m0->m43 * m1->m31 + m0->m44 * m1->m41;
    i42 = m0->m41 * m1->m12 + m0->m42 * m1->m22 + m0->m43 * m1->m32 + m0->m44 * m1->m42;
    i43 = m0->m41 * m1->m13 + m0->m42 * m1->m23 + m0->m43 * m1->m33 + m0->m44 * m1->m43;
    i44 = m0->m41 * m1->m14 + m0->m42 * m1->m24 + m0->m43 * m1->m34 + m0->m44 * m1->m44;

    d->m11 = i11; d->m12 = i12; d->m13 = i13; d->m14 = i14;
    d->m21 = i21; d->m22 = i22; d->m23 = i23; d->m24 = i24;
    d->m31 = i31; d->m32 = i32; d->m33 = i33; d->m34 = i34;
    d->m41 = i41; d->m42 = i42; d->m43 = i43; d->m44 = i44;
}

inline void mat4Vec4Mul(Vec4 *d, Mat4 *m, Vec4 *v)
{
    d->x = v->x*m->m11+v->y*m->m12+v->z*m->m13+v->w*m->m14;
    d->y = v->x*m->m21+v->y*m->m22+v->z*m->m23+v->w*m->m24;
    d->z = v->x*m->m31+v->y*m->m32+v->z*m->m33+v->w*m->m34;
    d->w = v->x*m->m41+v->y*m->m42+v->z*m->m43+v->w*m->m44;
}

Vec4 mat4GetRow(Mat4 *m, int rowIndex)
{
    Vec4 ret;
    switch(rowIndex)
    {
    case 0:
        ret.x = m->m11;
        ret.y = m->m12;
        ret.z = m->m13;
        ret.w = m->m14;
        break;
    case 1:
        ret.x = m->m21;
        ret.y = m->m22;
        ret.z = m->m23;
        ret.w = m->m24;
        break;
    case 2:
        ret.x = m->m31;
        ret.y = m->m32;
        ret.z = m->m33;
        ret.w = m->m34;
        break;
    case 3:
        ret.x = m->m41;
        ret.y = m->m42;
        ret.z = m->m43;
        ret.w = m->m44;
        break;
    }
    return ret;
}

inline Quaternion conjugate(Quaternion q)
{
    return quaternion(-q.w,-q.x,q.y,q.z);
}

inline Plane planeFromVec4(Vec4 *v)
{
    Plane ret;
    ret.a = v->x;
    ret.b = v->y;
    ret.c = v->z;
    ret.d = v->w;
    return ret;
}

inline r32 minf(r32 a, r32 b)
{
    return a < b ? a : b;
}

inline r32 maxf(r32 a, r32 b)
{
    return a > b ? a : b;
}

inline i32 min(i32 a, i32 b)
{
    return a < b ? a : b;
}

inline i32 max(i32 a, i32 b)
{
    return a > b ? a : b;
}

Mat4 projMatrix(float fov, float aspect, float zNear, float zFar)
{
    Mat4 ret;
    const float h = 1.0f/(float)tan(fov*PI_OVER_360);
    float neg_depth = zNear-zFar;

    ret.m[0] = h / aspect;
    ret.m[1] = 0;
    ret.m[2] = 0;
    ret.m[3] = 0;

    ret.m[4] = 0;
    ret.m[5] = h;
    ret.m[6] = 0;
    ret.m[7] = 0;

    ret.m[8] = 0;
    ret.m[9] = 0;
    ret.m[10] = (zFar + zNear)/neg_depth;
    ret.m[11] = -1;

    ret.m[12] = 0;
    ret.m[13] = 0;
    ret.m[14] = 2.0f*(zNear*zFar)/neg_depth;
    ret.m[15] = 0;

    return ret;
}

Mat4 invPerspective(Mat4 *perspectiveMat)
{
    Mat4 ret;
    float a = perspectiveMat->m[0];
    float b = perspectiveMat->m[5];
    float c = perspectiveMat->m[10];
    float d = perspectiveMat->m[14];
    float e = perspectiveMat->m[11];

    for(int i = 0; i < 16; i++)
        ret.m[i]  = 0.0f;

    ret.m[0]  = 1.0f / a;
    ret.m[5]  = 1.0f / b;
    ret.m[11] = 1.0f / d;
    ret.m[14] = 1.0f / e;
    ret.m[15] = -c / (d * e);

    return ret;
}

Mat4 ortho(r32 left, r32 right, r32 bottom, r32 top, r32 near, r32 far)
{
    Mat4 ret;

    ret.m11 = 2.f/(right-left);
    ret.m12 = 0.f;
    ret.m13 = 0.f;
    ret.m14 = -(right+left)/(right-left);
    ret.m21 = 0.f;
    ret.m22 = 2.f/(top-bottom);
    ret.m23 = 0.f;
    ret.m24 = -(top+bottom)/(top-bottom);
    ret.m31 = 0.f;
    ret.m32 = 0.f;
    ret.m33 = -2.f/(far-near);
    ret.m34 = -(far+near)/(far-near);
    ret.m41 = 0.f;
    ret.m42 = 0.f;
    ret.m43 = 0.f;
    ret.m44 = 1.0f;

    return ret;
}

Mat4 scale(Vec3 scale)
{
    Mat4 ret;
    ret.m[0] = scale.x;
    ret.m[1] = 0.0f;
    ret.m[2] = 0.0f;
    ret.m[3] = 0.0f;

    ret.m[4] = 0.0f;
    ret.m[5] = scale.y;
    ret.m[6] = 0.0f;
    ret.m[7] = 0.0f;

    ret.m[8] = 0.0f;
    ret.m[9] = 0.0f;
    ret.m[10] = scale.z;
    ret.m[11] = 0.0f;

    ret.m[12] = 0.0f;
    ret.m[13] = 0.0f;
    ret.m[14] = 0.0f;
    ret.m[15] = 1.0f;
    return ret;
}

Mat4 translate(Vec3 pos)
{
    Mat4 ret;
    ret.m[0] = 1.0f;
    ret.m[1] = 0.0f;
    ret.m[2] = 0.0f;
    ret.m[3] = 0.0f;

    ret.m[4] = 0.0f;
    ret.m[5] = 1.0f;
    ret.m[6] = 0.0f;
    ret.m[7] = 0.0f;

    ret.m[8] = 0.0f;
    ret.m[9] = 0.0f;
    ret.m[10] = 1.0f;
    ret.m[11] = 0.0f;

    ret.m[12] = pos.x;
    ret.m[13] = pos.y;
    ret.m[14] = pos.z;
    ret.m[15] = 1.0f;
    return ret;
}

void getFrustumPlanes(Mat4 *projectionMatrix, Plane *planeArray)
{
    Vec4 planes[6];
    Vec4 rows[4];
    rows[0] = mat4GetRow(projectionMatrix, 0);
    rows[1] = mat4GetRow(projectionMatrix, 1);
    rows[2] = mat4GetRow(projectionMatrix, 2);
    rows[3] = mat4GetRow(projectionMatrix, 3);
    vec4Sub(&planes[0], &rows[3], &rows[0]);
    vec4Add(&planes[1], &rows[3], &rows[0]);
    vec4Sub(&planes[2], &rows[3], &rows[1]);
    vec4Add(&planes[3], &rows[3], &rows[1]);
    vec4Sub(&planes[4], &rows[3], &rows[2]);
    vec4Add(&planes[5], &rows[3], &rows[2]);
    planeArray[0] = planeFromVec4(&planes[0]);
    planeArray[1] = planeFromVec4(&planes[1]);
    planeArray[2] = planeFromVec4(&planes[2]);
    planeArray[3] = planeFromVec4(&planes[3]);
    planeArray[4] = planeFromVec4(&planes[4]);
    planeArray[5] = planeFromVec4(&planes[5]);
}

inline r32 floorf(r32 x)
{
#ifdef USE_SIMD
    __m128 f = _mm_set_ss(x);
    __m128 one = _mm_set_ss(1.0f);

    __m128 t = _mm_cvtepi32_ps(_mm_cvttps_epi32(f));
    __m128 r = _mm_sub_ps(t, _mm_and_ps(_mm_cmplt_ps(f, t), one));

    return _mm_cvtss_f32(r);
#else
    // TODO: implement non SIMD version
    assert(false);
#endif
}

inline r32 powf(r32 x, r32 power)
{
    return (r32)pow(x, power);
}

inline i32 powInt(i32 x, i32 power)
{
    return (i32)lround(pow(x, power));
}

inline r32 absf(r32 x)
{
    return x < 0.0f ? -x : x;
}

inline r32 sqrtf(r32 x)
{
    return sqrt((r64)x);
}

b32 isnanf(r32 x)
{
    return isnan(x);
}
