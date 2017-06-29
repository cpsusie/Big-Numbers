#include "pch.h"

// Implementation of MT19937-64:

// See https://en.wikipedia.org/wiki/Mersenne_Twister

#define W 64
#define N ARRAYSIZE(m_mt)
#define M 156
#define R 31
#define A 0xB5026F5AA96619E9ui64
#define U 29
#define D 0x5555555555555555ui64
#define S 17
#define B 0x71D67FFFEDA60000ui64
#define T 37
#define C 0xFFF7EEE000000000ui64
#define L 43
#define F 6364136223846793005ui64

#define MASK_LOWER 0x7FFFFFFF
#define MASK_UPPER (~MASK_LOWER)

void MersenneTwister64::setSeed(INT64 seed) {
  m_mt[0] = seed;
  for(int i = 1; i < N; ++i) {
    m_mt[i] = (F * (m_mt[i-1] ^ (m_mt[i-1] >> (W-2))) + i);
  }
  m_index = N;
}

void MersenneTwister64::twist() {
  for(int i = 0; i < N; ++i) {
    UINT64 x  = (m_mt[i] & MASK_UPPER) + (m_mt[(i+1) % N] & MASK_LOWER);
    UINT64 xA = x >> 1;
    if(x & 0x1) {
      xA ^= A;
    }
    m_mt[i] = m_mt[(i+M) % N] ^ xA;
  }
  m_index = 0;
}

// Obtain a 64-bit random number
UINT64 MersenneTwister64::next64(UINT bits) {
  if(m_index >= N) {
    twist();
  }
  UINT64 y = m_mt[m_index++];
  y ^= (y >> U) & D;
  y ^= (y << S) & B;
  y ^= (y << T) & C;
  y ^= (y >> L);
  return (bits == 64) ? y : (y>>(64-bits));
}

UINT MersenneTwister64::next32(UINT bits) {
  return (UINT)next64(bits);
}
