#include "pch.h"
#include <float.h>
#include <DebugLog.h>

std::atomic<UINT>  DigitPool::s_totalAllocatedPageCount;

DigitPool::DigitPool(int id, const String &name, size_t intialDigitCount)
: BigRealResource(id)
, m_origName(name)
, m_name(name)
, m_allocatedPageCount(0)
, m_firstPage(NULL)
, m_freeDigits(NULL)
, m_initFlags(BR_MUTABLE)          // so that constants allocated
, m_refCount(0)
#if COUNT_DIGITPOOLFETCHDIGIT == 2
, m_requestCount(0)
#endif // COUNT_DIGITPOOLFETCHDIGIT
, m_continueCalculation(true)
{
  while(getAllocatedDigitCount() < intialDigitCount) {
    allocatePage();
  }

  m_0    = new BigInt(0, this);                         TRACE_NEW(m_0    );
  m_1    = new BigInt(1, this);                         TRACE_NEW(m_1    );
  m_2    = new BigInt(2, this);                         TRACE_NEW(m_2    );
  m_05   = new BigReal(e(BigReal(5, this), -1,this));   TRACE_NEW(m_05   ); // Don't use 0.5 here!!!
  m_nan  = new BigReal(this);                           TRACE_NEW(m_nan  );
  m_pinf = new BigReal(this);                           TRACE_NEW(m_pinf );
  m_ninf = new BigReal(this);                           TRACE_NEW(m_ninf );
  m_nan->clrInitDone().setToNan().setInitDone();
  m_pinf->clrInitDone().setToPInf().setInitDone();
  m_ninf->clrInitDone().setToNInf().setInitDone();
  m_1->modifyFlags(   0, BR_MUTABLE);
  m_2->modifyFlags(   0, BR_MUTABLE);
  m_05->modifyFlags(  0, BR_MUTABLE);
  m_nan->modifyFlags( 0, BR_MUTABLE);
  m_pinf->modifyFlags(0, BR_MUTABLE);
  m_ninf->modifyFlags(0, BR_MUTABLE);
}

DigitPool::~DigitPool() {
  const size_t pageCount = m_allocatedPageCount;
  SAFEDELETE(m_ninf);
  SAFEDELETE(m_pinf);
  SAFEDELETE(m_nan );
  SAFEDELETE(m_05  );
  SAFEDELETE(m_2   );
  SAFEDELETE(m_1   );
  SAFEDELETE(m_0   );

  assert(m_refCount == 0);

  int count = 0;
  for(DigitPage *p = m_firstPage, *q = NULL; p; p = q) {
    q = p->m_next;
    SAFEDELETE(p);
    count++;
  }
  assert(count == pageCount);
  if(pageCount) {
    s_totalAllocatedPageCount -= count;
  }
#if COUNT_DIGITPOOLFETCHDIGIT == 1
  debugLog(_T("DigitPool(%3u):%-20s:PageCount:%4d, DigitCount:%10s\n")
          ,getId(), getName().cstr(), count, format1000((INT64)count*DIGITPAGESIZE).cstr());
#elif COUNT_DIGITPOOLFETCHDIGIT == 2
  debugLog(_T("DigitPool(%3u):%-20s:PageCount:%4d, DigitCount:%10s, RequestCount:%15s\n")
          ,getId(), getName().cstr(), count, format1000((INT64)count*DIGITPAGESIZE).cstr(), format1000(m_requestCount).cstr());
#endif // COUNT_DIGITPOOLFETCHDIGIT
}

#ifdef CHECK_DIGITPOOLINVARIANT
#define ENTER checkInvariant(__TFUNCSIG__,true );
#define LEAVE checkInvariant(__TFUNCSIG__,false);
#define SAVECOUNT const size_t _origCount = count;
#define CHECKISLIST(head) checkIsDoubleLinkedList(__TFUNCSIG__, head, _origCount);
#else
#define ENTER
#define LEAVE
#define SAVECOUNT
#define CHECKISLIST(head)
#endif // CHECK_DIGITPOOLINVARIANT

Digit *DigitPool::fetchDigit() {
  ENTER
  if(m_freeDigits == NULL) allocatePage();
  Digit *p     = m_freeDigits;
  m_freeDigits = p->next;
#if COUNT_DIGITPOOLFETCHDIGIT == 2
  m_requestCount++;
#endif // COUNT_DIGITPOOLFETCHDIGIT
  LEAVE
  return p;
}

#ifdef USE_FETCHDIGITLIST
Digit *DigitPool::fetchDigitList(size_t count) {
  ENTER
  SAVECOUNT
  if(m_freeDigits == NULL) allocatePage();
  Digit *head = m_freeDigits, *last = head;
  for(Digit *q = head->next; --count; q = (last=q)->next) {
    if(q == NULL) {
      m_freeDigits = NULL;
      allocatePage();
      (q = last->next = m_freeDigits)->prev = last;
    }
  }
  m_freeDigits = last->next;
  head->prev = last;
#if COUNT_DIGITPOOLFETCHDIGIT == 2
  m_requestCount++;
#endif // COUNT_DIGITPOOLFETCHDIGIT
  CHECKISLIST(head)
  LEAVE
  return head;
}

Digit *DigitPool::fetchDigitList(size_t count, BRDigitType n) {
  ENTER
  SAVECOUNT
  if(m_freeDigits == NULL) allocatePage();
  Digit *head = m_freeDigits, *last = head;
  head->n = n;
  for(Digit *q = head->next; --count; q = (last = q)->next) {
    if(q == NULL) {
      m_freeDigits = NULL;
      allocatePage();
      (q = last->next = m_freeDigits)->prev = last;
    }
    q->n = n;
  }
  m_freeDigits = last->next;
  head->prev = last;
#if COUNT_DIGITPOOLFETCHDIGIT == 2
  m_requestCount++;
#endif // COUNT_DIGITPOOLFETCHDIGIT
  CHECKISLIST(head)
  LEAVE
  return head;
}

#endif // USE_FETCHDIGITLIST

void DigitPool::deleteDigits(Digit *first, Digit *last) {
#ifdef USE_FETCHDIGITLIST
  ENTER
  if(last->next = m_freeDigits)
    m_freeDigits->prev = last;
  m_freeDigits = first;
  LEAVE
#else
  last->next = m_freeDigits;
  m_freeDigits = first;
#endif // USE_FETCHDIGITLIST
}

void DigitPool::allocatePage() {
  ENTER
  m_firstPage  = new DigitPage(m_firstPage, m_freeDigits); TRACE_NEW(m_firstPage);
  m_freeDigits = m_firstPage->m_page;
  m_allocatedPageCount++;
  s_totalAllocatedPageCount++;
  LEAVE
}

size_t DigitPool::getFreeDigitCount() const {
  size_t count = 0;
  for(const Digit *p = m_freeDigits; p; p = p->next) {
    count++;
  }
  return count;
}

size_t DigitPool::getUsedDigitCount() const {
  return getAllocatedDigitCount() - getFreeDigitCount();
}

#ifdef CHECK_DIGITPOOLINVARIANT
void DigitPool::checkInvariant(const TCHAR *method, bool enter) const {
  size_t count = 0;
  for(const Digit *p = m_freeDigits, *last = p?p->prev:NULL; p; last = p, p = p->next, count++) {
    if(p->prev != last) {
      throwException(_T("%s %s:Invariant broken for digitPool %s"), enter?_T("enter"):_T("leave"), method, getName().cstr());
    }
  }
}

void DigitPool::checkIsDoubleLinkedList(const TCHAR *method, const Digit *head, size_t expectedLength) { // static
  size_t count = 1;
  const Digit *last = head->prev;
  for(const Digit *p = head, *q = last; p != last; q = p, p = p->next, count++) {
    if(p->prev != q) {
      throwException(_T("%s:%p not doublelinked list"), method, head);
    }
  }
  if(count != expectedLength) {
    throwException(_T("%s:length=%zu. expectedLength=%zu"), method, count, expectedLength);
  }
}
#endif // CHECK_DIGITPOOLINVARIANT

DigitPage::DigitPage(DigitPage *nextPage, Digit *nextDigit) {
  memset(this, 0, sizeof(DigitPage));
  m_next = nextPage;
  // then link digits together
  Digit *p = &LASTVALUE(m_page);
  if(p->next = nextDigit) {
    nextDigit->prev = p;
  }
  while(p-- > m_page) (p->next = p+1)->prev = p;
}

// Helper class to allocate vectors of BigReals with DigitPool != DEFAULT_DIGITPOOL
class BigReal1 : public BigReal {
public:
  static FastSemaphore s_lock;
  static DigitPool    *s_currentDigitPool; // protected by BigRealResourcePool wait()
  BigReal1() : BigReal(s_currentDigitPool) {
  }
};

FastSemaphore BigReal1::s_lock;
DigitPool    *BigReal1::s_currentDigitPool = NULL;

BigReal *DigitPool::allocVector(size_t count) {
  BigReal1::s_lock.wait();
  try {
    BigReal1::s_currentDigitPool = this;
    BigReal1 *p = new BigReal1[count];
    BigReal1::s_lock.notify();
    return p;
  } catch(...) {
    BigReal1::s_lock.notify();
    throw;
  }
}
