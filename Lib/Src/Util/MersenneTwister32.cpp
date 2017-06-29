#include "pch.h"

// Implementation of MT19937-32

// See https://en.wikipedia.org/wiki/Mersenne_Twister

// Assumes W = 32 (omitting this)
#define    N  ARRAYSIZE(m_mt)
#define    M  397
#define    R  31
#define    A  0x9908B0DF
#define    F  1812433253
#define    U  11
// Assumes D = 0xFFFFFFFF (omitting this)

#define    S  7
#define    B  0x9D2C5680
#define    T  15
#define    C  0xEFC60000
#define    L  18
#define    MASK_LOWER  ((1ull << R) - 1)
#define    MASK_UPPER   (1ull << R)

// Re-init with a given seed
void MersenneTwister32::setSeed(UINT64 seed) {
  m_mt[0] = (UINT)seed;
  for(int i = 1; i < N; i++) {
    m_mt[i] = (F * (m_mt[i-1] ^ (m_mt[i-1] >> 30)) + i);
  }
  m_index = N;
}

void MersenneTwister32::twist() {
  for(int i = 0; i < N; i++) {
    UINT x  = (m_mt[i] & MASK_UPPER) + (m_mt[(i+1) % N] & MASK_LOWER);
    UINT xA = x >> 1;
    if(x & 0x1) {
      xA ^= A;
    }
    m_mt[i] = m_mt[(i+M) % N] ^ xA;
  }
  m_index = 0;
}

// Obtain a 32-bit random number
UINT MersenneTwister32::next(UINT bits) {
  if(m_index >= N) {
    twist();
  }
  UINT y = m_mt[m_index++];
  y ^= (y >> U);
  y ^= (y << S) & B;
  y ^= (y << T) & C;
  y ^= (y >> L);
  return (bits == 32) ? y : (y>>(32-bits));
}
