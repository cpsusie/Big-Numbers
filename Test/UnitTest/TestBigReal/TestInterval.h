#pragma once

class TestInterval : public BigRealInterval {
private:
  const bool m_exponentialStep;
public:
  TestInterval(const BigReal &from, const BigReal &to, bool exponentialStep)
    : BigRealInterval(from, to)
    , m_exponentialStep(exponentialStep)
  {
  }
  inline bool isExponentialStep() const {
    return m_exponentialStep;
  }
};
