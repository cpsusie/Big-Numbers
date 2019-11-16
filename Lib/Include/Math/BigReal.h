#pragma once

#include <MyUtil.h>
#include <Thread.h>
#include <FastSemaphore.h>
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
#include <MyAssert.h>

// Define this to have 2 different version of getDecimalDigitCount64(UINT64 n).
// Measures of time show that getDecimalDigitCount64 is 5 times faster than getDecimalDigitCount64Loop
//#define HAS_LOOP_DIGITCOUNT

// use, if you want to have backup-check for correct multiplication, running in separate
// process built with x86 code, Serverprocess is called "MultiplicationServer", and protocol to this is quit simple:
// write 3 BigReals, x y f as text. Serverprogram will return x*y (as text) with an error no greater than |f|
// Can be repeated as long as you want
// #define USE_X32SERVERCHECK

// If this is defined, releaseDigitPool will check, that all used digits in the pool are released
//#define CHECKALLDIGITS_RELEASED

// If this is defined, product wil trace recursive calls for when multiplying long BigReals
// which will slow down the program (testBigReal) by a factor 20!!
//#define TRACEPRODUCTRECURSION

// If this is defined, load/save/the number of requests and cache-hits to pow2Cache will be logged
// and written to debugLog when program exit
//#define TRACEPOW2CACHE

// If this is defined, the number of requests and cache-hits to pow2Cache will be logged
// and written to debugLog when program exit
//#define TRACEPOW2CACHEHIT

#define BIGREAL_LOG10BASEx86                        8
#define BIGREAL_POW10TABLESIZEx86                  10
#define BIGREAL_NONNORMALx86               -900000000
#define BIGREAL_MAXEXPOx86                   99999999
#define BIGREAL_MINEXPOx86                  -99999999

typedef ULONG          BRDigitTypex86;
typedef UINT64         BR2DigitTypex86;
typedef long           BRExpoTypex86;
typedef long           BRDigitDiffTypex86;


#define BIGREAL_LOG10BASEx64                       18
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

#define BIGREAL_LOG10BASE  BIGREAL_LOG10BASEx86

#define BIGREAL_NONNORMAL  BIGREAL_NONNORMALx86
#define BIGREAL_MAXEXPO    BIGREAL_MAXEXPOx86
#define BIGREAL_MINEXPO    BIGREAL_MINEXPOx86

#define BRDigitType        BRDigitTypex86
#define BR2DigitType       BR2DigitTypex86
#define BRExpoType         BRExpoTypex86
#define BRDigitDiffType    BRDigitDiffTypex86

#else // IS64BIT

#define BIGREAL_LOG10BASE  BIGREAL_LOG10BASEx64

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
#define BIGREAL_INFLOW   1 // _isinf() is true, +/- infinite depending on BR_NEG bit in m_flags
#define BIGREAL_QNANLOW  2 // _isnan() is true

#define SP_OPT_NONE      0
#define SP_OPT_BY_FPU    1
#define SP_OPT_BY_FPU2   2
#define SP_OPT_BY_REG32  3
#define SP_OPT_BY_REG64  4

// Define SP_OPT_METHOD to one of these, to select the desired optimisation of shortProduct.
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

#if   BIGREAL_LOG10BASE == 2
#define SQRT_BIGREALBASE 10
#elif BIGREAL_LOG10BASE == 4
#define SQRT_BIGREALBASE 100
#elif BIGREAL_LOG10BASE == 6
#define SQRT_BIGREALBASE 1000
#elif BIGREAL_LOG10BASE == 8
#define SQRT_BIGREALBASE 10000
#else // BIGREAL_LOG10BASE
#error Illegal BIGREAL_LOG10BASE. Must be in the set {2,4,6 or 8}
#endif // BIGREAL_LOG10BASE
#define BIGREALBASE (SQRT_BIGREALBASE*SQRT_BIGREALBASE)

#elif(SP_OPT_METHOD == SP_OPT_BY_FPU)

#if   BIGREAL_LOG10BASE == 1
#define BIGREALBASE 10
#elif BIGREAL_LOG10BASE == 2
#define BIGREALBASE 100
#elif BIGREAL_LOG10BASE == 3
#define BIGREALBASE 1000
#elif BIGREAL_LOG10BASE == 4
#define BIGREALBASE 10000
#elif BIGREAL_LOG10BASE == 5
#define BIGREALBASE 100000
#elif BIGREAL_LOG10BASE == 6
#define BIGREALBASE 1000000
#elif BIGREAL_LOG10BASE == 7
#define BIGREALBASE 10000000
#elif BIGREAL_LOG10BASE == 8
#define BIGREALBASE 100000000
#else
#error Illegal BIGREAL_LOG10BASE. Must be in the interval [1-8]
#endif // BIGREAL_LOG10BASE
// Cannot use 1e9 because of overflow in function shortProductNoZeroCheck, when accumulating more than 18 products, which will
// occur when splitLength > 18. And a splitlength of 18 is far to small to give product a boost

#elif(SP_OPT_METHOD == SP_OPT_BY_REG32 || SP_OPT_METHOD == SP_OPT_BY_FPU2)

#if BIGREAL_LOG10BASE == 8
// this is the only valid BIGREALBASE for these optimizations
#define BIGREALBASE 100000000
#else
#error Illegal BIGREAL_LOG10BASE. Must be 8.
#endif // BIGREAL_LOG10BASE == 8

#elif (SP_OPT_METHOD == SP_OPT_BY_REG64)

#if BIGREAL_LOG10BASE == 18
// this is the only valid BIGREALBASE for SP_OPT_BY_REG64
#define BIGREALBASE 1000000000000000000
#else
#error Illegal BIGREAL_LOG10BASE. Must be 18.
#endif // BIGREAL_LOG10BASE == 18

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
private:
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
class BigRational;
class ConstBigInt;
class ConstBigReal;

class BigRealResource : public IdentifiedResource {
public:
  BigRealResource(int id) : IdentifiedResource(id) {
  }
};

class DigitPool : public BigRealResource {
private:
  // Holds the total number of allocated DigitPages in all digitpools
  static std::atomic<UINT>   s_totalAllocatedPageCount;
  static bool                s_dumpCountWhenDestroyed;
  String                     m_name;
  // Holds the number of allocated pages in this digitPool, (number of allocated digits = m_allocatedPageCount*DIGITPAGESIZE
  size_t                     m_allocatedPageCount;
  // Pointer to the first page in a linked list of pages
  DigitPage                 *m_firstPage;
  // Pointer to first free digit. see newDigit/deleteDigits
  Digit                     *m_freeDigits;
  // If m_continueCalculation is false, when a thread enters shortProductNoNormalCheck, it will call throwBigRealException("Operation was cancelled")
  bool                       m_continueCalculation;
  // All BigReals using this digitPool, will have the m_flags-member initialized to this value
  BYTE                       m_initFlags;
  // How many BigReal is using this digitpool (including members allocated below)
  std::atomic<UINT>          m_refCount;
  UINT                       m_refCountOnFetch;

  // Some frequently used constants. exist in every pool
  // = 0
  BigInt               *m_0;
  // = 1
  BigInt               *m_1;
  // = 2
  BigInt               *m_2;
  // = 0.5
  BigReal              *m_05;
  BigReal              *m_nan, *m_pinf, *m_ninf; // (quiet)nan, +/-infinity

  void allocatePage();
  DigitPool(const DigitPool &src);            // not implemented
  DigitPool &operator=(const DigitPool &src); // not implemented
public:

  // InitialDigitcount in BIGREALBASE-digits
  DigitPool(int id, const String &name, size_t intialDigitcount = 0);
  virtual ~DigitPool();

  // Set name. Return old name
  String setName(const String &name) {
    const String old = m_name;
    m_name = name;
    return old;
  }
  const String &getName() const {
    return m_name;
  }
  void setInitFlags(BYTE flags) {
    m_initFlags = flags;
  }
  inline BYTE getInitFlags() const {
    return m_initFlags;
  }
  static void setDumpWhenDestroyed(bool dump) {
    s_dumpCountWhenDestroyed = dump;
  }
  // Default implementation does NOT guarantee exclusive access to freelist , and will, if shared between threads,
  // cause data-race...sooner or later
  // Overwritten in DigitPoolWithLock, which solves this problem
  virtual Digit *fetchDigit();
  // See comment for newDigit
  virtual Digit *fetchDigitList(size_t count);
  // See comment for newDigit
  virtual void deleteDigits(Digit *first, Digit *last);
  virtual bool isWithLock() const {
    return false;
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
    return s_totalAllocatedPageCount * DIGITPAGESIZE;
  }
  inline size_t getPageCount() const {
    return m_allocatedPageCount;
  }
  // Returns the numer of digits allocated in this digitpool
  inline size_t getAllocatedDigitCount() const {
    return getPageCount() * DIGITPAGESIZE;
  }
  size_t getFreeDigitCount() const;
  size_t getUsedDigitCount() const;

  // Call this will cause the thread, using this digitPool to throw an exception ("Operaton was cancelled") when it enters
  // shortProductNoNormalCheck
  inline void terminatePoolCalculation() {
    m_continueCalculation = false;
  }
  inline bool continueCalculation() const {
    return m_continueCalculation;
  }
  // This will be called automatic when a digitpool is released.
  inline void resetPoolCalculation() {
    m_continueCalculation = true;
  }
  inline void incRefCount() {
    m_refCount++;
  }
  inline void decRefCount() {
    m_refCount--;
  }
  // Return number of instances of BigReals, referencing this, including own (const) members
  inline UINT getRefCount() const {
    return m_refCount;
  }
  inline UINT getRefCountOnFetch() const {
    return m_refCountOnFetch;
  }
  inline void saveRefCount() {
    m_refCountOnFetch = m_refCount;
  }
};

class DigitPoolWithLock : public DigitPool {
private:
  FastSemaphore m_lock;
public:
  DigitPoolWithLock(int id, const String &name) : DigitPool(id, name) {
  }

  Digit *fetchDigit() {
    m_lock.wait();
    Digit *d = __super::fetchDigit();
    m_lock.notify();
    return d;
  }
  // Return head of double linked list with count digits. prev-pointer of head points to last digit in list
  Digit *fetchDigitList(size_t count) {
    m_lock.wait();
    Digit *d = __super::fetchDigitList(count);
    m_lock.notify();
    return d;
  }
  void deleteDigits(Digit *first, Digit *last) {
    m_lock.wait();
    __super::deleteDigits(first, last);
    m_lock.notify();
  }
  bool isWithLock() const {
    return true;
  }
};

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
void throwNotValidException(                       TCHAR const * const function, _In_z_ _Printf_format_string_ const TCHAR *format, ...);
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

#define BR_NEG       0x01
#define BR_INITDONE  0x02
#define BR_MUTABLE   0x04

class BigReal {
private:
  static const BRDigitTypex86 s_power10Tablex86[BIGREAL_POW10TABLESIZEx86];
  static const BRDigitTypex64 s_power10Tablex64[BIGREAL_POW10TABLESIZEx64];
  // Defined in shortProduct.cpp. Split factors when length > s_splitLength
  static size_t               s_splitLength;

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

  // Multiplication helperfunctions
  friend class MultiplierThread;
  friend class Pow2Cache;
  friend class BigRealTestClass;
  friend class BigInt;
  friend class BigRational;
  friend class BigRealStream;
  friend class DigitPool;

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
    m_digitPool.incRefCount();
    m_flags = m_digitPool.getInitFlags() & ~BR_INITDONE;
  }
  inline void endInit() {
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
  // Assume p is a digit in digit-list of this.
  // Dont modify m_expo,m_low,m_flags
  void    insertZeroDigitsAfter(  Digit *p, size_t count);
  // Insert count digits=BIGREALBASE-1 after digit p.
  // Assume p is a digit in digit-list of this.
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

  // Assume &x != this && &y != this && *this == 0, x._isnormal() && y._isnormal()
  // Return *this = |x| + |y| with maximal error = f. Do care about sign(f)
  BigReal &addAbs(const BigReal &x, const BigReal &y, const BigReal &f);                          // return this
  // Assume &x != this && this->_isnormal() && x._isnormal()
  // Adds |x| to |this|.
  // Return *this
  BigReal &addAbs(const BigReal &x);                                                              // return this
  // Assume &x != this && this->_isnormal() && x._isnormal() && |x| < |*this|
  // Subtract |x| from |this| with maximal error = f
  BigReal &subAbs(const BigReal &x, const BigReal &f);                                            // return this

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

  // return true if x*y is normal (finite and != 0, (false if x*y is 0, +/-inf or nan)
  static inline bool isNormalProduct(const BigReal &x, const BigReal &y) {
    return x._isnormal() && y._isnormal();
  }

  // Return _FP_ZERO, _FPCLASS_QNAN
  // Assume !x._isnormal() || !y._isnormal() which will result in non-normal product (0 or nan)
  static int getNonNormalProductFpClass(const BigReal &x, const BigReal &y);

  // Return true if x*y is normal (finite and != 0). In this case, *this will not be changed
  // Return false, if x*y is not normal, and *this will be set to the corresponding result (0 or nan)
  inline bool checkIsNormalProduct(const BigReal &x, const BigReal &y) {
    if(isNormalProduct(x, y)) return true;
    setToNonNormalFpClass(getNonNormalProductFpClass(x, y));
    return false;
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
  // Return *this. Assume x._isnormal() && y._isnormal()
  BigReal &shortProductNoNormalCheck(        const BigReal &x, const BigReal &y, BRExpoType fexpo);
  // Set *this = x*y, with the specified number of loops, each loop, i, calculates
  // sum(y[j]*x[i-j]), j=[0..i], i=[0..loopCount], digits indexed [0..length-1], from head
  // Return *this. Assume x._isnormal() && y._isnormal() && (loopCount > 0)
  BigReal &shortProductNoZeroCheck(          const BigReal &x, const BigReal &y, UINT loopCount);
  // Set result = x*y, with |error| <= |f|
  // Return &result. Assume x._isnormal() && y._isnormal() && f._isfinite()
  static BigReal &product(  BigReal &result, const BigReal &x, const BigReal &y, const BigReal &f,              int level);
  // Set result = x*y, with |error| <= |f|
  // Return &result. Assume x._isnormal() && y._isnormal() && f._isfinite() && (x.getLength() >= y.getLength())
  static BigReal &productMT(BigReal &result, const BigReal &x, const BigReal &y, const BigReal &f, intptr_t  w, int level);
  // Assume this->_isnormal() && a.isZero() && b.isZero() && f._isfinite(). Dont care about sign(f)
  void    split(BigReal &a, BigReal &b, size_t n, const BigReal &f) const;
  // Assume src._isnormal() && length <= src.getLength() && m_first == m_last == NULL
  // Modify only m_first and m_last of this
  // Return *this
  BigReal &copyDigits(   const BigReal &src, size_t length);
  // Assume !_isnormal() && src._isnormal()
  // Return *this
  BigReal &copyAllDigits(const BigReal &src);
  // Copy specified number of decimal digits, truncating result. return *this
  BigReal &copyrTrunc(  const BigReal &src, size_t digits);
  inline BigReal &setSignByProductRule(const BigReal &x, const BigReal &y) {
    return setNegative(x.isNegative() != y.isNegative());
  }

  // Cut *this (assumed != 0) to the specified number of significant decimal digits, truncation toward zero
  // Assume _isnormal() && digits > 0 (digits is decimal digits).
  // Return *this
  BigReal &rTrunc(size_t digits);
  // Cut *this (assumed != 0) to the specified number of significant decimal digits, rounding
  // Assume isnormal() && digits > 0 (digits is decimal digits).
  // Return *this
  BigReal &rRound(size_t digits);

  // Division helperfunctions.

  static void validateQuotRemainderArguments(const TCHAR *method, const BigReal &x, const BigReal &y, const BigReal *quotient, const BigReal *remainder);

  // Return true if x/y is normal (finite and != 0, (false if x/y is 0, +/-inf or nan)
  static bool inline isNormalQuotient(const BigReal &x, const BigReal &y) {
    return y._isnormal() && x._isnormal();
  }

  // Return _FP_ZERO, _FPCLASS_PINF, _FPCLASS_NINF, _FPCLASS_QNAN
  // Assume !x._isnormal() || !y._isnormal() which will result in non-normal quotient (0,+/-inf or nan)
  static int getNonNormalQuotientFpClass(const BigReal &x, const BigReal &y);

  // Return true if x/y is normal (finite and != 0), (false if x/y is 0, +/-inf or nan)
  // If x/y is not normal, quotient and remainder will recieve the non-normal value (if not null)
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
  // Assume y._isnormal(). *this = approximately x/y
  // Return *this
  BigReal &approxQuot32(     const BigReal &x, const BigReal           &y);
  // Assume y._isnormal(). *this = approximately x/y
  // Return *this
  BigReal &approxQuot64(     const BigReal &x, const BigReal           &y);
  // Assume y != 0.        *this = approximately x/e(y,scale)
  // Return *this
  BigReal &approxQuot32Abs(  const BigReal &x, ULONG                    y, BRExpoType scale);
  // Assume y != 0.        *this = approximately x/e(y,scale)
  // Return *this
  BigReal &approxQuot64Abs(  const BigReal &x, const UINT64            &y, BRExpoType scale);
#ifdef IS64BIT
  // Assume y._isnormal(). *this = approximately x/y
  // Return *this
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
  // Assume this->_isnormal()
  Double80 getDouble80NoLimitCheck() const;

protected:
  inline void releaseDigits() { // should only be called from destructor
    if(m_first) {
      deleteDigits(m_first, m_last);
      m_first = NULL;
    }
  }

#define DEFAULT_DIGITPOOL BigReal::s_defaultDigitPool
#define CONST_DIGITPOOL   BigReal::s_constDigitPool

#define _INITDIGITPOOL(usePoolIfNull) m_digitPool(digitPool?(*digitPool):(usePoolIfNull))
#define _INITPOOLTODEFAULTIFNULL()    _INITDIGITPOOL(*DEFAULT_DIGITPOOL)
#define _SELECTDIGITPOOL(x)           DigitPool *pool = digitPool?digitPool:(x).getDigitPool()

  // Only ConstBigReal and ConstBigInt may be attached to s_constDigitPool
  virtual bool allowConstDigitPool() const {
    return false;
  }
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
  explicit inline BigReal(ByteInputStream &s, DigitPool *digitPool = NULL) : _INITPOOLTODEFAULTIFNULL() {
    startInit();
    initToZero();
    load(s);
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
  // Do *this *= pow(10,exp). (Fast) Check for overflow/underflow. Trim leading and trailling zeroes if necessessary
  // If force is true, there will be no call to CHECKISMUTABLE
  // Return *this
  BigReal &multPow10(BRExpoType exp, bool force=false);
  // Fast version of *this *= 2
  BigReal &multiply2();
  // Fast version of *this /= 2
  BigReal &divide2();

  // x+y with |error| <= f. Result.digitPool = x.digitPool, or digitPool if specified
  friend BigReal  sum(             const BigReal &x, const BigReal &y, const BigReal  &f, DigitPool *digitPool = NULL);
  // x-y with |error| <= f. Result.digitPool = x.digitPool, or digitPool if specified
  friend BigReal  dif(             const BigReal &x, const BigReal &y, const BigReal  &f, DigitPool *digitPool = NULL);
  // x*y with |error| <= f. Result.digitPool = x.digitPool, or digitPool if specified
  friend BigReal  prod(            const BigReal &x, const BigReal &y, const BigReal  &f, DigitPool *digitPool = NULL);
  // x/y with |error| <= f. Result.digitPool = x.digitPool, or digitPool if specified
  friend BigReal  quot(            const BigReal &x, const BigReal &y, const BigReal  &f, DigitPool *digitPool = NULL);
  // x/y with |error| <= f. Result.digitPool = x.digitPool, or digitPool if specified
  friend BigReal  fmod(            const BigReal &x, const BigReal &y                   , DigitPool *digitPool = NULL);
  // Integer division, Result.digitPool = x.digitPool, or digitPool if specified
  friend BigInt   quot(            const BigInt  &x, const BigInt  &y                   , DigitPool *digitPool = NULL);
  // Integer remainder, Result.digitPool = x.digitPool, or digitPool if specified
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

  // Assume x._isfinite() && y._isfinite(). Return sign(x-y) (=+/-1,0=
  static int compare(         const BigReal &x,  const BigReal &y);
  // Assume x._isfinite() && y._isfinite(). Return compare(|x|,|y|). (Faster than compare(fabs(x),fabs(y)))
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
    if(_isnormal() || _isinf()) { m_flags ^= BR_NEG; }
    return *this;
  }
  // call CHECKISMUTABLE(this)
  inline BigReal &setNegative(bool negative) {
    CHECKISMUTABLE(*this);
    if(negative) {
      m_flags |= BR_NEG;
    } else {
      m_flags &= ~BR_NEG;
    }
    return *this;
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
  // Assume x._isfinite(). Return (x == 0) ? 0 : approximately floor(log2(|x|))
  friend BRExpoType getExpo2(   const BigReal  &x);
  // return floor(log10(n))+1
  // Assume n = [1..1e8[
  static int     getDecimalDigitCount(BRDigitTypex86 n);
  // return floor(log10(n))+1
  // Assume n = [1..1e19[
  static int     getDecimalDigitCount(BRDigitTypex64 n);

  // Return 0 if n == 0, else max(p)|n % (10^p) = 0, p=[0..LOG10BASEx86/x64]
  template<class INTTYPE> static int getTrailingZeroCount(INTTYPE n) {
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
  // Return p if n = 10^p for p = [0..9]. else return -1.
  static int     isPow10(BRDigitTypex86 n);
  // Return p if n = 10^p for p = [0..19]. else return -1.
  static int     isPow10(BRDigitTypex64 n);
  // true if |x| = 10^p for p = [BIGREAL_MINEXPO..BIGREAL_MAXEXPO]
  static bool    isPow10(const BigReal &x);

  // Absolute value of x (=|x|)
  // Result.digitPool = x.digitPool
  friend BigReal fabs(     const BigReal &x                   , DigitPool *digitPool = NULL);
  // biggest integer <= x
  friend BigInt  floor(    const BigReal &x                   , DigitPool *digitPool = NULL);
  // smallest integer >= x
  friend BigInt  ceil(     const BigReal &x                   , DigitPool *digitPool = NULL);
  // sign(x) * (|x| - floor(|x|))
  friend BigReal fraction( const BigReal &x                   , DigitPool *digitPool = NULL);
  // sign(x) * floor(|x|*10^prec+0.5)/10^prec
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
  inline int getPoolId() const {
    return m_digitPool.getId();
  }
  ULONG hashCode() const;

  void save(ByteOutputStream &s) const;
  void load(ByteInputStream  &s);

  friend Packer &operator<<(Packer &p, const BigReal &v);
  friend Packer &operator>>(Packer &p,       BigReal &v);

  static UINT getMaxSplitLength();

  // Checks that this is a consistent BigReal with all the various invariants satisfied.
  // Throws an excpeption if not with a descripion of what is wrong. For debugging
  virtual void assertIsValid() const;

  // Return uniform distributed random BigReal between 0 (incl) and 1 (excl) with at most maxDigits decimal digits.
  // If maxDigits == 0, 0 will be returned
  // Digits generated with rnd
  // If digitPool == NULL, use DEFAULT_DIGITPOOL
  friend BigReal randBigReal(size_t maxDigits, RandomGenerator &rnd = *RandomGenerator::s_stdGenerator, DigitPool *digitPool = NULL);

  // Return uniform distributed random BigReal in [from;to] with at most maxDigits decimal digits.
  // Digits generated with rnd
  // If digitPool == NULL, use from.getDigitPool()
  friend BigReal  randBigReal(const BigReal &from, const BigReal &to, size_t maxDigits, RandomGenerator &rnd = *RandomGenerator::s_stdGenerator, DigitPool *digitPool = NULL);

  // ------------------------------------------------------------------------------------------

  friend long        getLong(    const BigReal &v);
  friend ULONG       getUlong(   const BigReal &v);
  friend inline int  getInt(     const BigReal &v) {
    return (int)getLong(v);
  }
  friend inline UINT getUint(    const BigReal &v) {
    return (UINT)getUlong(v);
  }
  friend INT64       getInt64(   const BigReal &v);
  friend UINT64      getUint64(  const BigReal &v);
  friend _int128     getInt128(  const BigReal &v);
  friend _uint128    getUint128( const BigReal &v);
  friend float       getFloat(   const BigReal &v);
  friend double      getDouble(  const BigReal &v);
  friend Double80    getDouble80(const BigReal &v);

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
  static const BR2DigitType s_BIGREALBASEBR2; // BIGREALBASE as BR2DigitType (__UINT64 or _uint128)
  static const ConstBigReal _BR_QNAN;    // non-signaling NaN (quiet NaN)
  static const ConstBigReal _BR_PINF;    // +infinity;
  static const ConstBigReal _BR_NINF;    // -infinity;
};

class ConstBigReal : public BigReal {
protected:
  bool allowConstDigitPool() const {
    return true;
  }

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
  explicit inline ConstBigReal(ByteInputStream &s) : BigReal(s, CONST_DIGITPOOL) {
  }
};

#define APCsum( bias, x, y, digitPool) BigReal::apcSum( #@bias, x, y, digitPool)
#define APCprod(bias, x, y, digitPool) BigReal::apcProd(#@bias, x, y, digitPool)
#define APCquot(bias, x, y, digitPool) BigReal::apcQuot(#@bias, x, y, digitPool)
#define APCpow( bias, x, y, digitPool) BigReal::apcPow( #@bias, x, y, digitPool)

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

int  isInt(    const BigReal &v);
UINT isUint(   const BigReal &v);
bool isInt64(  const BigReal &v);
bool isUint64( const BigReal &v);
bool isInt128( const BigReal &v);
bool isUint128(const BigReal &v);

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

class SynchronizedStringQueue : public SynchronizedQueue<TCHAR*>, public BigRealResource {
private:
  SynchronizedStringQueue(           const SynchronizedStringQueue &src); // not implemented
  SynchronizedStringQueue &operator=(const SynchronizedStringQueue &src); // not implemented
public:
  SynchronizedStringQueue(int id, const String &name) : BigRealResource(id) { // name not used
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
  BigRealThread(int id, const String &name);
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
  MultiplierThread(int id, const String &name);
  UINT run();

  void multiply(BigReal &result, const BigReal &x, const BigReal &y, const BigReal &f, int level);
  DigitPool *getDigitPool() const {
    return m_digitPool;
  }
  SynchronizedStringQueue &getQueue() {
    return *m_resultQueue;
  }
};

template<class T> class AbstractFilteredIterator : public AbstractIterator {
private:
  CompactArray<T>  &m_a;
  BitSet            m_activeSet;
  Iterator<size_t>  m_it;
public:
  AbstractFilteredIterator(const CompactArray<T> &a, const BitSet &set)
    : m_a((CompactArray<T>&)a)
    , m_activeSet(set)
  {
    m_it = m_activeSet.getIterator();
  }
  AbstractIterator *clone() {
    return new AbstractFilteredIterator(m_a, m_activeSet);
  }

  inline bool hasNext() const {
    return m_it.hasNext();
  }

  void *next() {
    if(m_it.hasNext()) {
      noNextElementError(__TFUNCTION__);
    }
    return &m_a[m_it.next()];
  }

  void remove() {
    throwUnsupportedOperationException(__TFUNCTION__);
  }
};

template <class T> class ResourcePool : public CompactArray<T*> {
private:
  const String      m_typeName;
  CompactStack<int> m_freeId;
protected:
  virtual void allocateNewResources(size_t count) {
    int id = (int)size();
    for(size_t i = 0; i < count; i++, id++) {
      m_freeId.push(id);
      T *r = new T(id, format(_T("Resource %s(%d)"),m_typeName.cstr(), id)); TRACE_NEW(r);
      add(r);
    }
  }
public:
  ResourcePool(const String &typeName) : m_typeName(typeName) {
  }
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
  BitSet getActiveIdSet() const {
    return getAllocatedIdSet() - getFreeIdSet();
  }

  Iterator<T*> getAllIterator() const {
    return __super::getIterator();

  }
  Iterator<T*> getActiveIterator() const {
    return Iterator<T*>(new AbstractFilteredIterator<T*>(*this, getActiveIdSet()));
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
  BigRealThreadPool() : ResourcePool(_T("ThreadPool")) {
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
  ResourcePool<DigitPoolWithLock>       m_lockedDigitPool;
  mutable FastSemaphore                 m_gate;
  int                                   m_processorCount;
  int                                   m_activeThreads, m_maxActiveThreads;

  BigRealResourcePool(const BigRealResourcePool &src);            // not implemented
  BigRealResourcePool &operator=(const BigRealResourcePool &src); // not implemented
public:
  BigRealResourcePool();
  ~BigRealResourcePool();
  static MThreadArray &fetchMTThreadArray(  MThreadArray &threads, int count);
  static void          releaseMTThreadArray(MThreadArray &threads);

  static DigitPool    *fetchDigitPool(bool withLock=false, BYTE initFlags = BR_MUTABLE);
  static void          releaseDigitPool(DigitPool *pool);
  // call terminatePoolCalculation() for all DigitPools in use
  static void          terminateAllPoolCalculations();

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
