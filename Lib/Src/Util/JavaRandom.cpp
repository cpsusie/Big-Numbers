#include "pch.h"
#include <Random.h>

// See https://en.wikipedia.org/wiki/Linear_congruential_generator

#define multiplier  (INT64)0x5DEECE66D
#define addend      0xB
#define mask        (((INT64)1 << 48) - 1)

void JavaRandom::setSeed(INT64 seed) {
  m_seed = (seed ^ multiplier) & mask;
}

UINT JavaRandom::next32(UINT bits) {
  m_seed = (m_seed * multiplier + addend) & mask;
  return (UINT)(m_seed >> (48 - bits));
}
