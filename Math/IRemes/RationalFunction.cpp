#include "stdafx.h"
#include "RationalFunction.h"

RationalFunction::RationalFunction() : m_domain(nullptr) {
  clear();
}

RationalFunction::RationalFunction(const RationalFunction &src) : m_domain(nullptr) {
  if(src.isEmpty()) {
    clear();
  } else {
    copyData(src);
  }
}

RationalFunction::RationalFunction(const ConstBigRealInterval &domain, UINT m, UINT k, const BigRealVector &coefVector, int coefVectorIndex, UINT digits)
: m_M(m)
, m_K(k)
, m_N(m + k)
, m_coefVectorIndex(coefVectorIndex)
, m_digits(digits)
{
  m_domain = new ConstBigRealInterval(domain); TRACE_NEW(m_domain);
  const size_t dim = coefVector.getDimension();
  m_coefVector.setCapacity(dim);
  for(size_t i = 0; i < dim; i++) {
    m_coefVector.add(coefVector[i]);
  }
  assert(m_coefVector.size() == m_N + 2);
}

RationalFunction::~RationalFunction() {
  clear();
}

RationalFunction &RationalFunction::operator=(const RationalFunction &src) {
  clear();
  if(!src.isEmpty()) {
    copyData(src);
  }
  return *this;
}

void RationalFunction::clear() {
  m_M = m_K = m_N = 0;
  m_coefVectorIndex = 0;
  m_digits          = 0;
  SAFEDELETE(m_domain);
  m_coefVector.clear();
}

void RationalFunction::copyData(const RationalFunction &src) {
  m_M = src.m_M;
  m_K = src.m_K;
  m_N = src.m_N;
  m_coefVectorIndex = src.m_coefVectorIndex;
  m_digits = src.m_digits;
  m_domain = new ConstBigRealInterval(*src.m_domain); TRACE_NEW(m_domain);
  const size_t dim = src.m_coefVector.size();
  m_coefVector.setCapacity(dim);
  for (size_t i = 0; i < dim; i++) {
    m_coefVector.add(src.m_coefVector[i]);
  }
  assert(m_coefVector.size() == m_N + 2);
}

bool RationalFunction::operator==(RationalFunction &rhs) const {
  return m_M               == rhs.m_M
      && m_K               == rhs.m_K
      && m_coefVectorIndex == rhs.m_coefVectorIndex
      && m_digits          == rhs.m_digits
      && m_domain          == rhs.m_domain
      && m_coefVector      == rhs.m_coefVector;
}

BigReal RationalFunction::operator()(const BigReal &x) {
  DigitPool *pool = x.getDigitPool();
  if(m_K) {
    BigReal sum1(pool), sum2(pool);
    sum1 = m_coefVector[m_M];
    sum2 = m_coefVector[m_N];
    for(int i = m_M; --i >= 0;) {
      sum1 = sum1 * x + m_coefVector[i];
    }
    for(UINT j = m_N; --j > m_M;) {
      sum2 = sum2 * x + m_coefVector[j];
    }
    return rQuot(sum1, sum2 * x + pool->_1() , m_digits);
  } else {
    BigReal sum(pool);
    sum = m_coefVector[m_M];
    for(int i = m_M; --i >= 0;) {
      sum = sum * x + m_coefVector[i];
    }
    return sum;
  }
}
