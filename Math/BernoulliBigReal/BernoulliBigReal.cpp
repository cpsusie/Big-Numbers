#include "stdafx.h"
#include <HashMap.h>
#include <InputValue.h>
#include <Math/BigReal/BigReal.h>

BigRational bernoulliDirect(UINT n) {
  if(n > 1 && (n & 1) == 1) {
    return BigRational::_0;
  }

  BigRational *A = new BigRational[n+1];
  DigitPool *pool = A->getDigitPool();

  for(UINT m = 0; m <= n; m++) {
    A[m] = BigRational(BigReal::_1,m+1);
    for(int j = m; j >= 1; j--) {
      A[j-1] = BigRational(j, pool) * (A[j-1] - A[j]);
    }
  }
  BigRational result = A[0];
  delete[] A;
  return result;
}

class BinomialKey {
private:
  int m_n,m_k;
public:
  inline BinomialKey(int n, int k) : m_n(n), m_k(k) {
  }
  inline ULONG hashCode() const {
    return m_n * 100 + m_k;
  }
  inline bool operator==(const BinomialKey &key) const {
    return (m_n == key.m_n) && (m_k == key.m_k);
  }
};

static inline ULONG binomialKeyHashCode(const BinomialKey &key) {
  return key.hashCode();
}

static inline int binomialKeyCmp(const BinomialKey &key1, const BinomialKey &key2) {
  return key1 == key2 ? 0 : 1;
}

const BigInt &factorial(UINT n) {
  static IntHashMap<BigInt> map;
  const BigInt *result = map.get(n);
  if(result == nullptr) {
    if(n <= 1) {
      map.put(n,1);
    } else {
      map.put(n,factorial(n-1)*n);
    }
    result = map.get(n);
  }
  return *result;
}

const BigInt &binomial(UINT n, UINT k) {
  static HashMap<BinomialKey, BigInt> map(binomialKeyHashCode,binomialKeyCmp);

  const BinomialKey key(n,k);
  const BigInt *result = map.get(key);
  if(result == nullptr) {
    map.put(key, factorial(n)/(factorial(k)*factorial(n-k)));
    result = map.get(key);
  }
  return *result;
}

const BigRational &bernoulli0(UINT m) {
  static IntHashMap<BigRational> map;
  const BigRational *result = map.get(m);
  if(result == nullptr) {
    if(m == 0) {
      map.put(m,1);
    } else {
      if(m > 1 && (m&1)) {
        map.put(m,0);
      } else {
        BigRational sum = (m == 0) ? 1 : 0;
        for(UINT k = 0; k < m; k++) {
          sum -= BigRational(binomial(m,k)) * bernoulli0(k) / (m-k+1);
        }
        map.put(m,sum);
      }
    }
    result = map.get(m);
  }
  return *result;
}

const BigRational &bernoulli1(UINT m) {
  static IntHashMap<BigRational> map;
  const BigRational *result = map.get(m);
  if(result == nullptr) {
    if(m == 0) {
      map.put(m,1);
    } else {
      if(m > 1 && (m&1)) {
        map.put(m,0);
      } else {
        BigRational sum = 1;
        for(UINT k = 0; k < m; k++) {
          sum -= BigRational(binomial(m,k)) * bernoulli1(k) / (m-k+1);
        }
        map.put(m,sum);
      }
    }
    result = map.get(m);
  }
  return *result;
}

int main(int argc, char **argv) {
  try {
    for(int i = 1; i < 200; i+= (i<2)?1:2) {
      _tprintf(_T("%d %-s\n"), i, toString(bernoulli0(i)).cstr());
    }
    return 0;

    for(;;) {
      int n = inputValue<int>(_T("Enter n:"));
      _tprintf(_T("bernoulli(%d)=%s\n"),n, toString(bernoulli1(n)).cstr());
    }
  } catch(Exception e) {
    _tprintf(_T("Exception:%s\n"), e.what());
    return -1;
  }
  return 0;
}
