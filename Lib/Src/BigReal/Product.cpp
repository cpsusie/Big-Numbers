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
  BigRealStream xs, ys, fs;
  xs << xf;
  ys << yf;
  fs << ff;

  m_gate.wait();
  try {
    send(_T("%s %s %s\n"), xs.cstr(), ys.cstr(), fs.cstr());
    const String line = receive();
    dst = BigReal(line, pool);
  }
  catch (Exception e) {
    _tprintf(_T("Exception:%s\n"), e.what());
    dst = 0;
  }
  catch (...) {
    _tprintf(_T("Unknown exception\n"));
    dst = 0;
  }
  m_gate.signal();
  return dst;
}

#endif // USE_X32SERVERCHECK

#ifdef _DEBUG
void traceRecursion(int level, _In_z_ _Printf_format_string_ TCHAR const * const format,...) {
  const String spaces = spaceString(level);
  va_list argptr;
  va_start(argptr,format);
  String msg = vformat(format, argptr);
  va_end(argptr);
  debugLog(_T("%s%2d:%s\n"), spaces.cstr(), level, msg.cstr());
}
#endif // _DEBUG

BigReal BigReal::shortProd(const BigReal &x, const BigReal &y, const BigReal &f, DigitPool *pool) { // static
  BigReal result(pool);
  return result.shortProduct(x,y,f.m_expo);
}

// NOTE: fexpo is NOT decimal exponent, but Digit-exponent. To get Decimal exponent,
// multiply by LOG10_BIGREALBASE !!!
BigReal &BigReal::shortProduct(const BigReal &x, const BigReal &y, BRExpoType fexpo) {
  if(!s_continueCalculation) throwBigRealException(_T("Operation was cancelled"));

  if(x.isZero() || y.isZero()) {
    setToZero();
    return *this;
  }
  const BRExpoType fm        = fexpo - 2;
  const BRExpoType lm        = x.m_low + y.m_low;
  const BRExpoType loopCount = (x.m_expo + y.m_expo) - max(fm, lm) + 2;

  if(loopCount <= 0) { // result is zero
    setToZero();
    return *this;
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
  ff = (fexpo == BIGREAL_ZEROEXPO) ? BIGREAL_0 : (e(BIGREAL_1, fexpo * LOG10_BIGREALBASE, pool));
  s_multiplyServer.mult(serverResult, x, y, ff);

  shortProductNoZeroCheck(x, y, loopCount);
  error = serverResult - *this;
  if (compareAbs(error, ff) > 0) {
    shortProductNoZeroCheck(x, y, loopCount);
  }
  return *this;
#else
  return shortProductNoZeroCheck(x, y, loopCount);
#endif
}

// Assume a == b == 0. Dont care about sign of f
void BigReal::split(BigReal &a, BigReal &b, size_t n, const BigReal &f) const {
  intptr_t i = n;
  const Digit *p;
  for(p = m_first; i-- && p; p = p->next) {
    a.appendDigit(p->n);
  }
  a.m_expo     = m_expo;
  a.m_negative = m_negative;
  a.m_low      = a.m_expo - (n-i-2);
  if(p && ((i = (a.m_low - f.m_expo)) > 0)) {
    for(; i-- && p; p = p->next) {
      b.appendDigit(p->n);
    }

    // i = a.low - f.expo - (#digits in b = loopCount)-1

    b.m_expo     = a.m_low - 1;
    b.m_negative = m_negative;
    b.m_low      = f.m_expo + i + 1; // NB:This works for any f (even 0).
                                     // f.expo + i + 1 = f.expo + (a.low-f.expo-loopCount-1) + 1
                                     // = a.low-loopCount.
                                     // b.expo = a.low-1 => a.low = b.expo+1. =>
                                     // b.low = (b.expo+1)-loopCount = b.expo-#digits in b + 1
    b.trimZeroes();
  }
  a.trimZeroes();                  // dont trim zeroes from a before b has been generated!
}

static const ConstBigReal C1third = 0.33333333333;

bool BigReal::s_continueCalculation = true;

BigReal &BigReal::product(BigReal &result, const BigReal &x, const BigReal &y, const BigReal &f, int level) { // static
//  _tprintf(_T("length(X):%5d length(Y):%5d\n"),length(x),length(y));
  const bool      swapXY  = x.getLength() < y.getLength();
  const BigReal  &X       = swapXY ? y : x;
  const BigReal  &Y       = swapXY ? x : y;
  const size_t    XLength = X.getLength();
  const size_t    YLength = Y.getLength();
  DigitPool      *pool    = result.getDigitPool();
  const intptr_t  w1      = XLength + YLength;
  intptr_t        w;

  if(f.isZero()) {
    w = w1;
  } else {
    const intptr_t w2 = X.m_expo + Y.m_expo - f.m_expo;
    w = min(w2, w1);
  }

//  TRACERECURSION(level, "product(result.pool:%2d, x.len,y.len,w:(%4d,%4d,%4d))"
//                 ,pool->getId(), XLength,YLength, w);

  if(YLength <= s_splitLength || w <= (intptr_t)s_splitLength) {
//    _tprintf(_T("shortProd X.length:%3d Y.length:%3d w:%d\n"),Y.length(),w);
    return result.shortProduct(X, Y, f.m_expo);
  }
  if(level < 3) {
    return productMT(result, X, Y, f, w, level+1);
  }

  const BigReal g = PAPCprod(#, C1third, f, pool);
  BigReal gpm10(g);
  gpm10.multPow10(-10);
  const intptr_t n = min((intptr_t)XLength, w)/2;
  BigReal a(pool), b(pool);
  const BigReal &zero = pool->get0();
  level++;
  X.split(a, b, n, g.isZero() ? zero : PAPCprod(#,gpm10,reciprocal(Y,pool),pool));            // a + b = X   O(n)
  if((intptr_t)YLength < n) {                                                                            //
    BigReal p1(pool),p2(pool);
    return result = product(p1, a, Y, zero, level) + product(p2, b, Y, g, level);              // a*Y+b*Y     O(2*n/2*n+n/2)
  }

  BigReal c(pool), d(pool);
  Y.split(c, d, n, g.isZero() ? zero : PAPCprod(#,gpm10,reciprocal(X, pool),pool));           // c + d = Y   O(n)

  const BRExpoType logBK = LOG10_BIGREALBASE * n;                    //                                       O(1)
                                                                     //
  b.multPow10(logBK);                                                //                                       O(1)
  d.multPow10(logBK);                                                //                                       O(1)
  BigReal Kg(g);                                                     //                                       O(1)
  Kg.multPow10(logBK);                                               //                                       O(1)
  BigReal r(pool), s(pool), t(pool);                                 //                                       O(1)
                                                                     //
  product(r, a  , c  , zero, level);                                 // r = a * c                             O(n^2/4)
  product(s, a+b, c+d, Kg  , level);                                 // s = (a+Kb) * (c+Kd)                   O(n^2/4+2n/2)
  product(t, b  , d  , Kg  , level);                                 // t = Kb * Kd                           O(n^2/4)
                                                                     //
  s -= r;                                                            //                                       O(n)
  s -= t;                                                            // s = (a+Kb) * (c+Kd) - (a*c + b*d)     O(n)
  s.multPow10(-logBK);                                               // s /= K                                O(1)
  r += s;                                                            //                                       O(n)
  t.multPow10(-2*logBK);                                             // t /= K^2                              O(1)
  return result = sum(r, t, g);                                      // return sum(r+(s-(r+t))/K,t/(K*K),g)   O(n)
                                                                     //                              Total:   O(3/4n^2 + 6n + k)
}

BigReal prod(const BigReal &x, const BigReal &y, const BigReal &f, DigitPool *digitPool) {
  BigReal result(digitPool ? digitPool : x.getDigitPool());
  BigReal::product(result,x,y,f,0);
  return result;
}

BigReal operator*(const BigReal &x, const BigReal &y) {
  DigitPool *pool = x.getDigitPool();
  BigReal result(pool);
  BigReal::product(result, x, y, pool->get0(), 0);
  return result;
}

BigReal &BigReal::operator*=(const BigReal &x) {
  return *this = *this * x;
}
