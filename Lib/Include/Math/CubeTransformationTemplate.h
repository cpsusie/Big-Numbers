#pragma once

#include "IntervalTransformation.h"
#include "CubeTemplate.h"

template<typename CubeType, typename PointType, typename SizeType, typename T, UINT dimension> class CubeTransformationTemplate {
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
  template<typename CT, typename PT, typename ST, typename S> void copyAll(const CubeTransformationTemplate<CT, PT, ST, S, dimension> &src) {
    for(UINT i = 0; i < dimension; i++) {
      const IntervalTransformation<S> &si = src[i];
      m_e[i] = allocateIntervalTransformation(NumberInterval<T>(si.getFromInterval()), NumberInterval<T>(si.getToInterval()), si.getScaleType());
    }
  }
  void cloneAll(const CubeTransformationTemplate &src) {
    for(UINT i = 0; i < dimension; i++) {
      m_e[i] = src[i].clone();
    }
  }
  void checkIndex(const TCHAR *method, UINT index) const {
    if(index >= dimension) {
      throwIndexOutOfRangeException(method, index, dimension);
    }
  }
public:
  CubeTransformationTemplate() {
    init();
    const IntervalScale     defaultScale        = LINEAR;
    const NumberInterval<T> defaultFromInterval = IntervalTransformationTemplate<T>::getDefaultFromInterval(defaultScale);
    const NumberInterval<T> defaultToInterval(0, 1);
    for(UINT i = 0; i < dimension; i++) {
      m_e[i] = allocateIntervalTransformation(defaultFromInterval, defaultToInterval, defaultScale);
    }
  }
  CubeTransformationTemplate(const CubeTransformationTemplate &src) {
    init();
    cloneAll(src);
  }
  template<typename CT, typename PT, typename ST, typename S> CubeTransformationTemplate(const CubeTransformationTemplate<CT, PT, ST, S,dimension> &src)
  { init();
    copyAll(src);
  }
  CubeTransformationTemplate &operator=(const CubeTransformationTemplate &src)
  { cleanup();
    cloneAll(src);
    return *this;
  }
  template<typename CT, typename PT, typename ST, typename S> CubeTransformationTemplate &operator=(const CubeTransformationTemplate<CT,PT,ST,S,dimension> &src)
  { cleanup();
    copyAll(src);
    return *this;
  }

  virtual ~CubeTransformationTemplate() {
    cleanup();
  }

  bool operator==(const CubeTransformationTemplate &rhs) const {
    for(UINT i = 0; i < dimension; i++) {
      if((*this)[i] != rhs[i]) {
        return false;
      }
    }
    return true;
  }
  bool operator!=(const CubeTransformationTemplate &rhs) const {
    return !(*this == rhs);
  }

  template<typename S> PointType forwardTransform(const FixedDimensionVector<S, dimension> &v) const {
    PointType result;
    for(UINT i = 0; i < dimension; i++) {
      result[i] = m_e[i]->forwardTransform(v[i]);
    }
    return result;
  }
  template<typename S> PointType backwardTransform(const FixedDimensionVector<S, dimension> &v) const {
    PointType result;
    for(UINT i = 0; i < dimension; i++) {
      result[i] = m_e[i]->backwardTransform(v[i]);
    }
    return result;
  }
  template<typename PT, typename ST, typename S> CubeType forwardTransform(const CubeTemplate<PT,ST,S,dimension> &c) const {
    CubeType result;
    for(UINT i = 0; i < dimension; i++) {
      result.setInterval(i, m_e[i]->forwardTransform(c.getInterval(i)));
    }
    return result;
  }
  template<typename PT, typename ST, typename S> CubeType backwardTransform(const CubeTemplate<PT,ST,S,dimension> &c) const {
    CubeType result;
    for(UINT i = 0; i < dimension; i++) {
      result.setInterval(i, m_e[i]->backwardTransform(c.getInterval(i)));
    }
    return result;
  }

  // Return *this
  CubeTransformationTemplate &setTransformation(UINT index, const IntervalTransformationTemplate<T> &t) {
    checkIndex(__TFUNCTION__, index);
    IntervalTransformationTemplate<T> *newT = t.clone();
    SAFEDELETE(m_e[index]);
    m_e[i] = newT;
    return *this;
  }

  // Return *this
  CubeTransformationTemplate &setTransformation(UINT index, const NumberInterval<T> &from, const NumberInterval<T> &to, IntervalScale scale) {
    checkIndex(__TFUNCTION__, index);
    IntervalTransformationTemplate<T> *newT = allocateIntervalTransformation(from, to, scale);
    SAFEDELETE(m_e[index]);
    m_e[index] = newT;
    return *this;
  }
  const IntervalTransformationTemplate<T> &operator[](UINT index) const {
    checkIndex(__TFUNCTION__, index);
    return *m_e[index];
  }
  IntervalTransformationTemplate<T> &operator[](UINT index) {
    checkIndex(__TFUNCTION__, index);
    return *m_e[index];
  }

  IntervalScale getScaleType(UINT index) const {
    checkIndex(__TFUNCTION__, index);
    return m_e[index]->getScaleType();
  }

  // Return *this
  CubeTransformationTemplate &setScaleType(UINT index, IntervalScale scale) {
    checkIndex(__TFUNCTION__, index);
    if(scale != getScaleType(index)) {
      const IntervalTransformationTemplate<T> *t = m_e[index];
      IntervalTransformationTemplate<T> *newT = allocateIntervalTransformation(t->getFromInterval(), t->getToInterval(), scale);
      SAFEDELETE(m_e[index]);
      m_e[index] = newT;
    }
    return *this;
  }

  CubeType getFromCube() const {
    CubeType result;
    for(UINT d = 0; d < dimension; d++) {
      result.setInterval(d, m_e[d]->getFromInterval());
    }
    return result;
  }

  // Return *this
  template<typename PT, typename ST, typename S> CubeTransformationTemplate &setFromCube(const CubeTemplate<PT,ST,S,dimension> &cube) {
    for(UINT d = 0; d < dimension; d++) {
      m_e[d]->setFromInterval(cube.getInterval(d));
    }
    return *this;
  }

  CubeType getToCube() const {
    CubeType result;
    for(UINT d = 0; d < dimension; d++) {
      result.setInterval(d,m_e[d]->getToInterval());
    }
    return result;
  }

  // Return *this
  template<typename PT, typename ST, typename S> CubeTransformationTemplate &setToCube(const CubeTemplate<PT,ST,S,dimension> &cube) {
    for(UINT d = 0; d < dimension; d++) {
      m_e[d]->setToInterval(cube.getInterval(d));
    }
    return *this;
  }
};
