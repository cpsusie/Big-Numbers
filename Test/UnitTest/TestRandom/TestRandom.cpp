#include "stdafx.h"
#include <limits.h>
#include <Random.h>
#include <Math/MathLib.h>
#include <Math/Int128.h>
#include <Math/PrimeFactors.h>
#include <Math/Statistic.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestRandom {

#include <UnitTestTraits.h>

  static String getDirName(bool is64Bit) {
    return format(_T("c:\\temp\\TestDir\\TestRandom\\%s"), is64Bit?_T("64"):_T("32"));
  }

  static void dumpAllPValues(const TCHAR *method, const TCHAR *generatorName, bool is64Bit, const CompactDoubleArray &a) {
    if(a.size() <= 1) return;
    CompactDoubleArray b(a);
    b.sort(doubleHashCmp);

    StringArray names(Tokenizer(method, _T(":")));
    const String fileName = format(_T("%s\\AllPValues_%s%s.txt")
                                  ,getDirName(is64Bit).cstr()
                                  ,generatorName
                                  ,names.last().cstr());
    FILE        *f    = MKFOPEN(fileName, _T("w"));
    const size_t n    = a.size();
    const double step = 1.0 / (n-1);
    double       x    = 0;
    for(const double *yp = &b.first(), *endp = &b.last(); yp<=endp; x += step) {
      fprintf(f, "%.6lf %10.6le\n", x, *(yp++));
    }
    fclose(f);
  }

  static CompactUIntArray filterFactors(const CompactInt64Array &a) {
    CompactUIntArray result;
    for(size_t i = 0; i < a.size(); i++) {
      const INT64 f = a[i];
      if((f == 1) || (f > 256)) continue;
      result.add((int)f);
    }
    return result;
  }

  template<class T> CompactDoubleArray checkIsUniformDist(const CompactArray<T> &a, bool is64Bit, T from, T to) {
    const size_t n = a.size();
    CompactArray<_int128> b(n);
    for(size_t i = 0; i < n; i++) {
      const T x = a[i];
      verify((from <= x) && (x <= to));
      b.add(x);
      b.last() -= from;
    }
    const _int128           length = (_int128)to - from + 1;
    const CompactUIntArray  allFactors = filterFactors(PrimeFactorArray((length > ULLONG_MAX)?((INT64)256):((INT64)length),227).getAllFactors().sort(int64HashCmp));
    CompactDoubleArray      allPValues;
    for(size_t i = 0; i < allFactors.size(); i++) {
      const UINT bucketCount = allFactors[i];
      CompactDoubleArray observed(bucketCount);
      observed.insert(0, 0.0, bucketCount);
      for(size_t i = 0; i < n; i++) {
        _int128 t128 = b[i];
        t128 *= bucketCount;
        t128 /= length;
        const UINT index = (UINT)t128;
        observed[index]++;
      }
      const double e = (double)n / bucketCount;
      CompactDoubleArray expected(bucketCount);
      expected.insert(0,e,bucketCount);

      const double pvalue = chiSquareGoodnessOfFitTest(observed, expected,false);
      allPValues.add(pvalue);
      if(pvalue < 1e-9) {
        debugLog(_T("%10.17le %10s-%10s  bucketCount:%5u\n"), pvalue, toString(from).cstr(), toString(to).cstr(), bucketCount);
        CompactDoubleArray c(n);
        for(size_t i = 0; i < n; i++) {
          c.add((double)a[i]);
        }
        c.sort(doubleHashCmp);
        for(size_t i = 0; i < n; i++) {
          debugLog(_T("%le\n"), c[i]);
        }
      }
    }
    return allPValues;
  }

#define SAMPLECOUNT 100000

  void _TestNextInt32_INT_MIN_MAX(RandomGenerator &rnd) {
    rnd.randomize();
    INFO(_T("%s gen:%s"), __TFUNCTION__,rnd.getName());
    CompactIntArray samples(SAMPLECOUNT);
    for(int i = 0; i < SAMPLECOUNT; i++) {
      const int x = rnd.nextInt();
      samples.add(x);
    }
    dumpAllPValues(__TFUNCTION__, rnd.getName(), false, checkIsUniformDist(samples, false, INT_MIN, INT_MAX));
  }

  void _TestNextInt64_LLONG_MIN_MAX(RandomGenerator &rnd) {
    rnd.randomize();
    INFO(_T("%s gen:%s"), __TFUNCTION__,rnd.getName());
    CompactInt64Array samples(SAMPLECOUNT);
    for(int i = 0; i < SAMPLECOUNT; i++) {
      const INT64 x = rnd.nextInt64();
      samples.add(x);
    }
    dumpAllPValues(__TFUNCTION__, rnd.getName(), true, checkIsUniformDist(samples, true, LLONG_MIN, LLONG_MAX));
  }

  void _TestNextInt32_0_n(RandomGenerator &rnd) {
    rnd.randomize();
    INFO(_T("%s gen:%s"), __TFUNCTION__,rnd.getName());
    CompactDoubleArray allPValues;
    for(int n = 10; n > 0; n *= 3) {
      CompactIntArray samples(SAMPLECOUNT);
      for(int i = 0; i < SAMPLECOUNT; i++) {
        const int x = rnd.nextInt(n);
        verify((0 <= x) && (x < n));
        samples.add(x);
      }
      allPValues.addAll(checkIsUniformDist(samples, false, 0, n-1));
    }
    dumpAllPValues(__TFUNCTION__, rnd.getName(), false, allPValues);
  }

  void _TestNextInt64_0_n(RandomGenerator &rnd) {
    rnd.randomize();
    CompactDoubleArray allPValues;
    INFO(_T("%s gen:%s"), __TFUNCTION__,rnd.getName());
    for(INT64 n = 10; n > 0; n *= 3) {
      CompactInt64Array samples(SAMPLECOUNT);
      for(int i = 0; i < SAMPLECOUNT; i++) {
        const INT64 x = rnd.nextInt64(n);
        verify((0 <= x) && (x < n));
        samples.add(x);
      }
      allPValues.addAll(checkIsUniformDist(samples, true, (INT64)0, n-1));
    }
    dumpAllPValues(__TFUNCTION__, rnd.getName(), true, allPValues);
  }

  static JavaRandom        javaRnd;
  static MersenneTwister32 m32;
  static MersenneTwister64 m64;

  static RandomGenerator *randomGenerators[] = {
    &javaRnd
   ,&m32
   ,&m64
  };

  TEST_CLASS(TestRandom) {
  public:
    TEST_METHOD(TestNextInt32_INT_MIN_MAX) {
      for(int i = 0; i < ARRAYSIZE(randomGenerators); i++)
        _TestNextInt32_INT_MIN_MAX(*randomGenerators[i]);
    }

    TEST_METHOD(TestNextInt64_LLONG_MIN_MAX) {
      for(int i = 0; i < ARRAYSIZE(randomGenerators); i++)
        _TestNextInt64_LLONG_MIN_MAX(*randomGenerators[i]);
    }

    TEST_METHOD(TestNextInt32_0_n) {
      for(int i = 0; i < ARRAYSIZE(randomGenerators); i++)
        _TestNextInt32_0_n(*randomGenerators[i]);
    }

    TEST_METHOD(TestNextInt64_0_n) {
      for(int i = 0; i < ARRAYSIZE(randomGenerators); i++)
        _TestNextInt64_0_n(*randomGenerators[i]);
    }

    TEST_METHOD(TestRandInt32_0_UINT_MAX) {
      for(int i = 0; i < ARRAYSIZE(randomGenerators); i++) {
        RandomGenerator &newRnd = *randomGenerators[i];
        RandomGenerator &oldRnd = RandomGenerator::setStdGenerator(newRnd);
        randomize();
        CompactUIntArray samples(SAMPLECOUNT);
        for(int i = 0; i < SAMPLECOUNT; i++) {
          const UINT x = randInt();
          samples.add(x);
        }
        dumpAllPValues(__TFUNCTION__, newRnd.getName(), false, checkIsUniformDist(samples, false, (UINT)0, UINT_MAX));
        RandomGenerator::setStdGenerator(oldRnd);
      }
    }

    TEST_METHOD(TestRandInt64_0_UINT64_MAX) {
      for(int i = 0; i < ARRAYSIZE(randomGenerators); i++) {
        RandomGenerator &newRnd = *randomGenerators[i];
        RandomGenerator &oldRnd = RandomGenerator::setStdGenerator(newRnd);
        randomize();
        CompactUInt64Array samples(SAMPLECOUNT);
        for(int i = 0; i < SAMPLECOUNT; i++) {
          const UINT64 x = randInt64();
          samples.add(x);
        }
        dumpAllPValues(__TFUNCTION__, newRnd.getName(), true, checkIsUniformDist(samples, true, (UINT64)0, ULLONG_MAX));
        RandomGenerator::setStdGenerator(oldRnd);
      }
    }

    TEST_METHOD(TestRandInt32_0_n) {
      for(int i = 0; i < ARRAYSIZE(randomGenerators); i++) {
        RandomGenerator &newRnd = *randomGenerators[i];
        RandomGenerator &oldRnd = RandomGenerator::setStdGenerator(newRnd);
        randomize();
        CompactDoubleArray allPValues;
        for(INT64 n = 10; n <= UINT_MAX; n = n * 3 + 1) {
          CompactUIntArray samples(SAMPLECOUNT);
          for(int i = 0; i < SAMPLECOUNT; i++) {
            const UINT x = randInt((UINT)n);
            samples.add(x);
          }
          allPValues.addAll(checkIsUniformDist(samples, false, (UINT)0, (UINT)n-1));
        }
        dumpAllPValues(__TFUNCTION__, newRnd.getName(), false, allPValues);
        RandomGenerator::setStdGenerator(oldRnd);
      }
    }

    TEST_METHOD(TestRandInt64_0_n) {
      for(int i = 0; i < ARRAYSIZE(randomGenerators); i++) {
        RandomGenerator &newRnd = *randomGenerators[i];
        RandomGenerator &oldRnd = RandomGenerator::setStdGenerator(newRnd);
        randomize();
        CompactDoubleArray allPValues;
        for(UINT64 n = 10; n <= LLONG_MAX; n = n * 3 + 1) {
          CompactUInt64Array samples(SAMPLECOUNT);
          for(int i = 0; i < SAMPLECOUNT; i++) {
            const UINT64 x = randInt64(n);
            samples.add(x);
          }
          allPValues.addAll(checkIsUniformDist(samples, true, (UINT64)0, n-1));
        }
        dumpAllPValues(__TFUNCTION__, newRnd.getName(), true, allPValues);
        RandomGenerator::setStdGenerator(oldRnd);
      }
    }

    TEST_METHOD(TestRandInt32_from_to) {
      for(int i = 0; i < ARRAYSIZE(randomGenerators); i++) {
        RandomGenerator &newRnd = *randomGenerators[i];
        RandomGenerator &oldRnd = RandomGenerator::setStdGenerator(newRnd);
        randomize();
        CompactDoubleArray allPValues;
        for(int from = 1; from < INT_MAX/3; from *= 3) {
          for(int to = from + 10; to > 0; to = to*3 + 1) {
            CompactIntArray samples(SAMPLECOUNT);
            for(int i = 0; i < SAMPLECOUNT; i++) {
              const int x = randInt(from, to);
              verify((from <= x) && (x <= to));
              samples.add(x);
            }
            allPValues.addAll(checkIsUniformDist(samples, false, from, to));
          }
        }
        dumpAllPValues(__TFUNCTION__, newRnd.getName(), false, allPValues);
        RandomGenerator::setStdGenerator(oldRnd);
      }
    }

    TEST_METHOD(TestRandInt64_from_to) {
      for(int i = 0; i < ARRAYSIZE(randomGenerators); i++) {
        RandomGenerator &newRnd = *randomGenerators[i];
        RandomGenerator &oldRnd = RandomGenerator::setStdGenerator(newRnd);
        randomize();
        CompactDoubleArray allPValues;
        for(INT64 from = 1; from < LLONG_MAX/3; from *= 3) {
          for(INT64 to = from + 10; to > 0; to = to*3 + 1) {
            CompactInt64Array samples(SAMPLECOUNT);
            for(int i = 0; i < SAMPLECOUNT; i++) {
              const INT64 x = randInt64(from, to);
              verify((from <= x) && (x <= to));
              samples.add(x);
            }
            allPValues.addAll(checkIsUniformDist(samples, true, from, to));
          }
        }
        dumpAllPValues(__TFUNCTION__, newRnd.getName(), true, allPValues);
        RandomGenerator::setStdGenerator(oldRnd);
      }
    }
  };
}
