#include "pch.h"

BigReal &BigReal::productMT(BigReal &result, const BigReal &x, const BigReal &y, const BigReal &f, intptr_t w, int level) { // static
  assert(isNormalProduct(x, y) && f._isfinite() && (x.getLength() >= y.getLength()));
  const bool   sameXY  = &x == &y;
  const size_t XLength = x.getLength();
  const size_t YLength = sameXY ? XLength : y.getLength();
  DigitPool    *pool   = result.getDigitPool();

  LOGPRODUCTRECURSION(_T("result.pool:%2d, x.len,y.len,w:(%4s,%4s,%4s)")
                     ,pool->getId(), format1000(XLength).cstr(),format1000(YLength).cstr(), format1000(w).cstr());

  if((YLength <= s_splitLength) || (w <= (intptr_t)s_splitLength)) {
//    _tprintf(_T("shortProd x.length:%3d y.length:%3d w:%d\n"),y.length(),w);
    return result.shortProductNoNormalCheck(x, y, f.m_expo);
  }

  const BigInt &_0 = pool->_0();
  const BigReal  g = f.isZero() ? _0 : APCprod(#, BigReal::_C1third,f,pool);
  BigReal gpm10(g);
  gpm10.multPow10(-10,true);
  const intptr_t n = min((intptr_t)XLength, w)/2;

  BigReal a(pool), b(pool);
  level++;
  x.split(a, b, n, g.isZero() ? _0 : APCprod(#, gpm10, reciprocal(y, pool),pool));   // a + b = x   (=O(n))

  MThreadArray threads;

  if(!sameXY && ((intptr_t)YLength < n)) {
    BigRealResourcePool::fetchMTThreadArray(threads, 1);

    MultiplierThread &thread = threads.get(0);

    BigReal p1(thread.getDigitPool());
    thread.multiply(p1, a,y,_0, level);

    result.setToZero();
    product(result, b, y, g, level);

    threads.waitForAllResults();

    result += p1;
    p1.setToZero();

    return result;
  }

  const BRExpoType logBK = BIGREAL_LOG10BASE * n;
  BigReal Kg(g);
  Kg.multPow10(logBK, true);
  BigRealResourcePool::fetchMTThreadArray(threads, 2);
  MultiplierThread &threadR = threads.get(0), &threadT = threads.get(1);
  BigReal r(threadR.getDigitPool()), s(pool), t(threadT.getDigitPool());

  b.multPow10(logBK, true);
  if(sameXY) {
    threadR.multiply(r, a    , a    , _0, level);
    threadT.multiply(t, b    , b    , Kg, level);
    const BigReal sumAB(a + b);
    product(         s, sumAB, sumAB, Kg, level);

    threads.waitForAllResults();

  } else {
    BigReal c(pool), d(pool);
    y.split(c, d, n, g.isZero() ? _0 : APCprod(#, gpm10, reciprocal(x, pool),pool));               // c + d = y   O(n)
    d.multPow10(logBK,true);

    threadR.multiply(r, a    , c    , _0, level);
    threadT.multiply(t, b    , d    , Kg, level);
    product(         s, a + b, c + d, Kg, level);

    threads.waitForAllResults();
  }

  s -= r;
  s -= t;
  s.multPow10(-logBK,true);
  r += s;
  t.multPow10(-2*logBK,true);
  result = sum(r, t, g);

  r.setToZero();
  s.setToZero();
  t.setToZero();
  return result;
}
