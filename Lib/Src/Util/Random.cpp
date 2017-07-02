#include "pch.h"
#include <Math.h>

UINT64 Random::next64(UINT bits) {
  const UINT n = min(bits,32);
  UINT64 result = next32(n);
  bits -= n;
  if(bits) {
    result <<= bits;
    result |= next32(bits);
  }
  return result;
}

/*
 * return the next pseudorandom, uniformly distributed int
 * value between 0 (inclusive) and n (exclusive)
 * from this random number generator's sequence
 * n is the bound on the random number to be returned. Must be positive.
 */
int Random::nextInt(int n) {
  if(n <= 0) {
    throwInvalidArgumentException(__TFUNCTION__, _T("n must be positive"));
  }

  if((n & -n) == n) { // i.e., n is a power of 2
    return (int)(((INT64)n * (INT64)next32(31)) >> 31);
  }

  int bits, val;
  do {
    bits = next32(31);
    val = bits % n;
  } while(bits - val + (n-1) < 0);
  return val;
}

INT64 Random::nextInt64(INT64 n) {
  if(n <= 0) {
    throwInvalidArgumentException(__TFUNCTION__, _T("n must be positive"));
  }

  if((n & -n) == n) { // i.e., n is a power of 2
    return next64(63) & (n-1);
  }

  INT64 bits, val;
  do {
    bits = next64(63);
    val = bits % n;
  } while(bits - val + (n-1) < 0);
  return val;
}

float Random::nextFloat() {
  return next32(24) / ((float)(1 << 24));
}

float Random::nextFloat(float low, float high) {
  return (high-low)*nextFloat()+low;
}

double Random::nextDouble() {
  INT64 l = (((UINT64)next32(26)) << 27) + next32(27);
  return l / (double)((INT64)1 << 53);
}

double Random::nextDouble(double low, double high) {
  return (high-low)*nextDouble()+low;
}

double Random::nextGaussian(double mean, double s) {
  const double u1 = nextDouble();
  double u2;
  while((u2 = nextDouble()) == 0); // u2 != 0
  return mean + s * cos(2 * M_PI * u1) * sqrt(-2 * log(u2));
}
