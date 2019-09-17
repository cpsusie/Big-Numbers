#include "stdafx.h"
#include <FileNameSplitter.h>
#include "TestDataArray.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

static const double defaultTestValues[] = {
  0
 ,1.2345678901234567e-13
 ,1.2345678901234567e-12
 ,1.2345678901234567e-11
 ,1.2345678901234567e-10
 ,1.2345678901234567e-9
 ,1.2345678901234567e-8
 ,1.2345678901234567e-7
 ,1.2345678901234567e-6
 ,1.2345678901234567e-5
 ,1.2345678901234567e-4
 ,1.2345678901234567e-3
 ,1.2345678901234567e-2
 ,1.2345678901234567e-1
 ,1
 ,12
 ,123
 ,1234
 ,12345
 ,123456
 ,1234567
 ,12345678
 ,123456789
 ,1234567890
 ,12345678901
 ,123456789012
 ,1234567890123
 ,12345678901234
 ,123456789012345
 ,1234567890123456
};

// ---------------------------------------------------------------------------

int getLastMantissaDigit(const char *s) {
  const char *epos = strchr(s, 'e');
  if(epos == NULL) epos = strchr(s, 'E');
  const char *last;;
  if(epos != NULL) {
    last = epos - 1;
  } else {
    for(last = s + strlen(s); last >= s && isspace(*last); last--);
  }
  if(isdigit(*last)) {
    return *last - '0';
  } else {
    return -2;
  }
}

namespace TestToString {		

#define TTS_D80                0x01
#define TTS_BR                 0x02
#define TTS_DUMPALLFORMATFLAGS 0x04

#include <UnitTestTraits.h>

  TEST_CLASS(TestToString) {

    // return timeusage in sec
    static void testToString(UINT flags
                            ,const String &errorName, TestDataArray &a
                            ,UINT acceptableErrors, UINT acceptableLengthErrors
                            ) {
      TestIterator it             = a.getIterator();
      const UINT   totalTestCount = (UINT)it.getMaxIterationCount();
      const int    openMode       = ofstream::out | ofstream::trunc;
      UINT         loopCounter    = 0;
      UINT         testCounterD80 = 0, mismatchD80    = 0, lengthMismatchD80 = 0;
      UINT         testCounterBR  = 0, mismatchBR     = 0, lengthMismatchBR  = 0;
      tofstream    errorLogD80, errorLogBR;

      OUTPUT(_T("Total format-tests:%s"), format1000(it.getMaxIterationCount()).cstr());

      if(flags & TTS_DUMPALLFORMATFLAGS) {
        redirectDebugLog();
        it.dumpAllFormats();
      }
      FileNameSplitter spl(errorName);
      const String rawFileName = spl.getFileName();

      if(flags & TTS_D80) {
        errorLogD80.open(spl.setFileName(rawFileName + _T("D80")).getFullPath().cstr(), openMode);
      }
      if(flags & TTS_BR) {
        errorLogBR.open(spl.setFileName(rawFileName + _T("BigReal")).getFullPath().cstr(), openMode);
      }

      tstring bufD64, bufD80, bufBR;
      bool equalD80 = true, equalBR = true;

      const double startTime = getProcessTime();

      while(it.hasNext()) {
        TestElement element = it.next();
        tostrstream sD64;
#ifdef _DEBUG
        if(++loopCounter % 10000 == 0) {
          OUTPUT(_T("Count:%9s/%-9s:%s %6.2lf%%\r")
                ,format1000(loopCounter).cstr()
                ,format1000(totalTestCount).cstr()
                ,element.toString().cstr()
                ,PERCENT(loopCounter, totalTestCount)
          );
        }
#endif

        sD64 << element.m_param << element.m_values->getDouble();
        bufD64 = sD64.str();

        if(flags & TTS_D80) {
          testCounterD80++;
          tostrstream stream;
          stream << element.m_param << element.m_values->getDouble80();
          bufD80 = stream.str();
          if(bufD80 != bufD64) {
            errorLogD80 << element.toString() << _T("\tbufD64:<") << bufD64 << _T(">\tbuf80:<") << bufD80 << _T(">") << endl;
            mismatchD80++;
            if(bufD80.length() != bufD64.length()) {
              lengthMismatchD80++;
            }
          }
        }
        if(flags & TTS_BR) {
          if((element.m_param.flags() & ios::floatfield) != ios::hexfloat) { // hexfloat is not relevant for BigReal
            testCounterBR++;
            tostrstream stream;
            stream << element.m_param << element.m_values->getBigReal();
            bufBR = stream.str();
            if(bufBR != bufD64) {
              errorLogBR << element.toString() << _T("\tbufD64:<") << bufD64 << _T(">\tbufBr:<") << bufBR << _T(">") << endl;
              mismatchBR++;
              if(bufBR.length() != bufD64.length()) {
                lengthMismatchBR++;
              }
            }
          }
        }
      }

      const double timeUsage = getProcessTime() - startTime;

      if(flags & TTS_D80) {
        if((mismatchD80 > acceptableErrors) || (lengthMismatchD80 > acceptableLengthErrors)) {
          OUTPUT(_T("Number of tests for Double80:%s"          ), format1000(testCounterD80   ).cstr());
          OUTPUT(_T("Format mismatch for Double80:%s - %.2lf%%"), format1000(mismatchD80      ).cstr(), PERCENT(mismatchD80      , testCounterD80));
          OUTPUT(_T("Length mismatch for Double80:%s - %.2lf%%"), format1000(lengthMismatchD80).cstr(), PERCENT(lengthMismatchD80, testCounterD80));
          verify(false);
        }
      }
      if(flags & TTS_BR) {
        if((mismatchBR > acceptableErrors) || (lengthMismatchBR > acceptableLengthErrors)) {
          OUTPUT(_T("Number of tests for BigReal :%s"          ), format1000(testCounterBR    ).cstr());
          OUTPUT(_T("Format mismatch for BigReal :%s - %.2lf%%"), format1000(mismatchBR       ).cstr(), PERCENT(mismatchBR      , testCounterBR));
          OUTPUT(_T("Length mismatch for BigReal :%s - %.2lf%%"), format1000(lengthMismatchBR ).cstr(), PERCENT(lengthMismatchBR, testCounterBR));
          verify(false);
        }
      }
      OUTPUT(_T("Total time usage:%.3lf sec."), ((getProcessTime() - startTime) / 1e6));
    }

    static int doubleCompare(const double &d1, const double &d2) {
      return sign(d1 - d2);
    }

    static void showTypeTest(const TCHAR *method, UINT flags) {
      if(flags == 0) {
        throwInvalidArgumentException(__TFUNCTION__, _T("flags = %08x"), flags);
      }
      String testingTypeStr = _T("double");
      if (flags & TTS_D80) {
        testingTypeStr += _T("/Double80");
      }
      if (flags & TTS_BR) {
        testingTypeStr += _T("/BigReal");
      }
      OUTPUT(_T("%s - Testing operator<<(ostream &stream, %s)"), method, testingTypeStr.cstr());
    }

    static const CompactDoubleArray &getRandomTestArray() {
      static CompactDoubleArray values;
      if (values.isEmpty()) {
        for (int i = 0; i < 32; i++) {
          const double e = pow(10, randDouble(-200, 200));
          const double t = randDouble();
          values.add(t * e);
        }
        values.sort(doubleCompare);
      }
      return values;
    }

    TEST_METHOD(TestD80ToString_StandardValues) {
      const UINT flags = TTS_D80;
      showTypeTest(__TFUNCTION__,flags);

      TestDataArray a1(defaultTestValues, ARRAYSIZE(defaultTestValues));
      const String fileName = getTestFileName(__TFUNCTION__, _T("log"));

      testToString(flags, fileName, a1, 0, 0);
    }

    TEST_METHOD(TestD80ToString_RandomValues) {
      const UINT flags = TTS_D80;
      showTypeTest(__TFUNCTION__, flags);

      TestDataArray a(getRandomTestArray());
      const String fileName = getTestFileName(__TFUNCTION__, _T("log"));
      testToString(flags, fileName, a, 390400, 0);
    }

    TEST_METHOD(TestBigRealToString_StandardValues) {
      const UINT flags = TTS_BR;
      showTypeTest(__TFUNCTION__, flags);

      TestDataArray a(defaultTestValues, ARRAYSIZE(defaultTestValues));
      const String fileName = getTestFileName(__TFUNCTION__, _T("log"));

      testToString(flags, fileName, a, 0, 0);
    }

    TEST_METHOD(TestBigRealToString_RandomValues) {
      const UINT flags = TTS_BR;
      showTypeTest(__TFUNCTION__, flags);

      TestDataArray a(getRandomTestArray());
      const String fileName = getTestFileName(__TFUNCTION__, _T("log"));
      testToString(flags, fileName, a, 414400,0);
    }
  };
}
