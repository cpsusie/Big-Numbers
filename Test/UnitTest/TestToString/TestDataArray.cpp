#include "stdafx.h"
#include "TestDataArray.h"

DEFINECLASSNAME(AbstractTestElementIterator);

TestIterator::TestIterator(TestDataArray *array, UINT multibitFieldsFilter, StreamSize maxWidth, StreamSize maxPrecision)
: Iterator<TestElement>(new AbstractTestElementIterator(*array, multibitFieldsFilter, maxWidth, maxPrecision))
{
}

size_t TestIterator::getMaxIterationCount() const {
  return ((AbstractTestElementIterator*)m_it)->getMaxIterationCount();
}

void TestIterator::dumpAllFormats() const {
  ((AbstractTestElementIterator*)m_it)->dumpAllFormats();
}

TestIterator TestDataArray::getIterator(UINT multibitFieldsFilter, StreamSize maxWidth, StreamSize maxPrecision) {
  return TestIterator(this, multibitFieldsFilter, maxWidth, maxPrecision);
}

AbstractTestElementIterator::AbstractTestElementIterator(TestValueArray &a, UINT multibitFieldsFilter, StreamSize maxWidth, StreamSize maxPrecision)
  : m_valueArray(a)
  , m_maxWidth(maxWidth)
  , m_maxPrecision(maxPrecision)
  , m_valueIterator(a.getIterator())
  , m_paramIterator(StreamParameters::getFloatParamIterator(maxWidth, NumberInterval<StreamSize>(1,maxPrecision),0, ITERATOR_FLOATFORMATMASK, multibitFieldsFilter))
{
  m_hasNext = m_valueIterator.hasNext() && m_paramIterator.hasNext();
  if(m_hasNext) {
    m_next.m_values = &m_valueIterator.next();
    m_next.m_param  = m_paramIterator.next();
  }
}

void AbstractTestElementIterator::nextValue() {
  m_next.m_values  = &m_valueIterator.next();
  m_paramIterator.reset();
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
