#include "stdafx.h"
#include <ProcessTools.h>
#include <ThreadPool.h>
#include <InputValue.h>
#include "TestBigReal.h"
#include "BigRealTestClass.h"

#ifdef __NEVER__

#define INTERACTIVE_SHORTPROD

#ifdef INTERACTIVE_SHORTPROD
static void testShortProd() {
  DigitPool pool(-5);
//  for(;;) {
//    int useReferenceVersion = inputValue<int>(_T("Use reference version (1=reference, 2=debug"));

//    BigReal::setUseShortProdRefenceVersion(useReferenceVersion == 1);

//    float maxError32Ref = getRelativeError32(FLT_MIN,&pool);
//    BigReal::setUseShortProdRefenceVersion(false);
//    float maxError32FPU = getRelativeError32(FLT_MIN,&pool);
//    BigReal::setUseShortProdRefenceVersion(true);

    const FullFormatBigReal x = BigReal(spaceString(14700,'9'),&pool); // inputBigReal(pool, _T("Enter x:"));
    const FullFormatBigReal y = BigReal(spaceString(14700,'9'),&pool); // inputBigReal(pool, _T("Enter y:"));

    _tprintf(_T("X:%s\nY:%s\n"), toString(x).cstr(), toString(y).cstr());

    FullFormatBigReal p1(&pool), p2(&pool);

    p1 = BigReal::shortProd(x, y, pool._0(), &pool);

    _tprintf(_T("p1:%s\n"), toString(p1).cstr());

    BigReal::setUseShortProdRefenceVersion(false);

    p2 = BigReal::shortProd(x, y, pool._0(), &pool);

    BigReal::setUseShortProdRefenceVersion(true);

    _tprintf(_T("p2:%s\n"), toString(p2).cstr());

    try {
      p1.assertIsValid();
    } catch(Exception e) {
      _tprintf(_T("p1 failed:%s\n"), e.what());
    }
    try {
      p2.assertIsValid();
    } catch(Exception e) {
      _tprintf(_T("p2 failed:%s\n"), e.what());
    }
//  }
}

#else

static void testShortProd() {
  DigitPool pool(-5);
  JavaRandom rnd;

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
      p1.assertIsValid();
    } catch(Exception e) {
      printf(_T("p1 failed:%s\n"), e.what());
      continue;
    }

    BigReal::setUseShortProdRefenceVersion(false);
    p2 = BigReal::shortProd(x, y, F, &pool);

    try {
      p2.assertIsValid();
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
      p1.assertIsValid();
    } catch(Exception e) {
      printf(_T("p1 failed:%s\n"), e.what());
    }
    try {
      p2.assertIsValid();
    } catch(Exception e) {
      printf(_T("p2 failed:%s\n"), e.what());
    }
  }
*/
}
#endif

#endif


static void testRandomBigReal() {
  DigitPool *pool = BigRealResourcePool::fetchDigitPool();
  MersenneTwister64 rnd;
  for(;;) {
    int count  = inputValue<int>(_T("Enter number of random numbers:"));
    int length = inputValue<int>(_T("Enter length (decimal digits):"));
    const FullFormatBigReal low  = inputBigReal(*pool, _T("Enter low :"));
    const FullFormatBigReal high = inputBigReal(*pool, _T("Enter high:"));

    String fileName = format(_T("c:\\temp\\random%s-%s-length=%d.dat"), toString(low).cstr(), toString(high).cstr(), length);
    FILE *f = FOPEN(fileName, _T("w"));
    for(int i = 0; i < count; i++) {
      FullFormatBigReal x = randBigReal(low, high, length, rnd, pool);
      _ftprintf(f, _T("%s\n"), toString(x).cstr());
    }
    fclose(f);
  }
  BigRealResourcePool::releaseDigitPool(pool);
}

static void testAPCSum() {
  DigitPool *pool = BigRealResourcePool::fetchDigitPool();
  for(;;) {
    const FullFormatBigReal x = inputBigReal(*pool, _T("Enter x:"));
    const FullFormatBigReal y = inputBigReal(*pool, _T("Enter y:"));
    _tprintf(_T("Enter bias ('<','>','#'):"));
    char bias = getchar();
    FullFormatBigReal p = BigReal::apcSum(bias, x, y, pool);

    _tprintf(_T("x:%50s y:%50s\n"), toString(x).cstr(), toString(y).cstr());
    _tprintf(_T("APCSum(>,x,y) = %s\n"), toString(p).cstr());

    try {
      VALIDATEBIG(p);
    } catch(Exception e) {
      _tprintf(_T("%s\n"), e.what());
    }
  }
  BigRealResourcePool::releaseDigitPool(pool);
}

class SpecialTestClass : public BigRealTestClass {
public:
  void runTest();
};

void SpecialTestClass::runTest() {

//  DigitPool::setDumpWhenDestroyed(true);

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
  _ftprintf(stderr, _T("Usage:TestBigReal [-s] [-p] [-tThreadCount] [-d]\n"
                       "   -p: Set all threads priority to ABOVE_NORMAL\n"
                       "   -s: Run special test. Default is standard numbertest\n"
                       "   -g: Generate and save/dump hashtable. implies Threadcount = 1\n"
                       "   -d: Dump pow2Cache, either at load time, or at end, after all tests have filled the cache\n"
                       "   -h: Halt all threads on first error detected\n"
                       "   -tThreadcount: run testnumber with the specified number of threads. Default is the number of cores in the CPU\n"
                       "   -m: Start digit monitor. will write every 3 seconds the total number of allocated digits\n"
                       "   -2: Disable pre-load of pow2cache"
                      )
           );
  exit(-1);
}

typedef enum {
  CMD_STDTEST
 ,CMD_SPECIALTEST
 ,CMD_GENERATEPOW2CACHE
} Command;

int _tmain(int argc, TCHAR **argv) {
  try {
    Command command           = CMD_STDTEST;
    bool    highPriority      = false;
    bool    loadPow2          = true;
    bool    dumpPow2Cache     = false;
    bool    stopOnError       = false;
    bool    startDigitMonitor = false;
#ifdef _DEBUG
    int     threadCount       = getDebuggerPresent() ? 1 : 0;
#else
    int     threadCount       = 0;
#endif _DEBUG

    TCHAR *cp;
    for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
      for(cp++; *cp; cp++) {
        switch(*cp) {
        case 'p': highPriority  = true;
                  continue;
        case 's': command = CMD_SPECIALTEST;
                  continue;
        case 'g': command = CMD_GENERATEPOW2CACHE;
                  continue;
        case 'd': dumpPow2Cache = true;
                  continue;
        case 'h': stopOnError = true;
                  continue;
        case 't': if((_stscanf(cp+1, _T("%lu"), &threadCount) != 1) || (threadCount == 0)) {
                    usage();
                  }
                  break;
        case 'm': startDigitMonitor = true;
                  continue;
        case '2': loadPow2 = false;
                  continue;
        default : usage();
        }
        break;
      }
    }

    DigitMonitor digitMonitor;

    if(highPriority) {
      ThreadPool::setPriority(PRIORITY_ABOVE_NORMAL);
      ThreadPool::setPriorityBoost(true);

      setThreadPriority(PRIORITY_ABOVE_NORMAL);
      setPriorityClass(PRCLASS_ABOVE_NORMAL);
      setThreadPriorityBoost(true);
    }
    if (startDigitMonitor) {
      digitMonitor.start();
    }
    switch(command) {
    case CMD_STDTEST          :
    case CMD_SPECIALTEST      :
      if(loadPow2 && BigReal::pow2CacheHasFile()) {
        BigReal::pow2CacheLoad();
        if(dumpPow2Cache) {
          BigReal::pow2CacheDump();
          dumpPow2Cache = false; // dont dump at end
        }
      }
      if(command == CMD_SPECIALTEST) {
        SpecialTestClass stc;
        stc.runTest();
      } else {
        testBigReal(threadCount, stopOnError);
      }
      if(BigReal::pow2CacheChanged()) {
        BigReal::pow2CacheSave();
        if(dumpPow2Cache) {
          BigReal::pow2CacheDump();
        }
      }
      break;
    case CMD_GENERATEPOW2CACHE:
      testDouble80Conversions();
      if(BigReal::pow2CacheChanged()) {
        BigReal::pow2CacheSave();
        BigReal::pow2CacheDump();
      }
      break;
    }
  } catch(Exception e) {
    tcout << endl << "Exception:" << e.what() << endl;
    return -1;
  }
  tcout << endl << "All done" << endl;
  return 0;
}
