#include "pch.h"
#include <float.h>
#include <DebugLog.h>

// The order of declaration is importatnt here.!

#pragma warning(disable : 4073)
#pragma init_seg(lib)

class InitBigReal {
public:
  InitBigReal() {
    Double80::initClass();

    BigReal::s_defaultDigitPool = BigRealResourcePool::fetchDigitPool();
    BigReal::s_constDigitPool   = BigRealResourcePool::fetchDigitPool(true, 0);
    BigReal::s_defaultDigitPool->setName(_T("DEFAULT"));
    BigReal::s_constDigitPool->setName(  _T("CONST"  ));
  }
  ~InitBigReal() {
    BigRealResourcePool::releaseDigitPool(BigReal::s_defaultDigitPool);
    BigRealResourcePool::releaseDigitPool(BigReal::s_constDigitPool  );
  }
};

const BR2DigitType BigReal::s_BIGREALBASEBR2(BIGREALBASE);

std::atomic<UINT> DigitPool::s_totalAllocatedPageCount = 0;
bool              DigitPool::s_dumpCountWhenDestroyed  = false;
DigitPool        *BigReal::s_defaultDigitPool          = NULL; 
DigitPool        *BigReal::s_constDigitPool            = NULL;

static InitBigReal initBigReal;

const ConstBigInt  BigReal::_0(        0         );
const ConstBigInt  BigReal::_1(        1         );
const ConstBigInt  BigReal::_2(        2         );
const ConstBigInt  BigReal::_i16_min(  _I16_MIN  );
const ConstBigInt  BigReal::_i16_max(  _I16_MAX  );
const ConstBigInt  BigReal::_ui16_max( _UI16_MAX );
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
const ConstBigReal BigReal::_C1third(0.333333f   );

// no need for signalling NaN
const ConstBigReal     BigReal::_BR_QNAN( std::numeric_limits<double>::quiet_NaN());   // non-signaling NaN (quiet NaN)
const ConstBigReal     BigReal::_BR_PINF( std::numeric_limits<double>::infinity());    // +infinity;
const ConstBigReal     BigReal::_BR_NINF(-std::numeric_limits<double>::infinity());    // -infinity;

const ConstBigInt      BigInt::_BINT_QNAN(quot( BigReal::_0, BigReal::_0));  // non-signaling NaN (quiet NaN)
const ConstBigInt      BigInt::_BINT_PINF(quot( BigReal::_1, BigReal::_0));  // +infinity;
const ConstBigInt      BigInt::_BINT_NINF(quot(-BigReal::_1, BigReal::_0));  // -infinity;

const ConstBigRational BigRational::_0(         BigReal::_0, BigReal::_1);  // 0
const ConstBigRational BigRational::_05(        BigReal::_1, BigReal::_2);  // 1/2
const ConstBigRational BigRational::_1(         BigReal::_1, BigReal::_1);  // 1
const ConstBigRational BigRational::_2(         BigReal::_2, BigReal::_1);  // 2
const ConstBigRational BigRational::_BRAT_QNAN( BigReal::_0, BigReal::_0);  // non-signaling NaN (quiet NaN)
const ConstBigRational BigRational::_BRAT_PINF( BigReal::_1, BigReal::_0);  // +infinity;
const ConstBigRational BigRational::_BRAT_NINF(-BigReal::_1, BigReal::_0);  // -infinity;


DigitPool::DigitPool(int id, const String &name, size_t intialDigitCount)
: BigRealResource(id)
, m_name(name)
, m_allocatedPageCount(0)
, m_firstPage(NULL)
, m_freeDigits(NULL)
, m_initFlags(BR_MUTABLE)          // so that constants allocated 
, m_refCount(0)
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
    debugLog(_T("DigitPool(%3u):%-10s:PageCount:%3d, DigitCount:%10s\n")
            ,getId(), getName().cstr(), count, format1000((INT64)count*DIGITPAGESIZE).cstr());
  }
}

  Digit *DigitPool::fetchDigit() {
    if(m_freeDigits == NULL) allocatePage();
    Digit *p     = m_freeDigits;
    m_freeDigits = p->next;
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
