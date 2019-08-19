#pragma once

#include "TestValueArray.h"
#include "StreamParametersIterator.h"

class TestElement {
public:
  const TestValueArrayElement *m_values;
  StreamParameters             m_param;
  String toString() const {
    return ::toString(m_param) + format(_T(" v=%20.16le"), m_values->getDouble());
  }
};

class TestDataArray : public TestValueArray {
public:
  TestDataArray(const double *testValues, size_t n)
    : TestValueArray(testValues, n)
  {}
  TestDataArray(const CompactDoubleArray &testValues)
    : TestValueArray(testValues)
  {}

  Iterator<TestElement> getIterator(StreamSize maxWidth = 24, StreamSize maxPrecision = 14);
};

class AbstractTestElementIterator : public AbstractIterator {
private:
  DECLARECLASSNAME;
  TestValueArray                  &m_valueArray;
  const StreamSize                 m_maxWidth, m_maxPrecision;
  Iterator<TestValueArrayElement>  m_valueIterator;
  Iterator<StreamParameters>       m_paramIterator;
  TestElement                      m_buf, m_next;
  bool                             m_hasNext;
  void nextValue();
public:
  AbstractTestElementIterator(TestValueArray &a, StreamSize maxWidth, StreamSize maxPrecision);
  AbstractIterator *clone() {
    return new AbstractTestElementIterator(*this);
  }
  bool hasNext() const {
    return m_hasNext;
  }
  void *next();
  void remove() {
    unsupportedOperationError(__TFUNCTION__);
  }
};
