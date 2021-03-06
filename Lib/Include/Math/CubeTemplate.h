#pragma once

#include <NumberInterval.h>
#include "PointTemplate.h"

template<typename PointType, typename SizeType, typename T, UINT dimension> class CubeTemplate {
private:
  PointType m_p0;
  SizeType  m_size;
public:
  // By default, undefined p0 and size
  inline CubeTemplate() {
  }

  template<typename PT, typename ST, typename S> CubeTemplate(const CubeTemplate<PT,ST,S, dimension> &src)
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
  inline       PointType  &p0()         { return m_p0;    }
  inline const PointType  &p0()   const { return m_p0;    }
  inline       SizeType   &size()       { return m_size;  }
  inline const SizeType   &size() const { return m_size;  }

  inline const T          &getX() const { return p0()[0]; }
  inline const T          &getY() const { return p0()[1]; }

  // Return *this
  template<typename S> CubeTemplate &setInterval(UINT index, const NumberInterval<S> &interval) {
    m_p0[  index] = (T)interval.getFrom();
    m_size[index] = (T)interval.getLength();
    return *this;
  }
  NumberInterval<T> getInterval(UINT index) const {
    const T &from = m_p0[index];
    return NumberInterval<T>(from, from + m_size[index]);
  }

  inline NumberInterval<T> getXInterval() const { return getInterval(0); }
  inline NumberInterval<T> getYInterval() const { return getInterval(1); }

  // Move this by offset +v
  // Return *this
  template<typename S> CubeTemplate &operator+=(const FixedDimensionVector<S, dimension> &v) {
    m_p0 += v;
    return *this;
  }
  // Move this by offset -v
  // Return *this
  template<typename S> CubeTemplate &operator-=(const FixedDimensionVector<S, dimension> &v) {
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

  inline       T           getMinX()      const { return getMin(0);                 }
  inline       T           getMaxX()      const { return getMax(0);                 }
  inline       T           getMinY()      const { return getMin(1);                 }
  inline       T           getMaxY()      const { return getMax(1);                 }

  PointType getAllMin() const {
    PointType p;
    for(UINT i = 0; i < dimension; i++) {
      p[i] = getMin(i);
    }
    return p;
  }
  PointType getAllMax() const {
    PointType p;
    for(UINT i = 0; i < dimension; i++) {
      p[i] = getMax(i);
    }
    return p;
  }

  inline PointType getCenter() const {
    return p0() + size()/2;
  }

  template<typename S> bool contains(const FixedDimensionVector<S, dimension> &v) const {
    for(UINT i = 0; i < dimension; i++) {
      if(!getInterval(i).contains(v[i])) {
        return false;
      }
    }
    return true;
  }
  template<typename PT, typename ST, typename S> bool contains(const CubeTemplate<PT, ST, S, dimension> &c) const {
    return contains(c.getAllMin()) && contains(c.getAllMax());
  }

  // Return v if v is inside or on edge/corner of cube
  // else return surface point on cube nearest to v
  template<typename S> PointType getProjection(const FixedDimensionVector<S, dimension> &v) const {
    PointType result;
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
  static CubeTemplate getUnit() {
    return CubeTemplate((PointTemplate<T, dimension>&)PointTemplate<T, dimension>::nullVector()
                       ,(SizeTemplate< T, dimension>&)SizeTemplate< T, dimension>().setAll(1)
                                     );
  }

  String toString(std::streamsize precision = 3) const;
};

template<typename OSTREAMTYPE, typename PT, typename ST, typename T, UINT dimension>
  OSTREAMTYPE &operator<<(OSTREAMTYPE &out, const CubeTemplate<PT, ST, T, dimension> &c)
{
  out << "(" << c.p0() << "," << c.size() << ")";
  return out;
}

template<typename ISTREAMTYPE, typename PT, typename ST, typename T, UINT dimension>
  ISTREAMTYPE &operator>>(ISTREAMTYPE &in, CubeTemplate<PT, ST, T, dimension> &c)
{
  const FormatFlags flgs = in.flags();
  in.flags(flgs | std::ios::skipws);
  if(in.peek() == '(') {
    in.get();
  } else {
    in.flags(flgs);
    in.setstate(std::ios::failbit);
    return in;
  }
  FixedDimensionVector<T,dimension> p0, size;
  in >> p0;
  if(in.peek() == ',') {
    in.get();
  } else {
    in.flags(flgs);
    in.setstate(std::ios::failbit);
    return in;
  }
  in >> size;
  if(in.peek() == ')') {
    in.get();
  } else {
    in.setstate(std::ios::failbit);
  }
  if(in) {
    c.p0()   = p0;
    c.size() = size;
  }
  in.flags(flgs);

  return in;
}

template<typename PointType, typename SizeType, typename T, UINT dimension>
  String CubeTemplate<PointType, SizeType, T, dimension>::toString(std::streamsize prec) const
{
  std::wostringstream out;
  out.precision(prec);
  out << *this;
  return out.str().c_str();
}

template<typename PT1, typename ST1, typename T1, typename PT2, typename ST2, typename T2, UINT dimension>
  CubeTemplate<PT1, ST1, T1, dimension> getUnion(const CubeTemplate<PT1, ST1, T1, dimension> &c1
                                                             ,const CubeTemplate<PT2, ST2, T2, dimension> &c2)
{
  const PT1 minP1 = c1.getAllMin(), maxP1 = c1.getAllMax();
  const PT2 minP2 = c2.getAllMin(), maxP2 = c2.getAllMax();
  CubeTemplate<PT1, ST1, T1, dimension> result;
  for(UINT i = 0; i < dimension; i++) {
    result.setInterval(i, NumberInterval<T1>(min(minP1[i], minP2[i]), max(maxP1[i], maxP2[i])));
  }
  return result;
}

template<typename PT, typename ST, typename T, UINT dimension>
  CubeTemplate<PT, ST, T, dimension> &getBoundingCube(CubeTemplate<PT, ST, T, dimension> &cube, const ConstIterator<PT> &it)
{
  PT minP, maxP;
  getMinMaxPoints(minP, maxP, it);
  cube = CubeTemplate<PT, ST, T, dimension>(minP, maxP - minP);
  return cube;
}

template<typename PointType, UINT dimension> class PointArrayTemplate : public CompactArray<PointType> {
public:
  PointArrayTemplate() {
  }
  explicit PointArrayTemplate(size_t capacity) : CompactArray<PointType>(capacity)
  {
  }
  template<typename T> PointArrayTemplate(const CollectionBase<FixedDimensionVector<T, dimension> > &src) {
    setCapacity(src.size());
    addAll(src.getIterator());
  }
  template<typename T> PointArrayTemplate &operator=(const CollectionBase<FixedDimensionVector<T, dimension> > &src) {
    if((void*)&src == (void*)this) {
      return *this;
    }
    clear(src.size());
    addAll(src.getIterator());
    return *this;
  }

  template<typename T> bool add(const FixedDimensionVector<T, dimension> &v) {
    return __super::add(v);
  }

  template<typename T> PointArrayTemplate &operator+=(const FixedDimensionVector<T, dimension> &v) {
    for(PointType *p = begin(), *endp = end(); p < endp;) {
      *(p++) += v;
    }
    return *this;
  }

  template<typename PT, typename ST, typename T, UINT dimension>
    CubeTemplate<PT, ST, T, dimension> &getBoundingBox(CubeTemplate<PT, ST, T, dimension> &box) const
  {
    getBoundingCube(box, getIterator());
    return box;
  }
};
