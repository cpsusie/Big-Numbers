#include "stdafx.h"
#include "FunctionTest.h"

class ConversionTest {
public:
  TestStatistic &m_stat;
  double         m_maxError;
  int            m_conversionCount;
  int            m_errorCount;
  int            m_sumLength, m_maxLength;
  ConversionTest(TestStatistic &stat);
  ~ConversionTest();
  void inline test(double error) {
    m_conversionCount++;
    if(error != 0) {
      m_errorCount++;
      if(error > m_maxError) m_maxError = error;
    }
  }
  void inline addLength(int length) {
    m_sumLength += length;
    if(length > m_maxLength) m_maxLength = length ;
  }
  String toString() const;
};

ConversionTest::ConversionTest(TestStatistic &stat) : m_stat(stat) {
  m_maxError        = 0;
  m_conversionCount = 0;
  m_errorCount      = 0;
  m_sumLength       = 0;
  m_maxLength       = 0;
}

ConversionTest::~ConversionTest() {
  m_stat.setEndMessage(_T("%s"), toString().cstr());
}

String ConversionTest::toString() const {
  return format(_T("errors/conv:%4d/%-8d Max rel.error:%le, avgLen:%6.4lf maxLen:%d")
               ,m_errorCount
               ,m_conversionCount
               ,m_maxError
               ,(double)m_sumLength/m_conversionCount
               ,m_maxLength
               );
}

static void testFloatConversion(TestStatistic &stat, int sign) {
  DigitPool     *pool      = stat.getDigitPool();
  const float    loopStart = sign * FLT_MIN;
  const float    loopEnd   = sign * FLT_MAX / 2;
  const float    stepFactor = 1 + 0.0012345f / 2;
  int            length;
  ConversionTest convTest(stat);
  stat.setTotalTestCount(284190);

  if(sign > 0) {
    for(float f32 = loopStart; f32 < loopEnd; f32 *= stepFactor) {
      convTest.test(getRelativeError32(f32, pool, &length));
      convTest.addLength(length);
      if(stat.isTimeToPrint()) stat.printLoopMessage(_T("%s"), stat.getCountStr().cstr());
    }
  } else {
    for(float f32 = loopStart; f32 > loopEnd; f32 *= stepFactor) {
      convTest.test(getRelativeError32(f32, pool, &length));
      convTest.addLength(length);
      if(stat.isTimeToPrint()) stat.printLoopMessage(_T("%s"), stat.getCountStr().cstr());
    }
  }
}

static void testDoubleConversion(TestStatistic &stat, int sign) {
  DigitPool     *pool       = stat.getDigitPool();
  const double   loopStart  = sign * DBL_MIN;
  const double   loopEnd    = sign * DBL_MAX / 2;
  const double   stepFactor = 1 + 0.00456789 / 2;
  int            length;
  ConversionTest convTest(stat);
  stat.setTotalTestCount(621339);

  if(sign > 0) {
    for(double d64 = loopStart; d64 < loopEnd; d64 *= stepFactor) {
      convTest.test(getRelativeError64(d64, pool, &length));
      convTest.addLength(length);
      if(stat.isTimeToPrint()) stat.printLoopMessage(_T("%5.1lf%%"), stat.getPercentDone());
    }
  } else {
    for(double d64 = loopStart; d64 > loopEnd; d64 *= stepFactor) {
      convTest.test(getRelativeError64(d64, pool, &length));
      convTest.addLength(length);
      if(stat.isTimeToPrint()) stat.printLoopMessage(_T("%5.1lf%%"), stat.getPercentDone());
    }
  }
}

static void testDouble80Conversion(TestStatistic &stat, int sign) {
  DigitPool     *pool       = stat.getDigitPool();
  const Double80 loopStart  = sign * Double80::DBL80_MIN;
  const Double80 loopEnd    = sign * Double80::DBL80_MAX / 2;
  const Double80 stepFactor = 1 + 0.012345/4;
  int            length;
  ConversionTest convTest(stat);
  stat.setTotalTestCount(7370108);

  if(sign > 0) {
    for(Double80 d80 = loopStart; d80 < loopEnd; d80 *= stepFactor) {
      convTest.test(getRelativeError80(d80, pool, &length));
      convTest.addLength(length);
      if(stat.isTimeToPrint()) stat.printLoopMessage(_T("%5.1lf%%"), stat.getPercentDone());
    }
  } else {
    for(Double80 d80 = loopStart; d80 > loopEnd; d80 *= stepFactor) {
      convTest.test(getRelativeError80(d80, pool, &length));
      convTest.addLength(length);
      if(stat.isTimeToPrint()) stat.printLoopMessage(_T("%5.1lf%%"), stat.getPercentDone());
    }
  }
}

void testPositiveFloatConversion(TestStatistic &stat) {
  testFloatConversion(stat, 1);
}

void testNegativeFloatConversion(TestStatistic &stat) {
  testFloatConversion(stat, -1);
}

void testPositiveDoubleConversion(TestStatistic &stat) {
  testDoubleConversion(stat, 1);
}

void testNegativeDoubleConversion(TestStatistic &stat) {
  testDoubleConversion(stat, -1);
}

void testPositiveDouble80Conversion(TestStatistic &stat) {
  testDouble80Conversion(stat, 1);
}

void testNegativeDouble80Conversion(TestStatistic &stat) {
  testDouble80Conversion(stat, -1);
}

void testGetExpo2(TestStatistic &stat) {
  DigitPool     *pool       = stat.getDigitPool();
  const Double80 startValue = Double80::DBL80_MIN;
  const Double80 stepFactor = 1.00012345;

  ConversionTest convTest(stat);

  const String intervalString = format(_T("[%s..%s]"), toString(startValue).cstr(), toString(Double80::DBL80_MAX).cstr());
  const Double80 loopStart  = startValue;
  stat.out() << _T("Start:") << loopStart << _T(", Step:") << stepFactor << NEWLINE;
  for(Double80 x = loopStart; !isNan(x); x *= stepFactor) {
    convTest.test(getRelativeError80(x, pool));

    if(stat.isTimeToPrint()) {
      stat.printLoopMessage(_T("x:%s %s"), x.toString().cstr(), intervalString.cstr());
    }
  }
}
