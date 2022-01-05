#ifndef EV_HEADERS_INTERNAL_H
#define EV_HEADERS_INTERNAL_H

typedef signed char   i8;
typedef short int     i16;
typedef int           i32;
typedef long long int i64;

typedef unsigned char          u8;
typedef unsigned short int     u16;
typedef unsigned int           u32;
typedef unsigned long long int u64;

typedef float  f32;
typedef double f64;

typedef _Bool bool;
#define true 1
#define false 0

#define i8_const(v)  (i8){v}
#define i16_const(v) (i16){v}
#define i32_const(v) (i32){v}
#define i64_const(v) (i64){v}

#define u8_const(v)  (u8){v}
#define u16_const(v) (u16){v}
#define u32_const(v) (u32){v}
#define u64_const(v) (u64){v}

#define f32_const(v) (f32){v}
#define f64_const(v) (f64){v}

#endif // EV_HEADERS_INTERNAL_H
