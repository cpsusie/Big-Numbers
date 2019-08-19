#include <BasicIncludes.h>
#include <CompactArray.h>
#include <Iterator.h>
#include <Math/Double80.h>
#include <Math/BigReal.h>

class TestValueArrayElement {
  double    m_d;
  Double80  m_d80;
  BigReal   m_br;
public:
  TestValueArrayElement(double v);
  const double &getDouble() const {
    return m_d;
  }
  const Double80 &getDouble80() const {
    return m_d80;
  }
  const BigReal &getBigReal() const {
    return m_br;
  }
};

class TestValueArray : public Array<TestValueArrayElement> {
public:
  TestValueArray(const CompactDoubleArray &testValues);
  TestValueArray(const double *testValues, size_t n);
};
