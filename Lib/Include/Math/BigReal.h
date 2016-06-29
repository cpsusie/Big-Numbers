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
#ifdef IS64BIT
#include "Int128.h"
#endif

// Define this to have 2 different version of getDecimalDigitCount64(unsigned __int64 n).
// Measures og time show that getDecimalDigitCount64 is 5 times faster that getDecimalDigitCount64Loop
//#define HAS_LOOP_DIGITCOUNT

//#define USE_X32SERVERCHECK // use, if you want to have backup-check for correct multiplication, running in separate
// process built with x86 code, Serverprocess is called "MultiplicationServer", and protocol to this is quit simple:
// write 3 BigReals, x y f as text. Serverprogram will return x*y (as text) with an error no greater than |f|
// Can be repeated as long as you want
#ifdef USE_X32SERVERCHECK
#include <Math/ExternEngine.h>
#endif

// Basic definitions depends on registersize. 32- og 64-bit
#ifdef IS32BIT

#define LOG10_BIGREALBASE  8
#define POWER10TABLESIZE   10

#define BIGREAL_ZEROEXPO  -900000000
#define BIGREAL_MAXEXPO    99999999
#define BIGREAL_MINEXPO   -99999999

typedef unsigned long BRDigitType;
typedef long          BRExpoType;
typedef long          BRDigitDiffType;

#else // IS64BIT

#define LOG10_BIGREALBASE  18
#define POWER10TABLESIZE   20

#define BIGREAL_ZEROEXPO  -900000000000000000
#define BIGREAL_MAXEXPO    99999999999999999
#define BIGREAL_MINEXPO   -99999999999999999

typedef unsigned __int64 BRDigitType;
typedef __int64          BRExpoType;
typedef __int64          BRDigitDiffType;

#endif // IS32BIT

#define SP_OPT_NONE      0
#define SP_OPT_BY_FPU    1
#define SP_OPT_BY_FPU2   2
#define SP_OPT_BY_REG32  3
#define SP_OPT_BY_REG64  4

// Define SP_OPT_METHOD to one of these, to select the desired optmization of shortProduct. My own favorite is SP_OPT_BY_REG32

//#define SP_OPT_METHOD  SP_OPT_NONE
//#define SP_OPT_METHOD  SP_OPT_BY_FPU
//#define SP_OPT_METHOD  SP_OPT_BY_FPU2

#ifdef IS64BIT
#define SP_OPT_METHOD SP_OPT_BY_REG64
#else
#define SP_OPT_METHOD SP_OPT_BY_REG32
#endif

#ifndef SP_OPT_METHOD

#error Must define SP_OPT_METHOD

#elif((SP_OPT_METHOD != SP_OPT_NONE) && (SP_OPT_METHOD != SP_OPT_BY_FPU) && (SP_OPT_METHOD != SP_OPT_BY_FPU2) && (SP_OPT_METHOD != SP_OPT_BY_REG32) && (SP_OPT_METHOD != SP_OPT_BY_REG64))

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
// this is the only valid BIGREALBASE for these optimizations
#else
#error Illegal LOG10_BIGREALBASE. Must be 8.
#endif // LOG10_BIGREALBASE == 8

#elif (SP_OPT_METHOD == SP_OPT_BY_REG64)

#if LOG10_BIGREALBASE == 18
#define BIGREALBASE 1000000000000000000
// this is the only valid BIGREALBASE for SP_OPT_BY_REG64
#else
#error Illegal LOG10_BIGREALBASE. Must be 18.
#endif // LOG10_BIGREALBASE == 18

#endif // if(SP_OPT_METHOD == ...)

#define APC_DIGITS         6

#define CONVERSION_POW2DIGITCOUNT 23

class Digit { // The definition of Digit should NOT be changed!!! Se asm-code in ShortProduct*.cpp
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

class Pow2ArgumentKey {
public:
  int    m_n;
  size_t m_digits;
  Pow2ArgumentKey() {
  }
  inline Pow2ArgumentKey(int n, size_t digits) : m_n(n), m_digits(digits) {
  }
  inline Pow2ArgumentKey(ByteInputStream &s) {
    load(s);
  }
  inline unsigned long hashCode() const {
#ifdef IS32BIT
    return m_n * 23 + m_digits;
#else
    return m_n * 23 + uint64Hash(m_digits);
#endif
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

#define CACHE_LOADING 0x1
#define CACHE_LOADED  0x2
#define CACHE_EMPTY   0x4

class Pow2Cache : public CompactHashMap<Pow2ArgumentKey, BigReal*> {
private:
  DECLARECLASSNAME;
  Semaphore     m_gate;
  unsigned char m_state;
  size_t        m_startSize;

  void save(const String &fileName);
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
#ifdef _DEBUG
  static size_t s_cacheHitCount, s_cacheRequestCount;
#endif

  Pow2Cache();
  ~Pow2Cache();
  bool put(const Pow2ArgumentKey &key, BigReal * const &v);
  BigReal **get(const Pow2ArgumentKey &key);
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
  DECLARECLASSNAME;
  static size_t         s_totalDigitCount;
  static bool           s_dumpCountWhenDestroyed;
  size_t                m_digitCount;
  DigitPage            *m_firstPage;
  Digit                *m_freeDigits;;

  // Some frequently used constants. exist in every pool
  BigInt               *m_zero; // = 0
  BigInt               *m_one;  // = 1
  BigInt               *m_two;  // = 2
  BigReal              *m_half; // = 0.5

  static void addToCount(intptr_t n);
  DigitPool(           const DigitPool &src); // not implemented
  DigitPool &operator=(const DigitPool &src); // not implemented
  void allocatePage();
public:
  static DigitPool s_defaultDigitPool;

  DigitPool(int id, size_t intialDigitcount = 0); // initialDigitcount in BIGREALBASE-digits
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
      delete first;
      if(p == end) break;
      first = p;
    } 
#endif // USE_DIGITPOOL_FREELIST
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
void insertDigitAndIncrExpo(BigReal &v, BRDigitType n);
}
#endif

void throwInvalidToleranceException(               TCHAR const* const function);
void throwBigRealInvalidArgumentException(         TCHAR const* const function, _In_z_ _Printf_format_string_ TCHAR const * const format,...);
void throwBigRealGetIntegralTypeOverflowException( TCHAR const* const function, const BigReal &x, const String &maxStr);
void throwBigRealGetIntegralTypeUnderflowException(TCHAR const* const function, const BigReal &x, const String &minStr);
void throwBigRealException(_In_z_ _Printf_format_string_ TCHAR const* const format,...);

class BigReal {
private:
  DECLARECLASSNAME;

#ifdef USE_X32SERVERCHECK
  static ExternEngine s_multiplyServer;
  friend class InitBigReal;
#endif

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
  static const BRDigitType  s_power10Table[POWER10TABLESIZE];
  static size_t             s_splitLength;         // Defined in shortProduct.cpp. Split factors when length > s_splitLength
  static bool               s_continueCalculation; // Terminate all calculation with and Exception ("Operation was cancelled")
                                                   // if they enter shortProduct, or simply return whichever comes first
  Digit                    *m_first;               // Most significand  digit
  Digit                    *m_last;                // Least significand digit
  BRExpoType                m_expo, m_low;         // if isZero() then (m_expo,m_low)=(BIGREAL_ZEROEXPO,0) else m_expo = m_low+getLength()-1
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
#ifdef IS64BIT
  void init(const    _int128 &n);
  void init(        _uint128  n);
#endif // IS64BIT
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

  inline void appendDigit(BRDigitType n) {
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
  void    insertAfter(            Digit *q, BRDigitType n);
  void    insertDigit(                      BRDigitType n);
  void    insertZeroDigits(                 size_t count);
  void    insertZeroDigitsAfter(  Digit *p, size_t count);
  void    insertBorrowDigitsAfter(Digit *p, size_t count);
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
  void    trimHead(); // trim zeroes from head, AND if necessary from the tail to
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
  void    addSubProduct(unsigned __int64 n);

#elif(SP_OPT_METHOD == SP_OPT_BY_FPU)

  void    addFPUReg0();
  void    addSubProduct(unsigned __int64 n);

#elif((SP_OPT_METHOD == SP_OPT_BY_REG32) || (SP_OPT_METHOD == SP_OPT_BY_REG64) || (SP_OPT_METHOD == SP_OPT_BY_FPU2))

  inline Digit *fastAppendDigit(Digit *last) { // Assume last != NULL. new digit->n and ..->next are not initialized
    Digit *p = last->next = newDigit();        // Should only be called from shortProductNoZeroCheck. m_last will
    p->prev  = last;                           // not be touched, so invariant will be broken for af while,
    return p;                                  // and reestablished at the end of shortProductNoZeroCheck
  }                                            // when we clean up the chain head and end

#endif

#ifdef _DEBUG
  // One that works, and used for testing other versions of this important function
  BigReal &shortProductNoZeroCheckReference(const BigReal &x, const BigReal &y, int loopCount);   // return this
  friend void insertDigitAndIncrExpo(BigReal &v, BRDigitType n);
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

  // Don't call shortProductNoZeroCheck with numbers longer than getMaxSplitLength(), or you'll get a wrong result
  BigReal &shortProduct(                     const BigReal &x, const BigReal &y, BRExpoType fexpo    );  // return this
  BigReal &shortProductNoZeroCheck(          const BigReal &x, const BigReal &y, size_t     loopCount);  // return this
  static BigReal &product(  BigReal &result, const BigReal &x, const BigReal &y, const BigReal &f,              int level);
  static BigReal &productMT(BigReal &result, const BigReal &x, const BigReal &y, const BigReal &f, intptr_t  w, int level);

  void    split(BigReal &a, BigReal &b, size_t n, const BigReal &f) const;
  void    copyDigits(   const BigReal &src, size_t length);
  BigReal &copyrTrunc(  const BigReal &src, size_t digits); // copy specified number of decimal digits, truncating result. return *this
  void    copyAllDigits(const BigReal &src);
  inline BigReal &setSignByProductRule(const BigReal &x, const BigReal &y) {
    m_negative = x.m_negative != y.m_negative;
    SETBIGREALDEBUGSTRING(*this);
    return *this;
  }

  // rTrunc and rRound cut of a number (!= 0) to the specified number of significant decimal digits,
  // truncating towards zero, or rounding respectively.
  BigReal &rTrunc(size_t digits);  // assume *this != 0 and digits > 0 (digits is decimal digits). return this
  BigReal &rRound(size_t digits);  // assume *this != 0 and digits > 0 (digits is decimal digits). return this

  // Division helperfunctions, 
  // Result.digitPool = x x.digitPool
  static BigReal  reciprocal(const BigReal &x, DigitPool *digitPool = NULL);                      // approximately 1/x
  BigReal &approxQuot32(     const BigReal &x, const BigReal           &y           );            // approximately x/y
  BigReal &approxQuot64(     const BigReal &x, const BigReal           &y);                       // approximately x/y
  BigReal &approxQuot32Abs(  const BigReal &x, unsigned long            y, BRExpoType scale);     // approximately x/e(y,scale)
  BigReal &approxQuot64Abs(  const BigReal &x, const unsigned __int64  &y, BRExpoType scale);     // approximately x/e(y,scale)
#ifdef IS64BIT
  BigReal &approxQuot128(    const BigReal &x, const BigReal           &y);                       // approximately x/y
  BigReal &approxQuot128Abs( const BigReal &x, const _uint128          &y, BRExpoType scale);     // approximately x/e(y,scale)
#endif // IS64BIT
  static double  estimateQuotNewtonTime(const BigReal &x, const BigReal &y, const BigReal &f);
  static double  estimateQuotLinearTime(const BigReal &x, const BigReal &y, const BigReal &f);
  static bool    chooseQuotNewton(      const BigReal &x, const BigReal &y, const BigReal &f);

  // Misc
  BigReal  &adjustAPCResult(const char bias, const TCHAR *function); // return this
  Double80 getDouble80NoLimitCheck() const; 
  void     formatFixed(        String &result, streamsize precision, long flags, bool removeTrailingZeroes) const;
  void     formatScientific(   String &result, streamsize precision, long flags, BRExpoType expo10, bool removeTrailingZeroes) const;
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
#ifdef IS64BIT
  inline BigReal(_int128          x      , DigitPool *digitPool = NULL) : _SETDIGITPOOL() {
    init(x);
    SETBIGREALDEBUGSTRING(*this)
  }

  inline BigReal(_uint128         x      , DigitPool *digitPool = NULL) : _SETDIGITPOOL() {
    init(x);
    SETBIGREALDEBUGSTRING(*this)
  }
#endif // IS64BIT

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

#ifdef IS64BIT
  inline BigReal &operator=(const _int128   &n) {
    clearDigits(); init(n);
    SETBIGREALDEBUGSTRING(*this);
    return *this;
  }

  inline BigReal &operator=(const _uint128  &n) {
    clearDigits(); init(n);
    SETBIGREALDEBUGSTRING(*this);
    return *this;
  }
#endif // IS64BIT

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
#ifdef IS64BIT
  friend          _int128 getInt128(  const BigReal  &x);
  friend         _uint128 getUint128( const BigReal  &x);
#endif // IS64BIT
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
  BigReal &multPow10(BRExpoType exp);

  // Result.digitPool = x x.digitPool
  friend BigInt operator/ (     const BigInt &x, const BigInt &y);
  friend BigInt operator% (     const BigInt &x, const BigInt &y);
  friend BigInt operator- (     const BigInt &x);
  friend BigReal  sum(          const BigReal &x,  const BigReal &y, const BigReal  &f, DigitPool *digitPool = NULL); // x+y with |error| <= f
  friend BigReal  dif(          const BigReal &x,  const BigReal &y, const BigReal  &f, DigitPool *digitPool = NULL); // x-y with |error| <= f
  friend BigReal  prod(         const BigReal &x,  const BigReal &y, const BigReal  &f, DigitPool *digitPool = NULL); // x*y with |error| <= f
  friend BigReal  quot(         const BigReal &x,  const BigReal &y, const BigReal  &f, DigitPool *digitPool = NULL); // x/y with |error| <= f
  friend void    quotRemainder( const BigReal &x,  const BigReal &y, BigInt *quotient, BigReal *remainder);  // Calculates only quotient and/or remainder if specified
  friend void    quotRemainder64( const BigReal &x,  const BigReal &y, BigInt *quotient, BigReal *remainder);  // Calculates only quotient and/or remainder if specified
#ifdef IS64BIT
  friend void    quotRemainder128(const BigReal &x,  const BigReal &y, BigInt *quotient, BigReal *remainder);  // Calculates only quotient and/or remainder if specified
#endif
  static BigReal  apcSum(       const char bias,  const BigReal &x, const BigReal  &y, DigitPool *digitPool = NULL); // bias = '<','#' or '>'
  static BigReal  apcProd(      const char bias,  const BigReal &x, const BigReal  &y, DigitPool *digitPool = NULL); // bias = '<','#' or '>'
  static BigReal  apcQuot(      const char bias,  const BigReal &x, const BigReal  &y, DigitPool *digitPool = NULL); // bias = '<','#' or '>'
  static BigReal  apcPow(       const char bias,  const BigReal &x, const BigInt   &y, DigitPool *digitPool = NULL); // bias = '<','#' or '>'
  
  void           fractionate(BigInt *integerPart, BigReal *fractionPart) const;
  // result.digitPool = f.defaultDigitPool
  static BigReal ln10(         const BigReal &f);                                     // ln(10) with |error| < f
  static BigReal lnEstimate(   const BigReal &x);                                     // Approximatly ln(x). Assume 1 <= x <= 10
  static const BigReal &pow2(int n, size_t digits = 0);                               // 2^n, with the specified number of digits. if digits = 0, then full precision. Results are cached

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
  
  inline size_t getLength() const {                                                         // BigReal of BASE digits. 0 has length 1
    return isZero() ? 1 : (m_expo - m_low + 1);
  }      
  size_t getDecimalDigits()  const;                                                         // BigReal of decimal digits. 0 has length 1
  inline BRExpoType getLow() const {
    return isZero() ? 0 : m_low;
  }

  static inline BRExpoType getExpo10(const BigReal  &x) {                                   // x == 0 ? 0 : floor(log10(|x|))
    return x.isZero() ? 0 : (x.m_expo * LOG10_BIGREALBASE + getDecimalDigitCount(x.m_first->n) - 1);
  }

  static BigReal           getExpo10N( const BigReal  &x) {
    return BigReal(getExpo10(x), x.getDigitPool());
  }

  static inline BRDigitType pow10(unsigned int n) {                                         // Return 10^n. Assume n <= 9
#ifdef _DEBUG
    if(n < ARRAYSIZE(s_power10Table)) {
      return s_power10Table[n];
    }
    throwBigRealInvalidArgumentException(_T("pow10"), _T("n must be [0..%d] (=%d)"),ARRAYSIZE(s_power10Table)-1,n);
    return 1;
#else
    return s_power10Table[n];
#endif
  }

  static BRExpoType getExpo2(   const BigReal  &x);                                         // x == 0 ? 0 : approximately floor(log2(|x|))
  static inline int getDecimalDigitCount(BRDigitType n) {
#ifdef IS32BIT
    return getDecimalDigitCount32(n);
#else
    return getDecimalDigitCount64(n);
#endif
  }
  static int     getDecimalDigitCount32(unsigned long n   );                                // n == 0 ? 0 : (floor(log10(n))+1). Assume n < BIGREALBASE
  static int     getDecimalDigitCount64(unsigned __int64 n);                                // as above but for n < 1eMAXDIGITS_INT64

#ifdef HAS_LOOP_DIGITCOUNT
  static int     getDecimalDigitCount64Loop(unsigned __int64 n);
#endif

  static int     logBASE(double x);                                                         // (int)(log10(x) / LOG10_BIGREALBASE)
  static int     isPow10(size_t n);                                                         // Return p if n = 10^p for p = [0..POWER10TableSIZE[. else return -1.
  static bool    isPow10(const BigReal &x);                                                 // true if |x| = 10^p for p = [BIGREAL_MINEXPO..BIGREAL_MAXEXPO]

  // Result.digitPool = x x.digitPool
  friend BigReal fabs(     const BigReal &x);                                               // absolute value of x (=|x|)
  friend BigInt  floor(    const BigReal &x);                                               // biggest integer <= x
  friend BigInt  ceil(     const BigReal &x);                                               // smallest integer >= x
  friend BigReal fraction( const BigReal &x);                                               // sign(x) * (|x| - floor(|x|))
  friend BigReal round(    const BigReal &x, intptr_t prec = 0);                            // sign(x) * floor(|x|*10^prec+0.5)/10^prec
  friend BigReal trunc(    const BigReal &x, intptr_t prec = 0);                            // sign(x) * floor(|x|*10^prec)/10^prec
  friend BigReal cut(      const BigReal &x, size_t   digits, DigitPool *digitPool = NULL); // x truncated to the specified number of significant decimal digits
  friend BigReal e(        const BigReal &x, BRExpoType n   , DigitPool *digitPool = NULL); // x * pow(10,n)

  friend inline int sign(  const BigReal &x) {                                              // x < 0 ? -1 : x > 0 ? 1 : 0
    return x.isZero() ? 0 : x.isPositive() ? 1 : -1;
  }

  friend bool    odd(      const BigReal &x);                                               // x is an integer and x % 2 == 1
  friend bool    even(     const BigReal &x);                                               // x is an integer and x % 2 == 0
  friend inline bool isInteger(const BigReal &x) {                                          // fraction(x) == 0
    return x.getLow() >= 0;
  }

  static BigReal  random(size_t digits, Random *rnd = NULL, DigitPool *digitPool = NULL);   // 0 <= random < 1; with the specified number of decimal digits, 
                                                                                            // Digits generated with rnd. if rnd == NULL, _standardRandomGenerator is used

  friend BigReal &copy(BigReal &to, const BigReal &from, const BigReal &f);                 // Set to = from so |to-from| <= f. Return to
  friend BigReal &copy(BigReal &to, const BigReal &from, size_t    length);                 // Set to = from so to.getlength() = min(length,from.getlength(). Return to

  static BigReal quotNewton(   const BigReal &x, const BigReal &y, const BigReal &f, DigitPool *digitPool);   // x/y with |error| < f. Newton-rapthon iteration
  static BigReal quotLinear32( const BigReal &x, const BigReal &y, const BigReal &f, DigitPool *digitPool);   // x/y with |error| < f. School method. using build-in 32-bit division
  static BigReal quotLinear64( const BigReal &x, const BigReal &y, const BigReal &f, DigitPool *digitPool);   // x/y with |error| < f. School method. using build-in 64-bit division
#ifdef IS64BIT
  static BigReal quotLinear128(const BigReal &x, const BigReal &y, const BigReal &f, DigitPool *digitPool);   // x/y with |error| < f. School method. using class _int128 for division
#endif // IS64BIT

  static BigReal shortProd(    const BigReal &x, const BigReal &y, const BigReal &f, DigitPool *digitPool);   // x*y with |error| < f. Used to multiply short numbers. Used by prod.

  inline BRDigitType getFirstDigit() const {                                                // isZero() ? 0 : first BASE-digit.
    return isZero() ? 0 : m_first->n;
  }
  inline BRDigitType getLastDigit()  const {                                                // isZero() ? 0 : last BASE-digit.
    return isZero() ? 0 : m_last->n;
  }

  unsigned long    getFirst32(const unsigned int k, BRExpoType *scale = NULL) const;        // First k decimal digits. Assume k <= 9.
  unsigned __int64 getFirst64(const unsigned int k, BRExpoType *scale = NULL) const;        // First k decimal digits. Assume k <= 19.
#ifdef IS64BIT
  _uint128 &getFirst128(_uint128 &dst, const unsigned int k, BRExpoType *scale = NULL) const; // First k decimal digits. Assume k <= 38.
#endif
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

  static bool enableDebugString(bool enabled);                                              // return old value
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

class ConstDigitPool : private DigitPool {
private:
  DECLARECLASSNAME;
  Semaphore  m_gate;
  int        m_requestCount;
#ifdef _DEBUG
  int        m_ownerThreadId;
#endif

  static ConstDigitPool s_instance;

public:
  ConstDigitPool() : DigitPool(CONST_DIGITPOOL_ID), m_requestCount(0) {
#ifdef _DEBUG
    m_ownerThreadId = -1;
#endif
  }

  static inline DigitPool *requestInstance() {
    DEFINEMETHODNAME;
#ifdef _DEBUG
    const DWORD thrId = GetCurrentThreadId();
#endif
    s_instance.m_gate.wait();
    s_instance.m_requestCount++;
#ifdef _DEBUG
    s_instance.m_ownerThreadId = thrId;
#endif
    return &s_instance;
  }

  static inline void releaseInstance() {
#ifdef _DEBUG
    DEFINEMETHODNAME;
    const DWORD thrId = GetCurrentThreadId();
    if (thrId != s_instance.m_ownerThreadId) {
      throwMethodException(s_className, method
        , _T("Thread callling releaseInstance (=%d) is not the ownerthread (=%d)")
        , thrId, s_instance.m_ownerThreadId);
    }
    s_instance.m_ownerThreadId = -1;
#endif
    s_instance.m_gate.signal();
  }

  static inline const BigInt &getZero() {
    return s_instance.get0();
  }
  static inline const BigInt &getOne() {
    return s_instance.get1();
  }
  static inline const BigInt &getTwo() {
    return s_instance.get2();
  }
  static inline const BigReal &get05() {
    return s_instance.getHalf();
  }
};

#define REQUSETCONSTPOOL ConstDigitPool::requestInstance()
#define RELEASECONSTPOOL ConstDigitPool::releaseInstance()

class ConstBigReal : public BigReal {
public:
  inline ConstBigReal(int              x) : BigReal(x, REQUSETCONSTPOOL) {
    RELEASECONSTPOOL;
  }
  inline ConstBigReal(unsigned int     x) : BigReal(x, REQUSETCONSTPOOL) {
    RELEASECONSTPOOL;
  }
  inline ConstBigReal(long             x) : BigReal(x, REQUSETCONSTPOOL) {
    RELEASECONSTPOOL;
  }
  inline ConstBigReal(unsigned long    x) : BigReal(x, REQUSETCONSTPOOL) {
    RELEASECONSTPOOL;
  }
  inline ConstBigReal(__int64          x) : BigReal(x, REQUSETCONSTPOOL) {
    RELEASECONSTPOOL;
  }
  inline ConstBigReal(unsigned __int64 x) : BigReal(x, REQUSETCONSTPOOL) {
    RELEASECONSTPOOL;
  }
#ifdef IS64BIT
  inline ConstBigReal(const _int128   &x) : BigReal(x, REQUSETCONSTPOOL) {
    RELEASECONSTPOOL;
  }
  inline ConstBigReal(const _uint128  &x) : BigReal(x, REQUSETCONSTPOOL) {
    RELEASECONSTPOOL;
  }
#endif // IS64BIT

  ConstBigReal(float                   x) : BigReal(x, REQUSETCONSTPOOL) {
    RELEASECONSTPOOL;
  }
  ConstBigReal(double                  x) : BigReal(x, REQUSETCONSTPOOL) {
    RELEASECONSTPOOL;
  }
  ConstBigReal(const Double80         &x) : BigReal(x, REQUSETCONSTPOOL) {
    RELEASECONSTPOOL;
  }
  ConstBigReal(const BigReal          &x) : BigReal(x, REQUSETCONSTPOOL) {
    RELEASECONSTPOOL;
  }
  explicit inline ConstBigReal(const String &s) : BigReal(s, REQUSETCONSTPOOL) {
    RELEASECONSTPOOL;
  }
  explicit inline ConstBigReal(const TCHAR  *s) : BigReal(s, REQUSETCONSTPOOL) {
    RELEASECONSTPOOL;
  }
  explicit inline ConstBigReal(ByteInputStream &s) : BigReal(s, REQUSETCONSTPOOL) {
    RELEASECONSTPOOL;
  }

#ifdef UNICODE
  explicit inline ConstBigReal(const char   *s) : BigReal(s, REQUSETCONSTPOOL) {
    RELEASECONSTPOOL;
  }
#endif

  ~ConstBigReal() {
    REQUSETCONSTPOOL;
    setToZero();
    RELEASECONSTPOOL;
  }

  static const ConstBigReal _long_min;
  static const ConstBigReal _long_max;
  static const ConstBigReal _ulong_max;
  static const ConstBigReal _i64_min;
  static const ConstBigReal _i64_max;
  static const ConstBigReal _ui64_max;
#ifdef IS64BIT
  static const ConstBigReal _i128_min;
  static const ConstBigReal _i128_max;
  static const ConstBigReal _ui128_max;
#endif // IS64BIT
  static const ConstBigReal _flt_min;
  static const ConstBigReal _flt_max;
  static const ConstBigReal _dbl_min;
  static const ConstBigReal _dbl_max;
  static const ConstBigReal _dbl80_min;
  static const ConstBigReal _dbl80_max;
};

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
  BigRealStream(streamsize precision = 6, streamsize width = 0, int flags = 0) : StrStream(precision, width, flags) {
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
private:
  DECLARECLASSNAME;

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
private:
  DECLARECLASSNAME;
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
#ifdef IS64BIT
  BigInt(const _int128          &x, DigitPool *digitPool = NULL) : BigReal(x, digitPool) {
  }
  BigInt(const _uint128         &x, DigitPool *digitPool = NULL) : BigReal(x, digitPool) {
  }
#endif // IS64BIT

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

#define BIGREAL_0     ConstDigitPool::getZero()
#define BIGREAL_1     ConstDigitPool::getOne()
#define BIGREAL_2     ConstDigitPool::getTwo()
#define BIGREAL_HALF  ConstDigitPool::get05()
Real getReal(const BigReal &x);

class ConstBigInt : public BigInt {
private:
  DECLARECLASSNAME;
public:
  explicit ConstBigInt(const BigReal &x) : BigInt(x, REQUSETCONSTPOOL) {
    RELEASECONSTPOOL;
  };
  ConstBigInt(int                    x) : BigInt(x, REQUSETCONSTPOOL) {
    RELEASECONSTPOOL;
  }
  ConstBigInt(unsigned int           x) : BigInt(x, REQUSETCONSTPOOL) {
    RELEASECONSTPOOL;
  }
  ConstBigInt(long                   x) : BigInt(x, REQUSETCONSTPOOL) {
    RELEASECONSTPOOL;
  }
  ConstBigInt(unsigned long          x) : BigInt(x, REQUSETCONSTPOOL) {
    RELEASECONSTPOOL;
  }
  ConstBigInt(__int64                x) : BigInt(x, REQUSETCONSTPOOL) {
    RELEASECONSTPOOL;
  }
  ConstBigInt(unsigned __int64       x) : BigInt(x, REQUSETCONSTPOOL) {
    RELEASECONSTPOOL;
  }
#ifdef IS64BIT
  ConstBigInt(const _int128          &x) : BigInt(x, REQUSETCONSTPOOL) {
    RELEASECONSTPOOL;
  }
  ConstBigInt(const _uint128         &x) : BigInt(x, REQUSETCONSTPOOL) {
    RELEASECONSTPOOL;
  }
#endif // IS64BIT
  explicit ConstBigInt(const String &s) : BigInt(s, REQUSETCONSTPOOL) {
    RELEASECONSTPOOL;
  }
  explicit ConstBigInt(const TCHAR  *s) : BigInt(s, REQUSETCONSTPOOL) {
    RELEASECONSTPOOL;
  }


#ifdef UNICODE
  explicit ConstBigInt(const char   *s) : BigInt(s, REQUSETCONSTPOOL) {
    RELEASECONSTPOOL;
  }
#endif // UNICODE
  ~ConstBigInt() {
    REQUSETCONSTPOOL;
    setToZero();
    RELEASECONSTPOOL;
  }
};

#undef REQUSETCONSTPOOL
#undef RELEASECONSTPOOL

class BigRational {
private:
  DECLARECLASSNAME;
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

String      toString(const BigReal &n, streamsize precision=20, streamsize width=0, int flags=0);
BigReal     inputBigReal( DigitPool &digitPool, _In_z_ _Printf_format_string_ const TCHAR *format, ...);
BigInt      inputBigInt(  DigitPool &digitPool, _In_z_ _Printf_format_string_ const TCHAR *format, ...);
BigRational inputRational(DigitPool &digitPool, _In_z_ _Printf_format_string_ const TCHAR *format, ...);

class BigRealException : public Exception {
public:
  BigRealException(const TCHAR *text) : Exception(text) {
  };
};

BigInt randomInteger(size_t digits, Random *rnd = NULL, DigitPool *digitPool = NULL);

class RandomBigReal : public Random {
public:
  BigReal  nextBigReal( size_t digits, DigitPool *digitPool = NULL);   // Return uniform distributed random number between 0 (incl) and 1 (excl)
                                                                    // with digits decimal digits. If digitPool == NULL, use DEFAULT_DIGITPOOL
  BigReal  nextBigReal(const BigReal &low, const BigReal &high, size_t digits, DigitPool *digitPool = NULL);
                                                                    // Return uniform distributed random number between low (incl) and high (excl)
                                                                    // with digits decimal digits. If digitPool == NULL, use low.getDigitPool()
  BigInt nextInteger(size_t digits, DigitPool *digitPool = NULL);      // Return uniform distributed random BigInt in range [0..10^digits[
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

BigReal rRound(const BigReal &x,    size_t digits);
BigReal rSum(  const BigReal &x,    const BigReal &y, size_t digits, DigitPool *digitPool = NULL);
BigReal rDif(  const BigReal &x,    const BigReal &y, size_t digits, DigitPool *digitPool = NULL);
BigReal rProd( const BigReal &x,    const BigReal &y, size_t digits, DigitPool *digitPool = NULL);
BigReal rQuot( const BigReal &x,    const BigReal &y, size_t digits, DigitPool *digitPool = NULL);
BigReal rSqrt( const BigReal &x,    size_t digits);
BigReal rExp(  const BigReal &x,    size_t digits);
BigReal rLn(   const BigReal &x,    size_t digits);
BigReal rLog10(const BigReal &x,    size_t digits);
BigReal rLog(  const BigReal &base, const BigReal &x, size_t digits); // Logarithm(x) for the specified base. (ln(x)/ln(base))
BigReal rPow(  const BigReal &x,    const BigReal &y, size_t digits); // x^y
BigReal rRoot( const BigReal &x,    const BigReal &y, size_t digits);
BigReal rSin(  const BigReal &x,    size_t digits);
BigReal rCos(  const BigReal &x,    size_t digits);
BigReal rTan(  const BigReal &x,    size_t digits);
BigReal rCot(  const BigReal &x,    size_t digits);
BigReal rAsin( const BigReal &x,    size_t digits);
BigReal rAcos( const BigReal &x,    size_t digits);
BigReal rAtan( const BigReal &x,    size_t digits);
BigReal rAcot( const BigReal &x,    size_t digits);

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
  const BigReal            *m_x, *m_y, *m_f;
  BigReal                  *m_result;
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
  virtual void allocateNewResources(size_t count) {
    int id = (int)size();
    for(size_t i = 0; i < count; i++, id++) {
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
    for(size_t i = 0; i < size(); i++) {
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
  void allocateNewResources(size_t count) {
    const int oldSize = (int)size();
    ResourcePool<T>::allocateNewResources(count);
    for(int i = oldSize; i < (int)size(); i++) {
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

  static inline int getMaxActiveThreads() {
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

void traceRecursion(int level, _In_z_ _Printf_format_string_ const TCHAR *format,...);

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
BigReal modulusOperator64( const BigReal &x, const BigReal &y); // old operator%(const BigReal &x, const BigReal &y);
BigReal modulusOperator128(const BigReal &x, const BigReal &y); // old operator%(const BigReal &x, const BigReal &y);

#ifdef LONGDOUBLE
#pragma comment(lib, LIB_VERSION "LDBigReal.lib")
#else
#pragma comment(lib,  LIB_VERSION "BigReal.lib")
#endif

