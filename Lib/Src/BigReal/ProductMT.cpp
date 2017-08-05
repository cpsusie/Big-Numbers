#include "pch.h"

static const ConstBigReal C1third = 0.33333333333;

BigReal &BigReal::productMT(BigReal &result, const BigReal &x, const BigReal &y, const BigReal &f, intptr_t w, int level) { // static
  const size_t XLength = x.getLength();
  const size_t YLength = y.getLength();
  DigitPool    *pool   = result.getDigitPool();

  TRACERECURSION(level, _T("productMT(result.pool:%2d, x.len,y.len,w:(%4s,%4s,%4s))")
                 ,pool->getId(), format1000(XLength).cstr(),format1000(YLength).cstr(), format1000(w).cstr());

  if(YLength <= s_splitLength || w <= (intptr_t)s_splitLength) {
//    _tprintf(_T("shortProd x.length:%3d y.length:%3d w:%d\n"),y.length(),w);
    return result.shortProduct(x, y, f.m_expo);
  }


  const BigReal g = PAPCprod(#,C1third,f,pool);
  BigReal gpm10(g);
  gpm10.multPow10(-10);
  const intptr_t n = min((intptr_t)XLength, w)/2;

  BigReal a(pool), b(pool);
  level++;
  x.split(a, b, n, g.isZero() ? pool->get0() : PAPCprod(#, gpm10, reciprocal(y, pool),pool));   // a + b = x   (=O(n))


  MThreadArray threads;
  if((intptr_t)YLength < n) {
    BigRealResourcePool::fetchMTThreadArray(threads, 1);

    MultiplierThread &thread = threads.get(0);

    BigReal p1(thread.getDigitPool());
    thread.multiply(p1, a,y,pool->get0(), level);

    result = pool->get0();
    product(result, b, y, g, level);

    threads.waitForAllResults();

    result += p1;
    p1.setToZero();

    return result;
  }

  BigReal c(pool), d(pool);
  y.split(c, d, n, g.isZero() ? pool->get0() : PAPCprod(#, gpm10, reciprocal(x, pool),pool));               // c + d = y   O(n)

  const BRExpoType logBK = LOG10_BIGREALBASE * n;

  b.multPow10(logBK);
  d.multPow10(logBK);
  BigReal Kg(g);
  Kg.multPow10(logBK);

  BigRealResourcePool::fetchMTThreadArray(threads, 2);

  MultiplierThread &threadR = threads.get(0);
  MultiplierThread &threadS = threads.get(1);

  BigReal r(threadR.getDigitPool());
  BigReal s(threadS.getDigitPool());
  BigReal t(pool);

  BigReal tmpAB(a, threadS.getDigitPool());
  tmpAB += b;
  BigReal tmpCD(c, threadS.getDigitPool());
  tmpCD += d;

  threadR.multiply(r, a    , c    , pool->get0(), level);
  threadS.multiply(s, tmpAB, tmpCD, Kg          , level);
  product(         t, b    , d    , Kg          , level);

  threads.waitForAllResults();

  tmpAB.setToZero();
  tmpCD.setToZero();

  s -= r;
  s -= t;
  s.multPow10(-logBK);
  r += s;
  t.multPow10(-2*logBK);
  result = sum(r, t, g);

  r.setToZero();
  s.setToZero();

  return result;
}
