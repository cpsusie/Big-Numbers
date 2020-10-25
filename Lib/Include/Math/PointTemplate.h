#pragma once

#include <BasicIncludes.h>
#include "FixedSizeVector.h"

template<typename T, UINT dimension> class SizeTemplate : public FixedSizeVectorTemplate<T, dimension> {
public:
  inline SizeTemplate() {
  }
  // copy constructor
  template<typename S> SizeTemplate(const FixedSizeVectorTemplate<S, dimension> &src)
    : FixedSizeVectorTemplate<T, dimension>(src)
  {
  }
  // copy constructor
  template<typename S> SizeTemplate(const S *src)
    : FixedSizeVectorTemplate<T, dimension>(src)
  {
  }
  template<typename S, typename... Tail> inline SizeTemplate(const S &x0, Tail... tail)
    : FixedSizeVectorTemplate<T, dimension>(x0, tail...)
  {
  }
  SizeTemplate                      operator*(const T &k) const {
    return __super::operator*(k);
  }
  SizeTemplate                      operator/(const T &k) const {
    return __super::operator/(k);
  }
  template<typename S> T            operator*(const FixedSizeVectorTemplate<S, dimension> &v) const {
    return __super::operator*(v);
  }
};

template<typename T, UINT dimension> class PointTemplate : public FixedSizeVectorTemplate<T, dimension> {
public:
  inline PointTemplate() {
  }
  // copy constructor
  template<typename S> PointTemplate(const FixedSizeVectorTemplate<S, dimension> &src)
    : FixedSizeVectorTemplate<T, dimension>(src)
  {
  }
  // copy constructor
  template<typename S> PointTemplate(const S *src)
    : FixedSizeVectorTemplate<T, dimension>(src)
  {
  }
  template<typename S, typename... Tail> inline PointTemplate(const S &x0, Tail... tail)
    : FixedSizeVectorTemplate<T, dimension>(x0, tail...)
  {
  }
  template<typename S> PointTemplate            &operator=(const FixedSizeVectorTemplate<S, dimension> &v) {
    __super::operator=(v);
    return *this;
  }
  PointTemplate                                  operator*(const T &k) const {
    return __super::operator*(k);
  }
  PointTemplate                                  operator/(const T &k) const {
    return __super::operator/(k);
  }
  template<typename S> PointTemplate             operator+(const FixedSizeVectorTemplate<S, dimension> &v) const {
    return v + *this;
  }
  template<typename S> PointTemplate             operator-(const FixedSizeVectorTemplate<S, dimension> &v) const {
    return (PointTemplate<T, dimension>)__super::operator-(v);
  }

  template<typename S> PointTemplate             &operator+=(const FixedSizeVectorTemplate<S, dimension> &v) {
    __super::operator+=(v);
    return *this;
  }
  template<typename S> PointTemplate             &operator-=(const FixedSizeVectorTemplate<S, dimension> &v) {
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
  template<typename S> T                          operator*(const FixedSizeVectorTemplate<S, dimension> &v) const {
    return __super::operator*(v);
  }
};

template<typename T1, typename T2, UINT dimension> double distance(const PointTemplate<T1, dimension> &p1, const PointTemplate<T2, dimension> &p2) {
  return (p1 - p2).length();
}

template<typename T, UINT dimension, UINT coordIndex> class PointComparator : public Comparator<PointTemplate<T,dimension> > {
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
  int compare(const PointTemplate<T,dimension> &p1, const PointTemplate<T,dimension> &p2) override {
    return m_maximum ? sign(p2[coordIndex] - p1[coordIndex]) : sign(p1[coordIndex] - p2[coordIndex]);
  }
};
