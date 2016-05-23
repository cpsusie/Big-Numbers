#pragma once

#include <BitSet.h>

#define USEPRIMEFACTORDEBUGINFO

class PrimeFactor {
#ifdef USEPRIMEFACTORDEBUGINFO
private:
  String m_debugString;
public:
  void initDebugString() { m_debugString = toString(); }
  inline const String &getDebugString() const { return m_debugString; }
#define INITPRIMEFACTORDEBUGSTRING(p) (p).initDebugString()
#else
#define INITPRIMEFACTORDEBUGSTRING(p)
#endif
public:
  unsigned __int64 m_prime;
  unsigned int m_multiplicity;
  inline PrimeFactor() : m_prime(0), m_multiplicity(0) {
  }
  inline PrimeFactor(unsigned __int64 prime) : m_prime(prime), m_multiplicity(1) {
  }
  inline String toString() const {
    return (m_multiplicity==1) ? format(_T("%I64u"), m_prime) : format(_T("%I64u^%lu"), m_prime, m_multiplicity);
  }
};

class PrimeFactorArray;

class PrimeFactorSet : public BitSet {
#ifdef USEPRIMEFACTORDEBUGINFO
private:
  String m_debugString;
public:
  void initDebugString(const PrimeFactorArray &a);
#define INITPRIMEFACTORSETDEBUGSTRING(set, array) (set).initDebugString(array)
#else
#define INITPRIMEFACTORSETDEBUGSTRING(set, array)
#endif

public:
  inline explicit PrimeFactorSet(unsigned int capacity) : BitSet(capacity) {
  }
};

class PrimeFactorArray : public Array<PrimeFactor> {
#ifdef USEPRIMEFACTORDEBUGINFO
private:
  String m_debugString;
public:
  void initDebugString() { m_debugString = toString(); }
#define INITPRIMEFACTORARRAYDEBUGSTRING(a) (a).initDebugString()
#else
#define INITPRIMEFACTORARRAYDEBUGSTRING(a)
#endif
private:
  bool m_positive;
public:
  PrimeFactorArray(__int64 n);
  PrimeFactorSet findFactorsWithMultiplicityAtLeast(unsigned int m) const;
  __int64 getProduct() const;
  bool isPositive() const {
    return m_positive;
  }
  void setPositive() {
    m_positive = true;
    INITPRIMEFACTORARRAYDEBUGSTRING(*this);
  }
  int sign() const {
    return m_positive ? 1 : -1;
  }
  String toString() const;
};
