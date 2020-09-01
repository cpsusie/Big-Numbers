#pragma once

#include "BigRealConfig.h"
#include <ResourcePoolTemplate.h>

// The definition of Digit should NOT be changed!!! Se asm-code in ShortProduct*.cpp/BigRealShortProductx64.asm
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
  DigitPage(const DigitPage &src); // not implemented
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
  BigRealResource(UINT id) : IdentifiedResource(id) {
  }
};

class DigitPool : public BigRealResource, public AbstractVectorAllocator<BigReal> {
  friend class BigRealResourcePool;
private:
  // Holds the total number of allocated DigitPages in all digitpools
  static std::atomic<UINT> s_totalAllocatedPageCount;
  const String             m_origName;
  String                   m_name;
  // Holds the number of allocated pages in this digitPool, (number of allocated digits = m_allocatedPageCount*DIGITPAGESIZE
  size_t                   m_allocatedPageCount;
  // Pointer to the first page in a linked list of pages
  DigitPage               *m_firstPage;
  // Pointer to first free digit. see newDigit/deleteDigits
  Digit                   *m_freeDigits;
  // If m_continueCalculation is false, when a thread enters shortProductNoZeroCheck, it will call throwBigRealException("Operation was cancelled")
  bool                     m_continueCalculation;
  // All BigReals using this digitPool, will have the m_flags-member initialized to this value
  BYTE                     m_initFlags;
  // How many BigReals uses this digitpool (including members allocated below)
  std::atomic<UINT>        m_refCount;
  UINT                     m_refCountOnFetch;

#if COUNT_DIGITPOOLFETCHDIGIT == 2
  UINT                     m_requestCount;
#endif // COUNT_DIGITPOOLFETCHDIGIT

  // Some frequently used constants. exist in every pool. All will have m_flag-bit BR_MUTABLE=0
  // = 0
  BigInt                  *m_0;
  // = 1
  BigInt                  *m_1;
  // = 2
  BigInt                  *m_2;
  // = 0.5
  BigReal                 *m_05;
  BigReal                 *m_nan, *m_pinf, *m_ninf; // (quiet)nan, +/-infinity

  void allocatePage();
  DigitPool(const DigitPool &src);            // not implemented
  DigitPool &operator=(const DigitPool &src); // not implemented

#if defined(USE_FETCHDIGITLIST)
#if defined(CHECK_DIGITPOOLINVARIANT)
  // The free-list of digits, maintained by DigitPool
  //                +------+   +------+   +------+   +------+   +------+
  // m_freeDigits-->| next |-->| next |-->| next |-->| next |-->| next |-->nil
  //    Undefined<--| prev |<--| prev |<--| prev |<--| prev |<--| prev |
  //                +------+   +------+   +------+   +------+   +------+
  //
  void checkInvariant(const TCHAR *method, bool enter) const;
  static void checkIsDoubleLinkedList(const TCHAR *method, const Digit *head, size_t expectedLength);
#endif // CHECK_DIGITPOOLINVARIANT
#endif

public:

  // InitialDigitcount in BIGREALBASE-digits
  DigitPool(int id, const String &name, size_t initialDigitcount = 0);
  virtual ~DigitPool();

  inline UINT getId() const {
    return getResourceId();
  }
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
  // Default implementation does NOT guarantee exclusive access to freelist, and will, if shared between threads,
  // cause data-race...sooner or later
  // Overwritten in DigitPoolWithLock, which solves this problem
  virtual Digit *fetchDigit();

#if defined(USE_FETCHDIGITLIST)
  /** See comment for newDigit
   Returns a double linked list, L, with count digits. all undefined
   FetchDigitList(5) will return L which looks like following:
       +------+   +------+   +------+   +------+   +------+
   L-->| next |-->| next |-->| next |-->| next |-->| next |--->undefined
     +-| prev |<--| prev |<--| prev |<--| prev |<--| prev |<-+
     | +------+   +------+   +------+   +------+   +------+  |
     +-------------------------------------------------------+
  */
  virtual Digit *fetchDigitList(size_t count);
  // Returns a double linked list, L, with count digits.all values initialized to n
  virtual Digit *fetchDigitList(size_t count, BRDigitType n);
#endif // USE_FETCHDIGITLIST

  // See comment for newDigit
  // Assume first points to head of double linked list of digits which ends with the digit pointed to by last

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
  inline void resetName() {
    m_name = m_origName;
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
  // Allocates a vector (C-array) of BigReals, all using this digitPool
  // To deallocate array a, use delete[] a;
  // override virtual VectorAllocator::allocVector
  BigReal *allocVector(size_t count);
};

class DigitPoolWithLock : public DigitPool {
private:
  FastSemaphore m_lock;
public:
  DigitPoolWithLock(int id, const String &name)
    : DigitPool(id, name)
  {
  }

  Digit *fetchDigit() override {
    m_lock.wait();
    Digit *d = __super::fetchDigit();
    m_lock.notify();
    return d;
  }

#if defined(USE_FETCHDIGITLIST)
  // Return head of double linked list with count digits with undefined values. prev-pointer of head points to last digit in list
  Digit *fetchDigitList(size_t count) override {
    m_lock.wait();
    Digit *d = __super::fetchDigitList(count);
    m_lock.notify();
    return d;
  }
  // Return head of double linked list with count digits = n. prev-pointer of head points to last digit in list
  Digit *fetchDigitList(size_t count, BRDigitType n) override {
    m_lock.wait();
    Digit *d = __super::fetchDigitList(count, n);
    m_lock.notify();
    return d;
  }
#endif // USE_FETCHDIGITLIST

  void deleteDigits(Digit *first, Digit *last) override {
    m_lock.wait();
    __super::deleteDigits(first, last);
    m_lock.notify();
  }
  bool isWithLock() const override {
    return true;
  }
};
