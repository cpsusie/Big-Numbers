#include "stdafx.h"
#include <FileNameSplitter.h>
#include <ProcessTools.h>
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
#define TTS_TYPES              (TTS_D80 | TTS_BR)

static const TCHAR *getTypeName(UINT flags) {
  switch(flags & TTS_TYPES) {
  case TTS_D80: return _T("Double80");
  case TTS_BR : return _T("BigReal" );
  default: throwInvalidArgumentException(__TFUNCTION__, _T("flags=%04X"), flags);
  }
  return EMPTYSTRING;
}

#include <UnitTestTraits.h>

  TEST_CLASS(TestToString) {

    static void outputResult(UINT flags, double timeUsage, UINT itCounter, UINT errors, UINT lengthErrors, UINT acceptableErrors, UINT acceptableLengthErrors) {
      const TCHAR *typeName = getTypeName(flags);
      if((errors > acceptableErrors) || (lengthErrors > acceptableLengthErrors)) {
        OUTPUT(_T("Number of tests for %s:%s"    ), typeName, format1000(itCounter   ).cstr());
        OUTPUT(_T("Format errors for %s:%s - %.2lf%% (max=%s)")
              ,typeName, format1000(errors      ).cstr(), PERCENT(errors      , itCounter), format1000(acceptableErrors      ).cstr());
        OUTPUT(_T("Length errors for %s:%s - %.2lf%% (max=%s)")
              ,typeName, format1000(lengthErrors).cstr(), PERCENT(lengthErrors, itCounter), format1000(acceptableLengthErrors).cstr());
      }
      OUTPUT(_T("Total time usage:%.3lf sec."), timeUsage);
      verify((errors <= acceptableErrors) && (lengthErrors <= acceptableLengthErrors));
    }

    static void testToString(UINT          flags
                            ,const String &errorName
                            ,TestIterator &it
                            ,UINT          acceptableErrors
                            ,UINT          acceptableLengthErrors
                            ) {
      const UINT totalItCount = (UINT)it.getMaxIterationCount(), quatil = totalItCount/4;
      UINT       itCounter    = 0, errors = 0, lengthErrors = 0;
      const int  openMode     = ofstream::out | ofstream::trunc;
      tofstream  errorLog;

      OUTPUT(_T("Total format-tests:%s"), format1000(totalItCount).cstr());
      if(flags & TTS_DUMPALLFORMATFLAGS) {
        redirectDebugLog();
        it.dumpAllFormats();
      }
      errorLog.open(errorName.cstr(), openMode);

      tstring      d64Str, testStr;
      const double startTime = getProcessTime();
      const TCHAR *typeName  = getTypeName(flags);
      while(it.hasNext()) {
        const TestElement element = it.next();
        tostrstream d64Stream, testStream;
        if(++itCounter % quatil == 0) {
          OUTPUT(_T("Test %s progress:%.2lf%%"), typeName, PERCENT(itCounter, totalItCount));
        }
        const double d64 = element.m_values->getDouble();
        d64Stream  << element.m_param << d64;
        d64Str     =  d64Stream.str();
        switch(flags&TTS_TYPES) {
        case TTS_D80: testStream << element.m_param << element.m_values->getDouble80(); break;
        case TTS_BR : testStream << element.m_param << element.m_values->getBigReal();  break;
        }
        testStr    =  testStream.str();
        if(testStr != d64Str) {
          errorLog << "it:"; errorLog.width(6);
          errorLog << itCounter << "," << element.toString() << "\tD64Str:<" << d64Str << ">\ttestStr:<" << testStr << ">" << endl;
          errors++;
          if(testStr.length() != d64Str.length()) {
            lengthErrors++;
          }
#ifdef __NEVER__
          { tostrstream testStream1;
            testStream1 << element.m_param;
            switch(flags&TTS_TYPES) {
            case TTS_D80:
              testStream1 << element.m_values->getDouble80();
              break;
            case TTS_BR :
              testStream1 << element.m_values->getBigReal();
              break;
            }
          }
#endif
        }
      }
      const double timeUsage = (getProcessTime() - startTime) / 1e6;
      outputResult(flags, timeUsage, itCounter, errors, lengthErrors, acceptableErrors, acceptableLengthErrors);
    }

    static int doubleCompare(const double &d1, const double &d2) {
      return sign(d1 - d2);
    }

    static void showTypeTest(const TCHAR *method, UINT flags) {
      if(flags == 0) {
        throwInvalidArgumentException(__TFUNCTION__, _T("flags = %08x"), flags);
      }
      const String testingTypeStr = format(_T("double/%s"), getTypeName(flags));
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

    static String makeErrorFileName(const TCHAR *method) {
      String m = String(method);
      intptr_t lastColon = m.rfind(':');
      if(lastColon >= 0) m = substr(m, lastColon+1, 100);
      String configStr = _T("_") _PLATFORM_ _CONFIGURATION_;
      configStr.replace('/', '_');
      String fileName =  _T("TestToString\\") + m + configStr;
      if(fileName.last() == '_') fileName.removeLast();
      fileName = getTestFileName(fileName, _T("log"));
      FILE *f = MKFOPEN(fileName, "w");
      if(f) fclose(f);
      return fileName;
    }

    TEST_METHOD(TestD80ToString_StandardValues) {
      const UINT flags = TTS_D80;
      showTypeTest(__TFUNCTION__,flags);
      TestDataArray a(defaultTestValues, ARRAYSIZE(defaultTestValues));
      testToString(flags, makeErrorFileName(__TFUNCTION__), a.getIterator(), 0, 0);
    }

    TEST_METHOD(TestD80ToString_RandomValues) {
      const UINT flags = TTS_D80;
      showTypeTest(__TFUNCTION__, flags);
      TestDataArray a(getRandomTestArray());
      testToString(flags, makeErrorFileName(__TFUNCTION__), a.getIterator(), 390400, 0);
    }

    TEST_METHOD(TestBigRealToString_StandardValues) {
      const UINT flags = TTS_BR;
      showTypeTest(__TFUNCTION__, flags);
      TestDataArray a(defaultTestValues, ARRAYSIZE(defaultTestValues));
      testToString(flags, makeErrorFileName(__TFUNCTION__), a.getIterator(0), 0, 0);
    }

    TEST_METHOD(TestBigRealToString_RandomValues) {
      const UINT flags = TTS_BR;
      showTypeTest(__TFUNCTION__, flags);
      TestDataArray a(getRandomTestArray());
      testToString(flags, makeErrorFileName(__TFUNCTION__), a.getIterator(0), 414400, 0);
    }
  };
}
