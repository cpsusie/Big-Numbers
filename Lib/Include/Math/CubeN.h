#pragma once

#include <MyAssert.h>

template<typename T, UINT dimension> class SizeNTemplate {
protected:
  T m_v[dimension];
public:
  inline T &operator[](UINT i) {
    assert(i < dimension);
    return m_v[i];
  }
  inline const T &operator[](UINT i) const {
    assert(i < dimension);
    return m_v[i];
  }
  T *begin() {
    return m_v;
  }
  T *end() {
    return m_v + dimension;
  }
  const T *begin() const {
    return m_v
  }
  const T *end() const {
    return m_v + dimension;
  }
  inline UINT getDim() const {
    return dimension;
  }
  template<typename S> SizeNTemplate<T,dimension> &operator=(const SizeNTemplate<S,dimension> &s) {
    const S *sk = s.m_v;
    for(T vk : m_v) {
      vk = (T)(*(sk++));
    }
    return *this;
  }
  template<typename S> SizeNTemplate<T,dimension> &operator*=(const S &k) {
    const T tmp = (T)k;
    for(T v : m_v) {
      v *= tmp;
    }
    return this;
  }
  template<typename S> SizeNTemplate<T,dimension> &operator/=(const S &k) {
    const T tmp = (T)k;
    for(T v : m_v) {
      v /= tmp;
    }
    return this;
  }
  template<typename S> SizeNTemplate<T,dimension> operator*(const S &k) {
    const T tmp = (T)k;
    SizeNTemplate<T, dimension> result;
    T *rk = result.m_v;
    for(T vk : m_v) {
      *(rk++) = vk * tmp;
    }
    return result;
  }
  template<typename S> friend SizeNTemplate<T, dimension> operator*(const S &k, const SizeNTemplate<T, dimension> &s) {
    return s * k;
  }
  template<typename S> SizeNTemplate<T,dimension> operator/(const S &k) {
    const T tmp = (T)k;
    SizeNTemplate<T, dimension> result;
    T *rk = result.m_v;
    for(T vk : m_v) {
      *(rk++) = vk / tmp;
    }
    return result;
  }

  T length() const {
    T sum = 0;
    for(const T vk : m_v) {
      sum += vk * vk;
    }
    return (T)sqrt(sum);
  }
  template<typename S> SizeNTemplate<T, dimension> &normalize() {
    const T l = length();
    if(l != 0) {
      *this /= l;
    }
    return *this;
  }
  T volume() const {
    const T v = (T)1;
    for(const T vk : m_v) {
      v *= vk;
    }
    return v;
  }
};

template<typename T, UINT dimension> class PointNTemplate {
protected:
  T m_v[dimension];
public:
  inline T &operator[](UINT i) {
    assert(i < dimension);
    return m_v[i];
  }
  inline const T &operator[](UINT i) const {
    assert(i < dimension);
    return m_v[i];
  }
  T *begin() {
    return m_v;
  }
  T *end() {
    return m_v + dimension;
  }
  const T *begin() const {
    return m_v
  }
  const T *end() const {
    return m_v + dimension;
  }
  inline UINT getDim() const {
    return dimension;
  }
  template<typename S> PointNTemplate<T,dimension> &operator=(const PointNTemplate<S,dimension> &p) {
    const S *pk = p.m_v;
    for(T vk : m_v) {
      vk = (T)(*(pk++));
    }
    return *this;
  }

  template<typename S> PointNTemplate<T, dimension> &operator*=(const S &k) {
    const T tmp = (T)k;
    for(T v : m_v) {
      v *= tmp;
    }
    return this;
  }
  template<typename S> PointNTemplate<T, dimension> &operator/=(const S &k) {
    const T tmp = (T)k;
    for(T v : m_v) {
      v /= tmp;
    }
    return this;
  }
  template<typename S> PointNTemplate<T, dimension> &operator+=(const SizeNTemplate<S, dimension> &s) {
    const S *sk = &s[0];
    for(T vk : m_v) {
      vk += *(sk++);
    }
    return *this;
  }
  template<typename S> PointNTemplate<T, dimension> &operator-=(const SizeNTemplate<S, dimension> &s) {
    const S *sk = &s[0];
    for(T vk : m_v) {
      vk -= *(sk++);
    }
    return *this;
  }
  template<typename S> T operator*(const PointNTemplate<S, dimension> &v) const {
    T sum = 0;
    const S *v2 = v.m_v;
    for(const T vk : m_v) {
      sum += vk * *(v2++);
    }
    return sum;
  }
  template<typename S> SizeNTemplate<T, dimension> operator-(const PointNTemplate<S, dimension> &p) {
    SizeNTemplate<T, dimension> s;
    const S *pk = &p[0];
    T *sk = &s[0];
    for(T vk : m_v) {
      *(sk++) = vk * *(pk++);
    }
    return s;
  }
  T length() const {
    T sum = 0;
    for(const T vk : m_v) {
      sum += vk * vk;
    }
    return (T)sqrt(sum);
  }
  template<typename S> PointNTemplate<T, dimension> &normalize() {
    const T l = length();
    if(l != 0) {
      *this /= l;
    }
    return *this;
  }
};

template<typename T, UINT dimension> class CubeNTemplate {
protected:
  PointNTemplate<T, dimension> m_p0;
  SizeNTemplate< T, dimension> m_size;
public:
  PointNTemplate<T, dimension> &p0() {
    return m_p0;
  }
  const PointNTemplate<T, dimension> &p0() const {
    return m_p0;
  }
  SizeNTemplate<T, dimension> &size() {
    return m_size;
  }
  const SizeNTemplate<T, dimension> &size() const {
    return m_size;
  }
  // return this
  template<typename S> CubeNTemplate<T,dimension> &setInterval(UINT index, const NumberInterval<S> &interval) {
    m_p0[  index] = interval.getFrom();
    m_size[index] = interval.getLength();
    return *this;
  }
  NumberInterval<T> getInterval(UINT index) const {
    const T &p = m_p0[index];
    return NumberInterval<T>(p, p + m_size[index]);
  }
  inline UINT getDim() const {
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
    return (l <= 0) ? m_p0[index] : (m_p0[index] - l);
  }

  PointNTemplate<T, dimension> getAllMin() const {
    PointNTemplate<T, dimension> p;
    for(UINT i = 0; i < dimension; i++) {
      p[i] = getMin(i);
    }
    return p;
  }
  PointNTemplate<T, dimension> getAllMax() const {
    PointNTemplate<T, dimension> p;
    for(UINT i = 0; i < dimension; i++) {
      p[i] = getMax(i);
    }
    return p;
  }
};
