#include "pch.h"
#include <float.h>
#include <DebugLog.h>
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

const ConstBigInt  BigReal::_0(        0         );
const ConstBigInt  BigReal::_1(        1         );
const ConstBigInt  BigReal::_2(        2         );
const ConstBigInt  BigReal::_i32_min(  _I32_MIN  );
const ConstBigInt  BigReal::_i32_max(  _I32_MAX  );
const ConstBigInt  BigReal::_ui32_max( _UI32_MAX );
const ConstBigInt  BigReal::_i64_min(  _I64_MIN  );
const ConstBigInt  BigReal::_i64_max(  _I64_MAX  );
const ConstBigInt  BigReal::_ui64_max( _UI64_MAX );
const ConstBigInt  BigReal::_i128_min( _I128_MIN );
const ConstBigInt  BigReal::_i128_max( _I128_MAX );
const ConstBigInt  BigReal::_ui128_max(_UI128_MAX);

const ConstBigReal BigReal::_05(e(BigReal(5), -1));  // _05 and 2 must be initialize FIRST!!! and DONT use 0.5 here!!!
const ConstBigReal BigReal::_flt_min(  FLT_MIN   );
const ConstBigReal BigReal::_flt_max(  FLT_MAX   );
const ConstBigReal BigReal::_dbl_min(  DBL_MIN   );
const ConstBigReal BigReal::_dbl_max(  DBL_MAX   );
const ConstBigReal BigReal::_dbl80_min(DBL80_MIN );
const ConstBigReal BigReal::_dbl80_max(DBL80_MAX );
const ConstBigReal BigReal::_C1third(0.33333333333);

// no need for signalling NaN
const ConstBigReal BigReal::_BR_QNAN( std::numeric_limits<double>::quiet_NaN());   // non-signaling NaN (quiet NaN)
const ConstBigReal BigReal::_BR_PINF( std::numeric_limits<double>::infinity());    // +infinity;
const ConstBigReal BigReal::_BR_NINF(-std::numeric_limits<double>::infinity());    // -infinity;

DigitPool::DigitPool(int id, size_t intialDigitCount) : BigRealResource(id) {
  m_firstPage  = NULL;
  m_freeDigits = NULL;
  m_digitCount = 0;

  while(m_digitCount < intialDigitCount) {
    allocatePage();
  }

  m_0    = new BigInt(0, this);                    TRACE_NEW(m_0    );
  m_1    = new BigInt(1, this);                    TRACE_NEW(m_1    );
  m_2    = new BigInt(2, this);                    TRACE_NEW(m_2    );
  m_05   = new BigReal(e(BigReal(5, this), -1));   TRACE_NEW(m_05   ); // Don't use 0.5 here!!!
  m_nan  = new BigReal(this); m_nan->setToNan();   TRACE_NEW(m_nan  );
  m_pinf = new BigReal(this); m_pinf->setToPInf(); TRACE_NEW(m_pinf );
  m_ninf = new BigReal(this); m_ninf->setToNInf(); TRACE_NEW(m_ninf );
}

DigitPool::~DigitPool() {
  const size_t digitCount = m_digitCount;
  SAFEDELETE(m_ninf);
  SAFEDELETE(m_pinf);
  SAFEDELETE(m_nan );
  SAFEDELETE(m_05  );
  SAFEDELETE(m_2   );
  SAFEDELETE(m_1   );
  SAFEDELETE(m_0   );

  int pageCount = 0;
  for(DigitPage *p = m_firstPage, *q = NULL; p; p = q) {
    q = p->m_next;
    SAFEDELETE(p);
    pageCount++;
  }
  if(pageCount) {
    updateTotalDigitCount(-pageCount * DIGITPAGESIZE);
  }
  if(s_dumpCountWhenDestroyed) {
    debugLog(_T("DigitPool(%3d): pageCount:%3d, digitCount:%10s\n"), getId(), pageCount, format1000(digitCount).cstr());
  }
}

void DigitPool::updateTotalDigitCount(intptr_t n) { //static
  static Semaphore gate;
  gate.wait();
  s_totalDigitCount += n;
  gate.signal();
}

void DigitPool::allocatePage() {
  m_firstPage  = new DigitPage(m_firstPage, m_freeDigits); TRACE_NEW(m_firstPage);
  m_freeDigits = m_firstPage->m_page;
  m_digitCount += DIGITPAGESIZE;

  updateTotalDigitCount(DIGITPAGESIZE);
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
  // then link digits together
  Digit *p = &LASTVALUE(m_page);
  p->next = nextDigit;
  while(p-- > m_page) p->next = p+1;
}
