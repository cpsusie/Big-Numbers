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

    static void testToStringD80(UINT flags
                               ,const String &errorName, TestDataArray &a
                               ,UINT acceptableErrors, UINT acceptableLengthErrors
                               ) {
      TestIterator it             = a.getIterator();
      const UINT   totalItCount   = (UINT)it.getMaxIterationCount(), quatil = totalItCount/4;
      UINT         itCounter      = 0;
      UINT         testCounter    = 0, mismatch    = 0, lengthMismatch = 0;
      const int    openMode       = ofstream::out | ofstream::trunc;
      tofstream    errorLog;

      OUTPUT(_T("Total format-tests:%s"), format1000(totalItCount).cstr());

      if(flags & TTS_DUMPALLFORMATFLAGS) {
        redirectDebugLog();
        it.dumpAllFormats();
      }
      FileNameSplitter spl(errorName);
      const String rawFileName = spl.getFileName();

      errorLog.open(spl.setFileName(rawFileName + _T("D80")).getFullPath().cstr(), openMode);

      tstring      D64Str, testStr;
      const double startTime = getProcessTime();

      while(it.hasNext()) {
        TestElement element = it.next();
        tostrstream sD64;
        if(++itCounter % quatil == 0) {
          OUTPUT(_T("%s progress:%.2lf%%"), __TFUNCTION__, PERCENT(itCounter, totalItCount));
        }

        sD64 << element.m_param << element.m_values->getDouble();
        D64Str = sD64.str();

        tostrstream stream;
        stream << element.m_param << element.m_values->getDouble80();
        testStr = stream.str();
        testCounter++;
        if(testStr != D64Str) {
          errorLog << element.toString() << _T("\tD64Str:<") << D64Str << _T(">\ttestStr:<") << testStr << _T(">") << endl;
          mismatch++;
          if(testStr.length() != D64Str.length()) {
            lengthMismatch++;
          }
        }
      }

      const double timeUsage = getProcessTime() - startTime;

      if((mismatch > acceptableErrors) || (lengthMismatch > acceptableLengthErrors)) {
        OUTPUT(_T("Number of tests for Double80:%s"          ), format1000(testCounter   ).cstr());
        OUTPUT(_T("Format mismatch for Double80:%s - %.2lf%%"), format1000(mismatch      ).cstr(), PERCENT(mismatch      , testCounter));
        OUTPUT(_T("Length mismatch for Double80:%s - %.2lf%%"), format1000(lengthMismatch).cstr(), PERCENT(lengthMismatch, testCounter));
      }
      OUTPUT(_T("Total time usage:%.3lf sec."), ((getProcessTime() - startTime) / 1e6));
      verify((mismatch <= acceptableErrors) && (lengthMismatch <= acceptableLengthErrors));
    }

    static void testToStringBR(UINT flags
                              ,const String &errorName, TestDataArray &a
                              ,UINT acceptableErrors, UINT acceptableLengthErrors
                              ) {
      TestIterator it             = a.getIterator();
      const UINT   totalItCount   = (UINT)it.getMaxIterationCount(), quatil = totalItCount/4;
      UINT         itCounter      = 0;
      UINT         testCounter    = 0, mismatch     = 0, lengthMismatch  = 0;
      const int    openMode       = ofstream::out | ofstream::trunc;
      tofstream    errorLog;

      OUTPUT(_T("Total format-tests:%s"), format1000(totalItCount).cstr());

      if(flags & TTS_DUMPALLFORMATFLAGS) {
        redirectDebugLog();
        it.dumpAllFormats();
      }
      FileNameSplitter spl(errorName);
      const String rawFileName = spl.getFileName();

      errorLog.open(spl.setFileName(rawFileName + _T("BigReal")).getFullPath().cstr(), openMode);

      tstring D64Str, testStr;

      const double startTime = getProcessTime();

      while(it.hasNext()) {
        TestElement element = it.next();
        tostrstream sD64;
        if(++itCounter % quatil == 0) {
          OUTPUT(_T("%s progress:%.2lf%%"), __TFUNCTION__, PERCENT(itCounter, totalItCount));
        }

        sD64 << element.m_param << element.m_values->getDouble();
        D64Str = sD64.str();

        if((element.m_param.flags() & ios::floatfield) != ios::hexfloat) { // hexfloat is not relevant for BigReal
          testCounter++;
          tostrstream stream;
          stream << element.m_param << element.m_values->getBigReal();
          testStr = stream.str();
          if(testStr != D64Str) {
            errorLog << element.toString() << _T("\tD64Str:<") << D64Str << _T(">\ttestStr:<") << testStr << _T(">") << endl;
            mismatch++;
            if(testStr.length() != D64Str.length()) {
              lengthMismatch++;
            }
          }
        }
      }
      const double timeUsage = getProcessTime() - startTime;

      if((mismatch > acceptableErrors) || (lengthMismatch > acceptableLengthErrors)) {
        OUTPUT(_T("Number of tests for BigReal :%s"          ), format1000(testCounter    ).cstr());
        OUTPUT(_T("Format mismatch for BigReal :%s - %.2lf%%"), format1000(mismatch       ).cstr(), PERCENT(mismatch      , testCounter));
        OUTPUT(_T("Length mismatch for BigReal :%s - %.2lf%%"), format1000(lengthMismatch ).cstr(), PERCENT(lengthMismatch, testCounter));
      }
      OUTPUT(_T("Total time usage:%.3lf sec."), ((getProcessTime() - startTime) / 1e6));
      verify((mismatch <= acceptableErrors) && (lengthMismatch <= acceptableLengthErrors));
    }

    static int doubleCompare(const double &d1, const double &d2) {
      return sign(d1 - d2);
    }

    static void showTypeTest(const TCHAR *method, UINT flags) {
      if(flags == 0) {
        throwInvalidArgumentException(__TFUNCTION__, _T("flags = %08x"), flags);
      }
      String testingTypeStr = _T("double");
      if(flags & TTS_D80) {
        testingTypeStr += _T("/Double80");
      }
      if(flags & TTS_BR) {
        testingTypeStr += _T("/BigReal");
      }
      OUTPUT(_T("%s - Testing operator<<(ostream &stream, %s)"), method, testingTypeStr.cstr());
    }

    static const CompactDoubleArray &getRandomTestArray() {
      static CompactDoubleArray values;
      if(values.isEmpty()) {
        for(int i = 0; i < 32; i++) {
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

      testToStringD80(flags, fileName, a1, 0, 0);
    }

    TEST_METHOD(TestD80ToString_RandomValues) {
      const UINT flags = TTS_D80;
      showTypeTest(__TFUNCTION__, flags);

      TestDataArray a(getRandomTestArray());
      const String fileName = getTestFileName(__TFUNCTION__, _T("log"));
      testToStringD80(flags, fileName, a, 390400, 0);
    }

    TEST_METHOD(TestBigRealToString_StandardValues) {
      const UINT flags = TTS_BR;
      showTypeTest(__TFUNCTION__, flags);

      TestDataArray a(defaultTestValues, ARRAYSIZE(defaultTestValues));
      const String fileName = getTestFileName(__TFUNCTION__, _T("log"));

      testToStringBR(flags, fileName, a, 0, 0);
    }

    TEST_METHOD(TestBigRealToString_RandomValues) {
      const UINT flags = TTS_BR;
      showTypeTest(__TFUNCTION__, flags);

      TestDataArray a(getRandomTestArray());
      const String fileName = getTestFileName(__TFUNCTION__, _T("log"));
      testToStringBR(flags, fileName, a, 414400,0);
    }
  };
}
