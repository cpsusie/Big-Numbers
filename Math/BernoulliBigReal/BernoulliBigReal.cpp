#include "stdafx.h"
#include <HashMap.h>
#include <Math/BigReal.h>

BigRational bernoulliDirect(UINT n) {
  if(n > 1 && (n & 1) == 1) {
    return 0;
  }

  BigRational *A = new BigRational[n+1];

  for(UINT m = 0; m <= n; m++) {
    A[m] = BigRational(BIGREAL_1,m+1);
    for(int j = m; j >= 1; j--) {
      A[j-1] = j * (A[j-1] - A[j]);
    }
  }
  BigRational result = A[0];
  delete[] A;
  return result;
}

class BinomialKey {
private:
  int n,k;
public:
  BinomialKey(int n, int k);
  ULONG hashCode() const;
  bool operator==(const BinomialKey &key) const;
};

BinomialKey::BinomialKey(int n, int k) {
  this->n = n;
  this->k = k;
}

ULONG BinomialKey::hashCode() const {
  return n * 100 + k;
}

bool BinomialKey::operator==(const BinomialKey &key) const {
  return n == key.n && k == key.k;
}


static ULONG binomialKeyHashCode(const BinomialKey &key) {
  return key.hashCode();
}

static int binomialKeyCmp(const BinomialKey &key1, const BinomialKey &key2) {
  return key1 == key2 ? 0 : 1;
}

const BigInt &fac(int n) {
  static IntHashMap<BigInt> map;
  const BigInt *result = map.get(n);
  if(result == NULL) {
    if(n <= 1) {
      map.put(n,1);
    } else {
      map.put(n,n*fac(n-1));
    }
    result = map.get(n);
  }
  return *result;
}

const BigInt &binomial(UINT n, UINT k) {
  static HashMap<BinomialKey, BigInt> map(binomialKeyHashCode,binomialKeyCmp);

  const BinomialKey key(n,k);
  const BigInt *result = map.get(key);
  if(result == NULL) {
    map.put(key,fac(n)/(fac(k)*fac(n-k)));
    result = map.get(key);
  }
  return *result;
}

const BigRational &bernoulli0(UINT m) {
  static IntHashMap<BigRational> map;
  const BigRational *result = map.get(m);
  if(result == NULL) {
    if(m == 0) {
      map.put(m,1);
    } else {
      if(m > 1 && (m&1)) {
        map.put(m,0);
      } else {
        BigRational sum = (m == 0) ? 1 : 0;
        for(UINT k = 0; k < m; k++) {
          sum -= binomial(m,k) * bernoulli0(k) / (m-k+1);
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
  if(result == NULL) {
    if(m == 0) {
      map.put(m,1);
    } else {
      if(m > 1 && (m&1)) {
        map.put(m,0);
      } else {
        BigRational sum = 1;
        for(UINT k = 0; k < m; k++) {
          sum -= binomial(m,k) * bernoulli1(k) / (m-k+1);
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
      _tprintf(_T("%d %-s\n"), i, bernoulli0(i).toString().cstr());
    }
    return 0;

    for(;;) {
      int n = inputInt(_T("Enter n:"));
      _tprintf(_T("bernoulli(%d)=%s\n"),n,bernoulli1(n).toString().cstr());
    }
  } catch(Exception e) {
    _tprintf(_T("Exception:%s\n"),e.what());
    return -1;
  }
  return 0;
}
