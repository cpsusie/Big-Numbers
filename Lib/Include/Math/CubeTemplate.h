#pragma once

#include <NumberInterval.h>
#include "PointTemplate.h"

template<typename T, UINT dimension> class CubeTemplate {
private:
  PointTemplate<T, dimension> m_p0;
  SizeTemplate< T, dimension> m_size;
public:
  // By default, undefined p0 and size
  inline CubeTemplate() {
  }

  template<typename S> CubeTemplate(const CubeTemplate<S, dimension> &src)
    : m_p0(src.p0()), m_size(src.size())
  {
  }
  template<typename P, typename S> CubeTemplate(const PointTemplate<P, dimension> &p0, const SizeTemplate<S, dimension> &size)
    : m_p0(p0), m_size(size)
  {
  }

  // Set p0,size = 0,0
  // Return *this
  CubeTemplate &clear() {
    m_p0.clear();
    m_size.clear();
    return *this;
  }
  inline       PointTemplate<T, dimension> &p0() {
    return m_p0;
  }
  inline const PointTemplate<T, dimension> &p0() const {
    return m_p0;
  }
  inline       SizeTemplate< T, dimension> &size() {
    return m_size;
  }
  inline const SizeTemplate< T, dimension> &size() const {
    return m_size;
  }
  // return *this
  template<typename S> CubeTemplate<T, dimension> &setInterval(UINT index, const NumberInterval<S> &interval) {
    m_p0[  index] = interval.getFrom();
    m_size[index] = interval.getLength();
    return *this;
  }
  NumberInterval<T> getInterval(UINT index) const {
    const T &p = m_p0[index];
    return NumberInterval<T>(p, p + m_size[index]);
  }

  // Move this by offset +v
  // Return *this
  template<typename S> CubeTemplate<T, dimension> &operator+=(const FixedSizeVectorTemplate<S, dimension> &v) {
    m_p0 += v;
    return *this;
  }
  // Move this by offset -v
  // Return *this
  template<typename S> CubeTemplate<T, dimension> &operator-=(const FixedSizeVectorTemplate<S, dimension> &v) {
    m_p0 -= v;
    return *this;
  }

  inline UINT getDimension() const {
    return dimension;
  }
  inline T getVolume() const {
    return m_size.volume();
  }
  T getMin(UINT index) const {
    const T &l = m_size[index];
    return (l >= 0) ? m_p0[index] : (m_p0[index] + l);
  }
  T getMax(UINT index) const {
    const T &l = m_size[index];
    return (l <= 0) ? m_p0[index] : (m_p0[index] + l);
  }

  PointTemplate<T, dimension> getAllMin() const {
    PointTemplate<T, dimension> p;
    for(UINT i = 0; i < dimension; i++) {
      p[i] = getMin(i);
    }
    return p;
  }
  PointTemplate<T, dimension> getAllMax() const {
    PointTemplate<T, dimension> p;
    for(UINT i = 0; i < dimension; i++) {
      p[i] = getMax(i);
    }
    return p;
  }

  inline PointTemplate<T, dimension> getCenter() const {
    return p0() + size()/2;
  }

  template<typename S> bool contains(const FixedSizeVectorTemplate<S, dimension> &v) const {
    for(UINT i = 0; i < dimension; i++) {
      if(!getInterval(i).contains(v[i])) {
        return false;
      }
    }
    return true;
  }
  template<typename S> bool contains(const CubeTemplate<S, dimension> &c) const {
    return contains(c.getAllMin()) && contains(c.getAllMax());
  }

  // Return v if v is inside or on edge/corner of cube
  // else return surface point on cube nearest to v
  template<typename S> PointTemplate<T, dimension> getProjection(const FixedSizeVectorTemplate<S, dimension> &v) const {
    PointTemplate<T, dimension> result;
    for(UINT i = 0; i < dimension; i++) {
      result[i] = minMax(v[i], getMin(i), getMax(i));
    }
    return result;
  }

  inline bool operator==(const CubeTemplate &c) const {
    return (p0() == c.p0()) && (size() == c.size());
  }
  inline bool operator!=(const CubeTemplate &r) const {
    return !(*this == c);
  }
  static CubeTemplate<T, dimension> getUnit() {
    return CubeTemplate<T, dimension>((PointTemplate<T, dimension>&)PointTemplate<T, dimension>::nullVector()
                                     ,(SizeTemplate< T, dimension>&)SizeTemplate< T, dimension>().setAll(1)
                                     );
  }
};

template<typename T1, typename T2, UINT dimension> CubeTemplate<T1, dimension> getUnion(const CubeTemplate<T1, dimension> &c1, const CubeTemplate<T2, dimension> &c2) {
  const PointTemplate<T1, dimension> minP1 = c1.getAllMin(), maxP1 = c1.getAllMax();
  const PointTemplate<T2, dimension> minP2 = c2.getAllMin(), maxP2 = c2.getAllMax();
  CubeTemplate<T1, dimension> result;
  for(UINT i = 0; i < dimension; i++) {
    result.setInterval(i, NumberInterval<T1>(min(minP1[i], minP2[i]), max(maxP1[i], maxP2[i])));
  }
  return result;
}

template<typename PointType, typename T, UINT dimension> CubeTemplate<T, dimension> &getBoundingCube(CubeTemplate<T, dimension> &cube, const ConstIterator<PointType> &it) {
  PointType minP, maxP;
  getMinMaxPoints(minP, maxP, it);
  cube = CubeTemplate<T, dimension>(minP, maxP - minP);
  return cube;
}
