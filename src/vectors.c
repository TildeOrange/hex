#include <math.h>
#include "camera.h" // for mat4
#include "vectors.h"

#define make_vectorN_functions(_suffix, _type, _dimensions)\
void v##_dimensions##_suffix##_add(const vector##_dimensions##_suffix *src1, const vector##_dimensions##_suffix *src2, vector##_dimensions##_suffix *dst)\
{\
    for (int i = 0; i < _dimensions; i++)\
        dst->arr[i] = src1->arr[i] + src2->arr[i];\
}\
void v##_dimensions##_suffix##_sub(const vector##_dimensions##_suffix *src1, const vector##_dimensions##_suffix *src2, vector##_dimensions##_suffix *dst)\
{\
    for (int i = 0; i < _dimensions; i++)\
        dst->arr[i] = src1->arr[i] - src2->arr[i];\
}\
_type v##_dimensions##_suffix##_dot(const vector##_dimensions##_suffix *src1, const vector##_dimensions##_suffix *src2)\
{\
    _type res = (_type) 0;\
    for (int i = 0; i < _dimensions; i++)\
        res += src1->arr[i] * src2->arr[i];\
    return res;\
}\
_type v##_dimensions##_suffix##_mag(const vector##_dimensions##_suffix *src)\
{\
    _type res = (_type) 0;\
    for (int i = 0; i < _dimensions; i++)\
        res += src->arr[i] * src->arr[i];\
    return (_type) sqrt(res);\
}\
void v##_dimensions##_suffix##_norm(const vector##_dimensions##_suffix *src, vector##_dimensions##_suffix *dst)\
{\
    _type len = (_type) 0;\
    for (int i = 0; i < _dimensions; i++)\
        len += src->arr[i] * src->arr[i];\
    len = (_type) 1 / (_type) sqrt(len);\
    for (int i = 0; i < _dimensions; i++)\
        dst->arr[i] = len * src->arr[i];\
}\
void v##_dimensions##_suffix##_scale(_type scalar, const vector##_dimensions##_suffix *src, vector##_dimensions##_suffix *dst)\
{\
    for (int i = 0; i < _dimensions; i++)\
        dst->arr[i] = scalar * src->arr[i];\
}\
b32 v##_dimensions##_suffix##_cmp(const vector##_dimensions##_suffix *src1, const vector##_dimensions##_suffix *src2)\
{\
    b32 res = true;\
    for (int i = 0; i < _dimensions; i++)\
        res &= (src1->arr[i] == src2->arr[i]);\
    return res;\
}

make_vectorN_functions(f, f32, 2);
make_vectorN_functions(f, f32, 3);
make_vectorN_functions(f, f32, 4);
make_vectorN_functions(d, f64, 2);
make_vectorN_functions(d, f64, 3);
make_vectorN_functions(d, f64, 4);
make_vectorN_functions(i, s32, 2);
make_vectorN_functions(i, s32, 3);
make_vectorN_functions(i, s32, 4);
make_vectorN_functions(u, u32, 2);
make_vectorN_functions(u, u32, 3);
make_vectorN_functions(u, u32, 4);

// special functions
void v3f_cross(const vector3f *src1, const vector3f *src2, vector3f *dst)
{
    dst->x = src1->y * src2->z - src1->z * src2->y;
    dst->y = src1->z * src2->x - src1->x * src2->z;
    dst->z = src1->x * src2->y - src1->y * src2->x;
}

void v3f_mul_mat4(const f32 *mat, const vector3f *src, vector3f *dst)
{
    dst->x = index_matrix(mat, 0, 0) * src->x + index_matrix(mat, 0, 1) * src->y + index_matrix(mat, 0, 2) * src->z + index_matrix(mat, 0, 3);
    dst->y = index_matrix(mat, 1, 0) * src->x + index_matrix(mat, 1, 1) * src->y + index_matrix(mat, 1, 2) * src->z + index_matrix(mat, 1, 3);
    dst->z = index_matrix(mat, 2, 0) * src->x + index_matrix(mat, 2, 1) * src->y + index_matrix(mat, 2, 2) * src->z + index_matrix(mat, 2, 3);
}