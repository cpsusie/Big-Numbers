#include "stdafx.h"
#include <Math.h>
#include "Lcs.h"
LineArray::LineArray() : m_ownStringPool(true) {
  m_stringPool = new StringPool(); TRACE_NEW(m_stringPool);
}

LineArray::LineArray(StringPool &stringPool) : m_ownStringPool(false), m_stringPool(&stringPool) {
}

LineArray::~LineArray() {
  if(m_ownStringPool) {
    SAFEDELETE(m_stringPool);
  }
}

void LineArray::clear() {
  m_offset.clear();
  if(m_ownStringPool) {
    m_stringPool->clear();
  }
}

void LineArray::setIndexCapacity(size_t textCapacity, size_t lineCount) {
  m_stringPool->setIndexCapacity(lineCount);
  m_stringPool->setTextCapacity(textCapacity);
}

void LineArray::setLineCapacity(size_t lineCount) {
  m_offset.setCapacity(lineCount);
}

void LineArray::updateCapacity() {
  if(m_offset.size() + 1000 < m_offset.getCapacity()) {
    m_offset.setCapacity(m_offset.size());
  }
  if(m_ownStringPool) {
    if(m_stringPool->getTextSize() + 20000 < m_stringPool->getTextCapacity()) {
      m_stringPool->setTextCapacity(m_stringPool->getTextSize());
    }
    m_stringPool->clearIndex();
  }
}

void LineArray::dump(const String &fname) const {
  FILE *f = fopen(fname,_T("w"));
  if(f == NULL) {
    return;
  }
  for(size_t i = 0; i < size(); i++) {
    _ftprintf(f, _T("%s\n"), (*this)[i]);
  }
  fclose(f);
}

Lcs::Lcs(LcsComparator &cmp, CompareJob *job)
: m_cmp(cmp)
, m_job(job)
{
  m_tresh = NULL;
  m_link  = NULL;
}

Lcs::~Lcs() {
  clear();
}

void Lcs::allocateTreshAndLinkArrays(size_t size) {
  m_tresh = new UINT[size];  TRACE_NEW(m_tresh);
  m_link  = new Link*[size]; TRACE_NEW(m_link);
  memset(m_tresh, 0, size*sizeof(m_tresh[0]));
  memset(m_link , 0, size*sizeof(m_link[ 0]));
}

void Lcs::clear() {
  SAFEDELETEARRAY(m_tresh);
  SAFEDELETEARRAY(m_link );
  m_linkPool.releaseAll();
}

void Lcs::stopAndThrow() {
  throwException(_T("Lcs interrupted"));
}

#if defined(_DEBUG)
void Lcs::dumpThreshLinks() const {
  debugLog(_T("Tresh and Links\n"));
  for(size_t i = 0; i < m_n; i++) {
    debugLog(_T("%5zu:%5u:"), i,m_tresh[i]);
    for(const Link *l = m_link[i]; l; l = l->m_next) {
      debugLog(_T("%s"), l->toString().cstr());
    }
    debugLog(_T("\n"));
  }
}
#endif // _DEBUG

// -------------------------------------------------------------------------------------

int IndexComparator::compare(const LcsElement &e1, const LcsElement &e2) {
  incrCompareCount();
  const int c = m_c.compare(e1.m_s, e2.m_s);
  if(c) return c;
  return e1.m_index - e2.m_index;
}

int IndexComparatorR::compare(const LcsElement &e1, const LcsElement &e2) {
  incrCompareCount();
  const int c = m_c.compare(e1.m_s, e2.m_s);
  if(c) return c;
  return e2.m_index - e1.m_index;
}

// -------------------------------------------------------------------------------------

class StdLcsComparator : public LcsComparator {
public:
  int compare(const TCHAR * const &s1, const TCHAR * const &s2);
  AbstractComparator *clone() const {
    return new StdLcsComparator();
  }
};

int StdLcsComparator::compare(const TCHAR * const &s1, const TCHAR * const &s2) {
  return _tcscmp(s1, s2);
}

static StdLcsComparator stdlcscmp;
LcsComparator *stdLcsComparator = &stdlcscmp;
