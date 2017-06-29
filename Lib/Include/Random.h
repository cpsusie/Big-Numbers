#pragma once

class Random {
private:
protected:
  virtual UINT   next32(UINT bits) = 0;
  // Default implementation of next64. Returns a concatenation of bits from 2 consequtive 32-bit int
  virtual UINT64 next64(UINT bits);
public:
  virtual void setSeed(INT64 seed) = 0;
  virtual const TCHAR *getName() const = 0;

  inline bool nextBool() {
    return next32(1) != 0;
  }
  inline int nextInt() {
    return next32(32);
  }
  inline INT64 nextInt64() {
    return next64(64);
  }
  int     nextInt(int n);                      // return uniform distributed int value between 0 (inclusive) and n (exclusive) n must be positive.
  INT64   nextInt64(INT64 n);                  // return uniform distributed int value between 0 (inclusive) and n (exclusive) n must be positive.
  float   nextFloat();                         // return uniform distributed random float  between 0 (inclusive) and 1 (exclusive)
  float   nextFloat(float low, float high);    // return uniform distributed random float  between low (inclusive) and high (exlucisve)
  double  nextDouble();                        // return uniform distributed random double between 0 (inclusive) and 1 (exclusive)
  double  nextDouble(double low, double high); // return uniform distributed random double between low (inclusive) and high (exlucisve)
  double  nextGaussian(double mean, double s); // return normal distributed(mean, s) random number with average = mean, and std. deviation = s

  inline void randomize() {                    // randomize RandomNumber-generator, to start the sequence at a random number. 
    setSeed(getRandomSeed());
  }
                                               // actually setSeed(getRandomSeed()); see below
  static INT64 getRandomSeed();                // gets a random seed to randomize. based upon next GUID
};

class JavaRandom : public Random {
private:
  INT64 m_seed;
protected:
  UINT next32( UINT   bits);
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
class MersenneTwister32 : public Random {
private:
  UINT  m_mt[624];
  UINT  m_index;
  void twist();
protected:
  UINT next32( UINT   bits);
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
class MersenneTwister64 : public Random {
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
class MMIXRandom : public Random {
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
 
void randomize();                              // randomize standard random-generator and _standardRandomGenerator (declared below)

extern Random *_standardRandomGenerator;

inline Random *setStdRandomGenerator(Random *rnd) {
  assert(rnd != NULL);
  Random *old = _standardRandomGenerator;
  _standardRandomGenerator = rnd;
  return old;
}

inline UINT   randInt() {                        // use _standardRandomGenerator
  return _standardRandomGenerator->nextInt();
}
inline UINT   randInt(UINT         n) {          // use _standardRandomGenerator. return random int in range [0..n-1]
  return randInt() % n;
}
inline int    randInt(int from, int to) {        // use _standardRandomGenerator. return random int in range [from..to]
  return randInt(to-from+1) + from;
}

inline UINT64 randInt64() {                      // use _standardRandomGenerator
  return _standardRandomGenerator->nextInt64();
}
inline UINT64 randInt64(UINT64     n) {
  return randInt64() % n;
}
inline INT64  randInt64(INT64 from, INT64 to) {
  return randInt64(to-from+1) + from;
}

inline float randFloat() {
  return _standardRandomGenerator->nextFloat();
}
inline float randFloat(float from, float to) {
  return _standardRandomGenerator->nextFloat(from, to);
}

inline double randDouble() {
  return _standardRandomGenerator->nextDouble();
}
inline double randDouble(double from, double to) {
  return _standardRandomGenerator->nextDouble(from, to);
}

#ifdef IS64BIT
#define randSizet  randInt64
#else
#define randSizet  randInt
#endif
