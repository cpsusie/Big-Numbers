#include "stdafx.h"
#include "TestValueArray.h"

TestValueArray::TestValueArray(const CompactDoubleArray &testValues) {
  const size_t n = testValues.size();
  for(size_t i = 0; i < n; i++) {
    add(TestValueArrayElement(testValues[i]));
  }
}

TestValueArray::TestValueArray(const double *testValues, size_t n) {
  for(size_t i = 0; i < n; i++) {
    add(TestValueArrayElement(testValues[i]));
  }
}

TestValueArrayElement::TestValueArrayElement(double v) {
  m_d   = v;
  m_d80 = v;
  m_br  = v;

  const int d64expo10 = getExpo10(m_d);
  const int d80expo10 = Double80::getExpo10(m_d80);
  const int brexpo10  = (int)BigReal::getExpo10(m_br);
  if((d80expo10 != d64expo10) || (brexpo10 != d64expo10)) {
    throwException(_T("getExpo10(%20.14le) mismatch. (double):%3d, (Double80):%3d, (BigReal):%3d")
                  ,m_d, d64expo10, d80expo10, brexpo10
                  );
  }
}
