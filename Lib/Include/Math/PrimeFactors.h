#pragma once

#include <BitSet.h>

class PrimeFactor {
public:
  UINT64 m_prime;
  UINT   m_multiplicity;
  inline PrimeFactor() : m_prime(0), m_multiplicity(0) {
  }
  inline PrimeFactor(UINT64 prime) : m_prime(prime), m_multiplicity(1) {
  }
  inline String toString() const {
    return (m_multiplicity==1) ? format(_T("%I64u"), m_prime) : format(_T("%I64u^%lu"), m_prime, m_multiplicity);
  }
};

class PrimeFactorSet : public BitSet {
public:
  inline explicit PrimeFactorSet(UINT capacity) : BitSet(capacity) {
  }
};

class PrimeFactorArray : public CompactArray<PrimeFactor> {
private:
  bool m_positive;
public:
  PrimeFactorArray(INT64 n);
  PrimeFactorSet findFactorsWithMultiplicityAtLeast(UINT m) const;
  // return list with all positive int-factors of n, ie. ignore sign
  CompactInt64Array getAllFactors() const;
  INT64 getProduct() const;
  bool isPositive() const {
    return m_positive;
  }
  void setPositive() {
    m_positive = true;
  }
  int sign() const {
    return m_positive ? 1 : -1;
  }
  String toString() const;
};
