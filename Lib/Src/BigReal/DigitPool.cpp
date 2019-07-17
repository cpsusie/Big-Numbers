#include "pch.h"
#include <float.h>
#include <Semaphore.h>

// the order of declaration is importatnt here.!

#pragma warning(disable : 4073)
#pragma init_seg(lib)

class InitBigReal {
public:
  InitBigReal() {
    Double80::initClass();
  }
};

static InitBigReal initBigReal;

size_t        DigitPool::s_totalDigitCount        = 0;
bool          DigitPool::s_dumpCountWhenDestroyed = false;
DigitPool     DigitPool::s_defaultDigitPool(DEFAULT_DIGITPOOL_ID, 510000);

ConstDigitPool ConstDigitPool::s_instance;

Pow2Cache     BigReal::s_pow2Cache;

const ConstBigReal ConstBigReal::_long_min(LONG_MIN   );
const ConstBigReal ConstBigReal::_long_max(LONG_MAX   );
const ConstBigReal ConstBigReal::_ulong_max(ULONG_MAX );
const ConstBigReal ConstBigReal::_i64_min(_I64_MIN    );
const ConstBigReal ConstBigReal::_i64_max(_I64_MAX    );
const ConstBigReal ConstBigReal::_ui64_max(_UI64_MAX  );
const ConstBigReal ConstBigReal::_i128_min(_I128_MIN  );
const ConstBigReal ConstBigReal::_i128_max(_I128_MAX  );
const ConstBigReal ConstBigReal::_ui128_max(_UI128_MAX);

const ConstBigReal ConstBigReal::_flt_min(FLT_MIN     );
const ConstBigReal ConstBigReal::_flt_max(FLT_MAX     );
const ConstBigReal ConstBigReal::_dbl_min(DBL_MIN     );
const ConstBigReal ConstBigReal::_dbl_max(DBL_MAX     );
const ConstBigReal ConstBigReal::_dbl80_min(DBL80_MIN);
const ConstBigReal ConstBigReal::_dbl80_max(DBL80_MAX);
const ConstBigReal ConstBigReal::_C1third(0.33333333333);

DigitPool::DigitPool(int id, size_t intialDigitCount) : BigRealResource(id) {
  m_firstPage  = NULL;
  m_freeDigits = NULL;
  m_digitCount = 0;

  const size_t wantedTotalDigitCount = m_digitCount + intialDigitCount;
  while(m_digitCount < wantedTotalDigitCount) {
    allocatePage();
  }

  m_zero = new BigInt(0, this);                    TRACE_NEW(m_zero);
  m_one  = new BigInt(1, this);                    TRACE_NEW(m_one );
  m_two  = new BigInt(2, this);                    TRACE_NEW(m_two );
  m_half = new BigReal(e(BigReal(5, this), -1));   TRACE_NEW(m_half);
  m_nan  = new BigReal(this); m_nan->setToNan();   TRACE_NEW(m_nan );
  m_pinf = new BigReal(this); m_pinf->setToInf();  TRACE_NEW(m_pinf );
  m_ninf = new BigReal(this); m_ninf->setToInf(); m_ninf->changeSign(); TRACE_NEW(m_ninf );
}

DigitPool::~DigitPool() {
  const size_t digitCount = m_digitCount;
  SAFEDELETE(m_ninf);
  SAFEDELETE(m_pinf);
  SAFEDELETE(m_nan );
  SAFEDELETE(m_half);
  SAFEDELETE(m_two );
  SAFEDELETE(m_one );
  SAFEDELETE(m_zero);

  int pageCount = 0;
  for(DigitPage *p = m_firstPage, *q = NULL; p; p = q) {
    q = p->m_next;
    SAFEDELETE(p);
    pageCount++;
  }
  if(pageCount) {
    addToCount(-pageCount * DIGITPAGESIZE);
  }
  if(s_dumpCountWhenDestroyed) {
    debugLog(_T("DigitPool(%3d): pageCount:%3d, digitCount:%10s\n"), getId(), pageCount, format1000(digitCount).cstr());
  }
}

void DigitPool::addToCount(intptr_t n) { //static
  static Semaphore gate;
  gate.wait();
  s_totalDigitCount += n;
  gate.signal();
}

void DigitPool::allocatePage() {
  m_firstPage  = new DigitPage(m_firstPage, m_freeDigits); TRACE_NEW(m_firstPage);
  m_freeDigits = m_firstPage->m_page;
  m_digitCount += DIGITPAGESIZE;

  addToCount(DIGITPAGESIZE);
}

size_t DigitPool::getFreeDigitCount() const {
  size_t count = 0;
  for(const Digit *p = m_freeDigits; p; p = p->next) {
    count++;
  }
  return count;
}

size_t DigitPool::getUsedDigitCount() const {
  return getPageCount() * DIGITPAGESIZE - getFreeDigitCount();
}

size_t DigitPool::getPageCount() const {
  size_t count = 0;
  for(const DigitPage *p = m_firstPage; p; p = p->m_next) {
    count++;
  }
  return count;
}

DigitPage::DigitPage(DigitPage *nextPage, Digit *nextDigit) {
  memset(this, 0, sizeof(DigitPage));
  m_next = nextPage;
  // then link digit together
  for(Digit *p = &LASTVALUE(m_page), *q = nextDigit; p >= m_page; q = p, p--) {
    p->next = q;
  }
}
