#pragma once

#include <MyUtil.h>
#include <MyAssert.h>
#include <Thread.h>
#include <CompactStack.h>
#include <CompactHashMap.h>
#include <SynchronizedQueue.h>
#include <StreamParameters.h>
#include <ByteStream.h>
#include <Packer.h>
#include <HashMap.h>
#include <Random.h>
#include "Double80.h"
#include "Real.h"
#include "Int128.h"

// Define this to have 2 different version of getDecimalDigitCount64(UINT64 n).
// Measures of time show that getDecimalDigitCount64 is 5 times faster than getDecimalDigitCount64Loop
//#define HAS_LOOP_DIGITCOUNT

// use, if you want to have backup-check for correct multiplication, running in separate
// process built with x86 code, Serverprocess is called "MultiplicationServer", and protocol to this is quit simple:
// write 3 BigReals, x y f as text. Serverprogram will return x*y (as text) with an error no greater than |f|
// Can be repeated as long as you want
// #define USE_X32SERVERCHECK

// If this is not defined, all digit-allocations/deallocations will be done with new/delete,
// which will slow down the program (testBigReal) by a factor 20!!
#define USE_DIGITPOOL_FREELIST

// If this is defined, releaseDigitPool will check, that all used digits in the pool are released
//#define CHECKALLDIGITS_RELEASED

// If this is defined, it will be checked, that the same thread that called ConstDigitPool::requestInstance() is the same
// thread that calls releaseInstance
//#define CHECKCONSTPOOLTHREAD

// If this is defined, product wil trace recursive calls for when multiplying long BigReals
// which will slow down the program (testBigReal) by a factor 20!!
//#define TRACEPRODUCTRECURSION

// If this is defined, load/save/the number of requests and cache-hits to pow2Cache will be logged
// and written to debugLog when program exit
//#define TRACEPOW2CACHE

// If this is defined, the number of requests and cache-hits to pow2Cache will be logged
// and written to debugLog when program exit
//#define TRACEPOW2CACHEHIT

// TODO Rename to BIGREAL_LOG10BASEx86 + alle de andre
#define LOG10_BIGREALBASEx86                        8
#define BIGREAL_POW10TABLESIZEx86                  10
#define BIGREAL_NONNORMALx86               -900000000
#define BIGREAL_MAXEXPOx86                   99999999
#define BIGREAL_MINEXPOx86                  -99999999

typedef ULONG          BRDigitTypex86;
typedef UINT64         BR2DigitTypex86;
typedef long           BRExpoTypex86;
typedef long           BRDigitDiffTypex86;


#define LOG10_BIGREALBASEx64                       18
#define BIGREAL_POW10TABLESIZEx64                  20
#define BIGREAL_NONNORMALx64      -900000000000000000
#define BIGREAL_MAXEXPOx64          99999999999999999
#define BIGREAL_MINEXPOx64         -99999999999999999

typedef UINT64         BRDigitTypex64;
typedef _uint128       BR2DigitTypex64;
typedef INT64          BRExpoTypex64;
typedef INT64          BRDigitDiffTypex64;

// Basic definitions depends on registersize. 32- og 64-bit
#ifdef IS32BIT

#define LOG10_BIGREALBASE  LOG10_BIGREALBASEx86

#define BIGREAL_NONNORMAL  BIGREAL_NONNORMALx86
#define BIGREAL_MAXEXPO    BIGREAL_MAXEXPOx86
#define BIGREAL_MINEXPO    BIGREAL_MINEXPOx86

#define BRDigitType        BRDigitTypex86
#define BR2DigitType       BR2DigitTypex86
#define BRExpoType         BRExpoTypex86
#define BRDigitDiffType    BRDigitDiffTypex86

#else // IS64BIT

#define LOG10_BIGREALBASE  LOG10_BIGREALBASEx64

#define BIGREAL_NONNORMAL  BIGREAL_NONNORMALx64
#define BIGREAL_MAXEXPO    BIGREAL_MAXEXPOx64
#define BIGREAL_MINEXPO    BIGREAL_MINEXPOx64

#define BRDigitType        BRDigitTypex64
#define BR2DigitType       BR2DigitTypex64
#define BRExpoType         BRExpoTypex64
#define BRDigitDiffType    BRDigitDiffTypex64

#endif // IS32BIT

// Values for BigReal::m_low, if m_expo == BIGREAL_NONNORMAL (_isnormal() is false)
#define BIGREAL_ZEROLOW  0 // isZero() is true
#define BIGREAL_INFLOW   1 // _isinf() is true, +/- infinite depending on m_negative
#define BIGREAL_QNANLOW  2 // _isnan() is true

#define SP_OPT_NONE      0
#define SP_OPT_BY_FPU    1
#define SP_OPT_BY_FPU2   2
#define SP_OPT_BY_REG32  3
#define SP_OPT_BY_REG64  4

// Define SP_OPT_METHOD to one of these, to select the desired optmization of shortProduct.
// Best performance in x86 is SP_OPT_BY_REG32. Best (and only) in x64-mode is SP_OPT_BY_REG64

#ifdef IS32BIT
//#define SP_OPT_METHOD  SP_OPT_NONE
//#define SP_OPT_METHOD  SP_OPT_BY_FPU
//#define SP_OPT_METHOD  SP_OPT_BY_FPU2
#define SP_OPT_METHOD SP_OPT_BY_REG32
#else
#define SP_OPT_METHOD SP_OPT_BY_REG64
#endif

#ifndef SP_OPT_METHOD

#error Must define SP_OPT_METHOD

#elif((SP_OPT_METHOD != SP_OPT_NONE) && (SP_OPT_METHOD != SP_OPT_BY_FPU) && (SP_OPT_METHOD != SP_OPT_BY_FPU2) && (SP_OPT_METHOD != SP_OPT_BY_REG32) && (SP_OPT_METHOD != SP_OPT_BY_REG64))

#error Must define SP_OPT_METHOD

#endif

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
// this is the only valid BIGREALBASE for these optimizations
#define BIGREALBASE 100000000
#else
#error Illegal LOG10_BIGREALBASE. Must be 8.
#endif // LOG10_BIGREALBASE == 8

#elif (SP_OPT_METHOD == SP_OPT_BY_REG64)

#if LOG10_BIGREALBASE == 18
// this is the only valid BIGREALBASE for SP_OPT_BY_REG64
#define BIGREALBASE 1000000000000000000
#else
#error Illegal LOG10_BIGREALBASE. Must be 18.
#endif // LOG10_BIGREALBASE == 18

#endif // if(SP_OPT_METHOD == ...)

#define APC_DIGITS         6

#define CONVERSION_POW2DIGITCOUNT 24

// The definition of Digit should NOT be changed!!! Se asm-code in ShortProduct*.cpp
class Digit {
public:
  BRDigitType    n;
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
class ConstBigInt;
class ConstBigReal;

class Pow2ArgumentKey {
public:
  int    m_n;
  size_t m_digits;
  Pow2ArgumentKey() {
  }
  inline Pow2ArgumentKey(int n, size_t digits) : m_n(n), m_digits(digits) {
  }
  inline ULONG hashCode() const {
    return m_n * 23 + sizetHash(m_digits);
  }
  inline bool operator==(const Pow2ArgumentKey &k) const {
    return (m_n == k.m_n) && (m_digits == k.m_digits);
  }
  String toString() const {
    return format(_T("(%6d,%3zu)"), m_n, m_digits);
  }
};

#define CACHE_LOADING 0x1
#define CACHE_LOADED  0x2
#define CACHE_SAVING  0x4
#define CACHE_EMPTY   0x8

class Pow2Cache : public CompactHashMap<Pow2ArgumentKey, BigReal*> {
private:
  mutable Semaphore m_gate;
  BYTE              m_state;
  size_t            m_updateCount, m_savedCount;

  void save(const String &fileName) const;
  void load(const String &fileName);
  void save(ByteOutputStream &s) const;
  void load(ByteInputStream  &s);
  void clear();
  inline bool isLoaded() const {
    return (m_state & CACHE_LOADED) ? true : false;
  }
  inline bool isLoading() const {
    return (m_state & CACHE_LOADING) ? true : false;
  }
public:
  Pow2Cache();
  ~Pow2Cache();
  bool put(const Pow2ArgumentKey &key, BigReal * const &v);
  BigReal **get(const Pow2ArgumentKey &key) const;
  inline bool isChanged() const {
    return m_updateCount != m_savedCount;
  }
  bool hasCacheFile() const;
  void load();
  void save();
  void dump() const;
};

class BigRealResource : public IdentifiedResource {
public:
  BigRealResource(int id) : IdentifiedResource(id) {
  }
};

#define DEFAULT_DIGITPOOL_ID -1
#define CONST_DIGITPOOL_ID   -2
#define PI_DIGITPOOL_ID      -3
#define LN_DIGITPOOL_ID      -4

class DigitPool : public BigRealResource {
private:
  static size_t         s_totalDigitCount;
  static bool           s_dumpCountWhenDestroyed;
  size_t                m_digitCount;
  DigitPage            *m_firstPage;
  Digit                *m_freeDigits;

  // Some frequently used constants. exist in every pool
  // = 0
  BigInt               *m_0;
  // = 1
  BigInt               *m_1;
  // = 2
  BigInt               *m_2;
  // = 0.5
  BigReal              *m_05;
  BigReal              *m_nan, *m_pinf, *m_ninf; // (quiet)nan, +infinity, -infinity

  static void updateTotalDigitCount(intptr_t n);
  DigitPool(           const DigitPool &src); // not implemented
  DigitPool &operator=(const DigitPool &src); // not implemented
  void allocatePage();
public:
  static DigitPool s_defaultDigitPool;

  // initialDigitcount in BIGREALBASE-digits
  DigitPool(int id, size_t intialDigitcount = 0);
  virtual ~DigitPool();

  static void setDumpWhenDestroyed(bool dump) {
    s_dumpCountWhenDestroyed = dump;
  }
  inline Digit *newDigit() {
#ifdef USE_DIGITPOOL_FREELIST
/*
    if((getId() == DEFAULT_DIGITPOOL_ID) || (getId() == CONST_DIGITPOOL_ID)) {
      int BREAKPOINT_HERE = 1; // to catch unwanted allocations from Default/const digitpools
    }
*/
    if(m_freeDigits == NULL) allocatePage();
    Digit *p     = m_freeDigits;
    m_freeDigits = p->next;
    return p;
#else
    Digit *d = new Digit; TRACE_NEW(d);
    return d;
#endif // USE_DIGITPOOL_FREELIST
  }

  inline void deleteDigits(Digit *first, Digit *last) {
#ifdef USE_DIGITPOOL_FREELIST
    last->next = m_freeDigits;
    m_freeDigits = first;
#else
    const Digit *end = last->next;
    for(;;) {
      Digit *p = first->next;
      SAFEDELETE(first);
      if(p == end) break;
      first = p;
    }
#endif // USE_DIGITPOOL_FREELIST
  }
  inline const BigInt &_0() const {
    return *m_0;
  }
  inline const BigInt &_1() const {
    return *m_1;
  }
  inline const BigInt &_2() const {
    return *m_2;
  }
  inline const BigReal &_05() const {
    return *m_05;
  }
  inline const BigReal &nan() const {
    return *m_nan;
  }
  inline const BigReal &pinf() const {
    return *m_pinf;
  }
  inline const BigReal &ninf() const {
    return *m_ninf;
  }

  static inline size_t getTotalAllocatedDigitCount() {
    return s_totalDigitCount;
  }
  inline size_t getAllocatedDigitCount() const {
    return m_digitCount;
  }
  size_t getFreeDigitCount() const;
  size_t getUsedDigitCount() const;
  size_t getPageCount()      const;
};

#define DEFAULT_DIGITPOOL DigitPool::s_defaultDigitPool

#define _SETDIGITPOOL() m_digitPool(digitPool?*digitPool:DEFAULT_DIGITPOOL)

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
void throwBigRealException(_In_z_ _Printf_format_string_ TCHAR const * const format,...);

class BigReal {
private:
  static Pow2Cache            s_pow2Cache;
  static const BRDigitTypex86 s_power10Tablex86[BIGREAL_POW10TABLESIZEx86];
  static const BRDigitTypex64 s_power10Tablex64[BIGREAL_POW10TABLESIZEx64];
  // Defined in shortProduct.cpp. Split factors when length > s_splitLength
  static size_t               s_splitLength;
  // Terminate all calculation with an Exception ("Operation was cancelled")
  // if they enter shortProduct, or simply return whichever comes first
  static bool                 s_continueCalculation;

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
  // True for x < 0. else false
  bool                      m_negative;
  DigitPool                &m_digitPool;

  // Multiplication helperfunctions
  friend class MultiplierThread;
  friend class BigRealTestClass;
  friend class BigInt;
  friend class BigRealStream;

  // Construction helperfunctions
  inline void copyNonPointerFields(const BigReal &src) {
    m_expo     = src.m_expo;
    m_low      = src.m_low;
    m_negative = src.m_negative;
  }
  // Can be called from constructor.
  // Set both m_first = m_last = NULL.
  inline void initToNonNormal(int low = BIGREAL_ZEROLOW, bool negative=false) {
    m_first    = m_last = NULL;
    m_expo     = BIGREAL_NONNORMAL;
    m_low      = low;
    m_negative = negative;
  }
  inline void initToZero() {
    initToNonNormal();
  }
  // Should NOT be called from constructor.
  // Assume pointers have been initialized
  // return *this
  inline BigReal &setToNonNormal(int low = BIGREAL_ZEROLOW, bool negative=false) {
    clearDigits();
    m_expo = BIGREAL_NONNORMAL;
    m_low = low;
    m_negative = negative;
    return *this;
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

  // Assume x.m_expo == BIGREAL_NONNORMAL
  // return one of FP_ZERO, FP_INFINITE, FP_NAN
  inline int classifyNonNormal() const {
    switch(m_low) {
    case BIGREAL_ZEROLOW: return FP_ZERO;
    case BIGREAL_INFLOW : return FP_INFINITE;
    case BIGREAL_QNANLOW: return FP_NAN;
    default             : NODEFAULT;
    }
  }

  // Set this to corresponding non-normal value, and return *this
  // Assume fpclass is one of {_FPCLASS_PZ,_FPCLASS_NZ, _FPCLASS_PINF, _FPCLASS_NINF, _FPCLASS_QNAN, _FPCLASS_SNAN }
  // throws exception if fpclass is not in the list above
  BigReal &setToNonNormalFpClass(int fpclass);

  // Basic digit manipulation
  inline Digit *newDigit() {
    return m_digitPool.newDigit();
  }
  inline void deleteDigits(Digit *first, Digit *last) {
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
  // Dont modify m_expo,m_low
  void    insertDigit(                      BRDigitType n);
  // Insert count zero-digits at head of this
  // Assume *this != zero. ie m_first != NULL (and m_last != NULL)
  void    insertZeroDigits(                 size_t count);
  // Insert one digit=n after digit p.
  // Assume p is a digit in digit-list of this
  // Dont modify m_expo,m_low
  void    insertAfter(            Digit *p, BRDigitType n);
  // Insert count zero-digits after digit p.
  // Assume p is a digit in digit-list of this.
  // Dont modify m_expo,m_low
  void    insertZeroDigitsAfter(  Digit *p, size_t count);
  // Insert count digits=BIGREALBASE-1 after digit p.
  // Assume p is a digit in digit-list of this.
  // Dont modify m_expo,m_low
  void    insertBorrowDigitsAfter(Digit *p, size_t count);
  // Releases all digits in digit-list to this.m_digitPool.
  // Dont modify m_expo,m_low
  inline void clearDigits() {
    if(m_first) {
      m_digitPool.deleteDigits(m_first, m_last);
      m_first = m_last = NULL;
    }
  }


  inline void incrExpo() {
    if(isZero()) m_expo = m_low = 0; else m_expo++;
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

  // Addition and subtraction helperfunctions
  BigReal &addAbs(const BigReal &x, const BigReal &y, const BigReal &f);                          // return this
  BigReal &addAbs(const BigReal &x);                                                              // return this
  BigReal &subAbs(const BigReal &x, const BigReal &f);                                            // return this

#if(  SP_OPT_METHOD == SP_OPT_NONE)

  void    baseb(const BigReal &x);
  void    baseb(const BigReal &x, int low);
  BigReal &baseB(const BigReal &x);                                                               // return this
  void    addSubProduct(BR2DigitType n);

#elif(SP_OPT_METHOD == SP_OPT_BY_FPU)

  void    addFPUReg0();
  void    addSubProduct(BR2DigitType n);

#elif((SP_OPT_METHOD == SP_OPT_BY_REG32) || (SP_OPT_METHOD == SP_OPT_BY_REG64) || (SP_OPT_METHOD == SP_OPT_BY_FPU2))

// Set digitChain to contain 1 digit (=0).
// Should only be called from shortProductNoZeroCheck. m_last will not be touched
// so invariant will be broken for af while, and reestablished at the end of shortProductNoZeroCheck,
// when we clean up the chain head and end
  inline Digit *clearDigits1() {
    if(m_first == NULL) {
      m_first = m_digitPool.newDigit();
    } else if(m_first->next) {
      m_digitPool.deleteDigits(m_first->next, m_last);
    }
    m_first->n = 0;
    return m_first;
  }
// Assume last != NULL. new digit->n and ..->next are not initialized
// Should only be called from shortProductNoZeroCheck. m_last will not be touched
// so invariant will be broken for af while, and reestablished at the end of shortProductNoZeroCheck,
// when we clean up the chain head and end
  inline Digit *fastAppendDigit(Digit *last) {
    Digit *p = last->next = m_digitPool.newDigit();
    p->prev  = last;
    return p;
  }

#endif

#ifdef _DEBUG
  // One that works, and used for testing other versions of this important function
  BigReal &shortProductNoZeroCheckReference(const BigReal &x, const BigReal &y, int loopCount);   // return this
  friend void insertDigitAndIncrExpo(BigReal &v, BRDigitType n);
  void    addSubProductReference1(UINT64 &n);
  void    addSubProductReference2(UINT64 &n);
  static  bool s_useShortProdReferenceVersion;
  BigReal &shortProductNoZeroCheckDebug(    const BigReal &x, const BigReal &y, size_t loopCount);   // return this
public:
  static bool setUseShortProdRefenceVersion(bool useReferenceVersion) {
    bool oldValue = s_useShortProdReferenceVersion; s_useShortProdReferenceVersion = useReferenceVersion; return oldValue;
  }
private:
#endif

  // return true if x*y is normal (finite and != 0, (false if x*y is 0, +/-inf or nan)
  static inline bool isNormalProduct(const BigReal &x, const BigReal &y) {
    return x._isnormal() && y._isnormal();
  }

  // Return _FP_ZERO, _FPCLASS_QNAN
  // Assume !x._isnormal() || !y._isnormal() which will result in non-normal product (0 or nan)
  static int getNonNormalProductFpClass(const BigReal &x, const BigReal &y);

  // Return true if x*y is normal (finite and != 0). in this case, *this will not be changed
  // return false, if x*y is not normal, and *this will be set to the corresponding result (0 or nan)
  inline bool checkIsNormalProduct(const BigReal &x, const BigReal &y) {
    if(isNormalProduct(x, y)) return false;
    setToNonNormalFpClass(getNonNormalProductFpClass(x, y));
    return false;
  }
  // Don't call shortProductNoZeroCheck with numbers longer than getMaxSplitLength(), or you'll get a wrong result
  BigReal &shortProduct(                     const BigReal &x, const BigReal &y, BRExpoType fexpo    );  // return this
  // Assume x and y are both normal
  BigReal &shortProductNoZeroCheck(          const BigReal &x, const BigReal &y, size_t     loopCount);  // return this
  static BigReal &product(  BigReal &result, const BigReal &x, const BigReal &y, const BigReal &f,              int level);
  static BigReal &productMT(BigReal &result, const BigReal &x, const BigReal &y, const BigReal &f, intptr_t  w, int level);

  // Assume a == b == 0 && _isfinite() && f._isfinite(). Dont care about sign of f
  void    split(BigReal &a, BigReal &b, size_t n, const BigReal &f) const;
  // Assume src._isnormal() && length <= src.getLength() && m_first == m_last == NULL
  // Modify only m_first and m_last of this
  void    copyDigits(   const BigReal &src, size_t length);
  // Assume !_isnormal() && src._isnormal()
  void    copyAllDigits(const BigReal &src);
  // Copy specified number of decimal digits, truncating result. return *this
  BigReal &copyrTrunc(  const BigReal &src, size_t digits);
  inline BigReal &setSignByProductRule(const BigReal &x, const BigReal &y) {
    m_negative = x.m_negative != y.m_negative;
    return *this;
  }

  // cut of a number (!= 0) to the specified number of significant decimal digits, truncation toward zero
  // Assume _isnormal() && digits > 0 (digits is decimal digits). return *this
  BigReal &rTrunc(size_t digits);
  // cut of a number (!= 0) to the specified number of significant decimal digits, rounding
  // Assume isnormal() && digits > 0 (digits is decimal digits). return *this
  BigReal &rRound(size_t digits);

  // Division helperfunctions.

  static void validateQuotRemainderArguments(const TCHAR *method, const BigReal &x, const BigReal &y, const BigReal *quotient, const BigReal *remainder);

  // return true if x/y is normal (finite and != 0, (false if x/y is 0, +/-inf or nan)
  static bool inline isNormalQuotient(const BigReal &x, const BigReal &y) {
    return y._isnormal() && x._isnormal();
  }

  // Return _FP_ZERO, _FPCLASS_PINF, _FPCLASS_NINF, _FPCLASS_QNAN
  // Assume !x._isnormal() || !y._isnormal() which will result in non-normal quotient (0,+/-inf or nan)
  static int getNonNormalQuotientFpClass(const BigReal &x, const BigReal &y);

  // Return true if x/y is normal (finite and != 0), (false if x/y is 0, +/-inf or nan)
  // if x/y is not normal, quotient and remainder will recieve the non-normal value (if not null)
  static bool inline checkIsNormalQuotient(const BigReal &x, const BigReal &y, BigReal *quotient, BigReal *remainder) {
    if(isNormalQuotient(x, y)) {
      return true;
    }
    const int qclass = getNonNormalQuotientFpClass(x, y);
    if(quotient ) quotient->setToNonNormalFpClass( qclass);
    if(remainder) remainder->setToNonNormalFpClass(qclass);
    return false;
  }

  // Approximately 1/x. Result.digitPool = x.digitPool
  static BigReal  reciprocal(const BigReal &x, DigitPool *digitPool = NULL);
  // Assume y._isnormal(). *this = approximately x/y.          Return *this
  BigReal &approxQuot32(     const BigReal &x, const BigReal           &y);
  // Assume y._isnormal(). *this = approximately x/y.          Return *this
  BigReal &approxQuot64(     const BigReal &x, const BigReal           &y);
  // Assume y != 0.        *this = approximately x/e(y,scale). Return *this
  BigReal &approxQuot32Abs(  const BigReal &x, ULONG                    y, BRExpoType scale);
  // Assume y != 0.        *this = approximately x/e(y,scale). Return *this
  BigReal &approxQuot64Abs(  const BigReal &x, const UINT64            &y, BRExpoType scale);
#ifdef IS64BIT
  // Assume y._isnormal(). *this = approximately x/y.          Return *this
  BigReal &approxQuot128(    const BigReal &x, const BigReal           &y);
  // Assume y != 0.        *this = approximately x/e(y,scale). Return *this
  BigReal &approxQuot128Abs( const BigReal &x, const _uint128          &y, BRExpoType scale);
#endif // IS64BIT
  static double  estimateQuotNewtonTime(const BigReal &x, const BigReal &y, const BigReal &f);
  static double  estimateQuotLinearTime(const BigReal &x, const BigReal &y, const BigReal &f);
  static bool    chooseQuotNewton(      const BigReal &x, const BigReal &y, const BigReal &f);

  // Misc
  // return *this
  // Assume bias is {<,>,#}
  BigReal  &adjustAPCResult(const char bias, const TCHAR *function);

  friend bool isFloat(   const BigReal &v, float    *flt = NULL);
  friend bool isDouble(  const BigReal &v, double   *dbl = NULL);
  friend bool isDouble80(const BigReal &v, Double80 *d80 = NULL);

  inline float getFloatNoLimitCheck() const {
    return getFloat(getDouble80NoLimitCheck());
  }
  inline double getDoubleNoLimitCheck() const {
    return getDouble(getDouble80NoLimitCheck());
  }
  Double80 getDouble80NoLimitCheck() const;

protected:
  inline void releaseDigits() { // should only be called from destructor
    if(m_first) {
      m_digitPool.deleteDigits(m_first, m_last);
      m_first = NULL;
    }
  }

public:
  inline BigReal(                  DigitPool *digitPool = NULL) : _SETDIGITPOOL() {
    initToZero();
  }
  inline BigReal(int      x      , DigitPool *digitPool = NULL) : _SETDIGITPOOL() {
    init(x);
  }
  inline BigReal(UINT     x      , DigitPool *digitPool = NULL) : _SETDIGITPOOL() {
    init(x);
  }
  inline BigReal(long     x      , DigitPool *digitPool = NULL) : _SETDIGITPOOL() {
    init((int)x);
  }
  inline BigReal(ULONG    x      , DigitPool *digitPool = NULL) : _SETDIGITPOOL() {
    init((UINT)x);
  }
  inline BigReal(INT64    x      , DigitPool *digitPool = NULL) : _SETDIGITPOOL() {
    init(x);
  }
  inline BigReal(UINT64   x      , DigitPool *digitPool = NULL) : _SETDIGITPOOL() {
    init(x);
  }

  inline BigReal(_int128  x      , DigitPool *digitPool = NULL) : _SETDIGITPOOL() {
    init(x);
  }
  inline BigReal(_uint128 x      , DigitPool *digitPool = NULL) : _SETDIGITPOOL() {
    init(x);
  }

  BigReal(float           x      , DigitPool *digitPool = NULL) : _SETDIGITPOOL() {
    init(x);
  }
  BigReal(double          x      , DigitPool *digitPool = NULL) : _SETDIGITPOOL() {
    init(x);
  }
  BigReal(const Double80  &x      , DigitPool *digitPool = NULL) : _SETDIGITPOOL() {
    init(x);
  }
  BigReal(const BigReal   &x      , DigitPool *digitPool = NULL);

  explicit inline BigReal(const String  &s, DigitPool *digitPool = NULL) : _SETDIGITPOOL() {
    init(s, true);
  }
  explicit inline BigReal(const char    *s, DigitPool *digitPool = NULL) : _SETDIGITPOOL() {
    init(s, true);
  }
  explicit inline BigReal(const wchar_t *s, DigitPool *digitPool = NULL) : _SETDIGITPOOL() {
    init(s, true);
  }
  explicit inline BigReal(ByteInputStream &s, DigitPool *digitPool = NULL) : _SETDIGITPOOL() {
    initToZero();
    load(s);
  }

  virtual ~BigReal() {
    releaseDigits();
  }

  inline BigReal &operator=(int              n) {
    clearDigits(); init(n);
    return *this;
  }
  inline BigReal &operator=(UINT             n) {
    clearDigits(); init(n);
    return *this;
  }
  inline BigReal &operator=(long             n) {
    clearDigits(); init(n);
    return *this;
  }
  inline BigReal &operator=(ULONG            n) {
    clearDigits(); init((UINT)n);
    return *this;
  }
  inline BigReal &operator=(INT64            n) {
    clearDigits(); init(n);
    return *this;
  }
  inline BigReal &operator=(UINT64           n) {
    clearDigits(); init(n);
    return *this;
  }

  inline BigReal &operator=(const _int128   &n) {
    clearDigits(); init(n);
    return *this;
  }
  inline BigReal &operator=(const _uint128  &n) {
    clearDigits(); init(n);
    return *this;
  }

  inline BigReal &operator=(float            x) {
    clearDigits(); init(x);
    return *this;
  }
  inline BigReal &operator=(double           x) {
    clearDigits(); init(x);
    return *this;
  }
  inline BigReal &operator=(const Double80  &x) {
    clearDigits(); init(x);
    return *this;
  }

  BigReal &operator=(const BigReal    &x);

  friend long       getLong(  const BigReal  &v);
  friend ULONG      getUlong( const BigReal  &v);
  friend inline int getInt(   const BigReal  &v) {
    return (int)getLong(v);
  }
  friend inline UINT getUint( const BigReal  &v) {
    return (UINT)getUlong(v);
  }
  friend INT64    getInt64(   const BigReal  &v);
  friend UINT64   getUint64(  const BigReal  &v);
  friend _int128  getInt128(  const BigReal  &v);
  friend _uint128 getUint128( const BigReal  &v);
  friend float    getFloat(   const BigReal  &v);
  friend double   getDouble(  const BigReal  &v);
  friend Double80 getDouble80(const BigReal  &v);

  // Result.digitPool = x.digitPool
  friend BigReal  operator+ (const BigReal &x, const BigReal &y);
  // Result.digitPool = x.digitPool
  friend BigReal  operator- (const BigReal &x, const BigReal &y);
  // Result.digitPool = x.digitPool
  friend BigReal  operator* (const BigReal &x, const BigReal &y);
  // Result.digitPool = x.digitPool
  friend BigReal  operator% (const BigReal &x, const BigReal &y);
  // Result.digitPool = x.digitPool
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
  // Do *this *= pow(10,exp). (Fast) Check for overflow/underflow. Trim leading and trailling zeroes if necessessary
  // Return *this
  BigReal &multPow10(BRExpoType exp);

  // x+y with |error| <= f. Result.digitPool = x.digitPool
  friend BigReal  sum(             const BigReal &x, const BigReal &y, const BigReal  &f, DigitPool *digitPool = NULL);
  // x-y with |error| <= f. Result.digitPool = x.digitPool
  friend BigReal  dif(             const BigReal &x, const BigReal &y, const BigReal  &f, DigitPool *digitPool = NULL);
  // x*y with |error| <= f. Result.digitPool = x.digitPool
  friend BigReal  prod(            const BigReal &x, const BigReal &y, const BigReal  &f, DigitPool *digitPool = NULL);
  // x/y with |error| <= f. Result.digitPool = x.digitPool
  friend BigReal  quot(            const BigReal &x, const BigReal &y, const BigReal  &f, DigitPool *digitPool = NULL);
  // integer division
  friend BigInt   quot(             const BigInt &x, const BigInt &y                    , DigitPool *digitPool = NULL);
  // Calculates only quotient and/or remainder if specified
  friend void     quotRemainder(   const BigReal &x, const BigReal &y, BigInt *quotient, BigReal *remainder);
  // Calculates only quotient and/or remainder if specified
  friend void     quotRemainder64( const BigReal &x, const BigReal &y, BigInt *quotient, BigReal *remainder);
#ifdef IS64BIT
  // Calculates only quotient and/or remainder if specified
  friend void     quotRemainder128(const BigReal &x,  const BigReal &y, BigInt *quotient, BigReal *remainder);
#endif
  // bias = '<','#' or '>'. Result.digitPool = x.digitPool
  static BigReal  apcSum(       const char bias,  const BigReal &x, const BigReal  &y, DigitPool *digitPool = NULL);
  // bias = '<','#' or '>'. Result.digitPool = x.digitPool
  static BigReal  apcProd(      const char bias,  const BigReal &x, const BigReal  &y, DigitPool *digitPool = NULL);
  // bias = '<','#' or '>'. Result.digitPool = x.digitPool
  static BigReal  apcQuot(      const char bias,  const BigReal &x, const BigReal  &y, DigitPool *digitPool = NULL);
  // bias = '<','#' or '>'. Result.digitPool = x.digitPool
  static BigReal  apcPow(       const char bias,  const BigReal &x, const BigInt   &y, DigitPool *digitPool = NULL);

  void           fractionate(BigInt *integerPart, BigReal *fractionPart) const;
  // ln(10) with |error| < f. result.digitPool = f.defaultDigitPool
  static BigReal ln10(         const BigReal &f);
  // Approximatly ln(x). Assume 1 <= x <= 10. result.digitPool = f.defaultDigitPool
  static BigReal lnEstimate(   const BigReal &x);
  // 2^n, with the specified number of digits. if digits = 0, then full precision. Results are cached
  static const BigReal &pow2(int n, size_t digits = 0);

  static void pow2CacheLoad();
  static void pow2CacheSave();
  static bool pow2CacheHasFile();
  static bool pow2CacheChanged();
  static void pow2CacheDump();

  // Assume x._isfinite() && y._isfinite(). Return sign(x-y)
  friend int compare(         const BigReal &x,  const BigReal &y);
  // Assume x._isfinite() && y._isfinite(). compare(|x|,|y|). (Faster than compare(fabs(x),fabs(y)))
  friend int compareAbs(      const BigReal &x,  const BigReal &y);

  // returns one of
  // FP_INFINITE
  // FP_NAN
  // FP_NORMAL
  // FP_SUBNORMAL .... NOT used
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
  inline bool isConst() const {
    return getPoolId() == CONST_DIGITPOOL_ID;
  }
  inline BigReal &setToZero() {
    return setToNonNormal();
  }
  inline BigReal &setToPInf() {
    return setToNonNormal(BIGREAL_INFLOW);
  }
  inline BigReal &setToNInf() {
    return setToNonNormal(BIGREAL_INFLOW, true);
  }
  inline BigReal &setToNan() {
    return setToNonNormal(BIGREAL_QNANLOW);
  }
  inline BigReal &changeSign() {
    if(_isnormal() || _isinf()) { m_negative = !m_negative; }
    return *this;
  }
  inline void setPositive() {
    m_negative = false;
  }
  // Return !isZero() && !m_negative
  inline  bool isPositive() const {
    return !isZero() && !m_negative;
  }
  // Return m_negative
  inline bool isNegative() const {
    return m_negative;
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

  // Return x._isnormal() ? floor(log10(|x|)) : 0
  static inline BRExpoType getExpo10(const BigReal  &x) {
    return x._isnormal() ? (x.m_expo * LOG10_BIGREALBASE + getDecimalDigitCount(x.m_first->n) - 1) : 0;
  }

  // Return x._isnormal() ? getExpo10(x) as BigReal : x
  static BigReal           getExpo10N( const BigReal  &x) {
    return x._isnormal() ? BigReal(getExpo10(x), x.getDigitPool()) : x;
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
  // Assume x._isfinite(). Return (x == 0) ? 0 : approximately floor(log2(|x|))
  friend BRExpoType getExpo2(   const BigReal  &x);
  // return floor(log10(n))+1
  // Assume n = [1..1e8[
  static int     getDecimalDigitCount(BRDigitTypex86 n);
  // return floor(log10(n))+1
  // Assume n = [1..1e19[
  static int     getDecimalDigitCount(BRDigitTypex64 n);

#ifdef HAS_LOOP_DIGITCOUNT
  static int     getDecimalDigitCountLoopx64(BRDigitTypex64 n);
#endif
  // (int)(log10(x) / LOG10_BIGREALBASE)
  static int     logBASE(double x);
  // Return p if n = 10^p for p = [0..9]. else return -1.
  static int     isPow10(BRDigitTypex86 n);
  // Return p if n = 10^p for p = [0..19]. else return -1.
  static int     isPow10(BRDigitTypex64 n);
  // true if |x| = 10^p for p = [BIGREAL_MINEXPO..BIGREAL_MAXEXPO]
  static bool    isPow10(const BigReal &x);

  // Absolute value of x (=|x|)
  // Result.digitPool = x.digitPool
  friend BigReal fabs(     const BigReal &x);
  // biggest integer <= x
  friend BigInt  floor(    const BigReal &x);
  // smallest integer >= x
  friend BigInt  ceil(     const BigReal &x);
  // sign(x) * (|x| - floor(|x|))
  friend BigReal fraction( const BigReal &x);
  // sign(x) * floor(|x|*10^prec+0.5)/10^prec
  friend BigReal round(    const BigReal &x, intptr_t prec = 0);
  // sign(x) * floor(|x|*10^prec)/10^prec
  friend BigReal trunc(    const BigReal &x, intptr_t prec = 0);
  // x truncated to the specified number of significant decimal digits
  friend BigReal cut(      const BigReal &x, size_t   digits, DigitPool *digitPool = NULL);
  // x * pow(10,n)
  friend BigReal e(        const BigReal &x, BRExpoType n   , DigitPool *digitPool = NULL);

  // Assume x._isfinite(). Return x < 0 ? -1 : x > 0 ? 1 : 0
  friend inline int sign(  const BigReal &x) {
    assert(x._isfinite());
    return x._isnormal() ? x.m_negative ? -1 : 1 : 0;
  }

  // Return true if x._isfinite() && isInteger(x) && x % 2 == 0
  friend bool    even(     const BigReal &x);
  //  Return true if x._isfinite() && isInteger(x) && x % 2 == 1
  friend bool    odd(      const BigReal &x);
  // Return true if x._isfinite() && (fraction(x) == 0)
  friend inline bool isInteger(const BigReal &x) {
    return x._isfinite() && x.getLow() >= 0;
  }

  // Assume f._isfinite(). Set to = from so |to-from| <= f. Return to
  friend BigReal &copy(BigReal &to, const BigReal &from, const BigReal &f);
  // Set to = from so to.getlength() = min(length,from.getlength(). Return to
  friend BigReal &copy(BigReal &to, const BigReal &from, size_t length);

  // Assume x and y are both normal (finite and != 0) and f>0.
  // x/y with |error| < f. Newton-rapthon iteration
  static BigReal quotNewton(   const BigReal &x, const BigReal &y, const BigReal &f, DigitPool *digitPool);
  // Assume x and y are both normal (finite and != 0) and f>0.
  // x/y with |error| < f. School method. using built-in 32-bit division
  static BigReal quotLinear32( const BigReal &x, const BigReal &y, const BigReal &f, DigitPool *digitPool);
  // Assume x and y are both normal (finite and != 0) and f>0.
  // x/y with |error| < f. School method. using built-in 64-bit division
  static BigReal quotLinear64( const BigReal &x, const BigReal &y, const BigReal &f, DigitPool *digitPool);
#ifdef IS64BIT
  // Assume x and y are both normal (finite and != 0) and f>0.
  // x/y with |error| < f. School method. using class _int128 for division
  static BigReal quotLinear128(const BigReal &x, const BigReal &y, const BigReal &f, DigitPool *digitPool);
#endif // IS64BIT
  // x*y with |error| < f. Used to multiply short numbers. Used by prod.
  static BigReal shortProd(    const BigReal &x, const BigReal &y, const BigReal &f, DigitPool *digitPool);

  // Return _isnormal() ? first BASE-digit : 0.
  inline BRDigitType getFirstDigit() const {
    return _isnormal() ? m_first->n : 0;
  }
  // Return _isnormal() ? last BASE-digit : 0.
  inline BRDigitType getLastDigit()  const {
    return _isnormal() ? m_last->n : 0;
  }

  // First k decimal digits. Assume k <= 9.
  ULONG  getFirst32(const UINT k, BRExpoType *scale = NULL) const;
  // First k decimal digits. Assume k <= 19.
  UINT64 getFirst64(const UINT k, BRExpoType *scale = NULL) const;
#ifdef IS64BIT
  // First k decimal digits. Assume k <= 38.
  _uint128 &getFirst128(_uint128 &dst, const UINT k, BRExpoType *scale = NULL) const;
#endif
  inline DigitPool *getDigitPool() const {
    return &m_digitPool;
  }
  inline int getPoolId() const {
    return m_digitPool.getId();
  }
  ULONG hashCode() const;

  void save(ByteOutputStream &s) const;
  void load(ByteInputStream  &s);

  friend Packer &operator<<(Packer &p, const BigReal &v);
  friend Packer &operator>>(Packer &p,       BigReal &v);

  static UINT getMaxSplitLength();

  // Call this when terminateCalculation has been called and you want to start a new calculation
  static void resumeCalculation() {
    s_continueCalculation = true;
  }

  // Call this to make the calculation stop as soon as possible. In current version it will
  // terminate all Threads started from BigRealThreadPool
  static void terminateCalculation() {
    s_continueCalculation = false;
  }

  void assertIsValidBigReal() const; // checks that this is a consistent number with all the various invariants satisfied.
                                     // Throws an excpeption if not with a descripion of what is wrong. For debugging

  // Return uniform distributed random BigReal between 0 (incl) and 1 (excl) with at most maxDigits decimal digits.
  // If maxDigits == 0, 0 will be returned
  // Digits generated with rnd
  // If digitPool == NULL, use DEFAULT_DIGITPOOL
  friend BigReal randBigReal(size_t maxDigits, RandomGenerator &rnd = *RandomGenerator::s_stdGenerator, DigitPool *digitPool = NULL);

  // Return uniform distributed random BigReal in [from;to] with at most maxDigits decimal digits.
  // Digits generated with rnd
  // If digitPool == NULL, use from.getDigitPool()
  friend BigReal  randBigReal(const BigReal &from, const BigReal &to, size_t maxDigits, RandomGenerator &rnd = *RandomGenerator::s_stdGenerator, DigitPool *digitPool = NULL);

  // Comnon used constants allocated with ConstDigitPool (see below)
  static const ConstBigInt  _0;          // 0
  static const ConstBigInt  _1;          // 1
  static const ConstBigInt  _2;          // 2
  static const ConstBigReal _05;         // 0.5

  static const ConstBigInt  _i32_min;    // _I32_MIN
  static const ConstBigInt  _i32_max;    // _I32_MAX
  static const ConstBigInt  _ui32_max;   // _UI32_MAX
  static const ConstBigInt  _i64_min;    // _I64_MIN
  static const ConstBigInt  _i64_max;    // _I64_MAX
  static const ConstBigInt  _ui64_max;   // _UI64_MAX
  static const ConstBigInt  _i128_min;   // _I128_MIN
  static const ConstBigInt  _i128_max;   // _I128_MAX
  static const ConstBigInt  _ui128_max;  // _UI128_MAX
  static const ConstBigReal _flt_min;    // FLT_MIN
  static const ConstBigReal _flt_max;    // FLT_MAX
  static const ConstBigReal _dbl_min;    // DBL_MIN
  static const ConstBigReal _dbl_max;    // DBL_MAX
  static const ConstBigReal _dbl80_min;  // DBL80_MIN
  static const ConstBigReal _dbl80_max;  // DBL80_MAX
  static const ConstBigReal _C1third;    // approx 1/3

  static const ConstBigReal _BR_QNAN;    // non-signaling NaN (quiet NaN)
  static const ConstBigReal _BR_PINF;    // +infinity;
  static const ConstBigReal _BR_NINF;    // -infinity;
};

class ConstDigitPool : private DigitPool {
private:
  Semaphore  m_gate;
  int        m_requestCount;
#ifdef CHECKCONSTPOOLTHREAD
  int        m_ownerThreadId;
#endif // CHECKCONSTPOOLTHREAD

  static ConstDigitPool s_instance;

public:
  ConstDigitPool() : DigitPool(CONST_DIGITPOOL_ID), m_requestCount(0) {
#ifdef CHECKCONSTPOOLTHREAD
    m_ownerThreadId = -1;
#endif // CHECKCONSTPOOLTHREAD
  }

  static inline DigitPool *requestInstance() {
#ifdef CHECKCONSTPOOLTHREAD
    const DWORD thrId = GetCurrentThreadId();
#endif // CHECKCONSTPOOLTHREAD
    s_instance.m_gate.wait();
    s_instance.m_requestCount++;
#ifdef CHECKCONSTPOOLTHREAD
    s_instance.m_ownerThreadId = thrId;
#endif // CHECKCONSTPOOLTHREAD
    return &s_instance;
  }

  static inline void releaseInstance() {
#ifdef CHECKCONSTPOOLTHREAD
    const DWORD thrId = GetCurrentThreadId();
    if(thrId != s_instance.m_ownerThreadId) {
      throwException(_T("%s: Calling thread =%d, is not the ownerthread (=%d)")
                    ,__TFUNCTION__, thrId, s_instance.m_ownerThreadId);
    }
    s_instance.m_ownerThreadId = -1;
#endif // CHECKCONSTPOOLTHREAD
    s_instance.m_gate.signal();
  }
};

#define REQUESTCONSTPOOL ConstDigitPool::requestInstance()
#define RELEASECONSTPOOL ConstDigitPool::releaseInstance()

class ConstBigReal : public BigReal {
public:
  inline ConstBigReal(int              x) : BigReal(x, REQUESTCONSTPOOL) {
    RELEASECONSTPOOL;
  }
  inline ConstBigReal(UINT             x) : BigReal(x, REQUESTCONSTPOOL) {
    RELEASECONSTPOOL;
  }
  inline ConstBigReal(long             x) : BigReal(x, REQUESTCONSTPOOL) {
    RELEASECONSTPOOL;
  }
  inline ConstBigReal(ULONG            x) : BigReal(x, REQUESTCONSTPOOL) {
    RELEASECONSTPOOL;
  }
  inline ConstBigReal(INT64            x) : BigReal(x, REQUESTCONSTPOOL) {
    RELEASECONSTPOOL;
  }
  inline ConstBigReal(UINT64           x) : BigReal(x, REQUESTCONSTPOOL) {
    RELEASECONSTPOOL;
  }
  inline ConstBigReal(const _int128   &x) : BigReal(x, REQUESTCONSTPOOL) {
    RELEASECONSTPOOL;
  }
  inline ConstBigReal(const _uint128  &x) : BigReal(x, REQUESTCONSTPOOL) {
    RELEASECONSTPOOL;
  }

  ConstBigReal(float                   x) : BigReal(x, REQUESTCONSTPOOL) {
    RELEASECONSTPOOL;
  }
  ConstBigReal(double                  x) : BigReal(x, REQUESTCONSTPOOL) {
    RELEASECONSTPOOL;
  }
  ConstBigReal(const Double80         &x) : BigReal(x, REQUESTCONSTPOOL) {
    RELEASECONSTPOOL;
  }
  ConstBigReal(const BigReal          &x) : BigReal(x, REQUESTCONSTPOOL) {
    RELEASECONSTPOOL;
  }
  explicit inline ConstBigReal(const String  &s) : BigReal(s, REQUESTCONSTPOOL) {
    RELEASECONSTPOOL;
  }
  explicit inline ConstBigReal(const char    *s) : BigReal(s, REQUESTCONSTPOOL) {
    RELEASECONSTPOOL;
  }
  explicit inline ConstBigReal(const wchar_t *s) : BigReal(s, REQUESTCONSTPOOL) {
    RELEASECONSTPOOL;
  }
  explicit inline ConstBigReal(ByteInputStream &s) : BigReal(s, REQUESTCONSTPOOL) {
    RELEASECONSTPOOL;
  }

  ~ConstBigReal() {
    REQUESTCONSTPOOL;
    releaseDigits();
    RELEASECONSTPOOL;
  }
};

#undef REQUESTCONSTPOOL
#undef RELEASECONSTPOOL

#define APCsum( bias, x, y, pool) BigReal::apcSum( #@bias, x, y, pool)
#define APCprod(bias, x, y, pool) BigReal::apcProd(#@bias, x, y, pool)
#define APCquot(bias, x, y, pool) BigReal::apcQuot(#@bias, x, y, pool)
#define APCpow( bias, x, y, pool) BigReal::apcPow( #@bias, x, y, pool)

// Assume fpclass in {_FPCLASS_PZ,_FPCLASS_NZ, _FPCLASS_PINF, _FPCLASS_NINF, _FPCLASS_QNAN, _FPCLASS_SNAN }
template<class NumberType> NumberType getNonNormalValue(int fpclass, const NumberType &zero) {
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

// returns one of
// FP_INFINITE
// FP_NAN
// FP_NORMAL
// FP_SUBNORMAL ---- NOT used
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
  return isunordered(x,y) ? false : (compare(x,y) == 0);
}
inline bool operator!=(const BigReal &x, const BigReal &y) {
  return isunordered(x,y) ? false : (compare(x,y) != 0);
}
inline bool operator>=(const BigReal &x, const BigReal &y) {
  return isunordered(x,y) ? false : (compare(x,y) >= 0);
}
inline bool operator<=(const BigReal &x, const BigReal &y) {
  return isunordered(x,y) ? false : (compare(x,y) <= 0);
}
inline bool operator> (const BigReal &x, const BigReal &y) {
  return isunordered(x,y) ? false : (compare(x,y) >  0);
}
inline bool operator< (const BigReal &x, const BigReal &y) {
  return isunordered(x,y) ? false : (compare(x,y) <  0);
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
  DigitPool *pool = x.getDigitPool();
  return x.isNegative() ? -pool->_1() : x.isPositive() ? pool->_1() : pool->_0();
}
inline BigReal  dmax(const BigReal &x, const BigReal &y) {
  return (compare(x, y) >= 0) ? x : y;
}
inline BigReal  dmin(const BigReal &x, const BigReal &y) {
  return (compare(x, y) <= 0) ? x : y;
}

int  isInt(     const BigReal &v);
UINT isUint(    const BigReal &v);
bool isInt64(   const BigReal &v);
bool isUint64(  const BigReal &v);
bool isInt128(  const BigReal &v);
bool isUint128( const BigReal &v);

// pow(a,r) mod n
BigInt powmod(const BigInt &a, const BigInt &r, const BigInt &n, bool verbose = false);

BigReal sqrt( const BigReal &x, const BigReal &f);
BigReal exp(  const BigReal &x, const BigReal &f);
BigReal ln(   const BigReal &x, const BigReal &f);
BigReal ln1(  const BigReal &x, const BigReal &f);
// log(x) base 10
BigReal log10(const BigReal &x, const BigReal &f);
// Logarithm(x) for the specified base. (ln(x)/ln(base))
BigReal log(  const BigReal &base, const BigReal &x, const BigReal &f);
// x^y
BigReal pow(  const BigReal &x, const BigReal &y, const BigReal &f);
BigReal sin(  const BigReal &x, const BigReal &f);
BigReal cos(  const BigReal &x, const BigReal &f);
BigReal tan(  const BigReal &x, const BigReal &f);
BigReal cot(  const BigReal &x, const BigReal &f);
BigReal asin( const BigReal &x, const BigReal &f);
BigReal acos( const BigReal &x, const BigReal &f);
BigReal atan( const BigReal &x, const BigReal &f);
BigReal acot( const BigReal &x, const BigReal &f);
// result.digitPool = f.digitPool or digitpool if specified
BigReal pi(   const BigReal &f, DigitPool *digitPool = NULL);

// Calculates with relative precision ie. with the specified number of decimal digits
BigReal rRound(const BigReal &x,    size_t digits);
BigReal rSum(  const BigReal &x,    const BigReal &y, size_t digits, DigitPool *digitPool = NULL);
BigReal rDif(  const BigReal &x,    const BigReal &y, size_t digits, DigitPool *digitPool = NULL);
BigReal rProd( const BigReal &x,    const BigReal &y, size_t digits, DigitPool *digitPool = NULL);
BigReal rQuot( const BigReal &x,    const BigReal &y, size_t digits, DigitPool *digitPool = NULL);
BigReal rSqrt( const BigReal &x,    size_t digits);
BigReal rExp(  const BigReal &x,    size_t digits);
BigReal rLn(   const BigReal &x,    size_t digits);
BigReal rLog10(const BigReal &x,    size_t digits);
// Logarithm(x) for the specified base. (ln(x)/ln(base))
BigReal rLog(  const BigReal &base, const BigReal &x, size_t digits);
// x^y
BigReal rPow(  const BigReal &x,    const BigReal &y, size_t digits);
BigReal rRoot( const BigReal &x,    const BigReal &y, size_t digits);
BigReal rSin(  const BigReal &x,    size_t digits);
BigReal rCos(  const BigReal &x,    size_t digits);
BigReal rTan(  const BigReal &x,    size_t digits);
BigReal rCot(  const BigReal &x,    size_t digits);
BigReal rAsin( const BigReal &x,    size_t digits);
BigReal rAcos( const BigReal &x,    size_t digits);
BigReal rAtan( const BigReal &x,    size_t digits);
BigReal rAcot( const BigReal &x,    size_t digits);
BigReal rPi(                         size_t digits, DigitPool *digitPool = NULL);
BigReal rGamma(    const BigReal &x, size_t digits);
BigReal rFactorial(const BigReal &x, size_t digits);

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
  UINT run();
  void execute(Runnable &job, SynchronizedStringQueue &resultQueue);
};

class MultiplierThread : public Thread, public BigRealResource {
private:
  DigitPool                *m_digitPool;
  SynchronizedStringQueue  *m_resultQueue;
  Semaphore                 m_execute;
  const BigReal            *m_x, *m_y, *m_f;
  BigReal                  *m_result;
  int                       m_requestCount;
  int                       m_level;
  friend class BigRealResourcePool;
public:
  MultiplierThread(int id);
  UINT run();

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
  virtual void allocateNewResources(size_t count) {
    int id = (int)size();
    for(size_t i = 0; i < count; i++, id++) {
      m_freeId.push(id);
      T *r = new T(id); TRACE_NEW(r);
      add(r);
    }
  }
public:
  virtual ~ResourcePool() {
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
    for(size_t i = 0; i < size(); i++) {
      SAFEDELETE((*this)[i]);
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
  friend class BigRealResourcePool;
public:
  void waitForAllResults();
  ~MThreadArray();
  MultiplierThread &get(UINT i) {
    return *(*this)[i];
  }
};

#ifdef CHECKALLDIGITS_RELEASED
class DigitPoolWithCheck : public DigitPool {
public:
  UINT m_usedDigits;
  DigitPoolWithCheck(int id, UINT intialDigitcount = 0) : DigitPool(id, intialDigitcount) {
  }
};
typedef DigitPoolWithCheck MTDigitPoolType;
#else
typedef DigitPool MTDigitPoolType;
#endif // CHECKALLDIGITS_RELEASED

template <class T> class BigRealThreadPool : public ResourcePool<T> {
private:
  int  m_threadPriority;
  bool m_disablePriorityBoost;

  Thread &get(UINT index) {
    return *((Thread*)((*this)[index]));
  }

protected:
  void allocateNewResources(size_t count) {
    const int oldSize = (int)size();
    __super::allocateNewResources(count);
    for(int i = oldSize; i < (int)size(); i++) {
      Thread &thr = get(i);
      thr.setPriority(m_threadPriority);
      thr.setPriorityBoost(m_disablePriorityBoost);
    }
  }
public:
  BigRealThreadPool() {
    m_threadPriority       = THREAD_PRIORITY_NORMAL;
    m_disablePriorityBoost = false;
  }
  void setPriority(int priority) {
    if(priority == m_threadPriority) return;
    m_threadPriority = priority;
    for(int i = 0; i < (int)size(); i++) get(i).setPriority(priority);
  }
  int getPriority() const {
    return m_threadPriority;
  }
  void setPriorityBoost(bool disablePriorityBoost) {
    if(disablePriorityBoost == m_disablePriorityBoost) return;
    m_disablePriorityBoost = disablePriorityBoost;
    for(int i = 0; i < (int)size(); i++) get(i).setPriorityBoost(disablePriorityBoost);
  }

  bool getPriorityBoost() const {
    return m_disablePriorityBoost;
  }
};

typedef CompactArray<Runnable*> BigRealJobArray;

class BigRealResourcePool {
private:
  BigRealThreadPool<BigRealThread>      m_threadPool;
  BigRealThreadPool<MultiplierThread>   m_MTThreadPool;
  ResourcePool<SynchronizedStringQueue> m_queuePool;
  ResourcePool<MTDigitPoolType>         m_digitPool;
  mutable Semaphore                     m_gate;
  int                                   m_processorCount;
  int                                   m_activeThreads, m_maxActiveThreads;

  BigRealResourcePool();
  BigRealResourcePool(const BigRealResourcePool &src);            // not implemented
  BigRealResourcePool &operator=(const BigRealResourcePool &src); // not implemented
  friend class BigRealResourcePoolCreator;
public:
  ~BigRealResourcePool();
  static MThreadArray &fetchMTThreadArray(  MThreadArray &threads, int count);
  static void          releaseMTThreadArray(MThreadArray &threads);

  static DigitPool    *fetchDigitPool();
  static void          releaseDigitPool(DigitPool *pool);

  // Blocks until all jobs are done. If any of the jobs throws an exception,
  // the rest of the jobs will be terminated and an exception with the same
  // message will be thrown to the caller
  static void executeInParallel(BigRealJobArray &jobs);

  static inline int getMaxActiveThreads() {
    return getInstance().m_maxActiveThreads;
  }

  static String toString(); // for debug
  static void startLogging();
  static void stopLogging();
  // Sets the priority for all running and future running threads
  // Default is THREAD_PRIORITY_BELOW_NORMAL
  // THREAD_PRIORITY_IDLE,-PRIORITY_LOWEST,-PRIORITY_BELOW_NORMAL,-PRIORITY_NORMAL,-PRIORITY_ABOVE_NORMAL
  static void setPriority(int priority);

  static void setPriorityBoost(bool disablePriorityBoost);
  static BigRealResourcePool &getInstance();
};

#ifdef TRACEPRODUCTRECURSION
void logProductRecursion(UINT level, const TCHAR *method, _In_z_ _Printf_format_string_ const TCHAR * const format, ...);
#define LOGPRODUCTRECURSION(...) logProductRecursion(level, __TFUNCTION__, __VA_ARGS__)
#else
#define LOGPRODUCTRECURSION(...)
#endif TRACEPRODUCTRECURSION

// Old version sign(x) * (|x| - floor(|x|))
BigReal oldFraction(const BigReal &x);
// old operator%(const BigReal &x, const BigReal &y);
BigReal modulusOperator64( const BigReal &x, const BigReal &y);
// old operator%(const BigReal &x, const BigReal &y);
BigReal modulusOperator128(const BigReal &x, const BigReal &y);

#pragma comment(lib,  TM_LIB_VERSION "BigReal.lib")
