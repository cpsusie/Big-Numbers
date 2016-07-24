#include "pch.h"
#include <Math/Double80.h>
#include <Math/Expression/PrimeFactors.h>

class Primes {
private:
  static Primes *s_instance;
  BitSet         m_primes;

  Primes();
  void removeNonPrimes(size_t start);     // remove all nonprimes from m_primes
  void extendPrimeSet(unsigned int upperLimit); // extend m_primes to contain all primes from start..upperLimit (incl)
  friend void deallocatePrimesInstance();
public:
  Iterator<size_t> getIterator(unsigned int upperLimit);
  static Primes &getInstance();
};

Primes::Primes() : m_primes(3) {
  m_primes.add(2);
}

void Primes::extendPrimeSet(unsigned int upperLimit) { // extend m_primes to contain all primes [2..upperLimit]
  const size_t lastUpperLimit = m_primes.getCapacity() - 1;
  if(upperLimit > lastUpperLimit) {
    m_primes.setCapacity(upperLimit+1);
    m_primes.add(lastUpperLimit+1, upperLimit);
    removeNonPrimes(lastUpperLimit);
  }
}

void Primes::removeNonPrimes(size_t start) {
  const size_t upper = m_primes.getCapacity()-1;
  for(Iterator<size_t> it = m_primes.getIterator(); it.hasNext();) {
    const size_t p = it.next();
    const size_t q = max(start / p,1) + 1;
    for(size_t j = q*p; j <= upper; j += p) {
      m_primes.remove(j);
    }
  }
}

Iterator<size_t> Primes::getIterator(unsigned int upperLimit) {
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
  for(Iterator<size_t> it = Primes::getInstance().getIterator(upperLimit); it.hasNext();) {
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
  if(n != 1) {
    add(PrimeFactor(n));
  }
}

PrimeFactorSet PrimeFactorArray::findFactorsWithMultiplicityAtLeast(unsigned int m) const {
  PrimeFactorSet result((UINT)size() + 1);
  for(size_t i = 0; i < size(); i++) {
    const PrimeFactor &pf = (*this)[i];
    if(pf.m_multiplicity >= m) {
      result.add(i);
    }
  }
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
