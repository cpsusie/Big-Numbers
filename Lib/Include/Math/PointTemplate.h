#pragma once

#include <BasicIncludes.h>
#include "MathLib.h"
#include "FixedDimensionVector.h"


template<typename T, UINT dimension> class SizeTemplate : public FixedDimensionVector<T, dimension> {
public:
  inline SizeTemplate() {
  }
  // copy constructor
  template<typename S> SizeTemplate(const FixedDimensionVector<S, dimension> &src)
    : FixedDimensionVector<T, dimension>(src)
  {
  }
  // copy constructor
  template<typename S> SizeTemplate(const S *src)
    : FixedDimensionVector<T, dimension>(src)
  {
  }
  template<typename S, typename... Tail> inline SizeTemplate(const S &x0, Tail... tail)
    : FixedDimensionVector<T, dimension>(x0, tail...)
  {
  }
  SizeTemplate                      operator*(const T &k) const {
    return __super::operator*(k);
  }
  SizeTemplate                      operator/(const T &k) const {
    return __super::operator/(k);
  }
  template<typename S> T            operator*(const FixedDimensionVector<S, dimension> &v) const {
    return __super::operator*(v);
  }
};

template<typename T, UINT dimension> class PointTemplate : public FixedDimensionVector<T, dimension> {
public:
  inline PointTemplate() {
  }
  // copy constructor
  template<typename S> PointTemplate(const FixedDimensionVector<S, dimension> &src)
    : FixedDimensionVector<T, dimension>(src)
  {
  }
  // copy constructor
  template<typename S> PointTemplate(const S *src)
    : FixedDimensionVector<T, dimension>(src)
  {
  }
  template<typename S, typename... Tail> inline PointTemplate(const S &x0, Tail... tail)
    : FixedDimensionVector<T, dimension>(x0, tail...)
  {
  }
  template<typename S> PointTemplate            &operator=(const FixedDimensionVector<S, dimension> &v) {
    __super::operator=(v);
    return *this;
  }
  PointTemplate                                  operator*(const T &k) const {
    return __super::operator*(k);
  }
  PointTemplate                                  operator/(const T &k) const {
    return __super::operator/(k);
  }
  template<typename S> PointTemplate             operator+(const FixedDimensionVector<S, dimension> &v) const {
    return v + *this;
  }
  template<typename S> PointTemplate             operator-(const FixedDimensionVector<S, dimension> &v) const {
    return (PointTemplate<T, dimension>)__super::operator-(v);
  }

  template<typename S> PointTemplate             &operator+=(const FixedDimensionVector<S, dimension> &v) {
    __super::operator+=(v);
    return *this;
  }
  template<typename S> PointTemplate             &operator-=(const FixedDimensionVector<S, dimension> &v) {
    __super::operator-=(v);
    return *this;
  }

  // point1 - point2 -> size
  template<typename S> SizeTemplate<T, dimension> operator-(const PointTemplate<S, dimension> &p) const {
    return (SizeTemplate<T, dimension>)__super::operator-(p);
  }
  PointTemplate                                   operator-() const {
    return __super::operator-();
  }

  // Return dot product = *this * s
  template<typename S> T                          operator*(const FixedDimensionVector<S, dimension> &v) const {
    return __super::operator*(v);
  }
};

template<typename T1, typename T2, UINT dimension> double distance(const PointTemplate<T1, dimension> &p1, const PointTemplate<T2, dimension> &p2) {
  return (p1 - p2).length();
}

template<typename PointType, UINT coordIndex> class PointComparator : public Comparator<PointType> {
private:
  const bool m_maximum;
  PointComparator(const PointComparator &src)
    : m_maximum(   src.m_maximum   )
  {
  }
public:
  PointComparator(bool maximum)
    : m_maximum(maximum)
  {
  }
  AbstractComparator *clone() const override {
    return new PointComparator(*this);
  }
  int compare(const PointType &p1, const PointType &p2) override {
    return m_maximum ? sign(p2[coordIndex] - p1[coordIndex]) : sign(p1[coordIndex] - p2[coordIndex]);
  }
};

// Return minimal cube containg all points returned by iterator
template<typename PointType> void getMinMaxPoints(PointType &minP, PointType &maxP, const ConstIterator<PointType> &it) {
  minP.clear();
  maxP = minP;
  bool firstTime = true;
  for(auto it1 = it; it1.hasNext();) {
    const PointType &p = it1.next();
    if(firstTime) {
      minP = maxP = p;
      firstTime = false;
    } else {
      minP = Min(p, minP);
      maxP = Max(p, maxP);
    }
  }
}

template<typename PointType, UINT dimension> class PointRefArrayTemplate : public ValueRefArrayTemplate<PointType> {
public:
  inline PointRefArrayTemplate() {
  }
  inline PointRefArrayTemplate(size_t capacity) : ValueRefArrayTemplate(capacity) {
  }
  inline PointRefArrayTemplate(CompactArray<PointType> &src)
    : ValueRefArrayTemplate(src.size())
  {
    addAll(src);
  }

  template<typename S> PointRefArrayTemplate &operator+=(const FixedDimensionVector<S,dimension> &v) {
    for(PointType **p = begin(), **endp = end(); p < endp;) {
      **(p++) += v;
    }
    return *this;
  }
};
