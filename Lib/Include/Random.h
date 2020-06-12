#pragma once

#include "BasicIncludes.h"

class RandomGenerator {
private:
protected:
  // bits <= 32
  virtual UINT   next32(UINT bits) = 0;
  // Default implementation of next64. Returns a concatenation of bits from 2 consequtive 32-bit int
  // bits <= 64
  virtual UINT64 next64(UINT bits);
public:

  static RandomGenerator *s_stdGenerator;
  static inline RandomGenerator &setStdGenerator(RandomGenerator &rnd) {
    RandomGenerator &old = *s_stdGenerator;
    s_stdGenerator = &rnd;
    return old;
  }

  virtual void setSeed(INT64 seed) = 0;
  virtual const TCHAR *getName() const = 0;

  inline bool nextBool() {
    return next32(1) != 0;
  }
  // return uniform distributed int value between [_I32_MIN; _I32_MAX], both inclusive.
  inline int nextInt() {
    return next32(32);
  }
  // return uniform distributed INT64 value between [_I64_MIN; _I64_MAX], both inclusive.
  inline INT64 nextInt64() {
    return next64(64);
  }
  // return uniform distributed int value between 0 (inclusive) and n (exclusive) n must be positive.
  int     nextInt(int n);
  // return uniform distributed int value between 0 (inclusive) and n (exclusive) n must be positive.
  INT64   nextInt64(INT64 n);
  // return uniform distributed random float  between 0 (inclusive) and 1 (exclusive)
  float   nextFloat();
  // return uniform distributed random float  between low (inclusive) and high (exlucisve)
  float   nextFloat(float low, float high);
  // return uniform distributed random double between 0 (inclusive) and 1 (exclusive)
  double  nextDouble();
  // return uniform distributed random double between low (inclusive) and high (exlucisve)
  double  nextDouble(double low, double high);
  // return normal distributed(mean, s) random number with average = mean, and std. deviation = s
  double  nextGaussian(double mean, double s);
  // randomize RandomNumber-generator, to start the sequence at a random number.
  // setSeed(getRandomSeed());
  inline void randomize() {
    setSeed(getRandomSeed());
  }
  // gets a random seed to randomize. based upon next GUID
  static INT64 getRandomSeed();
};

class JavaRandom : public RandomGenerator {
private:
  INT64 m_seed;
protected:
  UINT next32(UINT bits);
public:
  inline JavaRandom(INT64 seed = 0) {
    setSeed(seed);
  }
  void setSeed(INT64 seed);
  const TCHAR *getName() const {
    return _T("Java");
  }
};

// See https://en.wikipedia.org/wiki/Mersenne_Twister
// MT19937 (32-bit RNG)
class MersenneTwister32 : public RandomGenerator {
private:
  UINT  m_mt[624];
  UINT  m_index;
  void twist();
protected:
  UINT next32(UINT bits);
public:
  inline MersenneTwister32(INT64 seed = 0) {
    setSeed(seed);
  }
  void setSeed(INT64 seed);
  const TCHAR *getName() const {
    return _T("MT19937-32");
  }
};

// See https://en.wikipedia.org/wiki/Mersenne_Twister
// MT19937-64 (32-bit RNG)
class MersenneTwister64 : public RandomGenerator {
private:
  UINT64 m_mt[312];
  UINT   m_index;
  void twist();
protected:
  UINT   next32(UINT bits);
  UINT64 next64(UINT bits);
public:
  inline MersenneTwister64(INT64 seed = 0) {
    setSeed(seed);
  }
  void setSeed(INT64 seed);
  const TCHAR *getName() const {
    return _T("MT19937-64");
  }
};

// By Donald E. Knuth
class MMIXRandom : public RandomGenerator {
private:
  UINT64 m_seed;
protected:
  UINT next32(UINT bits) {
    m_seed = m_seed * 6364136223846793005i64 + 1442695040888963407i64;
    return (UINT)(m_seed >> (64 - bits));
  }
public:
  inline MMIXRandom(INT64 seed = 0) {
    setSeed(seed);
  }
  void setSeed(INT64 seed) {
    m_seed = seed;
  }
  const TCHAR *getName() const {
    return _T("MMIX");
  }
};

// randomize standard random-generator and RandomGenerator::s_stdGenerator
void randomize();

// return uniform distributed int value between [0; UINT32_MAX], both inclusive.
inline UINT   randInt(RandomGenerator &rnd = *RandomGenerator::s_stdGenerator) {
  return rnd.nextInt();
}
// return random int in range [0..n-1]
inline UINT   randInt(UINT n, RandomGenerator &rnd = *RandomGenerator::s_stdGenerator) {
  return randInt(rnd) % n;
}

// return random int in range[from..to], both  inclusive.
inline int    randInt(int from, int to, RandomGenerator &rnd = *RandomGenerator::s_stdGenerator) {
  return randInt(to-from+1, rnd) + from;
}

// return uniform distributed UINT64 value between [0;_UI64_MAX], both inclusive.
inline UINT64 randInt64(RandomGenerator &rnd = *RandomGenerator::s_stdGenerator) {
  return rnd.nextInt64();
}
// return random UINT64 in range [0..n-1]
inline UINT64 randInt64(UINT64 n, RandomGenerator &rnd = *RandomGenerator::s_stdGenerator) {
  return randInt64(rnd) % n;
}
// return random INT64 in range[from..to], both  inclusive.
inline INT64  randInt64(INT64 from, INT64 to, RandomGenerator &rnd = *RandomGenerator::s_stdGenerator) {
  if(from > to) {
    throwInvalidArgumentException(__TFUNCTION__, _T("from > to"));
  }
  return randInt64(to-from+1, rnd) + from;
}
// return uniform distributed random float  between 0 (inclusive) and 1 (exclusive)
inline float randFloat(RandomGenerator &rnd = *RandomGenerator::s_stdGenerator) {
  return rnd.nextFloat();
}
// return uniform distributed random float  between low (inclusive) and high (exlucisve)
inline float randFloat(float from, float to, RandomGenerator &rnd = *RandomGenerator::s_stdGenerator) {
  return rnd.nextFloat(from, to);
}
// return uniform distributed random double between 0 (inclusive) and 1 (exclusive)
inline double randDouble(RandomGenerator &rnd = *RandomGenerator::s_stdGenerator) {
  return rnd.nextDouble();
}
// return uniform distributed random double between low (inclusive) and high (exlucisve)
inline double randDouble(double from, double to, RandomGenerator &rnd = *RandomGenerator::s_stdGenerator) {
  return rnd.nextDouble(from, to);
}
// return normal distributed(mean, s) random number with average = mean, and std. deviation = s
inline double randGaussian(double mean, double s, RandomGenerator &rnd = *RandomGenerator::s_stdGenerator) {
  return rnd.nextGaussian(mean, s);
}

#if defined(IS64BIT)
#define randSizet  randInt64
#else
#define randSizet  randInt
#endif
