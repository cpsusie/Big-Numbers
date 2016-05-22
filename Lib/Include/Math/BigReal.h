#pragma once

#include <MyUtil.h>
#include <Thread.h>
#include <CompactStack.h>
#include <CompactHashMap.h>
#include <SynchronizedQueue.h>
#include <StrStream.h>
#include <ByteStream.h>
#include <Packer.h>
#include <HashMap.h>
#include <Random.h>
#include "Double80.h"
#include "Real.h"

// Number of decimal digits in 1 BigReal digit
#define LOG10_BIGREALBASE  8

#define SP_OPT_NONE      0
#define SP_OPT_BY_FPU    1
#define SP_OPT_BY_FPU2   2
#define SP_OPT_BY_REG32  3

// Define SP_OPT_METHOD to one of these, to select the desired optmization of shortProduct. My own favorite is SP_OPT_BY_REG32

//#define SP_OPT_METHOD  SP_OPT_NONE
//#define SP_OPT_METHOD  SP_OPT_BY_FPU
//#define SP_OPT_METHOD  SP_OPT_BY_FPU2
#define SP_OPT_METHOD SP_OPT_BY_REG32

#ifndef SP_OPT_METHOD

#error Must define SP_OPT_METHOD

#elif((SP_OPT_METHOD != SP_OPT_NONE) && (SP_OPT_METHOD != SP_OPT_BY_FPU) && (SP_OPT_METHOD != SP_OPT_BY_FPU2) && (SP_OPT_METHOD != SP_OPT_BY_REG32))

#error Must define SP_OPT_METHOD

#endif

// Define the following, to get faster, but unprecise conversion from/to float/double/Double80
//#define FAST_BIGREAL_CONVERSION

#if(SP_OPT_METHOD == SP_OPT_NONE)

#if   LOG10_BIGREALBASE == 2
#define SQRT_BIGREALBASE 10
#elif LOG10_BIGREALBASE == 4
#define SQRT_BIGREALBASE 100
#elif LOG10_BIGREALBASE == 6
#define SQRT_BIGREALBASE 1000
#elif LOG10_BIGREALBASE == 8
#define SQRT_BIGREALBASE 10000
#else // LOG10_BIGREALBASE
#error Illegal LOG10_BIGREALBASE. Must be in the set {2,4,6 or 8}
#endif // LOG10_BIGREALBASE
#define BIGREALBASE (SQRT_BIGREALBASE*SQRT_BIGREALBASE)

#elif(SP_OPT_METHOD == SP_OPT_BY_FPU)

#if   LOG10_BIGREALBASE == 1
#define BIGREALBASE 10
#elif LOG10_BIGREALBASE == 2
#define BIGREALBASE 100
#elif LOG10_BIGREALBASE == 3
#define BIGREALBASE 1000
#elif LOG10_BIGREALBASE == 4
#define BIGREALBASE 10000
#elif LOG10_BIGREALBASE == 5
#define BIGREALBASE 100000
#elif LOG10_BIGREALBASE == 6
#define BIGREALBASE 1000000
#elif LOG10_BIGREALBASE == 7
#define BIGREALBASE 10000000
#elif LOG10_BIGREALBASE == 8
#define BIGREALBASE 100000000
#else
#error Illegal LOG10_BIGREALBASE. Must be in the interval [1-8]
#endif // LOG10_BIGREALBASE
// Cannot use 1e9 because of overflow in function shortProductNoZeroCheck, when accumulating more than 18 products, which will
// occur when splitLength > 18. And a splitlength of 18 is far to small to give product a boost

#elif(SP_OPT_METHOD == SP_OPT_BY_REG32 || SP_OPT_METHOD == SP_OPT_BY_FPU2)

#if LOG10_BIGREALBASE == 8
#define BIGREALBASE 100000000
#else
#error Illegal LOG10_BIGREALBASE. Must be 8.
// this is the only valid BIGREALBASE for these optimizations
#endif // LOG10_BIGREALBASE

#endif // if(SP_OPT_METHOD == ...)

#define BIGREAL_ZEROEXPO  -900000000
#define BIGREAL_MAXEXPO    99999999
#define BIGREAL_MINEXPO   -99999999
#define APC_DIGITS         6

#define CONVERSION_POW2DIGITCOUNT 23

class Digit { // The definition of Digit should NOT be changed!!! Se asm-code in ShortProduct*.cpp
public:
  unsigned long n;
  Digit         *next;
  Digit         *prev;
};

#define DIGITPAGESIZE 30000
class DigitPage {
  Digit      m_page[DIGITPAGESIZE];
  DigitPage *m_next;
  friend class DigitPool;
  DigitPage(           const DigitPage &src); // not implemented
  DigitPage &operator=(const DigitPage &src); // not implemented
public:
  DigitPage(DigitPage *nextPage, Digit *nextDigit);
};

class BigInt;
class BigReal;

class Pow2ArgumentKey {
public:
  int          m_n;
  unsigned int m_digits;
  Pow2ArgumentKey() {
  }
  inline Pow2ArgumentKey(int n, unsigned int digits) : m_n(n), m_digits(digits) {
  }
  inline Pow2ArgumentKey(ByteInputStream &s) {
    load(s);
  }
  inline unsigned long hashCode() const {
    return m_n * 23 + m_digits;
  }
  inline bool operator==(const Pow2ArgumentKey &k) const {
    return (m_n == k.m_n) && (m_digits == k.m_digits);
  }
  void save(ByteOutputStream &s) const {
    s.putBytes((BYTE*)this, sizeof(Pow2ArgumentKey));
  }

  void load(ByteInputStream &s) {
    s.getBytesForced((BYTE*)this, sizeof(Pow2ArgumentKey));
  }
};

class Pow2Cache : public CompactHashMap<Pow2ArgumentKey, BigReal*> {
private:
  Semaphore  m_gate;
  bool       m_loaded;

  void save(const String &fileName);
  void load(const String &fileName);
  void save(ByteOutputStream &s) const;
  void load(ByteInputStream  &s);
  void clear();
public:
#ifdef _DEBUG
  static unsigned int s_cacheHitCount, s_cacheRequestCount;
#endif

  Pow2Cache();
  ~Pow2Cache();
  inline bool isLoaded() const {
    return m_loaded;
  }
};

class BigRealResource {
private:
  const int m_id;
public:
  BigRealResource(int id) : m_id(id) {
  }
  inline int getId() const {
    return m_id;
  }
};

#define DEFAULT_DIGITPOOL_ID -1
#define CONST_DIGITPOOL_ID   -2
#define PI_DIGITPOOL_ID      -3
#define LN_DIGITPOOL_ID      -4

#define USE_DIGITPOOL_FREELIST
// If this is not defined, all digit-allocations/deallocations will be done with new/delete, which will slow down the program
// (testBigReal) by a factor 20!!

class DigitPool : public BigRealResource {
private:
  static unsigned long  s_totalDigitCount;
  static bool           s_dumpCountWhenDestroyed;
  unsigned long         m_digitCount;
  DigitPage            *m_firstPage;
  Digit                *m_freeDigits;;

  // Some frequently used constants. exist in every pool
  BigInt               *m_zero; // = 0
  BigInt               *m_one;  // = 1
  BigInt               *m_two;  // = 2
  BigReal              *m_half; // = 0.5

  static void addToCount(int n);
  DigitPool(           const DigitPool &src); // not implemented
  DigitPool &operator=(const DigitPool &src); // not implemented
  void allocatePage();
public:
  static DigitPool s_defaultDigitPool;

  DigitPool(int id, unsigned int intialDigitcount = 0); // initialDigitcount in BIGREALBASE-digits
  virtual ~DigitPool();

  static void setDumpWhenDestroyed(bool dump) {
    s_dumpCountWhenDestroyed = dump;
  }
  inline Digit *newDigit() {
#ifdef USE_DIGITPOOL_FREELIST
/*
    if((getId() == DEFAULT_DIGITPOOL_ID) || (getId() == CONST_DIGITPOOL_ID)) {
      int fisk1 = 1;
    }
*/
    if(m_freeDigits == NULL) allocatePage();
    Digit *p     = m_freeDigits;
    m_freeDigits = p->next;
    return p;
#else
    return new Digit;
#endif
  }

  inline void deleteDigits(Digit *first, Digit *last) {
#ifdef USE_DIGITPOOL_FREELIST
    last->next = m_freeDigits;
    m_freeDigits = first;
#else
    const Digit *end = last->next;
    for(;;) {
      Digit *p = first->next;
      delete first;
      if(p == end) break;
      first = p;
    } 
#endif
  }
  inline const BigInt &get0() const {
    return *m_zero;
  }
  inline const BigInt &get1() const {
    return *m_one;
  }
  inline const BigInt &get2() const {
    return *m_two;
  }
  inline const BigReal &getHalf() const {
    return *m_half;
  }
  static inline unsigned long getTotalAllocatedDigitCount() {
    return s_totalDigitCount;
  }
  inline unsigned long getAllocatedDigitCount() const {
    return m_digitCount;
  }
  unsigned int getFreeDigitCount() const;
  unsigned int getUsedDigitCount() const;
  unsigned int getPageCount()      const;
};

#define DEFAULT_DIGITPOOL DigitPool::s_defaultDigitPool

#ifdef _DEBUG

#define SETBIGREALDEBUGSTRING( x) { if(BigReal::s_debugStringEnabled) { (x).ajourDebugString();  } }
#define COPYBIGREALDEBUGSTRING(x) { if(s_debugStringEnabled) { memcpy(m_debugString, (x).m_debugString, sizeof(m_debugString)); } }

#else // _DEBUG

#define SETBIGREALDEBUGSTRING( x)
#define COPYBIGREALDEBUGSTRING(x)

#endif // _DEBUG

class BigRealStream;

#define _SETDIGITPOOL() m_digitPool(digitPool?*digitPool:DEFAULT_DIGITPOOL)

#ifdef _DEBUG
extern "C" { 
void insertDigitAndIncrExpo(BigReal &v, unsigned int n);
}
#endif

void throwInvalidToleranceException(               const TCHAR *function);
void throwBigRealInvalidArgumentException(         const TCHAR *function, const TCHAR *format,...);
void throwBigRealGetIntegralTypeOverflowException( const TCHAR *function, const BigReal &x, const String &maxStr);
void throwBigRealGetIntegralTypeUnderflowException(const TCHAR *function, const BigReal &x, const String &minStr);
void throwBigRealException(const TCHAR *format,...);

class BigReal {

#ifdef _DEBUG
private:
  mutable TCHAR m_debugString[60];
public:
  void ajourDebugString() const;
  static Semaphore s_debugStringGate;
  static bool      s_debugStringEnabled;
#endif // _DEBUG

private:
  static Pow2Cache          s_pow2Cache;
  static const unsigned int s_power10Table[];
  static int                s_splitLength;         // Defined in shortProduct.cpp. Split factors when length > s_splitLength
  static bool               s_continueCalculation; // Terminate all calculation with and Exception ("Operation was cancelled")
                                                   // if they enter shortProduct, or simply return whichever comes first
  Digit                    *m_first;               // Most significand  digit
  Digit                    *m_last;                // Least significand digit
  int                       m_expo, m_low;         // if isZero() then (m_expo,m_low)=(BIGREAL_ZEROEXPO,0) else m_expo = m_low+getLength()-1
  bool                      m_negative;            // True for x < 0. else false
  DigitPool                &m_digitPool;

  // Multiplication helperfunctions
  friend class MultiplierThread;
  friend class BigRealTestClass;
  friend class BigInt;
  friend class RandomBigReal;

  // Construction helperfunctions
  inline void init() {
    m_first    = m_last = NULL;
    m_expo     = BIGREAL_ZEROEXPO;
    m_low      = 0;
    m_negative = false;
  }
  void init(           int    n);
  void init(unsigned   int    n);
  void init(         __int64  n);
  void init(unsigned __int64  n);
  void init(float             x);
  void init(double            x);
  void init(const Double80   &x);
  void init(const String     &s, bool allowDecimalPoint);

  // Basic digit manipulation
  inline Digit *newDigit() {
    return m_digitPool.newDigit();
  }
  inline void deleteDigits(Digit *first, Digit *last) {
    m_digitPool.deleteDigits(first, last);
  }

  inline void appendDigit(unsigned long n) {
  //  assert(n < BIGREALBASE);
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
  void    insertAfter(            Digit *q, unsigned long n);
  void    insertDigit(                      unsigned long n);
  void    insertZeroDigits(                 unsigned int count);
  void    insertZeroDigitsAfter(  Digit *p, unsigned int count);
  void    insertBorrowDigitsAfter(Digit *p, unsigned int count);
  inline void clearDigits() {
    if(m_first) {
      m_digitPool.deleteDigits(m_first, m_last);
      m_first = m_last = NULL;
    }
  }

  inline void    incrExpo() { 
    if(isZero()) m_expo = m_low = 0; else m_expo++;
  }
  
  inline BigReal &trimZeroes() {                                                                  // return this
    if(m_first) {
      if(m_first->n == 0)     trimHead();
      else if(m_last->n == 0) trimTail();
      if(m_expo > BIGREAL_MAXEXPO) throwBigRealException(_T("Overflow"));
      if(m_low  < BIGREAL_MINEXPO) throwBigRealException(_T("Underflow"));
    }
    return *this;
  }
  void    trimHead();
  void    trimTail();
  Digit  *findDigit(const int exponent) const;
  Digit  *findDigitSubtract(const BigReal &f) const;
  Digit  *findDigitAdd(const BigReal &f, int &low) const;

  // Addition and subtraction helperfunctions
  BigReal &addAbs(const BigReal &x, const BigReal &y, const BigReal &f);                          // return this
  BigReal &addAbs(const BigReal &x);                                                              // return this
  BigReal &subAbs(const BigReal &x, const BigReal &f);                                            // return this

#if(  SP_OPT_METHOD == SP_OPT_NONE)

  void    baseb(const BigReal &x);
  void    baseb(const BigReal &x, int low);
  BigReal &baseB(const BigReal &x);                                                               // return this
  void    addSubProduct(unsigned __int64 n);

#elif(SP_OPT_METHOD == SP_OPT_BY_FPU)

  void    addFPUReg0();
  void    addSubProduct(unsigned __int64 n);

#elif((SP_OPT_METHOD == SP_OPT_BY_REG32) || (SP_OPT_METHOD == SP_OPT_BY_FPU2))

  inline Digit *fastAppendDigit(Digit *last) { // Assume last != NULL. new digit->n and ..->next are not initialized
    Digit *p = last->next = newDigit();        // Should only be called from shortProductNoZeroCheck. m_last will
    p->prev  = last;                           // not be touched, so invariant will be broken for af while,
    return p;                                  // and reestablished at the end of shortProductNoZeroCheck
  }                                            // when we clean up the chain head and end

#endif

#ifdef _DEBUG
  // One that works, and used for testing other versions of this important function
  BigReal &shortProductNoZeroCheckReference(const BigReal &x, const BigReal &y, int loopCount);   // return this
  friend void insertDigitAndIncrExpo(BigReal &v, unsigned int n);
  void    addSubProductReference1(unsigned __int64 &n);
  void    addSubProductReference2(unsigned __int64 &n);
  static  bool s_useShortProdReferenceVersion;
  BigReal &shortProductNoZeroCheckDebug(    const BigReal &x, const BigReal &y, int loopCount);   // return this
public:
  static bool setUseShortProdRefenceVersion(bool useReferenceVersion) {
    bool oldValue = s_useShortProdReferenceVersion; s_useShortProdReferenceVersion = useReferenceVersion; return oldValue;
  }
private:
#endif

  BigReal &shortProduct(                     const BigReal &x, const BigReal &y, int fexpo    );  // return this
  BigReal &shortProductNoZeroCheck(          const BigReal &x, const BigReal &y, int loopCount);  // return this
  static BigReal &product(  BigReal &result, const BigReal &x, const BigReal &y, const BigReal &f,         int level);
  static BigReal &productMT(BigReal &result, const BigReal &x, const BigReal &y, const BigReal &f, int  w, int level);

  void    split(BigReal &a, BigReal &b, int n, const BigReal &f) const;
  void    copyDigits(   const BigReal &src, unsigned int length);
  BigReal &copyrTrunc(   const BigReal &src, unsigned int digits); // copy specified number of decimal digits, truncating result. return *this
  void    copyAllDigits(const BigReal &src);
  inline BigReal &setSignByProductRule(const BigReal &x, const BigReal &y) {
    m_negative = x.m_negative != y.m_negative;
    SETBIGREALDEBUGSTRING(*this);
    return *this;
  }

  // rTrunc and rRound cut of a number (!= 0) to the specified number of significant decimal digits,
  // truncating towards zero, or rounding respectively.
  BigReal &rTrunc(unsigned int digits);  // assume *this != 0 and digits > 0 (digits is decimal digits). return this
  BigReal &rRound(unsigned int digits);  // assume *this != 0 and digits > 0 (digits is decimal digits). return this

  // Division helperfunctions, 
  // Result.digitPool = x x.digitPool
  static BigReal  reciprocal(const BigReal &x, DigitPool *digitPool = NULL);                      // approximately 1/x
  BigReal &approxQuot32(     const BigReal &x, const BigReal           &y           );            // approximately x/y
  BigReal &approxQuot64(     const BigReal &x, const BigReal           &y);                       // approximately x/y
  BigReal &approxQuot32Abs(  const BigReal &x, unsigned long            y, int scale);            // approximately x/e(y,scale)
  BigReal &approxQuot64Abs(  const BigReal &x, const unsigned __int64  &y, int scale);            // approximately x/e(y,scale)
  static double  estimateQuotNewtonTime(const BigReal &x, const BigReal &y, const BigReal &f);
  static double  estimateQuotLinearTime(const BigReal &x, const BigReal &y, const BigReal &f);
  static bool    chooseQuotNewton(      const BigReal &x, const BigReal &y, const BigReal &f);

  // Misc
  BigReal  &adjustAPCResult(const char bias, const TCHAR *function); // return this
  Double80 getDouble80NoLimitCheck() const; 
  void     formatFixed(        String &result, int precision, long flags, bool removeTrailingZeroes) const;
  void     formatScientific(   String &result, int precision, long flags, int expo10, bool removeTrailingZeroes) const;
  void     formatWithSpaceChar(String &result, TCHAR spaceChar) const;

public:
  inline BigReal(                          DigitPool *digitPool = NULL) : _SETDIGITPOOL() {
    init();
    SETBIGREALDEBUGSTRING(*this)
  }

  inline BigReal(int              x      , DigitPool *digitPool = NULL) : _SETDIGITPOOL() {
    init(x);
    SETBIGREALDEBUGSTRING(*this)
  }

  inline BigReal(unsigned int     x      , DigitPool *digitPool = NULL) : _SETDIGITPOOL() {
    init(x);
    SETBIGREALDEBUGSTRING(*this)
  }

  inline BigReal(long             x      , DigitPool *digitPool = NULL) : _SETDIGITPOOL() {
    init((int)x);
    SETBIGREALDEBUGSTRING(*this)
  }

  inline BigReal(unsigned long    x      , DigitPool *digitPool = NULL) : _SETDIGITPOOL() {
    init((unsigned int)x);
    SETBIGREALDEBUGSTRING(*this)
  }

  inline BigReal(__int64          x      , DigitPool *digitPool = NULL) : _SETDIGITPOOL() {
    init(x);
    SETBIGREALDEBUGSTRING(*this)
  }

  inline BigReal(unsigned __int64 x      , DigitPool *digitPool = NULL) : _SETDIGITPOOL() {
    init(x);
    SETBIGREALDEBUGSTRING(*this)
  }

  BigReal(float                   x      , DigitPool *digitPool = NULL) : _SETDIGITPOOL() {
    init(x);
    SETBIGREALDEBUGSTRING(*this)
  }

  BigReal(double                  x      , DigitPool *digitPool = NULL) : _SETDIGITPOOL() {
    init(x);
    SETBIGREALDEBUGSTRING(*this)
  }

  BigReal(const Double80         &x      , DigitPool *digitPool = NULL) : _SETDIGITPOOL() {
    init(x);
    SETBIGREALDEBUGSTRING(*this)
  }

  BigReal(const BigReal           &x      , DigitPool *digitPool = NULL);

  explicit inline BigReal(const String &s, DigitPool *digitPool = NULL) : _SETDIGITPOOL() {
    init(s, true);
  }

  explicit inline BigReal(const TCHAR  *s, DigitPool *digitPool = NULL) : _SETDIGITPOOL() {
    init(s, true);
  }

#ifdef UNICODE
  explicit inline BigReal(const char   *s, DigitPool *digitPool = NULL) : _SETDIGITPOOL() {
    init(s, true);
  }
#endif

  explicit inline BigReal(ByteInputStream &s, DigitPool *digitPool = NULL) : _SETDIGITPOOL() {
    init();
    load(s);
  }

  virtual ~BigReal() {
    clearDigits();
  }

  inline BigReal &operator=(int              n) {
    clearDigits(); init(n);
    SETBIGREALDEBUGSTRING(*this);
    return *this;
  }

  inline BigReal &operator=(unsigned int     n) {
    clearDigits(); init(n);
    SETBIGREALDEBUGSTRING(*this);
    return *this;
  }

  inline BigReal &operator=(long             n) {
    clearDigits(); init(n);
    SETBIGREALDEBUGSTRING(*this);
    return *this;
  }

  inline BigReal &operator=(unsigned long    n) {
    clearDigits(); init((unsigned int)n);
    SETBIGREALDEBUGSTRING(*this);
    return *this;
  }

  inline BigReal &operator=(__int64          n) {
    clearDigits(); init(n);
    SETBIGREALDEBUGSTRING(*this);
    return *this;
  }

  inline BigReal &operator=(unsigned __int64 n) {
    clearDigits(); init(n);
    SETBIGREALDEBUGSTRING(*this);
    return *this;
  }

  inline BigReal &operator=(float            x) {
    clearDigits(); init(x);
    SETBIGREALDEBUGSTRING(*this);
    return *this;
  }

  inline BigReal &operator=(double           x) {
    clearDigits(); init(x);
    SETBIGREALDEBUGSTRING(*this);
    return *this;
  }

  inline BigReal &operator=(const Double80  &x) {
    clearDigits(); init(x);
    SETBIGREALDEBUGSTRING(*this);
    return *this;
  }

  BigReal &operator=(const BigReal    &x);

  friend          int     getInt(     const BigReal  &x);
  friend unsigned int     getUint(    const BigReal  &x);
  friend          long    getLong(    const BigReal  &x);
  friend unsigned long    getUlong(   const BigReal  &x);
  friend          __int64 getInt64(   const BigReal  &x);
  friend unsigned __int64 getUint64(  const BigReal  &x);
  friend float            getFloat(   const BigReal  &x);
  friend double           getDouble(  const BigReal  &x);
  friend Double80         getDouble80(const BigReal  &x);

  // Result.digitPool = x x.digitPool
  friend BigReal  operator+ (const BigReal &x, const BigReal &y);
  friend BigReal  operator- (const BigReal &x, const BigReal &y);
  friend BigReal  operator* (const BigReal &x, const BigReal &y);
  friend BigReal  operator% (const BigReal &x, const BigReal &y);
  friend BigReal  operator- (const BigReal &x);

  // digitPool unchanged
  BigReal &operator+=(const BigReal &x);
  BigReal &operator-=(const BigReal &x);
  BigReal &operator*=(const BigReal &x);
  BigReal &operator%=(const BigReal &x);
  BigReal &operator++();                                                      // prefix-form
  BigReal &operator--();                                                      // prefix-form
  BigReal  operator++(int);                                                   // postfix-form
  BigReal  operator--(int);                                                   // postfix-form
  BigReal &multPow10(int exp);

  // Result.digitPool = x x.digitPool
  friend BigInt operator/ (     const BigInt &x, const BigInt &y);
  friend BigInt operator% (     const BigInt &x, const BigInt &y);
  friend BigInt operator- (     const BigInt &x);
  friend BigReal  sum(          const BigReal &x,  const BigReal &y, const BigReal  &f, DigitPool *digitPool = NULL); // x+y with |error| <= f
  friend BigReal  dif(          const BigReal &x,  const BigReal &y, const BigReal  &f, DigitPool *digitPool = NULL); // x-y with |error| <= f
  friend BigReal  prod(         const BigReal &x,  const BigReal &y, const BigReal  &f, DigitPool *digitPool = NULL); // x*y with |error| <= f
  friend BigReal  quot(         const BigReal &x,  const BigReal &y, const BigReal  &f, DigitPool *digitPool = NULL); // x/y with |error| <= f
  friend void    quotRemainder( const BigReal &x,  const BigReal &y, BigInt *quotient, BigReal *remainder);  // Calculates only quotient and/or remainder if specified
  friend void    quotRemainder1(const BigReal &x,  const BigReal &y, BigInt *quotient, BigReal *remainder);  // Calculates only quotient and/or remainder if specified
  static BigReal  apcSum(       const char bias,  const BigReal &x, const BigReal  &y, DigitPool *digitPool = NULL); // bias = '<','#' or '>'
  static BigReal  apcProd(      const char bias,  const BigReal &x, const BigReal  &y, DigitPool *digitPool = NULL); // bias = '<','#' or '>'
  static BigReal  apcQuot(      const char bias,  const BigReal &x, const BigReal  &y, DigitPool *digitPool = NULL); // bias = '<','#' or '>'
  static BigReal  apcPow(       const char bias,  const BigReal &x, const BigInt   &y, DigitPool *digitPool = NULL); // bias = '<','#' or '>'
  
  void           fractionate(BigInt *integerPart, BigReal *fractionPart) const;
  // result.digitPool = f.defaultDigitPool
  static BigReal ln10(         const BigReal &f);                                     // ln(10) with |error| < f
  static BigReal lnEstimate(   const BigReal &x);                                     // Approximatly ln(x). Assume 1 <= x <= 10
  static const BigReal &pow2(int n, unsigned int digits = 0);                         // 2^n, with the specified number of digits. if digits = 0, then full precision. Results are cached

  friend int compare(         const BigReal &x,  const BigReal &y);                   // sign(x-y)
  friend int compareAbs(      const BigReal &x,  const BigReal &y);                   // compare(|x|,|y|). (Faster than compare(fabs(x),fabs(y)))
  
  inline bool isZero() const { 
    return m_expo == BIGREAL_ZEROEXPO;
  }
  inline void setToZero() {
    clearDigits();
    init();
    SETBIGREALDEBUGSTRING(*this);
  }

  inline BigReal &changeSign() {
    if(!isZero()) { m_negative = !m_negative; SETBIGREALDEBUGSTRING(*this); }
    return *this;
  }

  inline void setPositive() {
#ifdef _DEBUG
    if(m_negative) {
      changeSign();
    }
#else
    m_negative = false;
#endif
  }
  inline  bool isPositive() const {
    return m_expo != BIGREAL_ZEROEXPO && !m_negative;
  }
  
  inline bool isNegative() const {
    return m_expo != BIGREAL_ZEROEXPO && m_negative;
  }
  
  inline int getLength() const {                                                      // BigReal of BASE digits. 0 has length 1
    return isZero() ? 1 : (m_expo - m_low + 1);
  }      
  int getDecimalDigits()  const;                                                      // BigReal of decimal digits. 0 has length 1
  inline int getLow()     const {
    return isZero() ? 0 : m_low;
  }

  static inline int       getExpo10(  const BigReal  &x) {                            // x == 0 ? 0 : floor(log10(|x|))
    return x.isZero() ? 0 : (x.m_expo * LOG10_BIGREALBASE + getDecimalDigitCount(x.m_first->n) - 1);
  }

  static BigReal           getExpo10N( const BigReal  &x) {
    return BigReal(getExpo10(x), x.getDigitPool());
  }

  static inline int       pow10(unsigned int n) {                                     // Return 10^n. Assume n <= 9
#ifdef _DEBUG
    if(n <= 9) {
      return s_power10Table[n];
    }
    throwBigRealInvalidArgumentException(_T("pow10"), _T("n must be [0..9] (=%d)"),n);
    return 1;
#else
    return s_power10Table[n];
#endif
  }

  static int     getExpo2(   const BigReal  &x);                                       // x == 0 ? 0 : approximately floor(log2(|x|))
  static int     getDecimalDigitCount(unsigned long n);                                // n == 0 ? 0 : (floor(log10(n))+1). Assume n < BIGREALBASE
  static int     getDecimalDigitCount64(unsigned __int64 n);                           // as above but for n < 1eMAXDIGITS_INT64
  static int     logBASE(double x);                                                    // (int)(log10(x) / LOG10_BIGREALBASE)
  static int     isPow10(unsigned long n);                                             // Return p if n = 10^p for p = [0..9]. else return -1.
  static bool    isPow10(const BigReal &x);                                            // true if |x| = 10^p for p = [BIGREAL_MINEXPO..BIGREAL_MAXEXPO]

  // Result.digitPool = x x.digitPool
  friend BigReal fabs(     const BigReal &x);                                          // absolute value of x (=|x|)
  friend BigInt  floor(    const BigReal &x);                                          // biggest integer <= x
  friend BigInt  ceil(     const BigReal &x);                                          // smallest integer >= x
  friend BigReal fraction( const BigReal &x);                                          // sign(x) * (|x| - floor(|x|))
  friend BigReal round(    const BigReal &x, int prec = 0);                            // sign(x) * floor(|x|*10^prec+0.5)/10^prec
  friend BigReal trunc(    const BigReal &x, int prec = 0);                            // sign(x) * floor(|x|*10^prec)/10^prec
  friend BigReal cut(      const BigReal &x, unsigned int digits, DigitPool *digitPool = NULL); // x truncated to the specified number of significant decimal digits
  friend BigReal e(        const BigReal &x, int n,               DigitPool *digitPool = NULL); // x * pow(10,n)

  friend inline int sign(  const BigReal &x) {                                         // x < 0 ? -1 : x > 0 ? 1 : 0
    return x.isZero() ? 0 : x.isPositive() ? 1 : -1;
  }

  friend bool    odd(      const BigReal &x);                                          // x is an integer and x % 2 == 1
  friend bool    even(     const BigReal &x);                                          // x is an integer and x % 2 == 0
  friend inline bool isInteger(const BigReal &x) {                                     // fraction(x) == 0
    return x.getLow() >= 0;
  }

  static BigReal  random(int digits, Random *rnd = NULL, DigitPool *digitPool = NULL); // 0 <= random < 1; with the specified number of decimal digits, 
                                                                                       // Digits generated with rnd. if rnd == NULL, _standardRandomGenerator is used

  friend BigReal &copy(BigReal &to, const BigReal &from, const BigReal &f);            // Set to = from so |to-from| <= f. Return to
  friend BigReal &copy(BigReal &to, const BigReal &from, unsigned int length);         // Set to = from so to.getlength() = min(length,from.getlength(). Return to

  static BigReal quotNewton(   const BigReal &x, const BigReal &y, const BigReal &f, DigitPool *digitPool);   // x/y with |error| < f. Newton-rapthon iteration
  static BigReal quotLinear32( const BigReal &x, const BigReal &y, const BigReal &f, DigitPool *digitPool);   // x/y with |error| < f. School method. using build-in 32-bit division
  static BigReal quotLinear64( const BigReal &x, const BigReal &y, const BigReal &f, DigitPool *digitPool);   // x/y with |error| < f. School method. using build-in 64-bit division
  static BigReal shortProd(    const BigReal &x, const BigReal &y, const BigReal &f, DigitPool *digitPool);   // x*y with |error| < f. Used to multiply short numbers. Used by prod.

  inline unsigned long getFirstDigit() const {                                         // isZero() ? 0 : first BASE-digit.
    return isZero() ? 0 : m_first->n;
  }
  inline unsigned long  getLastDigit()  const {                                        // isZero() ? 0 : last BASE-digit.
    return isZero() ? 0 : m_last->n;
  }

  unsigned long    getFirst32(const unsigned int k, int *scale = NULL) const;          // First k decimal digits. Assume k <= 9.
  unsigned __int64 getFirst64(const unsigned int k, int *scale = NULL) const;          // First k decimal digits. Assume k <= 19.

  String toString() const;
  inline DigitPool *getDigitPool() const {
    return &m_digitPool;
  }
  inline int getPoolId() const {
    return m_digitPool.getId();
  }
  unsigned long hashCode() const;

  void print(FILE *f = stdout, bool spacing = false) const;
  void dump( FILE *f = stdout) const;

  void save(ByteOutputStream &s) const;
  void load(ByteInputStream  &s);

  friend Packer &operator<<(Packer &p, const BigReal &n);
  friend Packer &operator>>(Packer &p,       BigReal &n);

  friend BigRealStream &operator<<(BigRealStream &stream, const BigReal &x);

  static bool enableDebugString(bool enabled);                                         // return old value
  static bool isDebugStringEnabled();

  static unsigned int getMaxSplitLength();

  static void resumeCalculation() {    // Call this when terminateCalculation has been called and you want to start a new calculation
    s_continueCalculation = true;
  }

  static void terminateCalculation() { // Call this to make the calculation stop as soon as possible. In current version it will 
                                       // terminate all Threads started from BigRealThreadPool
    s_continueCalculation = false;
  }

  void assertIsValidBigReal() const; // checks that this is a consistent number with all the various invariants satisfied.
                                     // Throws an excpeption if not with a descripion of what is wrong. For debugging

};

class ConstBigReal : public BigReal {
private:
  static Semaphore s_gate;
  static DigitPool s_digitPool;
  static int       s_requestCount;

  friend class ConstInteger;

  static inline DigitPool *requestConstDigitPool() {
    s_gate.wait();
    s_requestCount++;
    return &s_digitPool;
  }
  static inline void releaseConstDigitPool() {
    s_gate.signal();
  }
#define REQPOOL requestConstDigitPool()
#define RELPOOL releaseConstDigitPool();
public:
  static inline const DigitPool &getDigitPool() {
    return s_digitPool;
  }
  static inline int getPoolRequestCount() {
    return s_requestCount;
  }

  inline ConstBigReal(int              x) : BigReal(x, REQPOOL) {
    RELPOOL;
  }

  inline ConstBigReal(unsigned int     x) : BigReal(x, REQPOOL) {
    RELPOOL;
  }

  inline ConstBigReal(long             x) : BigReal(x, REQPOOL) {
    RELPOOL;
  }

  inline ConstBigReal(unsigned long    x) : BigReal(x, REQPOOL) {
    RELPOOL;
  }

  inline ConstBigReal(__int64          x) : BigReal(x, REQPOOL) {
    RELPOOL;
  }

  inline ConstBigReal(unsigned __int64 x) : BigReal(x, REQPOOL) {
    RELPOOL;
  }

  ConstBigReal(float                   x) : BigReal(x, REQPOOL) {
    RELPOOL;
  }
  ConstBigReal(double                  x) : BigReal(x, REQPOOL) {
    RELPOOL;
  }
  ConstBigReal(const Double80         &x) : BigReal(x, REQPOOL) {
    RELPOOL;
  }
  ConstBigReal(const BigReal           &x) : BigReal(x, REQPOOL) {
    RELPOOL;
  }

  explicit inline ConstBigReal(const String &s) : BigReal(s, REQPOOL) {
    RELPOOL;
  }

  explicit inline ConstBigReal(const TCHAR  *s) : BigReal(s, REQPOOL) {
    RELPOOL;
  }

  explicit inline ConstBigReal(ByteInputStream &s) : BigReal(s, REQPOOL) {
    RELPOOL;
  }

#ifdef UNICODE
  explicit inline ConstBigReal(const char   *s) : BigReal(s, REQPOOL) {
    RELPOOL;
  }
#endif

  ~ConstBigReal() {
    REQPOOL;
    setToZero();
    RELPOOL;
  }

  static const ConstBigReal _long_min;
  static const ConstBigReal _long_max;
  static const ConstBigReal _ulong_max;
  static const ConstBigReal _i64_min;
  static const ConstBigReal _i64_max;
  static const ConstBigReal _ui64_max;
  static const ConstBigReal _flt_min;
  static const ConstBigReal _flt_max;
  static const ConstBigReal _dbl_min;
  static const ConstBigReal _dbl_max;
  static const ConstBigReal _dbl80_min;
  static const ConstBigReal _dbl80_max;

};

#define CONST_DIGITPOOL ConstBigReal::getDigitPool()

#define BIGREAL_0     CONST_DIGITPOOL.get0()
#define BIGREAL_1     CONST_DIGITPOOL.get1()
#define BIGREAL_2     CONST_DIGITPOOL.get2()
#define BIGREAL_HALF  CONST_DIGITPOOL.getHalf()
Real getReal(const BigReal &x);

/*
#define APCsum(  bias, x, y)       BigReal::apcSum( #bias[0], x, y)
#define APCprod( bias, x, y)       BigReal::apcProd(#bias[0], x, y)
#define APCquot( bias, x, y)       BigReal::apcQuot(#bias[0], x, y)
#define APCpow(  bias, x, y)       BigReal::apcPow( #bias[0], x, y)
*/

#define PAPCsum( bias, x, y, pool) BigReal::apcSum( #bias[0], x, y, pool)
#define PAPCprod(bias, x, y, pool) BigReal::apcProd(#bias[0], x, y, pool)
#define PAPCquot(bias, x, y, pool) BigReal::apcQuot(#bias[0], x, y, pool)
#define PAPCpow( bias, x, y, pool) BigReal::apcPow( #bias[0], x, y, pool)

bool operator==(const BigReal &x, const BigReal &y);
bool operator!=(const BigReal &x, const BigReal &y);
bool operator>=(const BigReal &x, const BigReal &y);
bool operator<=(const BigReal &x, const BigReal &y);
bool operator> (const BigReal &x, const BigReal &y);
bool operator< (const BigReal &x, const BigReal &y);

class BigRealStream : public StrStream { // Don't derive from standardclass strstream. Its slow!!! (at least in windows)
private:
  TCHAR m_spaceChar;
public:
  BigRealStream(int precision = 6, int width = 0, int flags = 0) : StrStream(precision, width, flags) {
    m_spaceChar = 0;
  }

  BigRealStream(const StreamParameters &param) : StrStream(param) {
    m_spaceChar = 0;
  }

  BigRealStream(tostream &out) : StrStream(out) {
    m_spaceChar = 0;
  }

  BigRealStream &operator<<(const StreamParameters &param) {
    StrStream::operator<<(param);
    return *this;
  }

  TCHAR setSpaceChar(TCHAR value);
  TCHAR getSpaceChar() const {
    return m_spaceChar;
  }
};

class FullFormatBigReal : public BigReal {
public:
  FullFormatBigReal(const BigReal &n, DigitPool *digitPool = NULL) : BigReal(n, digitPool) {
  }
  FullFormatBigReal(DigitPool *digitPool = NULL) : BigReal(digitPool) {
  }
  FullFormatBigReal &operator=(const BigReal &x) {
    BigReal::operator=(x);
    return *this;
  }
  void print(FILE *f = stdout, bool spacing = false) const;
  String toString(bool spacing = false) const;
};

class BigInt : public BigReal {
public:
  BigInt(DigitPool *digitPool = NULL) : BigReal(digitPool) {
  }

  // Declared explicit to avoid accidently use of operator/ on BigReals
  explicit BigInt(const BigReal &x, DigitPool *digitPool = NULL);

  BigInt(int                    x, DigitPool *digitPool = NULL) : BigReal(x, digitPool) {
  }

  BigInt(unsigned int           x, DigitPool *digitPool = NULL) : BigReal(x, digitPool) {
  }

  BigInt(long                   x, DigitPool *digitPool = NULL) : BigReal(x, digitPool) {
  }

  BigInt(unsigned long          x, DigitPool *digitPool = NULL) : BigReal(x, digitPool) {
  }

  BigInt(__int64                x, DigitPool *digitPool = NULL) : BigReal(x, digitPool) {
  }

  BigInt(unsigned __int64       x, DigitPool *digitPool = NULL) : BigReal(x, digitPool) {
  }

  explicit BigInt(const String &s, DigitPool *digitPool = NULL);
  explicit BigInt(const TCHAR  *s, DigitPool *digitPool = NULL);
#ifdef UNICODE
  explicit BigInt(const char   *s, DigitPool *digitPool = NULL);
#endif
  BigInt &operator=( const BigReal &x);
  BigInt &operator/=(const BigInt  &x);

  void print(FILE *f = stdout, bool spacing = false) const;

  String toString() const;
};

BigInt operator+(const BigInt &x, const BigInt &y);
BigInt operator-(const BigInt &x, const BigInt &y);
BigInt operator*(const BigInt &x, const BigInt &y);

class ConstInteger : public BigInt {
private:
  static inline DigitPool *requestConstDigitPool() {
    return ConstBigReal::requestConstDigitPool();
  }
  static inline void releaseConstDigitPool() {
    ConstBigReal::releaseConstDigitPool();
  }

public:
  explicit ConstInteger(const BigReal &x) : BigInt(x, REQPOOL) {
    RELPOOL;
  };
  ConstInteger(int                    x) : BigInt(x, REQPOOL) {
    RELPOOL;
  }
  ConstInteger(unsigned int           x) : BigInt(x, REQPOOL) {
    RELPOOL;
  }
  ConstInteger(long                   x) : BigInt(x, REQPOOL) {
    RELPOOL;
  }
  ConstInteger(unsigned long          x) : BigInt(x, REQPOOL) {
    RELPOOL;
  }
  ConstInteger(__int64                x) : BigInt(x, REQPOOL) {
    RELPOOL;
  }
  ConstInteger(unsigned __int64       x) : BigInt(x, REQPOOL) {
    RELPOOL;
  }
  explicit ConstInteger(const String &s) : BigInt(s, REQPOOL) {
    RELPOOL;
  }
  explicit ConstInteger(const TCHAR  *s) : BigInt(s, REQPOOL) {
    RELPOOL;
  }
#ifdef UNICODE
  explicit ConstInteger(const char   *s) : BigInt(s, REQPOOL) {
    RELPOOL;
  }
#endif
  ~ConstInteger() {
    REQPOOL;
    setToZero();
    RELPOOL;
  }
};

#undef REQPOOL
#undef RELPOOL

class BigRational {
private:
  BigInt m_numerator, m_denominator;
  static BigInt findGCD(const BigInt &a, const BigInt &b);
  void init(const BigInt &numerator, const BigInt &denominator);
  void init(const String &s);
public:
  BigRational(DigitPool *digitPool = NULL);
  BigRational(const BigInt &numerator, const BigInt &denominator, DigitPool *digitPool = NULL);
  BigRational(const BigInt &n, DigitPool *digitPool = NULL);
  BigRational(int           n, DigitPool *digitPool = NULL);
  explicit BigRational(const String &s, DigitPool *digitPool = NULL);
  explicit BigRational(const TCHAR  *s, DigitPool *digitPool = NULL);
#ifdef UNICODE
  explicit BigRational(const char   *s, DigitPool *digitPool = NULL);
#endif

  friend BigRational operator+(const BigRational &l, const BigRational &r);
  friend BigRational operator-(const BigRational &l, const BigRational &r);
  friend BigRational operator-(const BigRational &r);
  friend BigRational operator*(const BigRational &l, const BigRational &r);
  friend BigRational operator/(const BigRational &l, const BigRational &r);

  BigRational &operator+=(const BigRational &r);
  BigRational &operator-=(const BigRational &r);
  BigRational &operator*=(const BigRational &r);
  BigRational &operator/=(const BigRational &r);

  const BigInt &getNumerator() const;
  const BigInt &getDenominator() const;

  DigitPool *getDigitPool() const {
    return m_denominator.getDigitPool();
  }
  String toString() const;
};

tistream     &operator>>(tistream     &in ,       BigReal     &x);
tistream     &operator>>(tistream     &in ,       BigInt      &n);
tistream     &operator>>(tistream     &in ,       BigRational &r);
tostream     &operator<<(tostream     &out, const BigReal     &x);
tostream     &operator<<(tostream     &out, const BigInt      &n);
tostream     &operator<<(tostream     &out, const BigRational &r);
tostream     &operator<<(tostream     &out, const FullFormatBigReal &x);

BigRealStream &operator<<(BigRealStream &out, const FullFormatBigReal &n);
BigRealStream &operator<<(BigRealStream &out, const BigInt &n);

String toString(const BigReal &n, int precision=20, int width=0, int flags=0);
BigReal inputBigReal( DigitPool &digitPool, const TCHAR *format,...);
BigInt inputBigInt( DigitPool &digitPool, const TCHAR *format,...);

class BigRealException : public Exception {
public:
  BigRealException(const TCHAR *text) : Exception(text) {
  };
};

BigInt randomInteger(int digits, Random *rnd = NULL, DigitPool *digitPool = NULL);

class RandomBigReal : public Random {
public:
  BigReal  nextBigReal( int digits, DigitPool *digitPool = NULL);   // Return uniform distributed random number between 0 (incl) and 1 (excl)
                                                                    // with digits decimal digits. If digitPool == NULL, use DEFAULT_DIGITPOOL
  BigReal  nextBigReal(const BigReal &low, const BigReal &high, int digits, DigitPool *digitPool = NULL);
                                                                    // Return uniform distributed random number between low (incl) and high (excl)
                                                                    // with digits decimal digits. If digitPool == NULL, use low.getDigitPool()
  BigInt nextInteger(int digits, DigitPool *digitPool = NULL);      // Return uniform distributed random BigInt in range [0..10^digits[
};

inline BigReal  Max(const BigReal &x, const BigReal &y) {
  return (compare(x, y) >= 0) ? x : y;
}

inline BigReal  Min(const BigReal &x, const BigReal &y) {
  return (compare(x, y) <= 0) ? x : y;
}

BigInt powmod(const BigInt &a, const BigInt &r, const BigInt &n);           // pow(a,r) mod n

BigReal sqrt( const BigReal &x, const BigReal &f);
BigReal exp(  const BigReal &x, const BigReal &f);
BigReal ln(   const BigReal &x, const BigReal &f);
BigReal ln1(  const BigReal &x, const BigReal &f);
BigReal log10(const BigReal &x, const BigReal &f);                          // log(x) base 10
BigReal log(  const BigReal &base, const BigReal &x, const BigReal &f);     // Logarithm(x) for the specified base. (ln(x)/ln(base))
BigReal pow(  const BigReal &x, const BigReal &y, const BigReal &f);        // x^y
BigReal sin(  const BigReal &x, const BigReal &f);
BigReal cos(  const BigReal &x, const BigReal &f);
BigReal tan(  const BigReal &x, const BigReal &f);
BigReal cot(  const BigReal &x, const BigReal &f);
BigReal asin( const BigReal &x, const BigReal &f);
BigReal acos( const BigReal &x, const BigReal &f);
BigReal atan( const BigReal &x, const BigReal &f);
BigReal acot( const BigReal &x, const BigReal &f);
BigReal pi(   const BigReal &f, DigitPool *digitPool = NULL);               // result.digitPool = f.digitPool or digitpool if specified

// Calculates with relative precision ie. with the specified number of decimal digits

BigReal rRound(const BigReal &x,    unsigned int digits);
BigReal rSum(  const BigReal &x,    const BigReal &y, unsigned int digits, DigitPool *digitPool = NULL);
BigReal rDif(  const BigReal &x,    const BigReal &y, unsigned int digits, DigitPool *digitPool = NULL);
BigReal rProd( const BigReal &x,    const BigReal &y, unsigned int digits, DigitPool *digitPool = NULL);
BigReal rQuot( const BigReal &x,    const BigReal &y, unsigned int digits, DigitPool *digitPool = NULL);
BigReal rSqrt( const BigReal &x,    unsigned int digits);
BigReal rExp(  const BigReal &x,    unsigned int digits);
BigReal rLn(   const BigReal &x,    unsigned int digits);
BigReal rLog10(const BigReal &x,    unsigned int digits);
BigReal rLog(  const BigReal &base, const BigReal &x, int unsigned digits); // Logarithm(x) for the specified base. (ln(x)/ln(base))
BigReal rPow(  const BigReal &x,    const BigReal &y, unsigned int digits); // x^y
BigReal rRoot( const BigReal &x,    const BigReal &y, unsigned int digits);
BigReal rSin(  const BigReal &x,    unsigned int digits);
BigReal rCos(  const BigReal &x,    unsigned int digits);
BigReal rTan(  const BigReal &x,    unsigned int digits);
BigReal rCot(  const BigReal &x,    unsigned int digits);
BigReal rAsin( const BigReal &x,    unsigned int digits);
BigReal rAcos( const BigReal &x,    unsigned int digits);
BigReal rAtan( const BigReal &x,    unsigned int digits);
BigReal rAcot( const BigReal &x,    unsigned int digits);

class SynchronizedStringQueue : public SynchronizedQueue<TCHAR*>, public BigRealResource {
private:
  SynchronizedStringQueue(           const SynchronizedStringQueue &src); // not implemented
  SynchronizedStringQueue &operator=(const SynchronizedStringQueue &src); // not implemented
public:
  SynchronizedStringQueue(int id) : BigRealResource(id) {
  }
  void waitForResults(int expectedResultCount);
};

class BigRealThread : public Thread, public BigRealResource {
private:
  Runnable                 *m_job;
  SynchronizedStringQueue  *m_resultQueue;
  Semaphore                 m_execute;
  int                       m_requestCount;
public:
  BigRealThread(int id);
  unsigned int run();
  void execute(Runnable &job, SynchronizedStringQueue &resultQueue);
};

class MultiplierThread : public Thread, public BigRealResource {
private:
  DigitPool                *m_digitPool;
  SynchronizedStringQueue  *m_resultQueue;
  Semaphore                 m_execute;
  const BigReal             *m_x, *m_y, *m_f;
  BigReal                   *m_result;
  int                       m_requestCount;
  int                       m_level;
  friend class BigRealThreadPool;
public:
  MultiplierThread(int id);
  unsigned int run();

  void multiply(BigReal &result, const BigReal &x, const BigReal &y, const BigReal &f, int level);
  DigitPool *getDigitPool() const {
    return m_digitPool;
  }
  SynchronizedStringQueue &getQueue() {
    return *m_resultQueue;
  }
};

template <class T> class ResourcePool : public CompactArray<T*> {
private:
  CompactStack<int> m_freeId;
protected:
  virtual void allocateNewResources(int count) {
    int id = size();
    for(int i = 0; i < count; i++, id++) {
      m_freeId.push(id);
      add(new T(id));
    }
  }
public:
  ResourcePool() {
    deleteAll();
  }
  T *fetchResource() {
    if(m_freeId.isEmpty()) {
      allocateNewResources(5);
    }
    const int index = m_freeId.pop();
    return (*this)[index];
  }

  void releaseResource(const BigRealResource *resource) {
    m_freeId.push(resource->getId());
  }

  void deleteAll() {
    for(int i = 0; i < size(); i++) {
      delete (*this)[i];
    }
    clear();
    m_freeId.clear();
  }
  BitSet getAllocatedIdSet() const {
    if(size() == 0) {
      return BitSet(8);
    } else {
      BitSet result(size());
      return result.invert();
    }
  }
  BitSet getFreeIdSet() const {
    const int n = m_freeId.getHeight();
    if(n == 0) {
      return BitSet(8);
    } else {
      BitSet result(size());
      for(int i = 0; i < n; i++) {
        result.add(m_freeId.top(i));
      }
      return result;
    }
  }
  String toString() const {
    const BitSet allocatedIdSet = getAllocatedIdSet();
    const BitSet freeIdSet      = getFreeIdSet();
    return format(_T("Free:%s. In use:%s")
                 ,freeIdSet.toString().cstr()
                 ,(allocatedIdSet - freeIdSet).toString().cstr()
                 );
  }
};

class MThreadArray : private CompactArray<MultiplierThread*> {
  friend class BigRealThreadPool;
public:
  void waitForAllResults();
  ~MThreadArray();
  MultiplierThread &get(unsigned int i) {
    return *(*this)[i];
  }
};

//#define CHECKALLDIGITS_RELEASED
#ifdef CHECKALLDIGITS_RELEASED
class DigitPoolWithCheck : public DigitPool {
public:
  unsigned int m_usedDigits;
  DigitPoolWithCheck(int id, unsigned int intialDigitcount = 0) : DigitPool(id, intialDigitcount) {
  }
};

typedef DigitPoolWithCheck MTDigitPoolType;

#else

typedef DigitPool MTDigitPoolType;

#endif

template <class T> class ThreadPool : public ResourcePool<T> {
private:
  int  m_threadPriority;
  bool m_disablePriorityBoost;

  Thread &get(unsigned int index) {
    return *((Thread*)((*this)[index]));
  }

protected:
  void allocateNewResources(int count) {
    const int oldSize = size();
    ResourcePool<T>::allocateNewResources(count);
    for(int i = oldSize; i < size(); i++) {
      Thread &thr = get(i);
      thr.setPriority(m_threadPriority);
      thr.setPriorityBoost(m_disablePriorityBoost);
    }
  }
public:
  ThreadPool() {
    m_threadPriority       = THREAD_PRIORITY_NORMAL;
    m_disablePriorityBoost = false;
  }
  void setPriority(int priority) {
    if(priority == m_threadPriority) return;
    m_threadPriority = priority;
    for(int i = 0; i < size(); i++) get(i).setPriority(priority);
  }
  int getPriority() const {
    return m_threadPriority;
  }
  void setPriorityBoost(bool disablePriorityBoost) {
    if(disablePriorityBoost == m_disablePriorityBoost) return;
    m_disablePriorityBoost = disablePriorityBoost;
    for(int i = 0; i < size(); i++) get(i).setPriorityBoost(disablePriorityBoost);
  }

  bool getPriorityBoost() const {
    return m_disablePriorityBoost;
  }
};


typedef CompactArray<Runnable*> BigRealJobArray;

class BigRealThreadPool {
private:
  ThreadPool<BigRealThread>             m_threadPool;
  ThreadPool<MultiplierThread>          m_MTThreadPool;
  ResourcePool<SynchronizedStringQueue> m_queuePool;
  ResourcePool<MTDigitPoolType>         m_digitPool;
  mutable Semaphore                     m_gate;
  int                                   m_processorCount;
  int                                   m_activeThreads, m_maxActiveThreads;
  static BigRealThreadPool              s_instance;

  BigRealThreadPool();
  BigRealThreadPool(const BigRealThreadPool &src);            // not implemented
  BigRealThreadPool &operator=(const BigRealThreadPool &src); // not implemented
public:
  ~BigRealThreadPool();
  static MThreadArray &fetchMTThreadArray(  MThreadArray &threads, int count);
  static void          releaseMTThreadArray(MThreadArray &threads);

  static DigitPool    *fetchDigitPool();
  static void          releaseDigitPool(DigitPool *pool);

  static void executeInParallel(BigRealJobArray &jobs); // Blocks until all jobs are done. If any of the jobs throws an exception
                                                        // the rest of the jobs will be terminated and an exception with the same
                                                        // message will be thrown to the caller

  static inline int    getMaxActiveThreads() {
    return getInstance().m_maxActiveThreads;
  }

  static String toString(); // for debug
  static void startLogging();
  static void stopLogging();
  static void setPriority(int priority); // Sets the priority for all running and future running threads
  // Default is THREAD_PRIORITY_BELOW_NORMAL
  // THREAD_PRIORITY_IDLE,-PRIORITY_LOWEST,-PRIORITY_BELOW_NORMAL,-PRIORITY_NORMAL,-PRIORITY_ABOVE_NORMAL

  static void setPriorityBoost(bool disablePriorityBoost);
  static BigRealThreadPool &getInstance();
};

void traceRecursion(int level, const TCHAR *format,...);

#ifdef _DEBUG
#define TRACERECURSION(p) traceRecursion p

#define ENTER_CRITICAL_SECTION_BIGREAL_DEBUGSTRING()                 \
  bool _debugStringEnabledOldValue;                                  \
  const bool _debuggerPresent = getDebuggerPresent();                \
  if(_debuggerPresent) {                                             \
    BigReal::s_debugStringGate.wait();                               \
    _debugStringEnabledOldValue = BigReal::s_debugStringEnabled;     \
    BigReal::s_debugStringEnabled = false;                           \
  }

#define LEAVE_CRITICAL_SECTION_BIGREAL_DEBUGSTRING(stmt)             \
  if(_debuggerPresent) {                                             \
    BigReal::s_debugStringEnabled = _debugStringEnabledOldValue;     \
    BigReal::s_debugStringGate.signal();                             \
    stmt                                                             \
  }

#else
#define TRACERECURSION(p)

#define ENTER_CRITICAL_SECTION_BIGREAL_DEBUGSTRING()
#define LEAVE_CRITICAL_SECTION_BIGREAL_DEBUGSTRING(stmt)

#endif

BigReal oldFraction(const BigReal &x); // Old version sign(x) * (|x| - floor(|x|))
BigReal newModulusOperator(const BigReal &x, const BigReal &y); // old operator%(const BigReal &x, const BigReal &y);

#ifdef LONGDOUBLE
#pragma comment(lib, LIB_VERSION "LDBigReal.lib")
#else
#pragma comment(lib,  LIB_VERSION "BigReal.lib")
#endif

