#include "pch.h"
#include <float.h>
#include <DebugLog.h>

DigitPool::DigitPool(int id, const String &name, size_t intialDigitCount)
: BigRealResource(id)
, m_origName(name)
, m_name(name)
, m_allocatedPageCount(0)
, m_firstPage(NULL)
, m_freeDigits(NULL)
, m_initFlags(BR_MUTABLE)          // so that constants allocated 
, m_refCount(0)
#ifdef COUNT_DIGITPOOLFETCHDIGIT
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
  if(s_dumpCountWhenDestroyed) {
#ifndef COUNT_DIGITPOOLFETCHDIGIT
    debugLog(_T("DigitPool(%3u):%-20s:PageCount:%4d, DigitCount:%10s\n")
            ,getId(), getName().cstr(), count, format1000((INT64)count*DIGITPAGESIZE).cstr());
#else
    debugLog(_T("DigitPool(%3u):%-20s:PageCount:%4d, DigitCount:%10s, RequestCount:%15s\n")
            ,getId(), getName().cstr(), count, format1000((INT64)count*DIGITPAGESIZE).cstr(), format1000(m_requestCount).cstr());
#endif // COUNT_DIGITPOOLFETCHDIGIT
  }
}

Digit *DigitPool::fetchDigit() {
  if(m_freeDigits == NULL) allocatePage();
  Digit *p     = m_freeDigits;
  m_freeDigits = p->next;
#ifdef COUNT_DIGITPOOLFETCHDIGIT
  m_requestCount++;
#endif // COUNT_DIGITPOOLFETCHDIGIT
  return p;
}

Digit *DigitPool::fetchDigitList(size_t count) {
  Digit *head = m_freeDigits, *last = head;
  for(Digit *q = head->next; --count; last=q, q = q->next) {
    if(q == NULL) {
      m_freeDigits = NULL;
      allocatePage();
      last->next = q = m_freeDigits;
    }
    q->prev = last;
  }
  m_freeDigits = last->next;
  last->next = NULL;
  head->prev = last;
#ifdef COUNT_DIGITPOOLFETCHDIGIT
  m_requestCount++;
#endif // COUNT_DIGITPOOLFETCHDIGIT
  return head;
}

void DigitPool::deleteDigits(Digit *first, Digit *last) {
  last->next = m_freeDigits;
  m_freeDigits = first;
}

void DigitPool::allocatePage() {
  m_firstPage  = new DigitPage(m_firstPage, m_freeDigits); TRACE_NEW(m_firstPage);
  m_freeDigits = m_firstPage->m_page;
  m_allocatedPageCount++;
  s_totalAllocatedPageCount++;
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

DigitPage::DigitPage(DigitPage *nextPage, Digit *nextDigit) {
  memset(this, 0, sizeof(DigitPage));
  m_next = nextPage;
  // then link digits together
  Digit *p = &LASTVALUE(m_page);
  p->next = nextDigit;
  while(p-- > m_page) p->next = p+1;
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

BigReal *DigitPool::newBigRealArray(size_t count) {
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
