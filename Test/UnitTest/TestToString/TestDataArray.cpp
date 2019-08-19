#include "stdafx.h"
#include "TestDataArray.h"

DEFINECLASSNAME(AbstractTestElementIterator);

Iterator<TestElement> TestDataArray::getIterator(StreamSize maxWidth, StreamSize maxPrecision) {
  return Iterator<TestElement>(new AbstractTestElementIterator(*this, maxWidth, maxPrecision));
}

AbstractTestElementIterator::AbstractTestElementIterator(TestValueArray &a, StreamSize maxWidth, StreamSize maxPrecision) 
  : m_valueArray(a)
  , m_maxWidth(maxWidth)
  , m_maxPrecision(maxPrecision)
  , m_valueIterator(a.getIterator())
  , m_paramIterator(createStreamParametersIterator(maxWidth, maxPrecision))
{
  m_hasNext = m_valueIterator.hasNext() && m_paramIterator.hasNext();
  if(m_hasNext) {
    m_next.m_values = &m_valueIterator.next();
    m_next.m_param  = m_paramIterator.next();
  }
}

void AbstractTestElementIterator::nextValue() {
  m_next.m_values  = &m_valueIterator.next();
  m_paramIterator  = createStreamParametersIterator(m_maxWidth, m_maxPrecision);
  m_next.m_param   = m_paramIterator.next();
  m_hasNext        = true;
}

void *AbstractTestElementIterator::next() {
  if(!m_hasNext) noNextElementError(s_className);
  m_buf = m_next;
  if(m_paramIterator.hasNext()) {
    m_next.m_param = m_paramIterator.next();
  } else if(m_valueIterator.hasNext()) {
    nextValue();
  } else {
    m_hasNext = false;
  }
  return &m_buf;
}
