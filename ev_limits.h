#ifndef EV_HEADERS_LIMITS_H
#define EV_HEADERS_LIMITS_H

#include "ev_types.h"

struct Int8Data  { i8  MIN; i8  MAX; };
struct Int16Data { i16 MIN; i16 MAX; };
struct Int32Data { i32 MIN; i32 MAX; };
struct Int64Data { i64 MIN; i64 MAX; };

struct UInt8Data  { u8  MIN; u8  MAX; };
struct UInt16Data { u16 MIN; u16 MAX; };
struct UInt32Data { u32 MIN; u32 MAX; };
struct UInt64Data { u64 MIN; u64 MAX; };

struct Float32Data { f32 MIN; f32 MAX; f32 MIN_POS; f32 EPS; };
struct Float64Data { f64 MIN; f64 MAX; f64 MIN_POS; f64 EPS; };

static const struct Int8Data  Int8 = 
{
  .MIN = -128,
  .MAX =  127
};

static const struct Int16Data Int16 =
{
  .MIN = -32767-1,
  .MAX =  32767
};

static const struct Int32Data Int32 =
{
  .MIN = -2147483647-1,
  .MAX =  2147483647
};

static const struct Int64Data Int64 =
{
  .MIN = -(9223372036854775807LL)-1,
  .MAX =  9223372036854775807LL
};

static const struct UInt8Data  UInt8 =
{
  .MIN = 0,
  .MAX = 255
};

static const struct UInt16Data UInt16 =
{
  .MIN = 0,
  .MAX = 65535
};

static const struct UInt32Data UInt32 =
{
  .MIN = 0,
  .MAX = 4294967295U
};

static const struct UInt64Data UInt64 =
{
  .MIN = 0,
  .MAX = 18446744073709551615ULL
};

static const struct Float32Data Float32 =
{
  .MIN_POS =  1.175494351e-38,
  .MIN     = -3.402823466e+38,
  .MAX     =  3.402823466e+38,
  .EPS     =  1.192093e-07
};

static const struct Float64Data Float64 =
{
  .MIN_POS =  2.2250738585072014e-308,
  .MIN     = -1.7976931348623158e+308,
  .MAX     =  1.7976931348623158e+308,
  .EPS     =  2.2204460492503131e-016
};

#endif // EV_HEADERS_LIMITS_H
