#pragma once

#include "IntervalTransformation.h"

template<typename T, UINT dim> class IntervalTransformationArray {
private:
  IntervalTransformationTemplate<T> *m_e[dim];

  inline void init() {
    for(UINT i = 0; i < dim; i++) {
      m_e[i] = nullptr;
    }
  }
  inline void cleanup() {
    for(UINT i = 0; i < dim; i++) {
      SAFEDELETE(m_e[i]);
    }
  }
  void checkIndex(const TCHAR *method, UINT index) const {
    if(index >= dim) {
      throwIndexOutOfRangeException(method, index, dim);
    }
  }
public:
  IntervalTransformationArray() {
    init();
    const IntervalScale     defaultScale = LINEAR;
    const NumberInterval<T> defaultFromInterval = IntervalTransformationTemplate<T>::getDefaultFromInterval(defaultScale);
    const NumberInterval<T> defaultToInterval(0, 1);
    for(UINT i = 0; i < dim; i++) {
      m_e[i] = allocateIntervalTransformation(defaultFromInterval, defaultToInterval, defaultScale);
    }
  }
  IntervalTransformationArray(const IntervalTransformationArray<T,dim> &src) {
    init();
    for(UINT i = 0; i < dim; i++) {
      m_e[i] = src[i].clone();
    }
  }
  IntervalTransformationArray<T,dim> &operator=(const IntervalTransformationArray<T,dim> &src) {
    cleanup();
    for(UINT i = 0; i < dim; i++) {
      m_e[i] = src[i].clone();
    }
    return *this;
  }

  virtual ~IntervalTransformationArray() {
    cleanup();
  }
  bool operator==(const IntervalTransformationArray<T, dim> &rhs) const {
    for(UINT i = 0; i < dim; i++) {
      if((*this)[i] != rhs[i]) {
        return false;
      }
    }
    return true;
  }
  bool operator!=(const IntervalTransformationArray<T, dim> &rhs) const {
    return !(*this == rhs);
  }

  inline IntervalTransformationTemplate<T> &operator[](UINT index) const {
    checkIndex(__TFUNCTION__,index);
    return *m_e[index];
  }
  // Return *this
  IntervalTransformationArray<T,dim> &setTransformation(UINT index, const IntervalTransformationTemplate<T> &t) {
    checkIndex(__TFUNCTION__,index);
    IntervalTransformationTemplate<T> *newT = t.clone();
    SAFEDELETE(m_e[index]);
    m_e[i] = newT;
    return *this;
  }
  // Return *this
  IntervalTransformationArray<T,dim> &setTransformation(UINT index, const NumberInterval<T> &from, const NumberInterval<T> &to, IntervalScale scale) {
    checkIndex(__TFUNCTION__,index);
    IntervalTransformationTemplate<T> *newT = allocateIntervalTransformation(from, to, scale);
    SAFEDELETE(m_e[index]);
    m_e[index] = newT;
    return *this;
  }
};
