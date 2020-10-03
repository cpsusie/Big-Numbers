#include "pch.h"
#include <Singleton.h>
#include <FastSemaphore.h>
#include <Math/Double80.h>
#include <Math/PrimeFactors.h>

class Primes : public Singleton {
private:
  BitSet        m_primeSet;
  FastSemaphore m_lock;
  Primes() : Singleton(__TFUNCTION__), m_primeSet(3) {
    m_primeSet.add(2);
  }
  // remove all nonprimes from m_primeSet
  void removeNonPrimes(size_t start);
  // extend m_primeSet to contain all primes from start..upperLimit (incl)
  void extendPrimeSet(UINT newCapacity);
public:
  Iterator<size_t> getIterator(UINT upperLimit);
  inline Primes &wait() {
    m_lock.wait();
    return *this;
  }
  inline Primes &notify() {
    m_lock.notify();
    return *this;
  }
  DEFINESINGLETON(Primes);
};

// extend m_primeSet to contain all primes [2..newCapacity-1]
void Primes::extendPrimeSet(UINT newCapacity) {
  const size_t lastUpperLimit = m_primeSet.getCapacity() - 1;
  m_primeSet.setCapacity(newCapacity).add(lastUpperLimit+1, newCapacity-1);
  removeNonPrimes(lastUpperLimit);
}

void Primes::removeNonPrimes(size_t start) {
  const size_t upper = m_primeSet.getCapacity();
  for(Iterator<size_t> it = m_primeSet.getIterator(); it.hasNext();) {
    const size_t p = it.next();
    const size_t q = max(start / p,1) + 1;
    for(size_t j = q*p; j < upper; j += p) {
      m_primeSet.remove(j);
    }
  }
}

Iterator<size_t> Primes::getIterator(UINT upperLimit) {
  if(upperLimit >= m_primeSet.getCapacity()) {
    extendPrimeSet(upperLimit+1);
  }
  return m_primeSet.getIterator();
}

PrimeFactorArray::PrimeFactorArray(INT64 n, UINT limit) : m_positive(n >= 0) {
  if(n == 0) {
    return;
  }
  if(!m_positive) {
    n = -n;
  }
  const UINT upperLimit = limit ? limit : ((UINT)sqrt(Double80(n)) + 1);
  Primes &primes = Primes::getInstance().wait();
  for(Iterator<size_t> it = primes.getIterator(upperLimit); it.hasNext();) {
    const size_t p = it.next();
    if(n % p == 0) {
      PrimeFactor pf(p);
      for(n /= p; n % p == 0; n /= p) {
        pf.m_multiplicity++;
      }
      add(pf);
    }
    if((n == 1) || (p > upperLimit)) {
      break;
    }
  }
  primes.notify();
  if((n != 1) && (limit == 0)) {
    add(PrimeFactor(n));
  }
}

PrimeFactorSet PrimeFactorArray::findFactorsWithMultiplicityAtLeast(UINT m) const {
  PrimeFactorSet result((UINT)size() + 1);
  for(size_t i = 0; i < size(); i++) {
    const PrimeFactor &pf = (*this)[i];
    if(pf.m_multiplicity >= m) {
      result.add(i);
    }
  }
  return result;
}

PrimeFactorSet PrimeFactorArray::findFactorsWithMultiplicityLessThan(UINT m) const {
  PrimeFactorSet result((UINT)size() + 1);
  for(size_t i = 0; i < size(); i++) {
    const PrimeFactor &pf = (*this)[i];
    if(pf.m_multiplicity < m) {
      result.add(i);
    }
  }
  return result;
}

bool PrimeFactorArray::hasFactorsWithNonDividableMultiplicity(UINT m) const {
  for(size_t i = 0; i < size(); i++) {
    const PrimeFactor &pf = (*this)[i];
    if((pf.m_multiplicity % m) != 0) {
      return true;
    }
  }
  return false;
}

CompactInt64Array PrimeFactorArray::getAllFactors() const {
  const PrimeFactorArray &pfa        = *this;
  const UINT              digitCount = (UINT)pfa.size();
  CompactUintArray        digit(digitCount);
  CompactInt64Array       maxProd(digitCount);
  UINT resultCapacity = 1;
  for(UINT i = 0; i < digitCount; i++) {
    const PrimeFactor &pf = pfa[i];
    digit.add(0);
    resultCapacity *= (pf.m_multiplicity + 1);
    UINT64 pm = pf.m_prime;
    for(UINT j = 1; j < pf.m_multiplicity; j++) {
      pm *= pf.m_prime;
    }
    maxProd.add(pm);
  }
  CompactInt64Array result(resultCapacity);
  INT64 prod = 1; // all digits are zero
  while(result.size() < resultCapacity) {
    result.add(prod);
    for(UINT d = 0; d < digitCount;) {
      if(++digit[d] <= pfa[d].m_multiplicity) {
        prod *= pfa[d].m_prime;
        break;
      } else {
        digit[d] = 0;
        prod /= maxProd[d++];
      }
    }
  }
  return result;
}

INT64 PrimeFactorArray::getProduct() const {
  INT64 result = m_positive ? 1 : -1;
  for(size_t i = 0; i < size(); i++) {
    const PrimeFactor &pf = (*this)[i];
    for(UINT j = 0; j < pf.m_multiplicity; j++) {
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
