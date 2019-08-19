#include "stdafx.h"
#include <Random.h>
#include <FileNameSplitter.h>
#include <Math/MathLib.h>
#include "TestToString.h"
#include "TestDataArray.h"

#define FSZ(n) format1000(n).cstr()

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
  const char *epos = strchr(s,'e');
  if(epos == NULL) epos = strchr(s,'E');
  const char *last;;
  if(epos != NULL) {
    last = epos - 1;
  } else {
    for(last = s + strlen(s); last >= s && isspace(*last); last--);
  }
  if(isdigit(*last))
    return *last - '0';
  else
    return -2;
}

static void testToString(const String &errorName, TestIterator &it) {
  UINT testCounter      = 0;
  UINT mismatchD80       = 0;
  UINT mismatchBR        = 0;
  UINT lengthMismatchD80 = 0;
  UINT lengthMismatchBR  = 0;
  const UINT totalTestCount = (UINT)it.getMaxIterationCount();

  FileNameSplitter spl(errorName);
  const String rawFileName = spl.getFileName();
  tofstream errorLogD80(spl.setFileName(rawFileName + _T("D80"    )).getFullPath().cstr());
  tofstream errorLogBR( spl.setFileName(rawFileName + _T("BigReal")).getFullPath().cstr());

  while(it.hasNext()) {
    TestElement element = it.next();
    tostrstream sD64,sD80,sBR;

    if(++testCounter % 10000 == 0) {
      tcout << format(_T("Count:%9s/%-9s:%s %6.2lf%%\r")
                     ,format1000(testCounter).cstr()
                     ,format1000(totalTestCount).cstr()
                     ,element.toString().cstr()
                     ,PERCENT(testCounter, totalTestCount)
                     );
    }
    sD64 << element.m_param << element.m_values->getDouble();
    sD80 << element.m_param << element.m_values->getDouble80();
    sBR  << element.m_param << element.m_values->getBigReal();

    tstring bufD64 = sD64.str();
    tstring bufD80 = sD80.str();
    tstring bufBR  = sBR.str();

    const bool equalD80 = bufD80 == bufD64;
    const bool equalBR  = bufBR  == bufD64;

    if(!equalD80) {
      errorLogD80 << element.toString() << _T("\tbufD64:<") << bufD64 << _T(">\tbuf80:<") << bufD80 << _T(">") << endl;
      mismatchD80++;
      if(bufD80.length() != bufD64.length()) {
        lengthMismatchD80++;
      }
    }

    if(!equalBR) {
      errorLogBR << element.toString() << _T("\tbufD64:<") << bufD64 << _T(">\tbufBr:<") << bufBR << _T(">") << endl;
      mismatchBR++;
      if(bufBR.length() != bufD64.length()) {
        lengthMismatchBR++;
      }
    }
  }

  tcout << spaceString(60) << endl;
  tcout << _T("Total Count                 :") << iparam(8) << testCounter       << _T(".") << endl;
  tcout << _T("Format mismatch for Double80:") << iparam(8) << mismatchD80       << _T(" ") << ufparam(2) << PERCENT(mismatchD80      , testCounter) << _T("%.") << endl;
  tcout << _T("Format mismatch for BigReal :") << iparam(8) << mismatchBR        << _T(" ") << ufparam(2) << PERCENT(mismatchBR       , testCounter) << _T("%.") << endl;
  tcout << _T("Length mismatch for Double80:") << iparam(8) << lengthMismatchD80 << _T(" ") << ufparam(2) << PERCENT(lengthMismatchD80, testCounter) << _T("%.") << endl;
  tcout << _T("Length mismatch for BigReal :") << iparam(8) << lengthMismatchBR  << _T(" ") << ufparam(2) << PERCENT(lengthMismatchBR , testCounter) << _T("%.") << endl;
}

static int doubleCompare(const double &d1, const double &d2) {
  return sign(d1-d2);
}

void testToString() {
  const double startTime = getProcessTime();
  tcout << _T("Testing operator<<(ostream &stream, BigReal/Double80/double)") << endl;

  TestDataArray a1(defaultTestValues, ARRAYSIZE(defaultTestValues));
  const String fileName1 = getTestFileName(__TFUNCTION__, _T("log"));
  testToString(fileName1, a1.getIterator());

  CompactDoubleArray values;
  for(int i = 0; i < 32; i++) {
    const double e = pow(10,randDouble(-200,200));
    const double t = randDouble();
    values.add(t * e);
  }

  values.sort(doubleCompare);
//  for(i = 0; i < values.size(); i++) {
//    printf("value[%2d]:%23.16le\n",i,values[i]);
//  }

  TestDataArray rndArray(values);
  const String fileName2 = getTestFileName(_T("toStringErrorsRnd"), _T("log"));
  testToString(fileName2, rndArray.getIterator());

  const double timeUsage = getProcessTime() - startTime;

  tcout << _T("Total time usage:") << ufparam(3) << ((getProcessTime() - startTime) / 1e6) << _T(" sec.") << endl;
}
