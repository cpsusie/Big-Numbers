#pragma once

// Common interface for all random number generators
class RandomGenerator {
public:
  virtual void setSeed(UINT64 seed) = 0;
  virtual UINT next(   UINT bits)   = 0;
  virtual RandomGenerator *clone() const = 0;
};

// See https://en.wikipedia.org/wiki/Mersenne_Twister
// MT19937 (32-bit RNG)
class MersenneTwister32 : public RandomGenerator {
private:
  UINT  m_mt[624];
  UINT  m_index;
  void twist();
public:
  MersenneTwister32() {
    setSeed(7);
  }
  void setSeed(UINT64 seed);
  UINT next(   UINT   bits);
  RandomGenerator *clone() const {
    return new MersenneTwister32(*this);
  }
};

// See https://en.wikipedia.org/wiki/Mersenne_Twister
// MT19937-64 (32-bit RNG)
class MersenneTwister64 : public RandomGenerator {
private:
  UINT64 m_mt[312];
  UINT   m_index;
  void twist();
public:
  MersenneTwister64() {
    setSeed(7);
  }
  void setSeed(UINT64 seed);
  UINT next(   UINT   bits);
  RandomGenerator *clone() const {
    return new MersenneTwister64(*this);
  }
};

// By Donald E. Knuth
class MMIXRandom : public RandomGenerator {
private:
  UINT64 m_seed;
public:
  MMIXRandom() {
    setSeed(23);
  }
  void setSeed(UINT64 seed) {
    m_seed = seed;
  }
  UINT next(UINT bits) {
    m_seed = m_seed * 6364136223846793005i64 + 1442695040888963407i64;
    return (UINT)(m_seed >> (64 - bits));
  }
  RandomGenerator *clone() const {
    return new MMIXRandom(*this);
  }
};
 
class JavaRandom : public RandomGenerator {
private:
  INT64 m_seed;
public:
  JavaRandom() {
    setSeed(0);
  }
  void setSeed(UINT64 seed);
  UINT next(   UINT bits);
  RandomGenerator *clone() const {
    return new JavaRandom(*this);
  }
};

class Random {
private:
  RandomGenerator *m_generator;
  inline void setGenerator(RandomGenerator *g) {
    m_generator = g ? g->clone() : new JavaRandom;
  }
  inline void releaseGenerator() {
    delete m_generator;
  }
protected:
  inline UINT next(UINT bits) {
    return m_generator->next(bits);
  }
  UINT64 next64(UINT bits);
public:
   // if parameter g == NULL, an instance of JavaRandom will be used
   // Initialize with m_seed = 0
  inline Random(     RandomGenerator *g = NULL) {
    setGenerator(g);
    setSeed(0);
  }
  inline Random(INT64 seed, RandomGenerator *g = NULL) {
    setGenerator(g);
    setSeed(seed);
  }
  inline Random(const Random &src) {
    setGenerator(src.m_generator->clone());
  }
  inline Random &operator=(const Random &src) {
    releaseGenerator();
    setGenerator(src.m_generator->clone());
  }
  virtual ~Random() {
    releaseGenerator();
  }
  inline void setSeed(INT64 seed) {
    m_generator->setSeed(seed);
  }

  inline bool nextBool() {
    return next(1) != 0;
  }
  inline int nextInt() {
    return next(32);
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
                                               // actually setSeed(/getRandomSeed()); see below
  static INT64 getRandomSeed();                // gets a random seed to randomize. based upon next GUID
};

void randomize();                              // randomize standard random-generator and _standardRandomGenerator (declared below)

UINT         randInt();                        // use _standardRandomGenerator
UINT         randInt(UINT         n);          // use _standardRandomGenerator. return random int in range [0..n-1]
int          randInt(int from, int to);        // use _standardRandomGenerator. return random int in range [from..to]
UINT64       randInt64();                      // use _standardRandomGenerator
UINT64       randInt64(UINT64     n);
INT64        randInt64(INT64 from, INT64 to);

#ifdef IS64BIT
#define randSizet  randInt64
#else
#define randSizet  randInt
#endif

extern Random _standardRandomGenerator;
