#pragma once

#include <MyUtil.h>
#include <FastSemaphore.h>
#include <Singleton.h>
#include <StreamParameters.h>
#include <Random.h>
#include <Math/Double80.h>
#include <Math/Real.h>
#include <MyAssert.h>
#include "DigitPool.h"

class Packer;

#ifdef IS32BIT
#ifdef _DEBUG
extern "C" {
void insertDigitAndIncrExpo(BigReal &v, BRDigitType n);
}
#endif // _DEBUG
#endif // IS32BIT

void throwInvalidToleranceException(               TCHAR const * const function);
void throwBigRealInvalidArgumentException(         TCHAR const * const function, _In_z_ _Printf_format_string_ TCHAR const * const format,...);
void throwBigRealGetIntegralTypeOverflowException( TCHAR const * const function, const BigReal &x, const String &maxStr);
void throwBigRealGetIntegralTypeUnderflowException(TCHAR const * const function, const BigReal &x, const String &minStr);
void throwBigRealGetIntegralTypeUndefinedException(TCHAR const * const function, const BigReal &x);
void throwNotValidException(                       TCHAR const * const file, int line, TCHAR const * const name, _In_z_ _Printf_format_string_ const TCHAR *format, ...);
void throwBigRealException(_In_z_ _Printf_format_string_ TCHAR const * const format,...);

#ifdef _DEBUG
void throwNotMutableException(TCHAR const * const file, int line, TCHAR const * const function, const BigReal     &x, TCHAR const * const name);
void throwNotMutableException(TCHAR const * const file, int line, TCHAR const * const function, const BigRational &x, TCHAR const * const name);
#define CHECKISMUTABLE(x)                                                 \
{ if(((x).getFlags() & (BR_INITDONE|BR_MUTABLE)) == BR_INITDONE) {        \
    throwNotMutableException(__TFILE__,__LINE__,__TFUNCTION__,x,_T(#x));  \
  }                                                                       \
}
#else
void throwNotMutableException(TCHAR const * const function, const BigReal     &x);
void throwNotMutableException(TCHAR const * const function, const BigRational &x);
#define CHECKISMUTABLE(x)                                                 \
{ if(((x).getFlags() & (BR_INITDONE|BR_MUTABLE)) == BR_INITDONE) {        \
    throwNotMutableException(__TFUNCTION__,x);                            \
  }                                                                       \
}
#endif // _DEBUG

class BigReal {
  friend class DigitPool;
  friend class SubProdRunnable;
  friend class Pow2Cache;
  friend class BigRealTestClass;
  friend class BigInt;
  friend class BigRational;
  friend class BigRealStream;

private:
  static const BRDigitTypex86 s_power10Tablex86[BIGREAL_POW10TABLESIZEx86];
  static const BRDigitTypex64 s_power10Tablex64[BIGREAL_POW10TABLESIZEx64];
  // Defined in shortProduct.cpp. Split factors when length > s_splitLength
  static size_t               s_splitLength;
  static const BR2DigitType   s_BIGREALBASEBR2; // BIGREALBASE as BR2DigitType (__UINT64 or _uint128)

  // Most significand  digit
  Digit                    *m_first;
  // Least significand digit
  Digit                    *m_last;
  // if(m_expo == BIGREAL_NONNORMAL) { => _isnormal() is false
  //   switch(m_low) {
  //   case BIGREAL_ZEROLOW    : isZero() is true
  //   case BIGREAL_INFLOW     : _isinf() is true +/- infinite depending on m_negative
  //   case BIGREAL_QNANLOW    : _isnan() is true
  //   }
  // } else { _isnormal() == true
  //   m_expo = m_low+getLength()-1
  // }
  BRExpoType                m_expo, m_low;
  DigitPool                &m_digitPool;
  BYTE                      m_flags;

  // First remove bits specified in remove
  // Then add. Order is important, because add and remove may have overlapping bits
  inline void modifyFlags(BYTE add, BYTE remove) {
    m_flags &= ~remove;
    m_flags |= add;
  }
  inline BigReal &clrFlags(BYTE mask) {
    m_flags &= ~mask;
    return *this;
  }
  inline BigReal &setFlags(BYTE mask) {
    m_flags |= mask;
    return *this;
  }
  inline void startInit() {
    m_flags = m_digitPool.getInitFlags() & ~BR_INITDONE;
  }
  inline void endInit() {
    m_digitPool.incRefCount();
    setInitDone();
  }
  inline BigReal &clrInitDone() {
    return clrFlags(BR_INITDONE);
  }
  inline BigReal &setInitDone() {
    return setFlags(BR_INITDONE);
  }
  // Copy sign-bit from x.
  // Does NOT call CHECKISMUTABLE
  inline BigReal &copySign(const BigReal &x) {
    modifyFlags(x.m_flags&BR_NEG, BR_NEG);
    return *this;
  }
  // Switch sign-bit
  // Does NOT call CHECKISMUTABLE
  // return *this;
  inline BigReal &flipSign() {
    m_flags ^= BR_NEG;
    return *this;
  }
  // Does NOT call CHECKISMUTABLE
  friend inline bool hasSameSign(const BigReal &x, const BigReal &y) {
    return ((x.m_flags ^ y.m_flags) & BR_NEG) == 0;
  }

  // Construction helperfunctions
  // copy m_expo, m_low and sign-bit
  // Does NOT call CHECKISMUTABLE
  inline void copyNonPointerFields(const BigReal &src) {
    m_expo     = src.m_expo;
    m_low      = src.m_low;
    copySign(src);
  }
  // Set both m_first = m_last = NULL WITHOUT call to deleteDigits
  // Does NOT call CHECKISMUTABLE
  inline void initToNonNormal(int low = BIGREAL_ZEROLOW, bool negative=false) {
    m_first    = m_last = NULL;
    m_expo     = BIGREAL_NONNORMAL;
    m_low      = low;
    modifyFlags(negative ? BR_NEG : 0, BR_NEG);
  }
  // Does NOT call CHECKISMUTABLE
  inline void initToZero() {
    initToNonNormal();
  }
  // Assume pointers have been initialized, call clearDigits.
  // Call CHECKISMUTABLE
  // Return *this
  inline BigReal &setToNonNormal(int low = BIGREAL_ZEROLOW, bool negative=false) {
    CHECKISMUTABLE(*this);
    clearDigits();
    m_expo = BIGREAL_NONNORMAL;
    m_low  = low;
    return negative ? setFlags(BR_NEG) : clrFlags(BR_NEG);
  }

  void init(int               n);
  void init(UINT              n);
  void init(INT64             n);
  void init(UINT64            n);
  void init(const _int128    &n);
  void init(      _uint128    n);
  void init(float             x);
  void init(double            x);
  void init(const Double80   &x);
  void init(const String     &s, bool allowDecimalPoint);

  // Assume this->_isnormal() == false
  // Return one of FP_ZERO, FP_INFINITE, FP_NAN
  inline int classifyNonNormal() const {
    assert(!_isnormal());
    switch(m_low) {
    case BIGREAL_ZEROLOW: return FP_ZERO;
    case BIGREAL_INFLOW : return FP_INFINITE;
    case BIGREAL_QNANLOW: return FP_NAN;
    default             : NODEFAULT;
    }
    return FP_NAN;
  }

  // Set this to corresponding non-normal value, and return *this.
  // Call CHECKISMUTABLE
  // Assume fpclass is one of {_FPCLASS_PZ,_FPCLASS_NZ, _FPCLASS_PINF, _FPCLASS_NINF, _FPCLASS_QNAN, _FPCLASS_SNAN }
  // throws exception if fpclass is not in the list above
  BigReal &setToNonNormalFpClass(int fpclass);

  // return true if x*y is normal (finite and != 0, (false if x*y is 0, +/-inf or nan)
  static inline bool isNormalProduct(const BigReal &x, const BigReal &y) {
    return x._isnormal() && y._isnormal();
  }
  // Return true if x/y is normal (finite and != 0, (false if x/y is 0, +/-inf or nan)
  static bool inline isNormalQuotient(const BigReal &x, const BigReal &y) {
    return y._isnormal() && x._isnormal();
  }
  // Return _FPCLASS_PZ, _FPCLASS_QNAN
  // Assume !x._isnormal() || !y._isnormal() which will result in non-normal product (0 or nan)
  static int getNonNormalProductFpClass(const BigReal &x, const BigReal &y);
  // Return _FPCLASS_PZ, _FPCLASS_PINF, _FPCLASS_NINF, _FPCLASS_QNAN
  // Assume !x._isnormal() || !y._isnormal() which will result in non-normal quotient (0,+/-inf or nan)
  static int getNonNormalQuotientFpClass(const BigReal &x, const BigReal &y);
  // Return _FPCLASS_PN,_FPCLASS_NN,_FPCLASS_PZ, _FPCLASS_PINF, _FPCLASS_QNAN
  static int getPowFpClass(const BigReal &x, const BigReal &y);

  // Return true if x*y is normal (finite and != 0). In this case, *this will not be changed
  // Return false, if x*y is not normal, and *this will be set to the corresponding result (0 or nan)
  inline bool checkIsNormalProduct(const BigReal &x, const BigReal &y) {
    if(isNormalProduct(x, y)) return true;
    setToNonNormalFpClass(getNonNormalProductFpClass(x, y));
    return false;
  }
  // Return true if x/y is normal (finite and != 0), (false if x/y is 0, +/-inf or nan)
  // If x/y is not normal, quotient and remainder will recieve the non-normal value (if not null)
  static bool inline checkIsNormalQuotient(const BigReal &x, const BigReal &y, BigReal *quotient, BigReal *remainder) {
    if(isNormalQuotient(x, y)) return true;
    const int qclass = getNonNormalQuotientFpClass(x, y);
    if(quotient ) quotient->setToNonNormalFpClass(qclass);
    if(remainder) remainder->setToNonNormalFpClass(qclass);
    return false;
  }

  // Return true if x^y is normal (finite and != 0), (false if x^y is 0, +/-inf or nan)
  // Return false if x^y is not normal OR = 1 (x^0 for x>!=0), and *this will be set to the corresponding result (1,0,+/-inf or nan)
  bool checkIsNormalPow(const BigReal &x, const BigReal &y);

  static void validateQuotRemainderArguments(const TCHAR *method, const BigReal &x, const BigReal &y, const BigReal *quotient, const BigReal *remainder);

  // Basic digit manipulation
  inline Digit *newDigit() const {
    return m_digitPool.fetchDigit();
  }
  inline void deleteDigits(Digit *first, Digit *last) const {
    m_digitPool.deleteDigits(first, last);
  }

  inline void appendDigit(BRDigitType n) {
    assert(n < BIGREALBASE);
    Digit *p = newDigit();
    p->n     = n;
    p->prev  = m_last;
    p->next  = NULL;
    if(m_last) m_last->next = p; else m_first = p;
    m_last = p;
  }

  inline void appendZero() {
    appendDigit(0);
  }
  // Insert one digit=n at head of this.
  // Dont modify m_expo,m_low,m_flags
  void    insertDigit(                      BRDigitType n);
  // Insert count zero-digits at head of this
  // Assume *this != zero. ie m_first != NULL (and m_last != NULL)
  void    insertZeroDigits(                 size_t count);
  // Insert one digit=n after digit p.
  // Assume p is a digit in digit-list of this
  // Dont modify m_expo,m_low,m_flags
  void    insertAfter(            Digit *p, BRDigitType n);
  // Insert count zero-digits after digit p.
  // Assume (count > 0) && p is a digit in digit-list of this.
  // Dont modify m_expo,m_low,m_flags
  void    insertZeroDigitsAfter(  Digit *p, size_t count);
  // Insert count digits=BIGREALBASE-1 after digit p.
  // Assume (count > 0) && p is a digit in digit-list of this.
  // Dont modify m_expo,m_low,m_flags
  void    insertBorrowDigitsAfter(Digit *p, size_t count);
  // Releases all digits in digit-list to this.m_digitPool.
  // Dont modify m_expo,m_low,m_flags
  inline BigReal &clearDigits() {
    if(m_first) {
      m_digitPool.deleteDigits(m_first, m_last);
      m_first = m_last = NULL;
    }
    return *this;
  }

  // Assume _isfinite()
  // Does NOT call CHECKISMUTABLE
  // if _isnormal() m_expo++; else m_expo = m_low = 0
  inline void incrExpo() {
    assert(_isfinite());
    if(_isnormal()) m_expo++; else m_expo = m_low = 0;
  }

  // Remove all zero-digits at both ends of digit-list, and check for overflow/underflow.
  // Adjust m_expo,m_low accordingly
  // Return *this
  inline BigReal &trimZeroes() {
    if(m_first) {
      if(m_first->n == 0)     trimHead();
      else if(m_last->n == 0) trimTail();
      if(m_expo > BIGREAL_MAXEXPO) throwBigRealException(_T("Overflow"));
      if(m_low  < BIGREAL_MINEXPO) throwBigRealException(_T("Underflow"));
    }
    return *this;
  }
  // Trim zeroes from head, AND if necessary from the tail too
  // Assume m_first && m_first->n == 0
  // Adjust m_expo,m_low accordingly
  void    trimHead();
  // Assume m_first != NULL (=> m_last != NULL) and m_last->n == 0
  // Adjust m_low accordingly
  void    trimTail();
  Digit  *findDigit(const BRExpoType exponent) const;
  Digit  *findDigitSubtract(const BigReal &f) const;
  Digit  *findDigitAdd(const BigReal &f, BRExpoType &low) const;

  // *this = |x| + |y| with maximal error <= f. If f<=0, then full precision
  // Assume &x != this && &y != this && *this == 0, x._isnormal() && y._isnormal()
  // Does NOT call CHECKISMUTABLE
  // Return *this;
  BigReal &addAbs(const BigReal &x, const BigReal &y, const BigReal &f);
  // Add |x| to |this|.
  // Assume &x != this && this->_isnormal() && x._isnormal()
  // Does NOT call CHECKISMUTABLE
  // Return *this
  BigReal &addAbs(const BigReal &x);
  // Subtract |x| from |this| with maximal error = f. If f<=0, then full precision
  // Assume &x != this && this->_isnormal() && x._isnormal() && |x| < |*this|
  // Does NOT call CHECKISMUTABLE
  // Return *this
  BigReal &subAbs(const BigReal &x, const BigReal &f);
  // Subtract |x| from |this|
  // Assume &x != this && this->_isnormal() && x._isnormal() && |x| < |*this|
  // Does NOT call CHECKISMUTABLE
  // Return *this
  inline BigReal &subAbs(const BigReal &x) {                                                      // return this
    return subAbs(x, (BigReal&)m_digitPool._0());
  }

#if(  SP_OPT_METHOD == SP_OPT_NONE)

  void    baseb(const BigReal &x);
  void    baseb(const BigReal &x, int low);
  BigReal &baseB(const BigReal &x);                                                               // return this
  void    addSubProduct(BR2DigitType n);

#elif(SP_OPT_METHOD == SP_OPT_BY_FPU)

  void    addFPUReg0();
  void    addSubProduct(BR2DigitType n);

#endif

// Set digitChain to contain 1 digit (=0).
// Should only be called from shortProductNoZeroCheck. m_last will not be touched
// so invariant will be broken for af while, and reestablished at the end of shortProductNoZeroCheck,
// when we clean up the chain head and end
  inline Digit *clearDigits1() {
    if(m_first == NULL) {
      m_first = newDigit();
    } else if(m_first->next) {
      deleteDigits(m_first->next, m_last);
    }
    m_first->n = 0;
    return m_first;
  }
// Assume last != NULL. new digit->n and ..->next are not initialized
// Should only be called from shortProductNoZeroCheck. m_last will not be touched
// so invariant will be broken for af while, and reestablished at the end of shortProductNoZeroCheck,
// when we clean up the chain head and end
  inline Digit *fastAppendDigit(Digit *last) {
    Digit *p = last->next = m_digitPool.fetchDigit();
    p->prev  = last;
    return p;
  }

  // Set *this = x*y, with |error| <= BIGREALBASE^fexpo. If x or y is 0 or infinite, a proper value is assigned to *this
  // Return *this
  // NOTE: fexpo is NOT decimal exponent, but Digit-exponent. To get decimal exponent multiply by BIGREAL_LOG10BASE !!!
  // Don't call shortProductNoZeroCheck with numbers longer than getMaxSplitLength(), or you'll get a wrong result
  inline BigReal &shortProduct(              const BigReal &x, const BigReal &y, BRExpoType fexpo) {
    if(!checkIsNormalProduct(x, y)) return *this;
    return shortProductNoNormalCheck(x, y, fexpo);
  }
  // Set *this = x*y, with |error| <= BIGREALBASE^fexpo
  // Assume isNormalProduct(x,y)
  // Return *this
  BigReal &shortProductNoNormalCheck(        const BigReal &x, const BigReal &y, BRExpoType fexpo);
  // Set *this = x*y, with the specified number of loops, each loop, i, calculates
  // sum(y[j]*x[i-j]), j=[0..i], i=[0..loopCount], digits indexed [0..length-1], from head
  // Assume isNormalProduct(x,y) && (loopCount > 0)
  // If m_digitPool.m_continueCalculation is false, when a thread enters this function,
  // it will call throwBigRealException("Operation was cancelled")
  // Return *this.
  BigReal &shortProductNoZeroCheck(          const BigReal &x, const BigReal &y, UINT loopCount);
  // Set result = x*y, with |error| <= |f|
  // Assume isNormalProduct(x, y) && f._isfinite())
  // Return &result.
  static BigReal &product(  BigReal &result, const BigReal &x, const BigReal &y, const BigReal &f,              int level);
  // Set result = x*y, with |error| <= |f|
  // Assume isNormalProduct(x, y) && f._isfinite() && (x.getLength() >= y.getLength())
  // Return &result.
  static BigReal &productMT(BigReal &result, const BigReal &x, const BigReal &y, const BigReal &f, intptr_t  w, int level);
  // Assume this->_isnormal() && a.isZero() && b.isZero() && f._isfinite(). Dont care about sign(f)
  void    split(BigReal &a, BigReal &b, size_t n, const BigReal &f) const;
  // Assume src._isnormal() && 0 < length <= src.getLength() && m_first == m_last == NULL
  // Modify only m_first and m_last of this
  // Does NOT call CHECKISMUTABLE
  // Return *this
  BigReal &copyDigits(   const BigReal &src, size_t length);
  // Assume !_isnormal() && src._isnormal()
  // Does NOT call CHECKISMUTABLE
  // Return *this
  BigReal &copyAllDigits(const BigReal &src);
  // Copy specified number of decimal digits, truncating result
  // call CHECKISMUTABLE
  // Return *this
  BigReal &copyrTrunc(  const BigReal &src, size_t digits);
  // Set sign-bit according to sign-rules for multiplication (and division)
  // Does NOT call CHECKISMUTABLE
  // return *this
  inline BigReal &setSignByProductRule(const BigReal &x, const BigReal &y) {
    return hasSameSign(x,y) ? clrFlags(BR_NEG) : setFlags(BR_NEG);
  }

  // Cut *this (assumed != 0) to the specified number of significant decimal digits, truncation toward zero
  // Assume _isnormal() && digits > 0 (digits is decimal digits).
  // call CHECKISMUTABLE
  // Return *this
  BigReal &rTrunc(size_t digits);
  // Cut *this (assumed != 0) to the specified number of significant decimal digits, rounding
  // Assume isnormal() && digits > 0 (digits is decimal digits).
  // call CHECKISMUTABLE
  // Return *this
  BigReal &rRound(size_t digits);

  // Division helperfunctions.
  // Result.digitPool = x.digitPool or digitPool if specified
  // Return Approximately 1/x.
  static BigReal  reciprocal(const BigReal &x, DigitPool *digitPool = NULL);
  // *this = approximately x / y
  // Assume y._isnormal().
  // Return *this
  BigReal &approxQuot32(     const BigReal &x, const BigReal           &y);
  // *this = approximately x / y
  // Assume y._isnormal().
  // Return *this
  BigReal &approxQuot64(     const BigReal &x, const BigReal           &y);
  // *this = approximately x / e(y, scale)
  // Assume y != 0
  // Return *this
  BigReal &approxQuot32Abs(  const BigReal &x, ULONG                    y, BRExpoType scale);
  // *this = approximately x / e(y, scale)
  // Assume y != 0
  // Return *this
  BigReal &approxQuot64Abs(  const BigReal &x, const UINT64            &y, BRExpoType scale);
#ifdef IS64BIT
  // *this = approximately |x/y|
  // Assume y._isnormal()
  // Return *this
  BigReal &approxQuot128(    const BigReal &x, const BigReal           &y);
  // *this = approximately |x/e(y,scale)|
  // Assume y != 0.        
  // Return *this
  BigReal &approxQuot128Abs( const BigReal &x, const _uint128          &y, BRExpoType scale);
#endif // IS64BIT
  static double  estimateQuotNewtonTime(const BigReal &x, const BigReal &y, const BigReal &f);
  static double  estimateQuotLinearTime(const BigReal &x, const BigReal &y, const BigReal &f);
  static bool    chooseQuotNewton(      const BigReal &x, const BigReal &y, const BigReal &f);

  // Misc
  // return *this
  // Assume bias is {<,>,#}
#ifdef _DEBUG
  BigReal  &adjustAPCResult(const char bias, const TCHAR *function);
#else 
  BigReal  &adjustAPCResult(const char bias);
#endif // _DEBUG

  friend bool isFloat(   const BigReal &v, float    *flt = NULL);
  friend bool isDouble(  const BigReal &v, double   *dbl = NULL);
  friend bool isDouble80(const BigReal &v, Double80 *d80 = NULL);

  inline float getFloatNoLimitCheck() const {
    return getFloat(getDouble80NoLimitCheck());
  }
  inline double getDoubleNoLimitCheck() const {
    return getDouble(getDouble80NoLimitCheck());
  }
  // Assume this->_isnormal()
  Double80 getDouble80NoLimitCheck() const;

protected:
  // *this *= pow(10,exp). (Fast version) Check for overflow/underflow. Trim leading and trailling zeroes if necessary
  // Call CHECKISMUTABLE if force == false, else this here will be no check. The public version calls this function with force=false
  // Return *this
  BigReal &multPow10(BRExpoType exp, bool force);

  inline void releaseDigits() { // should only be called from destructor
    if(m_first) {
      deleteDigits(m_first, m_last);
      m_first = NULL;
    }
  }

#define DEFAULT_DIGITPOOL BigReal::s_defaultDigitPool
#define CONST_DIGITPOOL   BigReal::s_constDigitPool

#define _SELECTPOINTER(p1, p2)       ((p1) ? (p1) : (p2))
#define _INITDIGITPOOL(alternative)   m_digitPool(*(_SELECTPOINTER(digitPool,alternative)))
#define _INITPOOLTODEFAULTIFNULL()    _INITDIGITPOOL(DEFAULT_DIGITPOOL)
#define _SELECTDIGITPOOL(x)           DigitPool *pool = _SELECTPOINTER(digitPool,(x).getDigitPool())

public:
  static DigitPool *s_defaultDigitPool;
  static DigitPool *s_constDigitPool;

  inline BigReal(                             DigitPool *digitPool = NULL) : _INITPOOLTODEFAULTIFNULL() {
    startInit();
    initToZero();
    endInit();
  }
  inline BigReal(int      x                 , DigitPool *digitPool = NULL) : _INITPOOLTODEFAULTIFNULL() {
    startInit();
    init(x);
    endInit();
  }
  inline BigReal(UINT     x                 , DigitPool *digitPool = NULL) : _INITPOOLTODEFAULTIFNULL() {
    startInit();
    init(x);
    endInit();
  }
  inline BigReal(long     x                 , DigitPool *digitPool = NULL) : _INITPOOLTODEFAULTIFNULL() {
    startInit();
    init((int)x);
    endInit();
  }
  inline BigReal(ULONG    x                 , DigitPool *digitPool = NULL) : _INITPOOLTODEFAULTIFNULL() {
    startInit();
    init((UINT)x);
    endInit();
  }
  inline BigReal(INT64    x                 , DigitPool *digitPool = NULL) : _INITPOOLTODEFAULTIFNULL() {
    startInit();
    init(x);
    endInit();
  }
  inline BigReal(UINT64   x                 , DigitPool *digitPool = NULL) : _INITPOOLTODEFAULTIFNULL() {
    startInit();
    init(x);
    endInit();
  }
  inline BigReal(_int128  x                 , DigitPool *digitPool = NULL) : _INITPOOLTODEFAULTIFNULL() {
    startInit();
    init(x);
    endInit();
  }
  inline BigReal(_uint128 x                 , DigitPool *digitPool = NULL) : _INITPOOLTODEFAULTIFNULL() {
    startInit();
    init(x);
    endInit();
  }
  BigReal(float           x                 , DigitPool *digitPool = NULL) : _INITPOOLTODEFAULTIFNULL() {
    startInit();
    init(x);
    endInit();
  }
  BigReal(double          x                 , DigitPool *digitPool = NULL) : _INITPOOLTODEFAULTIFNULL() {
    startInit();
    init(x);
    endInit();
  }
  BigReal(const Double80  &x                , DigitPool *digitPool = NULL) : _INITPOOLTODEFAULTIFNULL() {
    startInit();
    init(x);
    endInit();
  }
  BigReal(const BigReal   &x                , DigitPool *digitPool = NULL);

  explicit inline BigReal(const String    &s, DigitPool *digitPool = NULL) : _INITPOOLTODEFAULTIFNULL() {
    startInit();
    init(s, true);
    endInit();
  }
  explicit inline BigReal(const char      *s, DigitPool *digitPool = NULL) : _INITPOOLTODEFAULTIFNULL() {
    startInit();
    init(s, true);
    endInit();
  }
  explicit inline BigReal(const wchar_t   *s, DigitPool *digitPool = NULL) : _INITPOOLTODEFAULTIFNULL() {
    startInit();
    init(s, true);
    endInit();
  }
  virtual ~BigReal() {
    releaseDigits();
    m_digitPool.decRefCount();
  }

  inline BigReal &operator=(int              n) {
    CHECKISMUTABLE(*this);
    clearDigits(); init(n);
    return *this;
  }
  inline BigReal &operator=(UINT             n) {
    CHECKISMUTABLE(*this);
    clearDigits(); init(n);
    return *this;
  }
  inline BigReal &operator=(long             n) {
    CHECKISMUTABLE(*this);
    clearDigits(); init(n);
    return *this;
  }
  inline BigReal &operator=(ULONG            n) {
    CHECKISMUTABLE(*this);
    clearDigits(); init((UINT)n);
    return *this;
  }
  inline BigReal &operator=(INT64            n) {
    CHECKISMUTABLE(*this);
    clearDigits(); init(n);
    return *this;
  }
  inline BigReal &operator=(UINT64           n) {
    CHECKISMUTABLE(*this);
    clearDigits(); init(n);
    return *this;
  }
  inline BigReal &operator=(const _int128   &n) {
    CHECKISMUTABLE(*this);
    clearDigits(); init(n);
    return *this;
  }
  inline BigReal &operator=(const _uint128  &n) {
    CHECKISMUTABLE(*this);
    clearDigits(); init(n);
    return *this;
  }
  inline BigReal &operator=(float            x) {
    CHECKISMUTABLE(*this);
    clearDigits(); init(x);
    return *this;
  }
  inline BigReal &operator=(double           x) {
    CHECKISMUTABLE(*this);
    clearDigits(); init(x);
    return *this;
  }
  inline BigReal &operator=(const Double80  &x) {
    CHECKISMUTABLE(*this);
    clearDigits(); init(x);
    return *this;
  }

  BigReal &operator=(       const BigReal &x);

  // Result.digitPool = x.digitPool
  friend BigReal  operator+(const BigReal &x, const BigReal &y);
  // Result.digitPool = x.digitPool
  friend BigReal  operator-(const BigReal &x, const BigReal &y);
  // Result.digitPool = x.digitPool
  friend BigReal  operator*(const BigReal &x, const BigReal &y);
  // Result.digitPool = x.digitPool
  friend BigReal  operator%(const BigReal &x, const BigReal &y);
  // Result.digitPool = x.digitPool
  friend BigReal  operator-(const BigReal &x);

  // digitPool unchanged
  BigReal &operator+=(const BigReal &x);
  BigReal &operator-=(const BigReal &x);
  BigReal &operator*=(const BigReal &x);
  BigReal &operator%=(const BigReal &x);
  BigReal &operator++();                                                      // prefix-form
  BigReal &operator--();                                                      // prefix-form
  BigReal  operator++(int);                                                   // postfix-form
  BigReal  operator--(int);                                                   // postfix-form
  // *this *= pow(10,exp). (Fast version) Check for overflow/underflow. Trim leading and trailling zeroes if necessessary
  // call CHECKISMUTABLE
  // Return *this
  inline BigReal &multPow10(BRExpoType exp) {
    return multPow10(exp, false);
  }
  // Fast version of *this *= 2
  // Call CHECKISMUTABLE
  // return *this
  BigReal &multiply2();
  // Fast version of *this /= 2
  // Call CHECKISMUTABLE
  // return *this
  BigReal &divide2();

  // Return x+y with |error| <= f.
  // If f <= 0, full precision is used
  // If any of the operands is not finite, nan is returned
  // Result.digitPool = x.digitPool, or digitPool if specified
  friend BigReal  sum(             const BigReal &x, const BigReal &y, const BigReal  &f, DigitPool *digitPool = NULL);
  // Return x-y with |error| <= f
  // If f <= 0, full precision is used
  // If any of the operands is not finite, nan is returned
  // Result.digitPool = x.digitPool, or digitPool if specified
  friend BigReal  dif(             const BigReal &x, const BigReal &y, const BigReal  &f, DigitPool *digitPool = NULL);
  // Return x*y with |error| <= f
  // If f <= 0, full precision is used
  // If any of the operands is not finite, nan is returned
  // Result.digitPool = x.digitPool, or digitPool if specified
  friend BigReal  prod(            const BigReal &x, const BigReal &y, const BigReal  &f, DigitPool *digitPool = NULL);
  // x/y with |error| <= f. if f<= 0, an invalid argument exception is thrown
  // If any of the operands is not finite, nan is returned
  // If y==0, nan, +/-inf is returned, depending on value of x
  // Result.digitPool = x.digitPool, or digitPool if specified
  friend BigReal  quot(            const BigReal &x, const BigReal &y, const BigReal  &f, DigitPool *digitPool = NULL);
  // Return fmod(x,y)
  // If any of the operands is not finite, nan is returned
  // Result.digitPool = x.digitPool, or digitPool if specified
  friend BigReal  fmod(            const BigReal &x, const BigReal &y                   , DigitPool *digitPool = NULL);
  // Return x / y (Integer division)
  // If any of the operands is not finite, nan is returned.
  // If y==0, nan, +/-inf is returned, depending on value of x
  // Result.digitPool = x.digitPool, or digitPool if specified
  friend BigInt   quot(            const BigInt  &x, const BigInt  &y                   , DigitPool *digitPool = NULL);
  // Return x % y (Integer remainder)
  // If any of the operands is not finite, nan is returned
  // If y==0, nan, +/-inf is returned, depending on value of x
  // Result.digitPool = x.digitPool, or digitPool if specified
  friend BigInt   rem(             const BigInt  &x, const BigInt  &y                   , DigitPool *digitPool = NULL);
  // Calculates only quotient and/or remainder if specified
  friend void     quotRemainder(   const BigReal &x, const BigReal &y, BigInt *quotient , BigReal *remainder);
  // Calculates only quotient and/or remainder if specified
  friend void     quotRemainder64( const BigReal &x, const BigReal &y, BigInt *quotient , BigReal *remainder);
#ifdef IS64BIT
  // Calculates only quotient and/or remainder if specified
  friend void     quotRemainder128(const BigReal &x, const BigReal &y, BigInt *quotient , BigReal *remainder);
#endif
  // bias = '<','#' or '>'. Result.digitPool = x.digitPool, or digitPool if specified
  static BigReal  apcSum(       const char bias,  const BigReal &x, const BigReal  &y   , DigitPool *digitPool = NULL);
  // bias = '<','#' or '>'. Result.digitPool = x.digitPool, or digitPool if specified
  static BigReal  apcProd(      const char bias,  const BigReal &x, const BigReal  &y   , DigitPool *digitPool = NULL);
  // bias = '<','#' or '>'. Result.digitPool = x.digitPool, or digitPool if specified
  static BigReal  apcQuot(      const char bias,  const BigReal &x, const BigReal  &y   , DigitPool *digitPool = NULL);
  // bias = '<','#' or '>'. Result.digitPool = x.digitPool, or digitPool if specified
  static BigReal  apcPow(       const char bias,  const BigReal &x, const BigInt   &y   , DigitPool *digitPool = NULL);
  // bias = '<','#' or '>'. Result.digitPool = x.digitPool, or digitPool if specified
  static BigReal  apcAbs(       const char bias,  const BigReal &x                      , DigitPool *digitPool = NULL);

  // If specified, both integerPart and fractionPart, will have same sign as *this (if != 0)
  void           fractionate(BigInt *integerPart, BigReal *fractionPart) const;
  // ln(10) with |error| < f. result.digitPool = f.digitPool or digitPool if specified
  static BigReal ln10(         const BigReal &f, DigitPool *digitPool = NULL);
  // Approximatly ln(x). Assume 1 <= x <= 10. result.digitPool = x.digitPool or digitPool if specified
  static BigReal lnEstimate(   const BigReal &x, DigitPool *digitPool = NULL);
  // 2^n, with the specified number of digits. if digits = 0, then full precision. Results are cached, so dont modify
  // Returned value is not mutable and belongs to digitpool which is private to pow2Cache
  static const BigReal &pow2(int n, size_t digits = 0);

  static void pow2CacheLoad();
  static void pow2CacheSave();
  static bool pow2CacheHasFile();
  static bool pow2CacheChanged();
  static void pow2CacheDump();

  // Assume x._isfinite() && y._isfinite().
  // Return sign(x-y) (+/-1,0)
  static int compare(         const BigReal &x,  const BigReal &y);
  // Assume x._isfinite() && y._isfinite().
  // Return compare(|x|,|y|). (Faster than compare(fabs(x),fabs(y)))
  static int compareAbs(      const BigReal &x,  const BigReal &y);

  inline BYTE getFlags() const {
    return m_flags;
  }
  // Returns one of
  // FP_INFINITE
  // FP_NAN
  // FP_NORMAL
  // FP_ZERO
  friend int fpclassify(const BigReal &x);
  // Returns one of
  // _FPCLASS_QNAN  0x0002   quiet NaN
  // _FPCLASS_NINF  0x0004   negative infinity
  // _FPCLASS_NN    0x0008   negative normal
  // _FPCLASS_PZ    0x0040   +0
  // _FPCLASS_PN    0x0100   positive normal
  // _FPCLASS_PINF  0x0200   positive infinity
  friend int _fpclass(const BigReal &x);

  // Return true, if *this is a finite number != 0
  inline bool _isnormal() const {
    return m_expo != BIGREAL_NONNORMAL;
  }
  // Return true, if *this == 0
  inline bool isZero() const {
    return !_isnormal() && (m_low == BIGREAL_ZEROLOW);
  }
  // Return true, if _isnormal() || _isinf()
  inline bool _hasSign() const {
    return _isnormal() || (m_low == BIGREAL_INFLOW);
  }

  // Return true, if *this is +/-INFINITE
  inline bool _isinf() const {
    return !_isnormal() && (m_low == BIGREAL_INFLOW);
  }
  // Return true, if _isnormal() || isZero()
  inline bool _isfinite() const {
    return _isnormal() || (m_low == BIGREAL_ZEROLOW);
  }
  // return true, if *this is NaN
  inline bool _isnan() const {
    return !_isnormal() && (m_low == BIGREAL_QNANLOW);
  }
  // call CHECKISMUTABLE(this)
  inline BigReal &setToZero() {
    return setToNonNormal();
  }
  // call CHECKISMUTABLE(this)
  inline BigReal &setToPInf() {
    return setToNonNormal(BIGREAL_INFLOW);
  }
  // call CHECKISMUTABLE(this)
  inline BigReal &setToNInf() {
    return setToNonNormal(BIGREAL_INFLOW, true);
  }
  // call CHECKISMUTABLE(this)
  inline BigReal &setToNan() {
    return setToNonNormal(BIGREAL_QNANLOW);
  }
  // call CHECKISMUTABLE(this)
  inline BigReal &changeSign() {
    CHECKISMUTABLE(*this);
    return _hasSign() ? flipSign() : *this;
  }
  // call CHECKISMUTABLE(this)
  inline BigReal &setNegative(bool negative) {
    CHECKISMUTABLE(*this);
    return negative ? setFlags(BR_NEG) : clrFlags(BR_NEG);
  }
  // call CHECKISMUTABLE(this)
  inline void setPositive() {
    setNegative(false);
  }
  // Return !isZero() && !isNegative();
  inline  bool isPositive() const {
    return !isZero() && !isNegative();
  }
  // Return (m_flags & BR_NEG) != 0
  inline bool isNegative() const {
    return (m_flags & BR_NEG);
  }
  // Return true if _isfinite() && (m_low >= 0)
  inline bool _isinteger() const {
    return _isfinite() && (getLow() >= 0);
  }
  // Return number of BASE digits. 0 has length 1, undefined (nan,+inf,-inf) has length 0
  inline size_t getLength() const {
    return _isnormal() ? (m_expo - m_low + 1) : (m_low == BIGREAL_ZEROLOW) ? 1 : 0;
  }
  // Return number of decimal digits. 0 has length 1. undefined (nan,+inf,-inf) has length 0
  size_t getDecimalDigits()  const;

  // Assume _isfinite(). Return m_low
  inline BRExpoType getLow() const {
    assert(_isfinite());
    return isZero() ? 0 : m_low;
  }
  static String flagsToString(BYTE flags);
  String flagsToString() const {
    return flagsToString(m_flags);
  }

  // Return x._isnormal() ? floor(log10(|x|)) : 0
  static inline BRExpoType getExpo10(const BigReal  &x) {
    return x._isnormal() ? (x.m_expo * BIGREAL_LOG10BASE + getDecimalDigitCount(x.m_first->n) - 1) : 0;
  }

  // Return x._isnormal() ? getExpo10(x) as BigReal : x
  static BigReal           getExpo10N( const BigReal  &x, DigitPool *digitPool = NULL) {
    _SELECTDIGITPOOL(x);
    return x._isnormal() ? BigReal(getExpo10(x), pool) : BigReal(x,pool);
  }

  // Return 10^n.
  // Assume n < BIGREAL_POW10TABLESIZEx86 (=10)
  static inline BRDigitTypex86 pow10x86(UINT n) {
    assert(n < BIGREAL_POW10TABLESIZEx86);
    return s_power10Tablex86[n];
  }
  // Return 10^n.
  // Assume n < BIGREAL_POW10TABLESIZEx64 (=20)
  static inline BRDigitTypex64 pow10x64(UINT n) {
    assert(n < BIGREAL_POW10TABLESIZEx64);
    return s_power10Tablex64[n];
  }
  // Return 10^n.
  // In x86-mode: Assume n < POWER10TABLE3IZEx86 (=10)
  // in x64-mode: Assume n < POWER10TABLE3IZEx64 (=20)
  static inline BRDigitType pow10(UINT n) {
#ifdef IS32BIT
    return pow10x86(n);
#else // IS64BIT
    return pow10x64(n);
#endif // IS64BIT
  }
  // Assume x._isfinite().
  // Return (x == 0) ? 0 : approximately floor(log2(|x|))
  friend BRExpoType getExpo2(   const BigReal  &x);
  // Assume n = [1..1e8[
  // Return floor(log10(n))+1
  static int     getDecimalDigitCount(BRDigitTypex86 n);
  // Assume n = [1..1e19[
  // Return floor(log10(n))+1
  static int     getDecimalDigitCount(BRDigitTypex64 n);

  // Return (n == 0) ? 0 : max(p)|n % (10^p) == 0, p=[0..LOG10BASEx86/x64]
  template<typename INTTYPE> static int getTrailingZeroCount(INTTYPE n) {
    if(n == 0) return 0;
    int result = 0;
    for(;n % 10 == 0; n /= 10) result++;
    return result;
  }

#ifdef HAS_LOOP_DIGITCOUNT
  static int     getDecimalDigitCountLoopx64(BRDigitTypex64 n);
#endif
  // (int)(log10(x) / BIGREAL_LOG10BASE)
  static int     logBASE(double x);
  // Return p if n == 10^p for p = [0..9]. else Return -1.
  static int     isPow10(BRDigitTypex86 n);
  // Return p if n == 10^p for p = [0..19]. else Return -1.
  static int     isPow10(BRDigitTypex64 n);
  // true if |x| = 10^p for p = [BIGREAL_MINEXPO..BIGREAL_MAXEXPO]
  static bool    isPow10(const BigReal &x);

  // Return Absolute value of x (=|x|)
  // Result.digitPool = x.digitPool or digitPool, if specified
  friend BigReal fabs(     const BigReal &x                   , DigitPool *digitPool = NULL);
  // Return biggest integer <= x
  // Result.digitPool = x.digitPool or digitPool, if specified
  friend BigInt  floor(    const BigReal &x                   , DigitPool *digitPool = NULL);
  // Return smallest integer >= x
  // Result.digitPool = x.digitPool or digitPool, if specified
  friend BigInt  ceil(     const BigReal &x                   , DigitPool *digitPool = NULL);
  // Return sign(x) * (|x| - floor(|x|))
  // Result.digitPool = x.digitPool or digitPool, if specified
  friend BigReal fraction( const BigReal &x                   , DigitPool *digitPool = NULL);
  // Return sign(x) * floor(|x|*10^prec+0.5)/10^prec
  friend BigReal round(    const BigReal &x, intptr_t prec = 0, DigitPool *digitPool = NULL);
  // sign(x) * floor(|x|*10^prec)/10^prec
  friend BigReal trunc(    const BigReal &x, intptr_t prec = 0, DigitPool *digitPool = NULL);
  // x truncated to the specified number of significant decimal digits
  friend BigReal cut(      const BigReal &x, size_t   digits  , DigitPool *digitPool = NULL);
  // x * pow(10,n)
  friend BigReal e(        const BigReal &x, BRExpoType n     , DigitPool *digitPool = NULL);

  // Assume x._isfinite(). Return x < 0 ? -1 : x > 0 ? 1 : 0
  friend inline int sign(  const BigReal &x) {
    assert(x._isfinite());
    return x._isnormal() ? x.isNegative() ? -1 : 1 : 0;
  }

  // Return true if x._isfinite() && isInteger(x) && x % 2 == 0
  friend bool    isEven(     const BigReal &x);
  //  Return true if x._isfinite() && isInteger(x) && x % 2 == 1
  friend bool    isOdd(      const BigReal &x);
  // Return true if x._isfinite() && (fraction(x) == 0)
  friend inline bool isInteger(const BigReal &x) {
    return x._isinteger();
  }

  // Set to = from so |to - from| <= f. If f <= 0, then full precision
  // Assume f._isfinite().
  // call CHECKISMUTABLE
  // Return to
  friend BigReal &copy(BigReal &to, const BigReal &from, const BigReal &f);
  // Set to = from so to.getlength() = min(length,from.getlength().
  // call CHECKISMUTABLE
  // Return to
  friend BigReal &copy(BigReal &to, const BigReal &from, size_t length);

  // Assume x and y are both normal (finite and != 0) and f>0.
  // x/y with |error| < f. Newton-rapthon iteration
  static BigReal quotNewton(   const BigReal &x, const BigReal &y, const BigReal &f, DigitPool *digitPool);
  // Assume isNormalQuotient(x, y) and f>0.
  // x/y with |error| < f. School method. using built-in 32-bit division
  static BigReal quotLinear32( const BigReal &x, const BigReal &y, const BigReal &f, DigitPool *digitPool);
  // Assume isNormalQuotient(x, y) and f>0.
  // x/y with |error| < f. School method. using built-in 64-bit division
  static BigReal quotLinear64( const BigReal &x, const BigReal &y, const BigReal &f, DigitPool *digitPool);
#ifdef IS64BIT
  // Assume isNormalQuotient(x, y) and f>0.
  // x/y with |error| < f. School method. using class _int128 for division
  static BigReal quotLinear128(const BigReal &x, const BigReal &y, const BigReal &f, DigitPool *digitPool);
#endif // IS64BIT
  // x*y with |error| < f. Used to multiply short numbers. Used by prod.
  static inline BigReal shortProd(const BigReal &x, const BigReal &y, const BigReal &f, DigitPool *digitPool) {
    return BigReal(digitPool).shortProduct(x, y, f.m_expo);
  }
  // Return _isnormal() ? first BASE-digit : 0.
  inline BRDigitType getFirstDigit() const {
    return _isnormal() ? m_first->n : 0;
  }
  // Return _isnormal() ? last BASE-digit : 0.
  inline BRDigitType getLastDigit()  const {
    return _isnormal() ? m_last->n : 0;
  }

  // Return first k decimal digits of |*this|. Assume k <= 9.
  ULONG  getFirst32(const UINT k, BRExpoType *scale = NULL) const;
  // Return first k decimal digits of |*this|. Assume k <= 19.
  UINT64 getFirst64(const UINT k, BRExpoType *scale = NULL) const;
#ifdef IS64BIT
  // Return first k decimal digits of |*this|. Assume k <= 38.
  _uint128 &getFirst128(_uint128 &dst, const UINT k, BRExpoType *scale = NULL) const;
#endif
  inline DigitPool *getDigitPool() const {
    return &m_digitPool;
  }
  inline UINT getPoolId() const {
    return m_digitPool.getId();
  }
  ULONG hashCode() const;

  // save/load BigReal in binary format. Packer can be streamed to ByteOutputStream and read from ByteInputStream
  friend Packer &operator<<(Packer &p, const BigReal &v);
  // Call CHECKISMUTABLE
  friend Packer &operator>>(Packer &p,       BigReal &v);

  static UINT getMaxSplitLength();

  // Checks that this is a consistent BigReal with all the various invariants satisfied.
  // Throws an excpeption if not with a descripion of what is wrong. For debugging
#define VALIDATEBIG(x) { (x).assertIsValid(__TFILE__,__LINE__, _T(#x)); }
  virtual void assertIsValid(const TCHAR *file, int line, const TCHAR *name) const;
  void throwNotValidException(TCHAR const * const file, int line, TCHAR const * const name, _In_z_ _Printf_format_string_ const TCHAR *format, ...) const;

  // Return uniform distributed random BigReal between 0 (incl) and 1 (excl) with at most maxDigits decimal digits.
  // If maxDigits == 0, 0 will be returned
  // Digits generated with rnd
  // If digitPool == NULL, use DEFAULT_DIGITPOOL
  friend BigReal randBigReal(size_t maxDigits, RandomGenerator &rnd = *RandomGenerator::s_stdGenerator, DigitPool *digitPool = NULL);

  // Return uniform distributed random BigReal in [from;to] with at most maxDigits decimal digits.
  // If from > 0, an invalid argument exception is thrown
  // Digits generated with rnd
  // If digitPool == NULL, use from.getDigitPool()
  friend BigReal  randBigReal(const BigReal &from, const BigReal &to, size_t maxDigits, RandomGenerator &rnd = *RandomGenerator::s_stdGenerator, DigitPool *digitPool = NULL);

  // Return uniform distributed random BigInt in [0..e(1,maxDigits)-1]
  // If maxDigits == 0, 0 will be returned
  // Digits generated with rnd.
  // If digitPool == NULL, use DEFAULT_DIGITPOOL
  // ex:maxDigits = 3:returned value in interval [0..999]
  friend BigInt   randBigInt(size_t maxDigits, RandomGenerator &rnd = *RandomGenerator::s_stdGenerator, DigitPool *digitPool = NULL);

  // ------------------------------------------------------------------------------------------

  friend long        getLong(    const BigReal &v, bool validate = true);
  friend ULONG       getUlong(   const BigReal &v, bool validate = true);
  friend inline int  getInt(     const BigReal &v, bool validate = true) {
    return (int)getLong(v, validate);
  }
  friend inline UINT getUint(    const BigReal &v, bool validate = true) {
    return (UINT)getUlong(v, validate);
  }
  friend INT64       getInt64(   const BigReal &v, bool validate = true);
  friend UINT64      getUint64(  const BigReal &v, bool validate = true);
  friend _int128     getInt128(  const BigReal &v, bool validate = true);
  friend _uint128    getUint128( const BigReal &v, bool validate = true);
  friend float       getFloat(   const BigReal &v, bool validate = true);
  friend double      getDouble(  const BigReal &v, bool validate = true);
  friend Double80    getDouble80(const BigReal &v, bool validate = true);

  // Comnon used constants allocated with DEFAULT_DIGITPOOL (see below)
  static const BigInt  &_0;         // 0
  static const BigInt  &_1;         // 1
  static const BigInt  &_2;         // 2
  static const BigReal &_05;        // 0.5

  static const BigInt  _i16_min;    // _I16_MIN
  static const BigInt  _i16_max;    // _I16_MAX
  static const BigInt  _ui16_max;   // _UI16_MAX
  static const BigInt  _i32_min;    // _I32_MIN
  static const BigInt  _i32_max;    // _I32_MAX
  static const BigInt  _ui32_max;   // _UI32_MAX
  static const BigInt  _i64_min;    // _I64_MIN
  static const BigInt  _i64_max;    // _I64_MAX
  static const BigInt  _ui64_max;   // _UI64_MAX
  static const BigInt  _i128_min;   // _I128_MIN
  static const BigInt  _i128_max;   // _I128_MAX
  static const BigInt  _ui128_max;  // _UI128_MAX
  static const BigReal _flt_min;    // FLT_MIN
  static const BigReal _flt_max;    // FLT_MAX
  static const BigReal _dbl_min;    // DBL_MIN
  static const BigReal _dbl_max;    // DBL_MAX
  static const BigReal _dbl80_min;  // DBL80_MIN
  static const BigReal _dbl80_max;  // DBL80_MAX
  static const BigReal _C1third;    // approx 1/3
  static const BigReal &_BR_QNAN;   // non-signaling NaN (quiet NaN)
  static const BigReal &_BR_PINF;   // +infinity;
  static const BigReal &_BR_NINF;   // -infinity;
};

class ConstBigReal : public BigReal {
public:
  inline ConstBigReal(int                       x) : BigReal(x, CONST_DIGITPOOL) {
  }
  inline ConstBigReal(UINT                      x) : BigReal(x, CONST_DIGITPOOL) {
  }
  inline ConstBigReal(long                      x) : BigReal(x, CONST_DIGITPOOL) {
  }
  inline ConstBigReal(ULONG                     x) : BigReal(x, CONST_DIGITPOOL) {
  }
  inline ConstBigReal(INT64                     x) : BigReal(x, CONST_DIGITPOOL) {
  }
  inline ConstBigReal(UINT64                    x) : BigReal(x, CONST_DIGITPOOL) {
  }
  inline ConstBigReal(const _int128            &x) : BigReal(x, CONST_DIGITPOOL) {
  }
  inline ConstBigReal(const _uint128           &x) : BigReal(x, CONST_DIGITPOOL) {
  }

  inline ConstBigReal(float                     x) : BigReal(x, CONST_DIGITPOOL) {
  }
  inline ConstBigReal(double                    x) : BigReal(x, CONST_DIGITPOOL) {
  }
  inline ConstBigReal(const Double80           &x) : BigReal(x, CONST_DIGITPOOL) {
  }
  inline ConstBigReal(const BigReal            &x) : BigReal(x, CONST_DIGITPOOL) {
  }
  explicit inline ConstBigReal(const String    &s) : BigReal(s, CONST_DIGITPOOL) {
  }
  explicit inline ConstBigReal(const char      *s) : BigReal(s, CONST_DIGITPOOL) {
  }
  explicit inline ConstBigReal(const wchar_t   *s) : BigReal(s, CONST_DIGITPOOL) {
  }
};

#define APCsum( bias, ...) BigReal::apcSum( #@bias, __VA_ARGS__)
#define APCprod(bias, ...) BigReal::apcProd(#@bias, __VA_ARGS__)
#define APCquot(bias, ...) BigReal::apcQuot(#@bias, __VA_ARGS__)
#define APCpow( bias, ...) BigReal::apcPow( #@bias, __VA_ARGS__)
#define APCabs( bias, ...) BigReal::apcAbs( #@bias, __VA_ARGS__)

// Assume fpclass in {_FPCLASS_PZ,_FPCLASS_NZ, _FPCLASS_PINF, _FPCLASS_NINF, _FPCLASS_QNAN, _FPCLASS_SNAN }
template<typename NumberType> NumberType getNonNormalValue(int fpclass, const NumberType &zero) {
  switch(fpclass) {
  case _FPCLASS_PZ  :
  case _FPCLASS_NZ  : return  zero;
  case _FPCLASS_PINF: return  std::numeric_limits<NumberType>::infinity();
  case _FPCLASS_NINF: return -std::numeric_limits<NumberType>::infinity();
  case _FPCLASS_QNAN: return  std::numeric_limits<NumberType>::quiet_NaN();
  case _FPCLASS_SNAN: return  std::numeric_limits<NumberType>::signaling_NaN();
  default           : NODEFAULT;
  }
}

// Returns one of
// FP_INFINITE
// FP_NAN
// FP_NORMAL
// FP_ZERO
inline int fpclassify(const BigReal &x) {
  return x._isnormal() ? FP_NORMAL : x.classifyNonNormal();
}

inline bool isfinite(const BigReal &x) {
  return fpclassify(x) <= 0;
}
inline bool isinf(const BigReal &x) {
  return x._isinf();
}
inline bool isnan(const BigReal &x) {
  return x._isnan();
}
inline bool isnormal(const BigReal &x) {
  return x._isnormal();
}
inline bool isunordered(const BigReal &x, const BigReal &y) {
  return x._isnan() || y._isnan();
}
inline bool isPInfinity(const BigReal &x) {
  return isinf(x) && x.isPositive();
}
inline bool isNInfinity(const BigReal &x) {
  return isinf(x) && x.isNegative();
}

inline bool operator==(const BigReal &x, const BigReal &y) {
  return isunordered(x,y) ? false : (BigReal::compare(x,y) == 0);
}
inline bool operator!=(const BigReal &x, const BigReal &y) {
  return isunordered(x,y) ? false : (BigReal::compare(x,y) != 0);
}
inline bool operator>=(const BigReal &x, const BigReal &y) {
  return isunordered(x,y) ? false : (BigReal::compare(x,y) >= 0);
}
inline bool operator<=(const BigReal &x, const BigReal &y) {
  return isunordered(x,y) ? false : (BigReal::compare(x,y) <= 0);
}
inline bool operator> (const BigReal &x, const BigReal &y) {
  return isunordered(x,y) ? false : (BigReal::compare(x,y) >  0);
}
inline bool operator< (const BigReal &x, const BigReal &y) {
  return isunordered(x,y) ? false : (BigReal::compare(x,y) <  0);
}

class FullFormatBigReal : public BigReal {
public:
  FullFormatBigReal(const BigReal &n, DigitPool *digitPool = NULL) : BigReal(n, digitPool) {
  }
  FullFormatBigReal(DigitPool *digitPool = NULL) : BigReal(digitPool) {
  }
  FullFormatBigReal &operator=(const BigReal &x) {
    __super::operator=(x);
    return *this;
  }
};

#ifdef LONGDOUBLE
inline Real getReal(const BigReal &x) {
  return getDouble80(x);
}
#else
inline Real getReal(const BigReal &x) {
  return getDouble(x);
}
#endif // LONGDOUBLE

std::istream     &operator>>(std::istream  &in ,       BigReal           &x);
std::ostream     &operator<<(std::ostream  &out, const BigReal           &x);
std::ostream     &operator<<(std::ostream  &out, const FullFormatBigReal &x);

std::wistream    &operator>>(std::wistream &in,        BigReal           &x);
std::wostream    &operator<<(std::wostream &out, const BigReal           &x);
std::wostream    &operator<<(std::wostream &out, const FullFormatBigReal &x);

String      toString(const BigReal           &x, StreamSize precision=20, StreamSize width = 0, FormatFlags flags = 0, TCHAR separatorChar = 0);
String      toString(const FullFormatBigReal &x, StreamSize precision=-1, StreamSize width = 0, FormatFlags flags = 0, TCHAR separatorChar = 0);

BigReal     inputBigReal( DigitPool &digitPool, _In_z_ _Printf_format_string_ TCHAR const * const format, ...);

class BigRealException : public Exception {
public:
  BigRealException(const TCHAR *text) : Exception(text) {
  };
};

#include "BigInt.h"
#include "BigRational.h"

inline BigReal dsign(const BigReal &x) {
  DigitPool *digitPool = x.getDigitPool();
  return x.isNegative() ? -digitPool->_1() : x.isPositive() ? digitPool->_1() : digitPool->_0();
}
inline BigReal  dmax(const BigReal &x, const BigReal &y) {
  return (x>=y) ? x : y;
}
inline BigReal  dmin(const BigReal &x, const BigReal &y) {
  return (x<=y) ? x : y;
}

bool isInt(    const BigReal &v, int      *n = NULL);
bool isUint(   const BigReal &v, UINT     *n = NULL);
bool isInt64(  const BigReal &v, INT64    *n = NULL);
bool isUint64( const BigReal &v, UINT64   *n = NULL);
bool isInt128( const BigReal &v, _int128  *n = NULL);
bool isUint128(const BigReal &v, _uint128 *n = NULL);

BigReal sqrt(      const BigReal &x,                   const BigReal &f, DigitPool *digitPool = NULL);
BigReal exp(       const BigReal &x,                   const BigReal &f, DigitPool *digitPool = NULL);
BigReal ln(        const BigReal &x,                   const BigReal &f, DigitPool *digitPool = NULL);
BigReal ln1(       const BigReal &x,                   const BigReal &f, DigitPool *digitPool = NULL);
// log(x) base 10
BigReal log10(     const BigReal &x,                   const BigReal &f, DigitPool *digitPool = NULL);
// Logarithm(x) for the specified base. (ln(x)/ln(base))
BigReal log(    const BigReal &base, const BigReal &x, const BigReal &f, DigitPool *digitPool = NULL);
// x^y
BigReal pow(       const BigReal &x, const BigReal &y, const BigReal &f, DigitPool *digitPool = NULL);
BigReal sin(       const BigReal &x,                   const BigReal &f, DigitPool *digitPool = NULL);
BigReal cos(       const BigReal &x,                   const BigReal &f, DigitPool *digitPool = NULL);
BigReal tan(       const BigReal &x,                   const BigReal &f, DigitPool *digitPool = NULL);
BigReal cot(       const BigReal &x,                   const BigReal &f, DigitPool *digitPool = NULL);
BigReal asin(      const BigReal &x,                   const BigReal &f, DigitPool *digitPool = NULL);
BigReal acos(      const BigReal &x,                   const BigReal &f, DigitPool *digitPool = NULL);
BigReal atan(      const BigReal &x,                   const BigReal &f, DigitPool *digitPool = NULL);
BigReal acot(      const BigReal &x,                   const BigReal &f, DigitPool *digitPool = NULL);
// result.digitPool = f.digitPool or digitpool if specified
BigReal pi(                                           const BigReal &f, DigitPool *digitPool = NULL);

// Calculates with relative precision ie. with the specified number of decimal digits
BigReal rRound(    const BigReal &x,                  size_t digits,    DigitPool *digitPool = NULL);
BigReal rSum(      const BigReal &x,const BigReal &y, size_t digits,    DigitPool *digitPool = NULL);
BigReal rDif(      const BigReal &x,const BigReal &y, size_t digits,    DigitPool *digitPool = NULL);
BigReal rProd(     const BigReal &x,const BigReal &y, size_t digits,    DigitPool *digitPool = NULL);
BigReal rQuot(     const BigReal &x,const BigReal &y, size_t digits,    DigitPool *digitPool = NULL);
BigReal rSqrt(     const BigReal &x,                  size_t digits,    DigitPool *digitPool = NULL);
BigReal rExp(      const BigReal &x,                  size_t digits,    DigitPool *digitPool = NULL);
BigReal rLn(       const BigReal &x,                  size_t digits,    DigitPool *digitPool = NULL);
BigReal rLog10(    const BigReal &x,                  size_t digits,    DigitPool *digitPool = NULL);
// Logarithm(x) for the specified base. (ln(x)/ln(base))
BigReal rLog(  const BigReal &base, const BigReal &x, size_t digits,    DigitPool *digitPool = NULL);
// x^y
BigReal rPow(      const BigReal &x,const BigReal &y, size_t digits,    DigitPool *digitPool = NULL);
BigReal rRoot(     const BigReal &x,const BigReal &y, size_t digits,    DigitPool *digitPool = NULL);
BigReal rSin(      const BigReal &x,                  size_t digits,    DigitPool *digitPool = NULL);
BigReal rCos(      const BigReal &x,                  size_t digits,    DigitPool *digitPool = NULL);
BigReal rTan(      const BigReal &x,                  size_t digits,    DigitPool *digitPool = NULL);
BigReal rCot(      const BigReal &x,                  size_t digits,    DigitPool *digitPool = NULL);
BigReal rAsin(     const BigReal &x,                  size_t digits,    DigitPool *digitPool = NULL);
BigReal rAcos(     const BigReal &x,                  size_t digits,    DigitPool *digitPool = NULL);
BigReal rAtan(     const BigReal &x,                  size_t digits,    DigitPool *digitPool = NULL);
BigReal rAcot(     const BigReal &x,                  size_t digits,    DigitPool *digitPool = NULL);
BigReal rPi(                                          size_t digits,    DigitPool *digitPool = NULL);
BigReal rGamma(    const BigReal &x,                  size_t digits,    DigitPool *digitPool = NULL);
BigReal rFactorial(const BigReal &x,                  size_t digits,    DigitPool *digitPool = NULL);

// Old version sign(x) * (|x| - floor(|x|))
BigReal oldFraction(const BigReal &x);
// old operator%(const BigReal &x, const BigReal &y);
BigReal modulusOperator64( const BigReal &x, const BigReal &y);
// old operator%(const BigReal &x, const BigReal &y);
BigReal modulusOperator128(const BigReal &x, const BigReal &y);

#pragma comment(lib,  TM_LIB_VERSION "BigReal.lib")
