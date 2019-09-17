#pragma once

#include <StreamParameters.h>
#include "TestValueArray.h"

class TestElement {
public:
  const TestValueArrayElement *m_values;
  StreamParameters             m_param;
  String toString() const {
    return m_param.toString() + format(_T(" v=%20.16le"), m_values->getDouble());
  }
};

class TestDataArray;

class TestIterator : public Iterator<TestElement> {
public:
  TestIterator(TestDataArray *array, StreamSize maxWidth, StreamSize maxPrecision);
  size_t getMaxIterationCount() const;
  void dumpAllFormats() const;
};

class TestDataArray : public TestValueArray {
public:
  TestDataArray(const double *testValues, size_t n)
    : TestValueArray(testValues, n)
  {}
  TestDataArray(const CompactDoubleArray &testValues)
    : TestValueArray(testValues)
  {}

  TestIterator getIterator(StreamSize maxWidth = 24, StreamSize maxPrecision = 14);
};

class AbstractTestElementIterator : public AbstractIterator {
private:
  DECLARECLASSNAME;
  TestValueArray                  &m_valueArray;
  const StreamSize                 m_maxWidth, m_maxPrecision;
  Iterator<TestValueArrayElement>  m_valueIterator;
  StreamParametersIterator         m_paramIterator;
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
  inline size_t getMaxIterationCount() const {
    return m_valueArray.size() * m_paramIterator.getMaxIterationCount();
  }
  void dumpAllFormats() const {
    m_paramIterator.dumpAllFormats();
  }
};
