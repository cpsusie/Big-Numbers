#include "stdafx.h"
#include <ProcessTools.h>
#include "TestBigReal.h"
#include "BigRealTestClass.h"

#ifdef _DEBUG

#define INTERACTIVE_SHORTPROD

#ifdef INTERACTIVE_SHORTPROD
static void testShortProd() {
  DigitPool pool(-5);
//  for(;;) {
//    int useReferenceVersion = inputInt(_T("Use reference version (1=reference, 2=debug"));

//    BigReal::setUseShortProdRefenceVersion(useReferenceVersion == 1);

//    float maxError32Ref = getRelativeError32(FLT_MIN,&pool);
//    BigReal::setUseShortProdRefenceVersion(false);
//    float maxError32FPU = getRelativeError32(FLT_MIN,&pool);
//    BigReal::setUseShortProdRefenceVersion(true);

    const FullFormatBigReal x = BigReal(spaceString(14700,'9')); // inputBigReal(pool, _T("Enter x:"));
    const FullFormatBigReal y = BigReal(spaceString(14700,'9')); // inputBigReal(pool, _T("Enter y:"));

    _tprintf(_T("X:%s\nY:%s\n"), x.toString().cstr(), y.toString().cstr());

    FullFormatBigReal p1(&pool), p2(&pool);

    p1 = BigReal::shortProd(x, y, BIGREAL_0, &pool);

    _tprintf(_T("p1:%s\n"), p1.toString().cstr());

    BigReal::setUseShortProdRefenceVersion(false);

    p2 = BigReal::shortProd(x, y, BIGREAL_0, &pool);

    BigReal::setUseShortProdRefenceVersion(true);

    _tprintf(_T("p2:%s\n"), p2.toString().cstr());

    try {
      p1.assertIsValidBigReal();
    } catch(Exception e) {
      _tprintf(_T("p1 failed:%s\n"), e.what());
    }
    try {
      p2.assertIsValidBigReal();
    } catch(Exception e) {
      _tprintf(_T("p2 failed:%s\n"), e.what());
    }
//  }
}

#else

static void testShortProd() {
  DigitPool pool(-5);
  Random rnd;

  for(int i = 0; i < 500000;) {
    if((i++)%10000 == 9999) _tprintf(_T("i:%d\r"), i);
    int lengthX = randInt(4,500);
    int lengthY = randInt(4,500);
    int expoX   = randInt(-500,500);
    int expoY   = randInt(-500,500);
    const FullFormatBigReal x(e(randBigReal(lengthX, &rnd, &pool), expoX));
    const FullFormatBigReal y(e(randBigReal(lengthY, &rnd, &pool), expoY));

    FullFormatBigReal p1(&pool), p2(&pool);

//    printf(_T("x:%50s\n y:%50s\n"), x.toString().cstr(), y.toString().cstr());
//  for(int p = 0; p >= -100; p--) {
//    BigReal f(e(1, -70));
#define F BIGREAL_0

    BigReal::setUseShortProdRefenceVersion(true);
    p1 = BigReal::shortProd(x, y, F, &pool);

    try {
      p1.assertIsValidBigReal();
    } catch(Exception e) {
      printf(_T("p1 failed:%s\n"), e.what());
      continue;
    }

    BigReal::setUseShortProdRefenceVersion(false);
    p2 = BigReal::shortProd(x, y, F, &pool);

    try {
      p2.assertIsValidBigReal();
    } catch(Exception e) {
      printf(_T("p2 failed:%s\n"), e.what());
      continue;
    }

    if(p1 != p2) {
      _tprintf(_T("x:%s\ny:%s\npref:%s\np2:%s\n")
              ,x.toString().cstr()
              ,y.toString().cstr()
              ,p1.toString().cstr()
              ,p2.toString().cstr()
            );
      pause();
    }
//    printf(_T("p:%4d. prod:%s\n"), p, p1.toString().cstr());
  }
/*
//    useNewShortProduct = true;
//    p2 = BigReal::shortProd(x, y, BIGREAL_0, &pool);
//    useNewShortProduct = false;

    printf(_T("x*y (1) = %s\n"), p1.toString().cstr());
    printf(_T("x*y (2) = %s\n"), p2.toString().cstr());

    try {
      p1.assertIsValidBigReal();
    } catch(Exception e) {
      printf(_T("p1 failed:%s\n"), e.what());
    }
    try {
      p2.assertIsValidBigReal();
    } catch(Exception e) {
      printf(_T("p2 failed:%s\n"), e.what());
    }
  }
*/
}
#endif

#endif


static void testRandomBigReal() {
  DigitPool pool(-5);
  MersenneTwister64 rnd;
  for(;;) {
    int count  = inputInt(_T("Enter number of random numbers:"));
    int length = inputInt(_T("Enter length (decimal digits):"));
    const FullFormatBigReal low  = inputBigReal(pool, _T("Enter low :"));
    const FullFormatBigReal high = inputBigReal(pool, _T("Enter high:"));

    String fileName = format(_T("c:\\temp\\random%s-%s-length=%d.dat"), low.toString().cstr(), high.toString().cstr(), length);
    FILE *f = FOPEN(fileName, _T("w"));
    for(int i = 0; i < count; i++) {
      FullFormatBigReal x = randBigReal(low, high, length, &rnd, &pool);
      _ftprintf(f, _T("%s\n"), x.toString().cstr());
    }
    fclose(f);
  }
}

static void testAPCSum() {
  DigitPool pool(-5);
  for(;;) {
    const FullFormatBigReal x = inputBigReal(pool, _T("Enter x:"));
    const FullFormatBigReal y = inputBigReal(pool, _T("Enter y:"));
    _tprintf(_T("Enter bias ('<','>','#'):"));
    char bias = getchar();
    FullFormatBigReal p = BigReal::apcSum(bias, x, y, &pool);

    _tprintf(_T("x:%50s y:%50s\n"), x.toString().cstr(), y.toString().cstr());
    _tprintf(_T("APCSum(>,x,y) = %s\n"), p.toString().cstr());

    try {
      p.assertIsValidBigReal();
    } catch(Exception e) {
      _tprintf(_T("%s\n"), e.what());
    }
  }
}

class SpecialTestClass : public BigRealTestClass {
public:
  void runTest();
};

void SpecialTestClass::runTest() {

  DigitPool::setDumpWhenDestroyed(true);

//  testAPCSum();
//  return;

//  testTruncRound();
//  return;

//  testCopyrTrunc();
//  return;

//  testRandomBigReal();
//  return;

//  testShortProd();
//  return;

//  printf("max splitLength:%u\n", BigReal::getMaxSplitLength());
//  return;

//  measureProduct(true, true, false);
//  return;

//  measureSplitLength();
//  return;

//  measureQuot();
//  return;

//  measureQuotRemainder();
//  return;

//  testGetDecimalDigitCount64();
//  return;

#ifdef HAS_LOOP_DIGITCOUNT
//  measureGetDecimalDigitCount();
//  return;
#endif // HAS_LOOP_DIGITCOUNT

//  testQuotRemainder();
//  return;

//  testMultiThreadedProduct();
//  return;

//  testLnEstimate();
//  return;

//  testFullFormat();
//  return;

//  testGetExpo2();
//  return;

//  testGetDouble();
//  return;

//  testBigRealStream();
//  return;

//  testCopy();
//  return;

//  testCopyAllDigits();
//  return;

//  testCut();
//  return;

//  testFractionate();
//  return;
}

static void usage() {
  _ftprintf(stderr, _T("Usage:TestBigReal [-s] [-p] [-tThreadCount]\n"
                       "   -s: Run special test. Default is standard numbertest.\n"
                       "   -p: Set all threads priority to ABOVE_NORMAL.\n"
                       "   -tThreadcount: run testnumber with the specified number of threads. Default is the number of cores in the CPU\n.")
           );
  exit(-1);
}

int _tmain(int argc, TCHAR **argv) {
  try {
    bool   highPriority = false;
    bool   specialTest  = false;
    int    threadCount  = 0;
    TCHAR *cp;
    for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
      for(cp++; *cp; cp++) {
        switch(*cp) {
        case 'p': highPriority = true;
                  continue;
        case 's': specialTest  = true;
                  continue;
        case 't': if((_stscanf(cp+1, _T("%lu"), &threadCount) != 1) || (threadCount == 0)) {
                    usage();
                  }
                  break;
        default : usage();
        }
        break;
      }
    }

    if(highPriority) {
      BigRealResourcePool::setPriority(THREAD_PRIORITY_ABOVE_NORMAL);
      BigRealResourcePool::setPriorityBoost(true);

      if(!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL)) {
        throwLastErrorOnSysCallException(_T("SetThreadPriority"));
      }
      if(!SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS)) {
        throwLastErrorOnSysCallException(_T("SetPriorityClass"));
      }
      if(!SetThreadPriorityBoost(GetCurrentThread(), TRUE)) {
        throwLastErrorOnSysCallException(_T("SetThreadPriorityBoost"));
      }
    }
    if(BigReal::hasPow2CacheFile()) {
      BigReal::loadPow2Cache();
    }
    if(specialTest) {
      SpecialTestClass stc;
      stc.runTest();
    } else {
      testBigReal(threadCount);
    }
    if(BigReal::pow2CacheChanged()) {
      BigReal::savePow2Cache();
    }
  } catch(Exception e) {
    tcout << _T("\nException:") << e.what() << _T("\n");
    return -1;
  }
  return 0;
}
