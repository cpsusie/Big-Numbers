#include "stdafx.h"
#include <Random.h>
#include <Math.h>
#include <Math/MathFunctions.h>
#include <Math/Double80.h>
#include <Math/BigReal.h>
#include "TestToString.h"

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

class StringParametersIterator {
private:
  size_t         m_nextValueIndex;
  size_t         m_nextFlagIndex;
  int            m_nextPrecision;
  int            m_nextWidth;

  bool               m_hasNext;
  double             m_currentValue;
  long               m_currentFlags;
  size_t             m_currentValueIndex,m_currentFlagIndex;
  int                m_currentPrecision, m_currentWidth;
  CompactDoubleArray m_values;
  CompactLongArray   m_flagCombinations;
  StringArray        m_flagString;
  size_t             m_maxValueIndex;
  size_t             m_maxFlagIndex;

  void resetValueIndex()         { m_nextValueIndex = 0; }
  void resetFlagIndex()          { m_nextFlagIndex  = 0; }
  void resetPrecision()          { m_nextPrecision  = 1; }
  void resetWidth()              { m_nextWidth      = 0; }

  void nextValueIndex()          { m_nextValueIndex++;   }
  void nextFlagIndex()           { m_nextFlagIndex++;    }
  void nextPrecision()           { m_nextPrecision++;    }
  void nextWidth()               { m_nextWidth++;        }

  bool hasNextValueIndex() const { return m_nextValueIndex < m_maxValueIndex; }
  bool hasNextFlagIndex()  const { return m_nextFlagIndex  < m_maxFlagIndex;  }
  bool hasNextPrecision()  const { return m_nextPrecision  < 15;              }
  bool hasNextWidth()      const { return m_nextWidth      < 25;              }

  void setValueAndFlags();
  void init(const CompactDoubleArray &values);
  void initFlagCombinations();
  String getFlagString(long flags) const;
public:
  StringParametersIterator();
  StringParametersIterator(const CompactDoubleArray &values);
  void next();

  inline bool hasNext() const {
    return m_hasNext;
  }

  inline int getWidth() const {
    return m_currentWidth;
  }
  
  inline int getPrecision() const {
    return m_currentPrecision;
  }
  
  inline long getFlags()                 const { return m_currentFlags; }
  inline const double &getValue64()      const { return m_currentValue; }
  inline Double80      getValue80()      const { return m_currentValue; }
  inline BigReal       getValueBigReal() const { return m_currentValue; }
  String               toString()        const;
  inline operator StreamParameters() const {
    return StreamParameters(getPrecision(), getWidth(), getFlags());
  }
};

StringParametersIterator::StringParametersIterator() {
  CompactDoubleArray tmp;
  for(int i = 0; i < ARRAYSIZE(defaultTestValues); i++) {
    tmp.add(defaultTestValues[i]);
  }
  init(tmp);
}

StringParametersIterator::StringParametersIterator(const CompactDoubleArray &values) {
  init(values);
}

void StringParametersIterator::init(const CompactDoubleArray &values) {
  if(values.size() >= 256) {
    throwException(_T("Too many values (=%s). max is 255"), FSZ(values.size()));
  }
  m_values = values;

  for(size_t i = 0; i < values.size(); i++) {
    double   d64  = values[i];
    Double80 d80  = values[i];
    BigReal  n    = values[i];
    int d64expo10 = getExpo10(d64);
    int d80expo10 = Double80::getExpo10(d80);
    int nexpo10   = (int)BigReal::getExpo10(n);
    if(d80expo10 != d64expo10 || nexpo10 != d64expo10) {
      printf("expo10 mismatch:%20.14le d64expo10:%3d d80expo10:%3d numberExpo10:%3d\n",d64,d64expo10,d80expo10,nexpo10);
      abort();
    }
  }

  m_maxValueIndex = m_values.size()-1;
  initFlagCombinations();

  resetValueIndex();
  resetFlagIndex();
  resetPrecision();
  resetWidth();
  
  m_hasNext = true;
}

void StringParametersIterator::initFlagCombinations() {
  m_flagCombinations.add(0);
  m_flagString.add(getFlagString(0));

  for(long flags = 0;;) {
    if(!(flags & ios::uppercase)) {
      flags |= ios::uppercase;
    } else {
      flags &= ~ios::uppercase;
      if(!(flags & ios::floatfield)) {
        flags |= ios::fixed;
      } else {
        if(flags & ios::fixed) {
          flags &= ~ios::fixed;
          flags |= ios::scientific;
        } else {
          flags &= ~ios::scientific;
          if(!(flags & ios::left)) {
            flags |= ios::left;
          } else {
            flags &= ~ios::left;
            if(!(flags & ios::right)) {
              flags |= ios::right;
            } else {
              flags &= ~ios::right;
              if(!(flags & ios::showpoint)) {
                flags |= ios::showpoint;
              } else {
                flags &= ~ios::showpoint;
                if(!(flags & ios::showpos)) {
                  flags |= ios::showpos;
                } else {
                  break;
                }
              }
            }
          }
        }
      }
    }
    m_flagCombinations.add(flags);
    m_flagString.add(getFlagString(flags));
  }
  m_maxFlagIndex = m_flagCombinations.size() - 1;
}

void StringParametersIterator::setValueAndFlags() {
  m_currentValueIndex = m_nextValueIndex;
  m_currentFlagIndex  = m_nextFlagIndex;
  m_currentValue      = m_values[m_currentValueIndex];
  m_currentFlags      = m_flagCombinations[m_currentFlagIndex];
  m_currentWidth      = m_nextWidth;
  m_currentPrecision  = m_nextPrecision;
}

void StringParametersIterator::next() {
  if(!m_hasNext)
    return;

  setValueAndFlags();

  if(hasNextValueIndex()) {
    nextValueIndex();
  } else {
    resetValueIndex();
    if(hasNextFlagIndex()) {
      nextFlagIndex();
    } else {
      resetFlagIndex();
      if(hasNextPrecision()) {
        nextPrecision();
      } else {
        resetPrecision();
        if(hasNextWidth()) {
          nextWidth();
        } else {
          m_hasNext = false;
        }
      }
    }
  }
}

String StringParametersIterator::getFlagString(long flags) const {
  String result;
  result =  (flags & ios::showpos   ) ? "+" : " ";
  result += (flags & ios::showpoint ) ? "." : " ";
  result += (flags & ios::left      ) ? "l" : " ";
  result += (flags & ios::right     ) ? "r" : " ";
  result += (flags & ios::scientific) ? (flags & ios::uppercase) ? "E" : "e" : " ";
  result += (flags & ios::fixed     ) ? "f" : " ";
  return result;
}

String StringParametersIterator::toString() const {
  return m_flagString[m_currentFlagIndex] + format(_T(" %2d.%2d %20.16le"),getWidth(),getPrecision(),getValue64());
}

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
    
static void testToString(const String &errorName, StringParametersIterator &it) {
  UINT totalCounter           = 0;
  UINT mismatchDouble80       = 0;
  UINT mismatchBigReal        = 0;
  UINT lengthMismatchDouble80 = 0;
  UINT lengthMismatchBigReal  = 0;

  tofstream errorLog(errorName.cstr());

  while(it.hasNext()) {
    it.next();
    tostrstream s64,s80,sN;

/*
    if(((it.getFlags() & (ios::fixed|ios::scientific)) == ios::fixed) && it.getPrecision() + Double80::getExpo10(it.getValue80()) > 26) {
      continue; // Skip this check and get an uncatchable "access-violation"-Exception from ostrstream << it.getValue64() !!!
    }
*/

    if(++totalCounter % 10000 == 0) {
      tcout << _T("Count:") << totalCounter << _T(" ") << it.toString() << _T("          \r");
      tcout.flush();
    }
    const double d64 = it.getValue64();
    s64 << it << d64;
    s80 << it << it.getValue80();
    sN  << it << it.getValueBigReal();

    tstring buf64 = s64.str();
    tstring buf80 = s80.str();
    tstring bufN  = sN.str();

    const bool equal80 = buf80 == buf64;
    const bool equalN = bufN == buf64;

    if(!equal80 || !equalN) {
      errorLog << it.toString() << _T("\tbuf64:<") << buf64 << _T(">\tbuf80:<") << buf80 << _T(">\tbufN:<") << bufN << _T(">") << endl;
    
      if(!equal80) {
        mismatchDouble80++;
        if(buf80.length() != buf64.length()) {
          lengthMismatchDouble80++;
        }
      }

      if(!equalN) {
        mismatchBigReal++;
        if(bufN.length() != buf64.length()) {
          lengthMismatchBigReal++;
        }
      }
    }
  }

  tcout << spaceString(60) << endl;
  tcout << _T("Total Count                 :") << iparam(8) << totalCounter << _T(".") << endl;
  tcout << _T("Format mismatch for Double80:") << iparam(8) << mismatchDouble80       << _T(" ") << ufparam(2) << ((double)mismatchDouble80      / totalCounter * 100) << _T("%.") << endl;
  tcout << _T("Format mismatch for BigReal :") << iparam(8) << mismatchBigReal        << _T(" ") << ufparam(2) << ((double)mismatchBigReal       / totalCounter * 100) << _T("%.") << endl;
  tcout << _T("Length mismatch for Double80:") << iparam(8) << lengthMismatchDouble80 << _T(" ") << ufparam(2) << ((double)lengthMismatchDouble80/ totalCounter * 100) << _T("%.") << endl;
  tcout << _T("Length mismatch for BigReal :") << iparam(8) << lengthMismatchBigReal  << _T(" ") << ufparam(2) << ((double)lengthMismatchBigReal / totalCounter * 100) << _T("%.") << endl;
}

static int doubleCompare(const double &d1, const double &d2) {
  return sign(d1-d2);
}

void testToString() {

  const double startTime = getProcessTime();
  tcout << _T("Testing operator<<(ostream &stream, BigReal/Double80/double)") << endl;

  StringParametersIterator it1;
  testToString(_T("c:\\temp\\toStringErrorsStd.log"), it1);

  CompactDoubleArray values;
  Random             random;
  for(int i = 0; i < 32; i++) {
    const double e = pow(10,random.nextDouble(-200,200));
    const double t = random.nextDouble();
    values.add(t * e);
  }

  values.sort(doubleCompare);
//  for(i = 0; i < values.size(); i++) {
//    printf("value[%2d]:%23.16le\n",i,values[i]);
//  }

  StringParametersIterator it2(values);
  testToString(_T("c:\\temp\\toStringErrorsRnd.log"), it2);

  const double timeUsage = getProcessTime() - startTime;

  tcout << _T("Total time usage:") << ufparam(3) << ((getProcessTime() - startTime) / 1e6) << _T(" sec.") << endl;
}
