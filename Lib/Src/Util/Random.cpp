#include "pch.h"
#include <Math.h>
#include <Random.h>

static const __int64 multiplier  = (__int64)0x5DEECE66D;
static const __int64 addend      = 0xB;
static const __int64 mask        = ((__int64)1 << 48) - 1;

Random::Random() {
  setSeed(0);
}

Random::Random(__int64 seed) {
  setSeed(seed);
}

void Random::setSeed(__int64 seed) {
  m_seed = (seed ^ multiplier) & mask;
}

void Random::randomize() {
  setSeed(getRandomSeed());
}

UINT Random::next(UINT bits) {
  m_seed = (m_seed * multiplier + addend) & mask;
  return (UINT)(m_seed >> (48 - bits));
}

bool Random::nextBool() {
  return next(1) != 0;
}

int Random::nextInt() {
  return next(32);
}

/* 
 * return the next pseudorandom, uniformly distributed int
 * value between 0 (inclusive) and n (exclusive)
 * from this random number generator's sequence
 * n is the bound on the random number to be returned. Must be positive.
 */
int Random::nextInt(int n) {
  if(n<=0) {
    throwInvalidArgumentException(__TFUNCTION__, _T("n must be positive"));
  }

  if((n & -n) == n) { // i.e., n is a power of 2
    return (int)(((__int64)n * (__int64)next(31)) >> 31);
  }

  int bits, val;
  do { 
    bits = next(31);
    val = bits % n;
  } while(bits - val + (n-1) < 0);
  return val;
}

__int64 Random::nextInt64() {
  return ((__int64)(next(32)) << 31) + next(32);
}

__int64 Random::nextInt64(__int64 n) {
  return nextInt64() % n;
}

float Random::nextFloat() {
  return next(24) / ((float)(1 << 24));
}

float Random::nextFloat(float low, float high) {
  return (high-low)*nextFloat()+low;
}

double Random::nextDouble() {
  __int64 l = (((unsigned __int64)next(26)) << 27) + next(27);
  return l / (double)((__int64)1 << 53);
}

double Random::nextDouble(double low, double high) {
  return (high-low)*nextDouble()+low;
}

double Random::nextGaussian(double mean, double s) { 
  double u1 = nextDouble();
  double u2;
  while((u2 = nextDouble()) == 0); // u2 != 0
  return mean + s * cos(2 * M_PI * u1) * sqrt(-2 * log(u2));
}
