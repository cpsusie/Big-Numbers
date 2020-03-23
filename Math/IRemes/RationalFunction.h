#pragma once

#include <Math/MathLib.h>
#include <Math/BigReal/BigReal.h>
#include <Math/BigReal/BigRealMatrix.h>
#include <Math/BigReal/BigRealInterval.h>

class BigRealFunction : public FunctionTemplate<BigReal, BigReal> {
};

class RationalFunction : public BigRealFunction {
private:
  const ConstBigRealInterval *m_domain;
  UINT                        m_M, m_K, m_N;      // m_N = m_M + m_K
  Array<ConstBigReal>         m_coefVector;       // Coefficient[0..N+1] = { a[0]..a[M], b[1]..b[K], E }. b[0] = 1. Dim=N+2
  int                         m_coefVectorIndex;
  UINT                        m_digits;           // digits of precision in calculations
  void copyData(const RationalFunction &src);
public:
  RationalFunction();
  RationalFunction(const RationalFunction &src);
  RationalFunction(const ConstBigRealInterval &domain, UINT m, UINT k, const BigRealVector &coef, int coefVectorIndex, UINT digits);
  ~RationalFunction();
  RationalFunction &operator=(const RationalFunction &src);
  void clear();
  inline bool isEmpty() const {
    return m_N == 0;
  }
  inline UINT getDigits() const {
    return m_digits;
  }
  inline int getCoefVectorIndex() const {
    return m_coefVectorIndex;
  }
  inline const ConstBigRealInterval &getDomain() const {
    return *m_domain;
  }
  // Use x.digitpool to do calculation
  BigReal operator()(const BigReal &x);
  bool operator==(RationalFunction &rhs) const;
  inline bool operator!=(RationalFunction &rhs) const {
    return !(*this == rhs);
  }
};
