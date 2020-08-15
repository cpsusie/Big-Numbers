#include "stdafx.h"
#include <DebugLog.h>
#include <FileNameSplitter.h>
#include <Math/FPU.h>
#include <Math/Double80.h>
#include <Math/BigReal/BigReal.h>
#include <FloatFields.h>

using namespace std;

#define MINPOW10 numeric_limits<Double80>::min_exponent10
#define MAXPOW10 numeric_limits<Double80>::max_exponent10

class ExperimentalPow10Calculator;

class SigApprox {
public:
  UINT64  m_sig;
  int     m_adds;
  BigReal m_diff;
  SigApprox(const BigReal &x, UINT64 s0, int adds, const BigReal &p2);
};

SigApprox::SigApprox(const BigReal &x, UINT64 s0, int adds, const BigReal &p2) {
  m_adds = adds;
  m_sig = s0 + adds;
  m_diff = rRound(fabs(BigReal(m_sig) * p2 - x), 30);
}

static Double80 createPrciseDouble80(const BigReal &x) {
  DigitPool     &pool  = *x.getDigitPool();
  const BigReal  l2    = rLog(pool._2(), x, 40);
  int            expo2 = (int)floor(l2);
  UINT           expoField, normalizingSteps = 0;

  for(;;) { // normalize
    expoField = FloatFields::expo2ToExpoField(FT_DOUBLE80, expo2 + normalizingSteps);
    if(FloatFields::isNormalExpoField(FT_DOUBLE80, expoField) || (expoField == 0)) {
      break;
    }
    normalizingSteps++;
  }
  const BigReal &p2   = BigReal::pow2(expo2 + normalizingSteps - 63, CONVERSION_POW2DIGITCOUNT);
  const UINT64   sig0 = (UINT64)round(rQuot(x, p2, 40)) >> normalizingSteps;

  Array<SigApprox> sigArray;
  for(int adds = -2; adds <= 2; adds++) {
    sigArray.add(SigApprox(x, sig0, adds, p2));
  }
  int bestIndex = 0;
  for(size_t i = 1; i < sigArray.size(); i++) {
    if (sigArray[i].m_diff < sigArray[bestIndex].m_diff) {
      bestIndex = (int)i;
    }
  }
  const SigApprox &bestSigApprox = sigArray[bestIndex];
  if(bestSigApprox.m_adds != 0) {
    _tprintf(_T("\npicked significand with adds = %d\n"), bestSigApprox.m_adds);
  }
  return FloatFields(FT_DOUBLE80, false, expoField, bestSigApprox.m_sig).getDouble80();
}

static Double80 createPrciseD80Pow10(int p) {
  return createPrciseDouble80(e(1, p));
}

static Double80 createPrciseD80Pow5(int p) {
  static ConstBigReal _5(5);
  return createPrciseDouble80(rPow(_5, BigReal(p), 40));
}

Double80 getRelativeError(const Double80 &x, const Double80 &ref) {
  if (ref.isZero()) {
    return fabs(x);
  }
  return fabs((x - ref) / ref);
}

class TestData : public CompactArray<Double80> {
public:
  TestData();
  void listTestResult(ExperimentalPow10Calculator *calc) const;
};

class ExperimentalPow10Calculator : public Pow10Calculator {
private:
  static String d80ToHexString(const Double80 &x);
public:
  virtual void        selfTest();
  void                dumpAll();
  void                dumpAsCodeTable(const String &fileName = EMPTYSTRING) const;
  virtual String      getTestLabel() const = NULL;
};

void ExperimentalPow10Calculator::selfTest() {
  TestData data;
  data.listTestResult(this);
}

void ExperimentalPow10Calculator::dumpAll() {
  Pow10Calculator *oldCalc = Double80::setPow10Calculator(this);
  try {
    __super::dumpAll();
    Double80::setPow10Calculator(oldCalc);
  } catch (...) {
    Double80::setPow10Calculator(oldCalc);
    throw;
  }
}

String ExperimentalPow10Calculator::d80ToHexString(const Double80 &x) { // static
  const BYTE *dp = (BYTE*)&x;
  String result = format(_T("0x%02x"), *(dp++));
  for (int i = 1; i < 10; i++) {
    result += format(_T(",0x%02x"), *(dp++));
  }
  return result;
}

void ExperimentalPow10Calculator::dumpAsCodeTable(const String &fileName) const {
  String outName = fileName;
  if(outName.length() == 0) {
    outName = getTestLabel();
    outName.replace(':', '_').replace('\\', '_');
    outName = FileNameSplitter::getChildName(_T("\\temp"), outName);
    outName = FileNameSplitter(outName).setExtension(_T("log")).getAbsolutePath();
  }

  _ftprintf(stderr, _T("Dump code-table to %s\n"), outName.cstr());

  FILE *f = MKFOPEN(outName, _T("w"));
  _ftprintf(f, _T("static const BYTE pow10TableData[] = {\n"));

  TCHAR *delim = _T("   ");
  for(int p = MINPOW10; p <= MAXPOW10; p++) {
    const Double80 v = pow10(p);
    _ftprintf(f, _T("%s%s     // 1e%+03d\n"), delim, d80ToHexString(v).cstr(), p);
    delim = _T("  ,");
  }
  _ftprintf(f, _T("};\n"));
  fclose(f);
}

TestData::TestData() {
  const Double80 stepFactor = 9.6894694997873711067;
  for(Double80 p = DBL80_MIN; isfinite(p); p *= stepFactor) {
    add(p);
  }
}

void TestData::listTestResult(ExperimentalPow10Calculator *calc) const {
  Pow10Calculator *oldCalc = Double80::setPow10Calculator(calc);
  try {
    Double80 maxError = 0;
    UINT     errorCount = 0;
    const size_t n = size();
    for (size_t i = 0; i < n; i++) {
      const Double80 &v = (*this)[i];
      char buf[100];
      d80toa(buf, v);
      Double80 v1 = strtod80(buf, NULL);
      if (v1 != v) {
        errorCount++;
        const Double80 relError = getRelativeError(v1, v);
        if (relError > maxError) {
          maxError = relError;
        }
      }
    }
    debugLog(_T("%s: errors:%7.2lf%%, max rel.Error:%s\n"), calc->getTestLabel().cstr(), PERCENT(errorCount, n), ::toString(maxError).cstr());
    Double80::setPow10Calculator(oldCalc);
  } catch (...) {
    Double80::setPow10Calculator(oldCalc);
    throw;
  }
}

class RawPow10Cache {
private:
  const UINT             m_lowSize, m_highSize;
  CompactArray<Double80> m_pow10L, m_pow10H;

  static UINT highSize(UINT lowSize) {
    return MAXPOW10 / lowSize + 1;
  }
public:
  RawPow10Cache(UINT lowSize);
  Double80 pow10(int p) const;
  inline UINT getLowSize() const {
    return m_lowSize;
  }
  inline UINT getHighSize() const {
    return m_highSize;
  }
};

RawPow10Cache::RawPow10Cache(UINT lowSize) : m_lowSize(lowSize), m_highSize(highSize(lowSize)) {
  m_pow10L.setCapacity(m_lowSize );
  m_pow10H.setCapacity(m_highSize);

  Double80 p = 1;
  for(size_t i = 0; i < m_lowSize; i++, p *= 10) {
    m_pow10L.add(p);
  }
  for(UINT i = 0; i < m_highSize; i++) {
    m_pow10H.add(createPrciseD80Pow10(i * m_lowSize));
  }
}

Double80 RawPow10Cache::pow10(int p) const {
  if(p < 0) {
    return Double80::_1 / pow10(-p);
  } else {
    const int l = p % m_lowSize, h = p / m_lowSize;
    if(h) {
      if(h >= (int)m_highSize) {
        return DBL80_MAX;
      }
      return l ? (m_pow10H[h] * m_pow10L[l]) : m_pow10H[h];
    } else { // h == 0
      return m_pow10L[l];
    }
  }
  throwInvalidArgumentException(__TFUNCTION__, _T("p=%d"), p);
  return 0;
}

class AvgPow10Cache : public ExperimentalPow10Calculator {
private:
  Array<RawPow10Cache*>  m_rawCacheArray;
  CompactArray<Double80> m_avgp10;
  RawPow10Cache *m_c1, *m_c2;
  bool m_avgAllMode;
  inline Double80 pow10Avg(int p) const {
    return m_avgp10[p - MINPOW10];
  }
public:
  AvgPow10Cache();
  ~AvgPow10Cache();

  Double80 pow10(int p) const;
  inline NumberInterval<UINT> getLowInterval() const {
    return NumberInterval<UINT>(m_rawCacheArray[0]->getLowSize(), m_rawCacheArray.last()->getLowSize());
  }
  inline size_t getCacheCount() const {
    return m_rawCacheArray.size();
  }
  void setCurrentCacheIndex(size_t i1, size_t i2) {
    if (i1 == i2) {
      throwInvalidArgumentException(__TFUNCTION__, _T("i1=%zu, i2=%zu"), i1, i2);
    }
    m_c1 = m_rawCacheArray[i1];
    m_c2 = m_rawCacheArray[i2];
  }
  void setavgAllMode(bool on) {
    m_avgAllMode = on;
  }
  void selfTest();

  String getTestLabel() const {
    if(m_avgAllMode) {
      return _T("average of all");
    }
    return format(_T("low1:%d, low2:%d"), m_c1->getLowSize(), m_c2->getLowSize());
  }
};

AvgPow10Cache::AvgPow10Cache() {
  m_avgAllMode = false;
  for(int lowSize = 15; lowSize <= 26; lowSize++) {
    RawPow10Cache *c = new RawPow10Cache(lowSize);
    m_rawCacheArray.add(c);
  }
  m_c1 = m_rawCacheArray[0];
  m_c2 = m_rawCacheArray[1];

  const size_t n = m_rawCacheArray.size();
  for(int p = MINPOW10; p <= MAXPOW10; p++) {
    CompactArray<Double80> approx;
    for(size_t k = 0; k < n; k++) {
      approx.add(m_rawCacheArray[k]->pow10(p));
    }
    UINT64 sum = 0;
    for(size_t k = 0; k < approx.size(); k++) {
      UINT64 sig = getSignificand(approx[k]);
      sum += sig & 0xffffffff;
    }
    sum /= n;
    Double80 avgd80 = approx[0];
    *((UINT*)&avgd80) = (UINT)sum;
    m_avgp10.add(avgd80);
  }
}

AvgPow10Cache::~AvgPow10Cache() {
  for(size_t i = 0; i < m_rawCacheArray.size(); i++) {
    delete m_rawCacheArray[i];
  }
  m_rawCacheArray.clear();
}

Double80 AvgPow10Cache::pow10(int p) const {
  if(m_avgAllMode) {
    return pow10Avg(p);
  }
  Double80 d1 = m_c1->pow10(p);
  const Double80 d2 = m_c2->pow10(p);
  const UINT64 sig1 = *((UINT64*)&d1);
  const UINT64 sig2 = *((UINT64*)&d2);
  if(sig1 == sig2) {
    return d1;
  }
  (*(UINT64*)&d1) = (sig1 & 0xffffffff00000000) + (((sig1 & 0xffffffff) + (sig2 & 0xffffffff)) >> 1);
  return d1;
}

void AvgPow10Cache::selfTest() {
  TestData data;

  setavgAllMode(true);
  data.listTestResult(this);
  setavgAllMode(false);

  const size_t n = getCacheCount();
  for(size_t i1 = 0; i1 < n; i1++) {
    for(size_t i2 = i1 + 1; i2 < n; i2++) {
      setCurrentCacheIndex(i1, i2);
      _tprintf(_T("%zu,%zu\n"), i1, i2);
      data.listTestResult(this);
    }
  }
}

// --------------------------------------- BigReal Pow10 Cache -----------------------------
class BigRealGeneratedPow10Cache : public ExperimentalPow10Calculator {
private:
  CompactArray<Double80> m_p10Table;
  int                    m_low;
public:
  BigRealGeneratedPow10Cache();
  Double80 pow10(int p) const {
    return (p < m_low) ? m_p10Table[0] * pow10(p - m_low) : m_p10Table[p - m_low];
  }
  String getTestLabel() const {
    return _T("BigReal generated pow10-cache");
  }
  void selfTest();
};

BigRealGeneratedPow10Cache::BigRealGeneratedPow10Cache() {
  _tprintf(_T("Generating pow10-table from BigReals (i=[%d..%d]):\n"), MINPOW10, MAXPOW10);
  bool lowSet = false;
  for(int i = MINPOW10; i <= numeric_limits<Double80>::max_exponent10; i++) {
    if((i%100)==0) {
      _tprintf(_T("i:%+d     \r"), i);
    }
    const Double80 d80 = createPrciseD80Pow10(i);
    int fpcl = _fpclass(d80);
    if(fpcl & _FPCLASS_PN) {
      m_p10Table.add(d80);
      if(!lowSet) {
        m_low    = i;
        lowSet = true;
      }
    } else {
      if(lowSet) {
        _ftprintf(stderr, _T("Cannot discard anymore numbers"));
      }
      TCHAR tmpstr[100];
      _tprintf(_T("Skipping %s. fpclass=%d"), d80tot(tmpstr, d80), fpcl);
    }
  }
  _tprintf(_T("Done generating table\n"));
}

void BigRealGeneratedPow10Cache::selfTest() {
  __super::selfTest();
  dumpAll();
  dumpAsCodeTable();
}


// --------------------------------------- BigReal Pow5 Cache -----------------------------

class BigRealGeneratedPow5Cache : public ExperimentalPow10Calculator {
private:
  CompactArray<Double80> m_p5Table;
  int                    m_low;
public:
  BigRealGeneratedPow5Cache();
  Double80 pow5(int p) const {
    return (p < m_low) ? m_p5Table[0] * pow10(p - m_low) : m_p5Table[p - m_low];
  }
  Double80 pow10(int p) const {
    const Double80 p5    = pow5(p);
    const int      expo2 = getExpo2(p5);
    return FloatFields(FT_DOUBLE80, false, FloatFields::expo2ToExpoField(FT_DOUBLE80, expo2 + p), getSignificand(p5)).getDouble80();
  }

  String getTestLabel() const {
    return _T("BigReal generated pow5-cache");
  }
  void selfTest();
};

BigRealGeneratedPow5Cache::BigRealGeneratedPow5Cache() {
  _tprintf(_T("Generating pow5-table from BigReals (i=[%d..%d]):\n"), MINPOW10, MAXPOW10);
  bool lowSet = false;
  for(int i = MINPOW10; i <= numeric_limits<Double80>::max_exponent10; i++) {
    if((i % 100) == 0) {
      _tprintf(_T("i:%+d     \r"), i);
    }
    const Double80 d80 = createPrciseD80Pow5(i);
    int fpcl = _fpclass(d80);
    if (fpcl & _FPCLASS_PN) {
      m_p5Table.add(d80);
      if(!lowSet) {
        m_low = i;
        lowSet = true;
      }
    } else {
      if (lowSet) {
        _ftprintf(stderr, _T("Cannot discard anymore numbers"));
      }
      TCHAR tmpstr[100];
      _tprintf(_T("Skipping %s. fpclass=%d"), d80tot(tmpstr, d80), fpcl);
    }
  }
  _tprintf(_T("Done generating table\n"));
}

void BigRealGeneratedPow5Cache::selfTest() {
  __super::selfTest();
  dumpAll();
  dumpAsCodeTable();
}

// --------------------------------------------------------------------------------------

#undef min

int main() {
  try {
    redirectDebugLog();

//  BigRealGeneratedPow10Cache testCache;
  AvgPow10Cache            testCache;
    testCache.selfTest();

    return 0;

    String x64classstr, x80classstr;
    double   x64 = numeric_limits<double  >::min();
    Double80 x80 = numeric_limits<Double80>::min();
    StreamParameters param64(numeric_limits<double  >::max_digits10);
    StreamParameters param80(numeric_limits<Double80>::max_digits10);

    while((x64 != 0) || (x80 != 0)) {
      wostringstream  strstr64,  strstr80;
      wstring         string64,  string80;
      const wchar_t  *cp64    , *cp80;
      wchar_t        *ce64    , *ce80;
      strstr64 << param64 << x64;
      strstr80 << param80 << x80;
      strstr64.flush(); string64 = strstr64.str(); cp64 = string64.c_str();
      strstr80.flush(); string80 = strstr80.str(); cp80 = string80.c_str();

      double   r64 = wcstod(  cp64, &ce64);
      Double80 r80 = wcstod80(cp80, &ce80);

      x64 /= 2;
      x80 /= 2;
    }
  } catch (Exception e) {
    _tprintf(_T("Exception:%s\n"), e.what());
    return -1;
  }
  return 0;
}

