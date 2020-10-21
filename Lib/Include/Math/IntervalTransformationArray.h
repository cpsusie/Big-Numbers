#pragma once

#include "IntervalTransformation.h"
#include "CubeN.h"

template<typename T, UINT dimension> class IntervalTransformationArray {
private:
  IntervalTransformationTemplate<T> *m_e[dimension];

  inline void init() {
    for(UINT i = 0; i < dimension; i++) {
      m_e[i] = nullptr;
    }
  }
  inline void cleanup() {
    for(UINT i = 0; i < dimension; i++) {
      SAFEDELETE(m_e[i]);
    }
  }
  void checkIndex(const TCHAR *method, UINT index) const {
    if(index >= dimension) {
      throwIndexOutOfRangeException(method, index, dimension);
    }
  }
public:
  IntervalTransformationArray() {
    init();
    const IntervalScale     defaultScale = LINEAR;
    const NumberInterval<T> defaultFromInterval = IntervalTransformationTemplate<T>::getDefaultFromInterval(defaultScale);
    const NumberInterval<T> defaultToInterval(0, 1);
    for(UINT i = 0; i < dimension; i++) {
      m_e[i] = allocateIntervalTransformation(defaultFromInterval, defaultToInterval, defaultScale);
    }
  }
  IntervalTransformationArray(const IntervalTransformationArray<T,dimension> &src) {
    init();
    for(UINT i = 0; i < dimension; i++) {
      m_e[i] = src[i].clone();
    }
  }
  IntervalTransformationArray<T,dimension> &operator=(const IntervalTransformationArray<T,dimension> &src) {
    cleanup();
    for(UINT i = 0; i < dimension; i++) {
      m_e[i] = src[i].clone();
    }
    return *this;
  }

  virtual ~IntervalTransformationArray() {
    cleanup();
  }
  bool operator==(const IntervalTransformationArray<T, dimension> &rhs) const {
    for(UINT i = 0; i < dimension; i++) {
      if((*this)[i] != rhs[i]) {
        return false;
      }
    }
    return true;
  }
  bool operator!=(const IntervalTransformationArray<T, dimension> &rhs) const {
    return !(*this == rhs);
  }

  inline IntervalTransformationTemplate<T> &operator[](UINT index) const {
    checkIndex(__TFUNCTION__,index);
    return *m_e[index];
  }

  // Return *this
  IntervalTransformationArray<T,dimension> &setTransformation(UINT index, const IntervalTransformationTemplate<T> &t) {
    checkIndex(__TFUNCTION__,index);
    IntervalTransformationTemplate<T> *newT = t.clone();
    SAFEDELETE(m_e[index]);
    m_e[i] = newT;
    return *this;
  }
  // Return *this
  IntervalTransformationArray<T,dimension> &setTransformation(UINT index, const NumberInterval<T> &from, const NumberInterval<T> &to, IntervalScale scale) {
    checkIndex(__TFUNCTION__,index);
    IntervalTransformationTemplate<T> *newT = allocateIntervalTransformation(from, to, scale);
    SAFEDELETE(m_e[index]);
    m_e[index] = newT;
    return *this;
  }

  IntervalScale getScale(UINT index) const {
    checkIndex(__TFUNCTION__, index);
    return m_e[index]->getSccale();
  }
  IntervalTransformationArray<T, dimension> &setScale(UINT index, IntervalScale scale) {
    checkIndex(__TFUNCTION__, index);
    const IntervalTransformationTemplate<T> *t = m_e[index];
    IntervalTransformationTemplate<T> *newT = allocateIntervalTransformation(t->getFromInterval(), t->getToInterval(), scale);
    SAFEDELETE(m_e[index]);
    m_e[index] = newT;
    return *this;
  }
  CubeNTemplate<T,dimension> getFromCube() const {
    CubeNTemplate<T,dimension> result;
    for(UINT d = 0; d < dimension; d++) {
      result.setInterval(d,m_e[d]->getFromInterval());
    }
    return result;
  }

  // Return this
  template<typename S> IntervalTransformationArray<T,dimension> &setFromCube(const CubeNTemplate<S,dimension> &cube) {
    for(UINT d = 0; d < dimension; d++) {
      m_e[d]->setFromInterval(cube.getInterval(d));
    }
    return *this;
  }

  CubeNTemplate<T,dimension> getToCube() const {
    CubeNTemplate<T, dimension> result;
    for(UINT d = 0; d < dimension; d++) {
      result.setInterval(d,m_e[d]->getToInterval());
    }
    return result;
  }

  // Return this
  template<typename S> IntervalTransformationArray<T,dimension> &setToCube(const CubeNTemplate<S,dimension> &cube) {
    for(UINT d = 0; d < dimension; d++) {
      m_e[d]->setToInterval(cube.getInterval(d));
    }
    return *this;
  }
};
