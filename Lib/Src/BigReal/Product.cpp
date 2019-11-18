#include "pch.h"

#ifdef USE_X32SERVERCHECK
#include <ExternProcess.h>

class MultiplyServer : public ExternProcess {
private:
  Semaphore m_gate;
public:
  BigReal &mult(BigReal &dst, const BigReal &x, const BigReal &y, const BigReal &f);
  void startup();
};

void MultiplyServer::startup() {
  m_gate.wait();
  if (!isStarted()) {
    start(false, _T("c:\\bin\\multiplicationServer.exe"), NULL);
//    setVerbose(true);
  }
  m_gate.signal();
}

BigReal &MultiplyServer::mult(BigReal &dst, const BigReal &x, const BigReal &y, const BigReal &f) {
  DigitPool *pool = dst.getDigitPool();
  FullFormatBigReal xf(x, pool), yf(y, pool), ff(f, pool);
  std::wstringstream xs, ys, fs;
  xs << xf;
  ys << yf;
  fs << ff;

  m_gate.wait();
  try {
    send(_T("%s %s %s\n"), xs.str().c_str(), ys.str().c_str(), fs.str().c_str());
    const String line = receive();
    dst = BigReal(line, pool);
  } catch (Exception e) {
    _tprintf(_T("Exception:%s\n"), e.what());
    dst = 0;
  } catch (...) {
    _tprintf(_T("Unknown exception\n"));
    dst = 0;
  }
  m_gate.signal();
  return dst;
}

#endif // USE_X32SERVERCHECK

#ifdef TRACEPRODUCTRECURSION
void logProductRecursion(UINT level, const TCHAR *method, _In_z_ _Printf_format_string_ TCHAR const * const format,...) {
  va_list argptr;
  va_start(argptr, format);
  debugLog(_T("%*.*s%2u:%s(%s)\n"), level,level, EMPTYSTRING, level, method, vformat(format,argptr).cstr());
  va_end(argptr);
}
#endif // TRACEPRODUCTRECURSION

BigReal &BigReal::shortProductNoNormalCheck(const BigReal &x, const BigReal &y, BRExpoType fexpo) {
  assert(isNormalProduct(x, y));
  if(!m_digitPool.continueCalculation()) throwBigRealException(_T("Operation was cancelled"));
  const BRExpoType fm = fexpo - 2;
  const BRExpoType lm = x.m_low + y.m_low;
  const BRExpoType loopCount = (x.m_expo + y.m_expo) - max(fm, lm) + 2;

  if(loopCount <= 0) { // result is zero
    return setToZero();
  }

#ifdef USE_X32SERVERCHECK
  static bool firstTime = true;
  static MultiplyServer s_multiplyServer;

  if (firstTime) {
    firstTime = false;
    s_multiplyServer.startup();
  }
  DigitPool *pool = getDigitPool();
  BigReal ff(pool), serverResult(pool), error(pool);
  ff = (fexpo == BIGREAL_NONNORMAL) ? BigReal::_0 : (e(BigReal::_1, fexpo * BIGREAL_LOG10BASE, pool));
  s_multiplyServer.mult(serverResult, x, y, ff);

  shortProductNoZeroCheck(x, y, (UINT)loopCount);
  error = serverResult - *this;
  if(compareAbs(error, ff) > 0) {
    shortProductNoZeroCheck(x, y, (UINT)loopCount);
  }
  return *this;
#else
  return shortProductNoZeroCheck(x, y, (UINT)loopCount);
#endif
}

// Assume (a.isZero() && b.isZero() && _isnormal() && f._isfinite());
void BigReal::split(BigReal &a, BigReal &b, size_t n, const BigReal &f) const {
  assert(a.isZero() && b.isZero() && _isnormal() && f._isfinite());
  intptr_t i = n;
  const Digit *p;
  for(p = m_first; i-- && p; p = p->next) {
    a.appendDigit(p->n);
  }
  a.m_expo     = m_expo;
  a.copySign(*this);
  a.m_low      = a.m_expo - (n-i-2);
  if(p && ((i = (a.m_low - f.m_expo)) > 0)) {
    for(; i-- && p; p = p->next) {
      b.appendDigit(p->n);
    }

    // i = a.low - f.expo - (#digits in b = loopCount)-1

    b.m_expo     = a.m_low - 1;
    b.copySign(*this);
    b.m_low      = f.m_expo + i + 1; // NB:This works for any f (even 0).
                                     // f.expo + i + 1 = f.expo + (a.low-f.expo-loopCount-1) + 1
                                     // = a.low-loopCount.
                                     // b.expo = a.low-1 => a.low = b.expo+1. =>
                                     // b.low = (b.expo+1)-loopCount = b.expo-#digits in b + 1
    b.trimZeroes();
  }
  a.trimZeroes();                  // dont trim zeroes from a before b has been generated!
}

BigReal &BigReal::product(BigReal &result, const BigReal &x, const BigReal &y, const BigReal &f, int level) { // static
  assert(isNormalProduct(x,y) && f._isfinite());
  //  _tprintf(_T("length(X):%5d length(Y):%5d\n"),length(x),length(y));
  const bool      sameXY  = &x == &y;
  const bool      swapXY  = !sameXY && (x.getLength() < y.getLength());
  const BigReal  &X       = swapXY ? y : x;
  const BigReal  &Y       = swapXY ? x : y;
  const size_t    XLength = X.getLength(), YLength = sameXY ? XLength : Y.getLength();
  DigitPool      *pool    = result.getDigitPool();
  const intptr_t  w1      = XLength + YLength;
  intptr_t        w;

  if(f.isZero()) {
    w = w1;
  } else {
    const intptr_t w2 = X.m_expo + Y.m_expo - f.m_expo;
    w = min(w2, w1);
  }

  LOGPRODUCTRECURSION(_T("result.pool:%2d, x.len,y.len,w:(%4d,%4d,%4d)"),pool->getId(), XLength,YLength, w);

  if((YLength <= s_splitLength) || (w <= (intptr_t)s_splitLength)) {
//    _tprintf(_T("shortProd X.length:%3d Y.length:%3d w:%d\n"),Y.length(),w);
    return result.shortProductNoNormalCheck(X, Y, f.m_expo);
  }
  if(level < 3) {
    return productMT(result, X, Y, f, w, level+1);
  }
  const BigInt &_0 = pool->_0();
  const BigReal  g = f.isZero() ? _0 : APCprod(#, BigReal::_C1third, f, pool);
  BigReal gpm10(g);
  gpm10.multPow10(-10,true);
  const intptr_t n = min((intptr_t)XLength, w)/2;
  BigReal a(pool), b(pool);
  level++;
  X.split(a, b, n, g.isZero() ? _0 : APCprod(#,gpm10,reciprocal(Y,pool),pool));                 // a + b = X   O(n)

  if(!sameXY && ((intptr_t)YLength < n)) {                                                      //
    BigReal p1(pool),p2(pool);                                                                  //
    return result = product(p1, a, Y, _0, level) + product(p2, b, Y, g, level);                 // a*Y+b*Y     O(2*n/2*n+n/2)
  }

  const BRExpoType logBK = BIGREAL_LOG10BASE * n;
  BigReal r(pool), s(pool), t(pool);                                   //                                      O(1)

  if(sameXY) {
    const BigReal &c = a, &d = b;                                      // => a+b == c+d
    b.multPow10(logBK, true);                                          //                                      O(1)
//  d.multPow10(logBK); Done in last statement
    BigReal Kg(g);                                                     //                                      O(1)
    Kg.multPow10(logBK, true);                                         //                                      O(1)

    product(r, a  , c  , _0, level);                                   // &a=&c. r = a * a                     O((n/2)^2)      = O((n^2)/4)
    const BigReal AplusB = a + b;
    product(s, AplusB, AplusB, Kg, level);                             // s = (a+Kb) * (c+Kd)                  O((n/2)^2+2n/2) = O((n^2)/4+n)
    product(t, b  , d  , Kg, level);                                   // &b==&d. t = Kb * Kd                  O((n/2)^2)      = O((n^2)/4)

  } else {
    BigReal c(pool), d(pool);
    Y.split(c, d, n, g.isZero() ? _0 : APCprod(#,gpm10,reciprocal(X, pool),pool));              // c + d = Y   O(n)
                                                                       //
    b.multPow10(logBK, true);                                          //                                      O(1)
    d.multPow10(logBK, true);                                          //                                      O(1)
    BigReal Kg(g);                                                     //                                      O(1)
    Kg.multPow10(logBK, true);                                         //                                      O(1)
    BigReal r(pool), s(pool), t(pool);                                 //                                      O(1)
                                                                       //
    product(r, a  , c  , _0, level);                                   // r = a * c                            O((n/2)^2)      = O((n^2)/4)
    product(s, a+b, c+d, Kg, level);                                   // s = (a+Kb) * (c+Kd)                  O((n/2)^2+2n/2) = O((n^2)/4+n)
    product(t, b  , d  , Kg, level);                                   // t = Kb * Kd                          O((n/2)^2)      = O((n^2)/4)
  }
                                                                       //
  s -= r;                                                              //                                      O(n)
  s -= t;                                                              // s = (a+Kb) * (c+Kd) - (a*c + b*d)    O(n)
  s.multPow10(-logBK,true);                                            // s /= K                               O(1)
  r += s;                                                              //                                      O(n)
  t.multPow10(-2*logBK, true);                                         // t /= K^2                             O(1)
  return result = sum(r, t, g);                                        // return sum(r+(s-(r+t))/K,t/(K*K),g)  O(n)
                                                                       //                              Total:  O(3/4(n^2) + 6n + k)
}

BigReal prod(const BigReal &x, const BigReal &y, const BigReal &f, DigitPool *digitPool) {
  BigReal result(digitPool ? digitPool : x.getDigitPool());
  result.clrInitDone();
  if(result.checkIsNormalProduct(x, y)) {
    BigReal::product(result, x, y, f, 0);
  }
  return result.setInitDone();
}

BigReal operator*(const BigReal &x, const BigReal &y) {
  DigitPool *pool = x.getDigitPool();
  BigReal result(pool);
  result.clrInitDone();
  if(result.checkIsNormalProduct(x, y)) {
    BigReal::product(result, x, y, pool->_0(), 0);
  }
  return result.setInitDone();
}

BigReal &BigReal::operator*=(const BigReal &x) {
  return *this = *this * x;
}
