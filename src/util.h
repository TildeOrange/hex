#ifndef _UTIL_H
#define _UTIL_H

#include <stdint.h>

typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef u8 b8;
typedef u32 b32;

typedef float f32;
typedef double f64;

typedef uintptr_t uptr;

#ifndef true
# define true 1
#endif
#ifndef false
# define false 0
#endif

#include <stdlib.h>
#include <stdio.h>

// Prints the error
#define log_error(fmt, ...) fprintf(stderr, fmt, __VA_ARGS__);

// Prints the error with log_error and aborts
#define log_fatal_error(fmt, ...) { log_error(fmt, __VA_ARGS__); abort(); }

#define array_length(x) (sizeof(x) / sizeof(*(x)))

#define iterate_over_array(_type, _name, _array, _count) for (_type *_name = (_type*) _array; _name < ((_type*) (_array)) + (_count); _name++)

#include <math.h>

#define PI_F 3.14159265359f
#define SQRT_2 1.41421356237f
#define SQRT_3 1.73205080757f
#define SQRT_3_DIV_2 0.86602540378f
#define SQRT_2_DIV_2 0.70710678118f

#define rad(deg) ((f32)(deg) / 180.0f * PI_F)
#define deg(rad) ((f32)(rad) / PI_F * 180.0f)

#define clamp(x, a, b) (((x) < (a)) ? (a) : ((x) > (b)) ? (b) : (x))

#define rgba_hex_to_vec3f(x) \
v3f(\
(f32) ((0xff0000 & (x)) >> 16) / 256.0f,\
(f32) ((0xff00 & (x)) >> 8) / 256.0f,\
(f32) (0xff & (x)) / 256.0f)

#define rgba_hex_to_vec4f(x) \
v4f(\
(f32) ((0xff0000 & (x)) >> 16) / 256.0f,\
(f32) ((0xff00 & (x)) >> 8) / 256.0f,\
(f32) (0xff & (x)) / 256.0f,\
(f32) (0xff000000 & (x) >> 24) / 256.0f)

#endif