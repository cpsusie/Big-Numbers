#include "stdafx.h"
#include <Math/Rational.h>
#include <Math/PrimeFactors.h>

void hullDobellTest(UINT64 a, UINT64 c, UINT64 m) {
  if (m == 0) {
    throwException(_T("m == 0. m must > 0"));
  }
  if(a <= 1) {
    throwException(_T("a must be > 1"));
  }
  if(a >= m) {
    throwException(_T("a >= m. a must be < m"));
  }
  if (c >= m) {
    throwException(_T("c >= m. c must be < m"));
  }
  if (m % 4 == 0) {
    if ((a - 1) % 4 != 0) {
      throwException(_T("4 divides m, but not a-1"));
    }
  }
  INT64 gcd;
  if ((c > 0) && ((gcd=Rational::findGCD(m, c)) != 1)) {
    throwException(_T("m and c are not relatively prime. (gcd=%I64d"), gcd);
  }
  const PrimeFactorArray mfactors(m);
  for(size_t i = 0; i < mfactors.size(); i++) {
    const UINT64 pf = mfactors[i].m_prime;
    if((a - 1) % pf != 0) {
      PrimeFactorArray afactors(a-1);
      throwException(_T("Primefactor %llu of m does not divide a-1\n"
                        "Factors of m:%s\n"
                        "Factors of (a-1):%s\n")
                    ,pf
                    ,mfactors.toString().cstr()
                    ,afactors.toString().cstr());
    }
  }
}

int main(int argc, char **argv) {
  for(;;) {
    const UINT64 a = inputUint64(_T("Enter a:"));
    const UINT64 c = inputUint64(_T("Enter c:"));
    const UINT64 m = inputUint64(_T("Enter m:"));

    try {
      hullDobellTest(a, c, m);
      _tprintf(_T("x = (x * %llu + %llu) mod %llu has full period\n")
              ,a,c,m);
    } catch(Exception e) {
      _tprintf(_T("%s\n"), e.what());
    }
  }
  return 0;
}

