#ifndef _VECTORS_H
#define _VECTORS_H

// @warning the normalize function may not work as expected for all data types

#include "util.h"

#define make_vectorN_functions(_suffix, _type, _dimensions)\
void v##_dimensions##_suffix##_add(const vector##_dimensions##_suffix *src1, const vector##_dimensions##_suffix *src2, vector##_dimensions##_suffix *dst);\
void v##_dimensions##_suffix##_sub(const vector##_dimensions##_suffix *src1, const vector##_dimensions##_suffix *src2, vector##_dimensions##_suffix *dst);\
_type v##_dimensions##_suffix##_dot(const vector##_dimensions##_suffix *src1, const vector##_dimensions##_suffix *src2);\
_type v##_dimensions##_suffix##_mag(const vector##_dimensions##_suffix *src);\
void v##_dimensions##_suffix##_norm(const vector##_dimensions##_suffix *src, vector##_dimensions##_suffix *dst);\
void v##_dimensions##_suffix##_scale(_type scalar, const vector##_dimensions##_suffix *src, vector##_dimensions##_suffix *dst);\
b32 v##_dimensions##_suffix##_cmp(const vector##_dimensions##_suffix *src1, const vector##_dimensions##_suffix *src2);

#define make_vector2(_suffix, _type)\
typedef union vector2##_suffix\
{\
    struct { _type x, y; };\
    struct { _type width, height; };\
    _type arr[2];\
} vector2##_suffix;\
make_vectorN_functions(_suffix, _type, 2);

#define make_vector3(_suffix, _type)\
typedef union vector3##_suffix\
{\
    struct { _type x, y, z; };\
    struct { _type r, g, b; };\
    _type arr[3];\
} vector3##_suffix;\
make_vectorN_functions(_suffix, _type, 3);

#define make_vector4(_suffix, _type)\
typedef union vector4##_suffix\
{\
    struct { _type x, y, z, w; };\
    struct { _type r, g, b, a; };\
    _type arr[4];\
} vector4##_suffix;\
make_vectorN_functions(_suffix, _type, 4);

#define v2(x, y) { x, y }
#define v3(x, y, z) { x, y, z }
#define v4(x, y, z, w) { x, y, z, w }

#define v2_casted(_suffix, _type, x, y) (vector2##_suffix) { (_type) (x), (_type) (y) }
#define v3_casted(_suffix, _type, x, y, z) (vector3##_suffix) { (_type) (x), (_type) (y), (_type) (z) }
#define v4_casted(_suffix, _type, x, y, z, w) (vector4##_suffix) { (_type) (x), (_type) (y), (_type) (z), (_type) (w) }

make_vector2(f, f32);
make_vector3(f, f32);
make_vector4(f, f32);
#define v2f(x, y) v2_casted(f, f32, x, y)
#define v3f(x, y, z) v3_casted(f, f32, x, y, z)
#define v4f(x, y, z, w) v4_casted(f, f32, x, y, z, w)

make_vector2(d, f64);
make_vector3(d, f64);
make_vector4(d, f64);
#define v2d(x, y) v2_casted(d, f64, x, y)
#define v3d(x, y, z) v3_casted(d, f64, x, y, z)
#define v4d(x, y, z, w) v4_casted(d, f64, x, y, z, w)

make_vector2(i, s32);
make_vector3(i, s32);
make_vector4(i, s32);
#define v2i(x, y) v2_casted(i, s32, x, y)
#define v3i(x, y, z) v3_casted(i, s32, x, y, z)
#define v4i(x, y, z, w) v4_casted(i, s32, x, y, z, w)

make_vector2(u, u32);
make_vector3(u, u32);
make_vector4(u, u32);
#define v2u(x, y) v2_casted(u, u32, x, y)
#define v3u(x, y, z) v3_casted(u, u32, x, y, z)
#define v4u(x, y, z, w) v4_casted(u, u32, x, y, z, w)

#undef make_vector2
#undef make_vector3
#undef make_vector4
#undef make_vectorN_functions

// special functions
void v3f_cross(const vector3f *src1, const vector3f *src2, vector3f *dst);
void v3f_mul_mat4(const f32 *mat, const vector3f *src, vector3f *dst);

#endif 