#ifndef EV_HEADERS_HASH_H
#define EV_HEADERS_HASH_H

#include "ev_numeric.h"

/*!
 * \brief MurmurHash3 64-bit version. Returns 64-bit hash instead of 128
 */
u64 ev_hash_murmur3(const void *data, u32 len, u64 seed);

#ifdef EV_HASH_IMPLEMENTATION
#undef EV_HASH_IMPLEMENTATION

//-----------------------------------------------------------------------------
// MurmurHash3 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

// Note - The x86 and x64 versions do _not_ produce the same results, as the
// algorithms are optimized for their respective platforms. You can still
// compile and run any of them on any platform, but your performance with the
// non-native version will be less than optimal.

//-----------------------------------------------------------------------------
// Platform-specific functions and macros

// Microsoft Visual Studio

#if EV_CC_MSVC

#define ROTL64(x,y) _rotl64(x,y)
#define BIG_CONSTANT(x) (x)

// Other compilers

#else // EV_CC_MSVC

static inline u64 rotl64 ( u64 x, i8 r )
{
  return (x << r) | (x >> (64 - r));
}

#define ROTL64(x,y) rotl64(x,y)
#define BIG_CONSTANT(x) (x##LLU)

#endif // EV_CC_MSVC


//-----------------------------------------------------------------------------
// Block read - if your platform needs to do endian-swapping or can only
// handle aligned reads, do the conversion here

EV_FORCEINLINE u64 getblock64 ( const u64 * p, u32 i )
{
  return p[i];
}

//-----------------------------------------------------------------------------
// Finalization mix - force all bits of a hash block to avalanche

EV_FORCEINLINE u64 fmix64 ( u64 k )
{
  k ^= k >> 33;
  k *= BIG_CONSTANT(0xff51afd7ed558ccd);
  k ^= k >> 33;
  k *= BIG_CONSTANT(0xc4ceb9fe1a85ec53);
  k ^= k >> 33;

  return k;
}

//-----------------------------------------------------------------------------

void MurmurHash3_x64_128 ( const void * key, const u32 len,
                           const u32 seed, void * out )
{
  const u8 * data = (const u8*)key;
  const u32 nblocks = len / 16;

  u64 h1 = seed;
  u64 h2 = seed;

  const u64 c1 = BIG_CONSTANT(0x87c37b91114253d5);
  const u64 c2 = BIG_CONSTANT(0x4cf5ad432745937f);

  //----------
  // body

  const u64 * blocks = (const u64 *)(data);

  for(u32 i = 0; i < nblocks; i++)
  {
    u64 k1 = getblock64(blocks,i*2+0);
    u64 k2 = getblock64(blocks,i*2+1);

    k1 *= c1; k1  = ROTL64(k1,31); k1 *= c2; h1 ^= k1;

    h1 = ROTL64(h1,27); h1 += h2; h1 = h1*5+0x52dce729;

    k2 *= c2; k2  = ROTL64(k2,33); k2 *= c1; h2 ^= k2;

    h2 = ROTL64(h2,31); h2 += h1; h2 = h2*5+0x38495ab5;
  }

  //----------
  // tail

  const u8 * tail = (const u8*)(data + nblocks*16);

  u64 k1 = 0;
  u64 k2 = 0;

  switch(len & 15)
  {
  case 15: k2 ^= ((u64)tail[14]) << 48;
  case 14: k2 ^= ((u64)tail[13]) << 40;
  case 13: k2 ^= ((u64)tail[12]) << 32;
  case 12: k2 ^= ((u64)tail[11]) << 24;
  case 11: k2 ^= ((u64)tail[10]) << 16;
  case 10: k2 ^= ((u64)tail[ 9]) << 8;
  case  9: k2 ^= ((u64)tail[ 8]) << 0;
           k2 *= c2; k2  = ROTL64(k2,33); k2 *= c1; h2 ^= k2;

  case  8: k1 ^= ((u64)tail[ 7]) << 56;
  case  7: k1 ^= ((u64)tail[ 6]) << 48;
  case  6: k1 ^= ((u64)tail[ 5]) << 40;
  case  5: k1 ^= ((u64)tail[ 4]) << 32;
  case  4: k1 ^= ((u64)tail[ 3]) << 24;
  case  3: k1 ^= ((u64)tail[ 2]) << 16;
  case  2: k1 ^= ((u64)tail[ 1]) << 8;
  case  1: k1 ^= ((u64)tail[ 0]) << 0;
           k1 *= c1; k1  = ROTL64(k1,31); k1 *= c2; h1 ^= k1;
  };

  //----------
  // finalization

  h1 ^= len; h2 ^= len;

  h1 += h2;
  h2 += h1;

  h1 = fmix64(h1);
  h2 = fmix64(h2);

  h1 += h2;
  h2 += h1;

  ((u64*)out)[0] = h1;
  ((u64*)out)[1] = h2;
}

//-----------------------------------------------------------------------------

u64 ev_hash_murmur3(const void *data, u32 len, u64 seed)
{
  u64 out[2];
  MurmurHash3_x64_128(data, len, (u32)seed, out);
  return *out;
}

#endif // EV_HASH_IMPLEMENTATION

#endif // EV_HEADERS_HASH_H
