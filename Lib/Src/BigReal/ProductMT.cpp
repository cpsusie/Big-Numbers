#include "pch.h"
#include <ThreadPool.h>
#include "BigRealResourcePool.h"

BigReal &BigReal::productMT(BigReal &result, const BigReal &x, const BigReal &y, const BigReal &f, intptr_t w, int level) { // static
  assert(isNormalProduct(x, y) && f._isfinite() && (x.getLength() >= y.getLength()));
  const bool   sameXY  = &x == &y;
  const size_t XLength = x.getLength();
  const size_t YLength = sameXY ? XLength : y.getLength();
  DigitPool    *pool   = result.getDigitPool();

  LOGPRODUCTRECURSION(_T("result.pool:%2u, x.len,y.len,w:(%4zu,%4zu,%4zd)"), pool->getId(), XLength,YLength, w);

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

  SubProdRunnableArray spa;

  if(!sameXY && ((intptr_t)YLength < n)) {
    BigRealResourcePool::fetchSubProdRunnableArray(spa, 2,1);
    BigReal p1(spa.getDigitPool(0));
    spa.getRunnable(0).setInOut(p1    , a, y, _0, level);
    spa.getRunnable(1).setInOut(result, b, y,  g, level);
    ThreadPool::executeInParallel(spa);
    result += p1;
    return result;
  }

  const BRExpoType logBK = BIGREAL_LOG10BASE * n;
  BigReal Kg(g);
  Kg.multPow10(logBK, true);
  BigRealResourcePool::fetchSubProdRunnableArray(spa, 3, 2);
  BigReal r(spa.getDigitPool(0)), s(pool), t(spa.getDigitPool(1));
  b.multPow10(logBK, true);
  if(sameXY) {
    spa.getRunnable(0).setInOut(r, a    , a    , _0, level);
    spa.getRunnable(1).setInOut(t, b    , b    , Kg, level);
    const BigReal sumAB(a + b);
    spa.getRunnable(2).setInOut(s, sumAB, sumAB, Kg, level);
    ThreadPool::executeInParallel(spa);
  } else {
    BigReal c(pool), d(pool);
    y.split(c, d, n, g.isZero() ? _0 : APCprod(#, gpm10, reciprocal(x, pool),pool));               // c + d = y   O(n)
    d.multPow10(logBK,true);

    spa.getRunnable(0).setInOut(r, a    , c    , _0, level);
    spa.getRunnable(1).setInOut(t, b    , d    , Kg, level);
    const BigReal sumAB(a + b), sumCD(c + d);
    spa.getRunnable(2).setInOut(s, sumAB, sumCD, Kg, level);
    ThreadPool::executeInParallel(spa);
  }

  s -= r;
  s -= t;
  s.multPow10(-logBK,true);
  r += s;
  t.multPow10(-2*logBK,true);
  result = sum(r, t, g);

  return result;
}
