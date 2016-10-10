#pragma once

class Random {
private:
  __int64 m_seed;

protected:
  UINT next(UINT bits);

public:
  Random();                                    // Initialize with m_seed = 0
  Random(__int64 seed);
  bool    nextBool();
  int     nextInt();
  int     nextInt(int n);                      // return uniform distributed int value between 0 (inclusive) and n (exclusive) n must be positive.
  __int64 nextInt64();
  __int64 nextInt64(__int64 n);                // return uniform distributed int value between 0 (inclusive) and n (exclusive) n must be positive.
  float   nextFloat();                         // return uniform distributed random float  between 0 (inclusive) and 1 (exclusive)
  float   nextFloat(float low, float high);    // return uniform distributed random float  between low (inclusive) and high (exlucisve)
  double  nextDouble();                        // return uniform distributed random double between 0 (inclusive) and 1 (exclusive)
  double  nextDouble(double low, double high); // return uniform distributed random double between low (inclusive) and high (exlucisve)
  double  nextGaussian(double mean, double s); // return normal distributed(mean, s) random number with average = mean, and std. deviation = s

  void setSeed(__int64 seed);
  void randomize();                            // randomize RandomNumber-generator, to start the sequence at a random number. 
                                               // actually setSeed(/getRandomSeed()); see below
};

void randomize();                              // randomize standard random-generator and _standardRandomGenerator (declared below)
__int64 getRandomSeed();                       // gets a random seed to randomize. based upon next GUID

UINT         randInt();                        // use _standardRandomGenerator
UINT         randInt(UINT         n);          // use _standardRandomGenerator. return random int in range [0..n-1]
int          randInt(int from, int to);        // use _standardRandomGenerator. return random int in range [from..to]
__int64      randInt64();                      // use _standardRandomGenerator
__int64      randInt64(unsigned __int64 n);

#ifdef IS64BIT
#define randSizet  randInt64
#else
#define randSizet  randInt
#endif

extern Random _standardRandomGenerator;
