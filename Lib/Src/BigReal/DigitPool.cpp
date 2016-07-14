#include "pch.h"
#include <float.h>
#include <Semaphore.h>

// the order of declaration is importatnt here.!

#pragma warning(disable : 4073)
#pragma init_seg(lib)

#ifdef _DEBUG
bool      BigReal::s_debugStringEnabled = false;
Semaphore BigReal::s_debugStringGate;
#endif

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
DEFINECLASSNAME(ConstDigitPool);

Pow2Cache     BigReal::s_pow2Cache;

const ConstBigReal ConstBigReal::_long_min(LONG_MIN   );
const ConstBigReal ConstBigReal::_long_max(LONG_MAX   );
const ConstBigReal ConstBigReal::_ulong_max(ULONG_MAX );
const ConstBigReal ConstBigReal::_i64_min(_I64_MIN    );
const ConstBigReal ConstBigReal::_i64_max(_I64_MAX    );
const ConstBigReal ConstBigReal::_ui64_max(_UI64_MAX  );

#ifdef IS64BIT
const ConstBigReal ConstBigReal::_i128_min(_I128_MIN  );
const ConstBigReal ConstBigReal::_i128_max(_I128_MAX  );
const ConstBigReal ConstBigReal::_ui128_max(_UI128_MAX);
#endif // IS64BIT

const ConstBigReal ConstBigReal::_flt_min(FLT_MIN     );
const ConstBigReal ConstBigReal::_flt_max(FLT_MAX     );
const ConstBigReal ConstBigReal::_dbl_min(DBL_MIN     );
const ConstBigReal ConstBigReal::_dbl_max(DBL_MAX     );
const ConstBigReal ConstBigReal::_dbl80_min(Double80::DBL80_MIN);
const ConstBigReal ConstBigReal::_dbl80_max(Double80::DBL80_MAX);

DEFINECLASSNAME(DigitPool);

DigitPool::DigitPool(int id, size_t intialDigitCount) : BigRealResource(id) {
  m_firstPage  = NULL;
  m_freeDigits = NULL;
  m_digitCount = 0;

#ifdef _DEBUG
  const bool debuggerPresent = getDebuggerPresent();
  static Semaphore gate;

  if(debuggerPresent) {
    gate.wait();
    BigReal::enableDebugString(false);
  }
#endif

  const size_t wantedTotalDigitCount = m_digitCount + intialDigitCount;
  while(m_digitCount < wantedTotalDigitCount) {
    allocatePage();
  }

  m_zero = new BigInt(0, this);
  m_one  = new BigInt(1, this);
  m_two  = new BigInt(2, this);
  m_half = new BigReal(e(BigReal(5, this), -1));

#ifdef _DEBUG
  if(debuggerPresent) {
    BigReal::enableDebugString(true);
    gate.signal();

    SETBIGREALDEBUGSTRING(*m_zero);
    SETBIGREALDEBUGSTRING(*m_one );
    SETBIGREALDEBUGSTRING(*m_two );
    SETBIGREALDEBUGSTRING(*m_half);
  }
#endif
}

DigitPool::~DigitPool() {
  const size_t digitCount = m_digitCount;
  delete m_half;
  delete m_two;
  delete m_one;
  delete m_zero;

  int pageCount = 0;
  for(DigitPage *p = m_firstPage, *q = NULL; p; p = q) {
    q = p->m_next;
    delete p;
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
  m_firstPage  = new DigitPage(m_firstPage, m_freeDigits);
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
