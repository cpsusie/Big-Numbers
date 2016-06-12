#include "pch.h"
#include <Math/Double80.h>
#include <Math/Expression/PrimeFactors.h>

class Primes {
private:
  static Primes *s_instance;
  BitSet         m_primes;

  Primes();
  void removeNonPrimes(unsigned int start);     // remove all nonprimes from m_primes
  void extendPrimeSet(unsigned int upperLimit); // extend m_primes to contain all primes from start..upperLimit (incl)
  friend void deallocatePrimesInstance();
public:
  Iterator<unsigned int> getIterator(unsigned int upperLimit);
  static Primes &getInstance();
};

Primes::Primes() : m_primes(3) {
  m_primes.add(2);
}

void Primes::extendPrimeSet(unsigned int upperLimit) { // extend m_primes to contain all primes [2..upperLimit]
  const unsigned int lastUpperLimit = m_primes.getCapacity() - 1;
  if(upperLimit > lastUpperLimit) {
    m_primes.setCapacity(upperLimit+1);
    m_primes.add(lastUpperLimit+1, upperLimit);
    removeNonPrimes(lastUpperLimit);
  }
}

void Primes::removeNonPrimes(unsigned int start) {
  const unsigned int upper = m_primes.getCapacity()-1;
  for(Iterator<unsigned int> it = m_primes.getIterator(); it.hasNext();) {
    const unsigned int p = it.next();
    const unsigned int q = max(start / p,1) + 1;
    for(unsigned int j = q*p; j <= upper; j += p) {
      m_primes.remove(j);
    }
  }
}

Iterator<unsigned int> Primes::getIterator(unsigned int upperLimit) {
  extendPrimeSet(upperLimit);
  return m_primes.getIterator();
}

static void deallocatePrimesInstance() {
  if(Primes::s_instance != NULL) {
    delete Primes::s_instance;
    Primes::s_instance = NULL;
  }
}

Primes *Primes::s_instance = NULL;

Primes &Primes::getInstance() { // static
  if(s_instance == NULL) {
    s_instance = new Primes;
    atexit(deallocatePrimesInstance);
  }
  return *s_instance;
}

PrimeFactorArray::PrimeFactorArray(__int64 n) {
  if(n >= 0) {
    m_positive = true;
  } else {
    n = -n;
    m_positive = false;
  }
  const unsigned long upperLimit = getLong(sqrt(Double80(n))) + 1;
  for(Iterator<unsigned int> it = Primes::getInstance().getIterator(upperLimit); it.hasNext();) {
    const unsigned int p = it.next();
    if(n % p == 0) {
      PrimeFactor pf(p);
      for(n /= p; n % p == 0; n /= p) {
        pf.m_multiplicity++;
      }
      INITPRIMEFACTORDEBUGSTRING(pf);
      add(pf);
    }
    if((n == 1) || (p > upperLimit)) {
      break;
    }
  }
  if(n != 1) {
    add(PrimeFactor(n));
  }
  INITPRIMEFACTORARRAYDEBUGSTRING(*this);
}

PrimeFactorSet PrimeFactorArray::findFactorsWithMultiplicityAtLeast(unsigned int m) const {
  PrimeFactorSet result(size() + 1);
  for(size_t i = 0; i < size(); i++) {
    const PrimeFactor &pf = (*this)[i];
    if(pf.m_multiplicity >= m) {
      result.add(i);
    }
  }
  INITPRIMEFACTORSETDEBUGSTRING(result, *this);
  return result;
}

__int64 PrimeFactorArray::getProduct() const {
  __int64 result = m_positive ? 1 : -1;
  for(size_t i = 0; i < size(); i++) {
    const PrimeFactor &pf = (*this)[i];
    for(unsigned int j = 0; j < pf.m_multiplicity; j++) {
      result *= pf.m_prime;
    }
  }
  return result;
}

String PrimeFactorArray::toString() const {
  if(size() == 0) {
    return _T("1");
  } else {
    String result = (*this)[0].toString();
    for(size_t i = 1; i < size(); i++) {
      result += format(_T("*%s"), (*this)[i].toString().cstr());
    }
    return m_positive ? result : format(_T("-%s"), result.cstr());
  }
}

#ifdef USEPRIMEFACTORDEBUGINFO

void PrimeFactorSet::initDebugString(const PrimeFactorArray &a) {
  m_debugString = a.isPositive() ? _T("{") : _T("{-1");
  const TCHAR *delimiter = _T("");
  for(Iterator<unsigned int> it = getIterator(); it.hasNext(); delimiter = _T(",")) {
    m_debugString += format(_T("%s%u"), delimiter, a[it.next()].m_prime);
  }
  m_debugString += _T("}");
}

#endif
