#include "stdafx.h"
#include "CppUnitTest.h"
#include <limits.h>
#include <Random.h>
#include <Math/MathFunctions.h>
#include <Math/Double80.h>
#include <Math/Int128.h>
#include <Math/PrimeFactors.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestRandom {

#include <UnitTestTraits.h>

  static String getDirName(bool is64Bit) {
    return format(_T("c:\\temp\\TestRandom\\%s"), is64Bit?_T("64"):_T("32"));
  }

  static void dumpAllPValues(const TCHAR *method, bool is64Bit, const CompactDoubleArray &a) {
    if(a.size() == 0) return;
    StringArray names(Tokenizer(method, _T(":")));
    const String fileName = format(_T("%s\\AllPValues_%s.txt"), getDirName(is64Bit).cstr(), names.last().cstr());
    FILE *f = MKFOPEN(fileName, _T("w"));
    for (size_t i = 0; i < a.size(); i++) {
      _ftprintf(f, _T("%le\n"), a[i]);
    }
    fclose(f);
  }
  
  static CompactUintArray filterFactors(const CompactInt64Array &a) {
    CompactUintArray result;
    for (size_t i = 0; i < a.size(); i++) {
      const INT64 f = a[i];
      if((f == 1) || (f > 256)) continue;
      result.add((int)f);
    }
    return result;
  }

  template<class T> CompactDoubleArray checkIsUniformDist(const T &a, bool is64Bit, INT64 from, INT64 to) {
    const _int128           length = (_int128)to - from + 1;
    const CompactUintArray  allFactors = filterFactors(PrimeFactorArray((length > ULLONG_MAX)?256:length,227).getAllFactors().sort(int64HashCmp));
    CompactDoubleArray      allPValues;
    for(size_t i = 0; i < allFactors.size(); i++) {
      const UINT bucketCount = allFactors[i];
      CompactIntArray observed(bucketCount);
      observed.add(0, (int)0, bucketCount);
      for(size_t i = 0; i < a.size(); i++) {
        const INT64 x = a[i];
        verify((from <= x) && (x <= to));
        _int128 t128 = (_int128)x-from;
        t128 *= bucketCount;
        t128 /= length;
        UINT index = t128;
        observed[index]++;
      }
      const double e = (double)a.size() / bucketCount;
      CompactDoubleArray expected(bucketCount);
      expected.add(0,e,bucketCount);

      double Q = 0;
      for(UINT b = 0; b < bucketCount; b++) {
        Q += sqr(expected[b] - observed[b]) / e;
      }
      const Real P = 1.0 - chiSquaredDistribution(bucketCount-1,Q);
      if(P < 1e-12) {
#ifdef _DEBUG
        Real P1 = 1.0 - chiSquaredDistribution(bucketCount-1,Q);
#endif
        const String fileName = format(_T("%s\\LowPvalue-%I64d-%I64d-%03d.dat")
                                      ,getDirName(is64Bit).cstr()
                                      ,from,to,bucketCount);
        FILE *f = MKFOPEN(fileName,_T("w"));
        for (size_t i = 0; i < observed.size(); i++) {
          _ftprintf(f,_T("%d\n"), observed[i]);
        }
        TCHAR tmpstr[100];
        _ftprintf(f,_T("Length:%s, allFactors:%s\n"), _i128tot(length,tmpstr,10), allFactors.toStringBasicType().cstr());
        _ftprintf(f,_T("Expected:%le, Q:%le, P:%le\n"),e,Q,getDouble(P));
        fclose(f);
      }
      allPValues.add(P);
      if(P <= 0.05) {
        OUTPUT(_T("P=%s, Q=%lf, Interval[%lld,%lld], len:%llu, buckets:%u, factors:%s")
              ,toString(P).cstr(), Q, from,to,length,bucketCount
              ,allFactors.toStringBasicType().cstr()
              );
      }
    }
    return allPValues;
  }

  TEST_CLASS(TestRandom) {
  public:
    TEST_METHOD(TestNextInt32_INT_MIN_MAX) {
      Random rnd;
      rnd.randomize();
      CompactIntArray samples;
      for(int i = 0; i < 100000; i++) {
        const int x = rnd.nextInt();
        samples.add(x);
      }
      dumpAllPValues(__TFUNCTION__, false, checkIsUniformDist(samples, false, INT_MIN, INT_MAX));
    }

    TEST_METHOD(TestNextInt64_LLONG_MIN_MAX) {
      Random rnd;
      rnd.randomize();
      CompactInt64Array samples;
      for(int i = 0; i < 100000; i++) {
        const INT64 x = rnd.nextInt64();
        samples.add(x);
      }
      dumpAllPValues(__TFUNCTION__, true, checkIsUniformDist(samples, true, LLONG_MIN, LLONG_MAX));
    }

    TEST_METHOD(TestNextInt32_0_n) {
      Random rnd;
      rnd.randomize();
      CompactDoubleArray allPValues;
      for(int n = 10; n > 0; n *= 3) {
        CompactIntArray samples;
        for(int i = 0; i < 100000; i++) {
          const int x = rnd.nextInt(n);
          verify((0 <= x) && (x < n));
          samples.add(x);
        }
        allPValues.addAll(checkIsUniformDist(samples, false, 0, n-1));
      }
      dumpAllPValues(__TFUNCTION__, false, allPValues);
    }

    TEST_METHOD(TestNextInt64_0_n) {
      Random rnd;
      rnd.randomize();
      CompactDoubleArray allPValues;
      for(INT64 n = 10; n > 0; n *= 3) {
        CompactInt64Array samples;
        for(int i = 0; i < 100000; i++) {
          const INT64 x = rnd.nextInt64(n);
          verify((0 <= x) && (x < n));
          samples.add(x);
        }
        allPValues.addAll(checkIsUniformDist(samples, true, 0, n-1));
      }
      dumpAllPValues(__TFUNCTION__, true, allPValues);
    }

    TEST_METHOD(TestRandInt32_0_UINT_MAX) {
      randomize();
      CompactUintArray samples;
      for(int i = 0; i < 100000; i++) {
        const UINT x = randInt();
        samples.add(x);
      }
      dumpAllPValues(__TFUNCTION__, false, checkIsUniformDist(samples, false, 0, UINT_MAX));
    }

    TEST_METHOD(TestRandInt64_0_UINT64_MAX) {
      randomize();
      CompactUint64Array samples;
      for(int i = 0; i < 100000; i++) {
        const UINT64 x = randInt64();
        samples.add(x);
      }
      dumpAllPValues(__TFUNCTION__, true, checkIsUniformDist(samples, true, 0, ULLONG_MAX));
    }

    TEST_METHOD(TestRandInt32_0_n) {
      randomize();
      CompactDoubleArray allPValues;
      for(INT64 n = 10; n <= UINT_MAX; n = n * 3 + 1) {
        CompactUintArray samples;
        for(int i = 0; i < 100000; i++) {
          const UINT x = randInt((UINT)n);
          samples.add(x);
        }
        allPValues.addAll(checkIsUniformDist(samples, false, 0, n-1));
      }
      dumpAllPValues(__TFUNCTION__, false, allPValues);
    }

    TEST_METHOD(TestRandInt64_0_n) {
      randomize();
      CompactDoubleArray allPValues;
      for(UINT64 n = 10; n <= LLONG_MAX; n = n * 3 + 1) {
        CompactUint64Array samples;
        for(int i = 0; i < 100000; i++) {
          const UINT64 x = randInt64(n);
          samples.add(x);
        }
        allPValues.addAll(checkIsUniformDist(samples, true, 0, n-1));
      }
      dumpAllPValues(__TFUNCTION__, true, allPValues);
    }

    TEST_METHOD(TestRandInt32_from_to) {
      randomize();
      CompactDoubleArray allPValues;
      for(int from = 1; from < INT_MAX/3; from *= 3) {
        for(int to = from + 10; to > 0; to = to*3 + 1) {
          CompactIntArray samples;
          for(int i = 0; i < 100000; i++) {
            const int x = randInt(from, to);
            verify((from <= x) && (x <= to));
            samples.add(x);
          }
          allPValues.addAll(checkIsUniformDist(samples, false, from, to));
        }
      }
      dumpAllPValues(__TFUNCTION__, false, allPValues);
    }

    TEST_METHOD(TestRandInt64_from_to) {
      randomize();
      CompactDoubleArray allPValues;
      for(INT64 from = 1; from < LLONG_MAX/3; from *= 3) {
        for(INT64 to = from + 10; to > 0; to = to*3 + 1) {
          CompactInt64Array samples;
          for(int i = 0; i < 100000; i++) {
            const INT64 x = randInt64(from, to);
            verify((from <= x) && (x <= to));
            samples.add(x);
          }
          allPValues.addAll(checkIsUniformDist(samples, true, from, to));
        }
      }
      dumpAllPValues(__TFUNCTION__, true, allPValues);
    }
  };
}
