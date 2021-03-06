#pragma once

#include <MyString.h>
#include <MyAssert.h>

template<typename T, UINT dimension> class FixedDimensionVector {
private:
  T m_v[dimension];

  template<typename V> inline void initList(UINT i, const V &v) {
    assert(i == dimension - 1);
    m_v[i] = (T)v;
  }
  template<typename V, typename... Tail> inline void initList(UINT i, const V &v, Tail... tail) {
    assert(i < dimension);
    m_v[i] = (T)v;
    initList(i+1, tail...);
  }
public:
  // Undefined coordinates
  inline FixedDimensionVector() {
  }
  // Copy constructor with another base type of src, but same dimension
  template<typename S> FixedDimensionVector(const FixedDimensionVector<S, dimension> &src) {
    const S *sp   = src.begin();
    const T *endp = end();
    for(T *dp = begin(); dp < endp;) {
      *(dp++) = (T)(*(sp++));
    }
  }

  // Number of elements in array is NOT checked, if not enough, the last coordinates is undefined
  template<typename S> FixedDimensionVector(const S *src) {
    const S *sp   = src;
    const T *endp = end();
    for(T *dp = begin(); dp < endp;) {
      *(dp++) = (T)(*(sp++));
    }
  }

  // Number of arguments must match dimension
  template<typename S, typename... Tail> inline FixedDimensionVector(const S &x0, Tail... tail) {
    initList(0, x0, tail...);
  }
  // Assignment
  // return *this
  template<typename S> FixedDimensionVector<T, dimension> &operator=(const FixedDimensionVector<S, dimension> &src) {
    const S *sp   = src.begin();
    const T *endp = end();
    for(T *dp = begin(); dp < endp;) {
      *(dp++) = (T)(*(sp++));
    }
    return *this;
  }

  // Return reference to coordinate i
  inline T &operator[](UINT i) {
    assert(i < dimension);
    return m_v[i];
  }
  // Return const reference to coordinate i
  inline const T &operator[](UINT i) const {
    assert(i < dimension);
    return m_v[i];
  }

  // Iterator functions
  inline T *begin() {
    return m_v;
  }
  inline T *end() {
    return m_v + dimension;
  }
  inline const T *begin() const {
    return m_v;
  }
  inline const T *end() const {
    return m_v + dimension;
  }

  // Return dimension
  inline UINT getDimension() const {
    return dimension;
  }

  // Set all coordinates to v
  // Return *this
  template<typename S> FixedDimensionVector &setAll(const S &v) {
    const T tmp = (S)v;
    const T *endp = end();
    for(T *dp = begin(); dp < endp;) {
      *(dp++) = tmp;
    }
    return *this;
  }
  // Set all elements to 0
  // Return *this
  inline FixedDimensionVector &clear() {
    return setAll(0);
  }

  static FixedDimensionVector<T, dimension> nullVector() {
    return FixedDimensionVector<T, dimension>().clear();
  }

  // Return *this * k
  template<typename S> FixedDimensionVector<T, dimension> operator*(const S &k) const {
    FixedDimensionVector<T, dimension> result;
    const T                               tmp = (const T)k;
    T *dp = result.begin();
    for(const T vk : m_v) {
      *(dp++) = vk * tmp;
    }
    return result;
  }

  // Return *this / k
  template<typename S> FixedDimensionVector<T, dimension> operator/(const S &k) const {
    FixedDimensionVector<T, dimension> result;
    const T                               tmp = (T)k;
    T *dp = result.begin();
    for(const T vk : m_v) {
      *(dp++) = vk / tmp;
    }
    return result;
  }

  // operator*=, /= by scalar
  // Return *this
  template<typename S> FixedDimensionVector<T, dimension> &operator*=(const S &k) {
    const T tmp = (T)k, *endp = end();
    for(T *vp = begin(); vp < endp;) {
      *(vp++) *= tmp;
    }
    return *this;
  }

  // Return *this
  template<typename S> FixedDimensionVector<T, dimension> &operator/=(const S &k) {
    const T tmp = (T)k, *endp = end();
    for(T *vp = begin(); vp < endp;) {
      *(vp++) /= tmp;
    }
    return *this;
  }

  // Return *this + s
  template<typename S> FixedDimensionVector<T, dimension> operator+(const FixedDimensionVector<S, dimension> &s) const {
    FixedDimensionVector<T, dimension> result;
    const S *sp = s.begin();
    T       *dp = result.begin();
    for(const T vk : m_v) {
      *(dp++) = vk + (T)(*(sp++));
    }
    return result;
  }
  // Return *this - s
  template<typename S> FixedDimensionVector<T, dimension> operator-(const FixedDimensionVector<S, dimension> &s) const {
    FixedDimensionVector<T, dimension> result;
    const S *sp = s.begin();
    T       *dp = result.begin();
    for(const T vk : m_v) {
      *(dp++) = vk - (T)(*(sp++));
    }
    return result;
  }

  // Unary minus
  // Return -(*this)
  FixedDimensionVector<T, dimension> operator-() const {
    FixedDimensionVector<T, dimension> result;
    T *dp = result.begin();
    for(const T vk : m_v) {
      *(dp++) = -vk;
    }
    return result;
  }

  // Vector operator+=, -=
  // Return *this
  template<typename S> FixedDimensionVector<T, dimension> &operator+=(const FixedDimensionVector<S, dimension> &s) {
    const S *sp   = s.begin();
    const T *endp = end();
    for(T *vp = begin(); vp < endp;) {
      *(vp++) += (T)(*(sp++));
    }
    return *this;
  }

  // Return *this
  template<typename S> FixedDimensionVector<T, dimension> &operator-=(const FixedDimensionVector<S, dimension> &s) {
    const S *sp   = s.begin();
    const T *endp = end();
    for(T *vp = begin(); vp < endp;) {
      *(vp++) -= (T)(*(sp++));
    }
    return *this;
  }

  inline T length2() const {
    T sum = 0;
    for(const T vk : m_v) {
      sum += vk * vk;
    }
    return sum;
  }

  // Return euclidian length
  inline T length() const {
    return (T)sqrt(length2());
  }

  // Return dot product = *this * s
  template<typename S> T operator*(const FixedDimensionVector<S, dimension> &s) const {
    T sum = 0;
    const S *sp = s.begin();
    for(const T vk : m_v) {
      sum += vk * (T)(*(sp++));
    }
    return sum;
  }

  // Divide this by it's length to make it a unit-vector. leave unchanged if = 0-vector
  // Return *this
  FixedDimensionVector<T, dimension> &normalize() {
    const T l = length();
    if(l != (T)0) {
      *this /= l;
    }
    return *this;
  }

  // Return product of all coordinates
  T volume() const {
    T v = (T)1;
    for(const T vk : m_v) {
      v *= vk;
    }
    return v;
  }

  inline bool operator==(const FixedDimensionVector<T, dimension> &s) const {
    const T *sp = s.begin();
    for(T vk : m_v) {
      if(vk != *(sp++)) {
        return false;
      }
    }
    return true;
  }
  inline bool operator!=(const FixedDimensionVector<T, dimension> &s) const {
    return !(*this == s);
  }

  // Return true if for all corresponding coordinates in this is < s, or else false
  template<typename S> inline bool operator<(const FixedDimensionVector<S, dimension> &s) const {
    const S *sp = s.begin();
    for(T vk : m_v) {
      if(vk >= *(sp++)) {
        return false;
      }
    }
    return true;
  }
  // Return true if for all corresponding coordinates in this is <= s, or else false
  template<typename S> inline bool operator<=(const FixedDimensionVector<S, dimension> &s) const {
    const S *sp = s.begin();
    for(T vk : m_v) {
      if(vk > *(sp++)) {
        return false;
      }
    }
    return true;
  }

  String toString(std::streamsize precision = 3) const;
};

template<typename OSTREAMTYPE, typename T, UINT dimension> OSTREAMTYPE &operator<<(OSTREAMTYPE &out, const FixedDimensionVector<T, dimension> &v) {
  const StreamSize w = out.width();
  out << "[";
  for(UINT i = 0; i < dimension; i++) {
    if(i > 0) {
      out << " ";
    }
    out.width(w);
    out << v[i];
  }
  out << "]";
  return out;
}

template<typename ISTREAMTYPE, typename T, UINT dimension> ISTREAMTYPE &operator>>(ISTREAMTYPE &in, FixedDimensionVector<T, dimension> &v) {
  const FormatFlags flgs = in.flags();
  in.flags(flgs | std::ios::skipws);
  if(in.peek() == '[') {
    in.get();
  } else {
    in.flags(flgs);
    in.setstate(std::ios::failbit);
    return in;
  }
  for(UINT i = 0; i < dimension; i++) {
    in >> v[i];
  }
  if(in.peek() == ']') {
    in.get();
  } else {
    in.setstate(std::ios::failbit);
  }
  in.flags(flgs);
  return in;
}

template<typename T, UINT dimension> String FixedDimensionVector<T, dimension>::toString(std::streamsize prec) const {
  std::wostringstream out;
  out.precision(prec);
  out << *this;
  return out.str().c_str();
}


template<typename S, typename T, UINT dimension> FixedDimensionVector<T, dimension> operator*(const S &k, const FixedDimensionVector<T, dimension> &s) {
  return s * k;
}

template<typename T, UINT dimension> FixedDimensionVector<T, dimension> unitVector(const FixedDimensionVector<T, dimension> &v) {
  FixedDimensionVector<T, dimension> tmp(v);
  return tmp.normalize();
}

// Return vector V, where V[i] = min(v1[i],v2[i]), i=0..dimension-1
template<typename T1, typename T2, UINT dimension> FixedDimensionVector<T1, dimension> Min(const FixedDimensionVector<T1, dimension> &v1, const FixedDimensionVector<T2, dimension> &v2) {
  FixedDimensionVector<T1, dimension> result;
  for(UINT i = 0; i < dimension; i++) {
    result[i] = min((T1)v1[i], (T1)v2[i]);
  }
  return result;
}

// Return vector V, where V[i] = max(v1[i],v2[i]), i=0..dimension-1
template<typename T1, typename T2, UINT dimension> FixedDimensionVector<T1, dimension> Max(const FixedDimensionVector<T1, dimension> &v1, const FixedDimensionVector<T2, dimension> &v2) {
  FixedDimensionVector<T1, dimension> result;
  for(UINT i = 0; i < dimension; i++) {
    result[i] = max((T1)v1[i], (T1)v2[i]);
  }
  return result;
}

// Return angle in radians between v1 and v2, in range [0..pi]
template<typename T1, typename T2, UINT dimension> double angle(const FixedDimensionVector<T1, dimension> &v1, const FixedDimensionVector<T2,dimension> &v2) {
  const T1 l1 = v1.length();
  const T2 l2 = v2.length();
  if((l1 == 0) || (l2 == 0)) {
    return 0;
  } else {
    const T1 q = (v1 * v2) / ((T1)(l1 * l2));
    if(q <= (T1)-1) {
      return M_PI;
    } else if(q >= (T1)1) {
      return 0;
    } else {
      return acos(q);
    }
  }
}

template<typename T, UINT dimension> FixedDimensionVector<T, dimension> floor(const FixedDimensionVector<T, dimension> &v) {
  FixedDimensionVector<T, dimension> result;
  for(UINT i = 0; i < dimension; i++) {
    result[i] = floor(v[i]);
  }
  return result;
}

template<typename T, UINT dimension> FixedDimensionVector<T, dimension> ceil(const FixedDimensionVector<T, dimension> &v) {
  FixedDimensionVector<T, dimension> result;
  for(UINT i = 0; i < dimension; i++) {
    result[i] = ceil(v[i]);
  }
  return result;
}

template<typename T, UINT dimension> FixedDimensionVector<T, dimension> fraction(const FixedDimensionVector<T, dimension> &v) {
  FixedDimensionVector<T, dimension> result;
  for(UINT i = 0; i < dimension; i++) {
    result[i] = fraction(v[i]);
  }
  return result;
}


template<typename T> class Vector2DTemplate : public FixedDimensionVector<T, 2> {
};

// Special for 2D
template<typename T1, typename T2> double det(const FixedDimensionVector<T1, 2> &v0
                                             ,const FixedDimensionVector<T2, 2> &v1) {
  return (double)(v0[0]*v1[1] - v0[1]*v1[0]);
}

template<typename T, typename S> FixedDimensionVector<T, 2> rotate(const FixedDimensionVector<T, 2> &v, const S &rad) {
  const double cs = (double)cos(rad), sn = (double)sin(rad);
  return FixedDimensionVector<T, 2>(cs * v[0] - sn * v[1], sn * v[0] + cs * v[1]);
}

template<typename T1, typename T2> double angle2D(const FixedDimensionVector<T1, 2> &v1, const FixedDimensionVector<T2, 2> &v2) {
  return angle(v1,v2) * sign(det(v1,v2));
}

// Special for 3D

template<typename T> class Vector3DTemplate : public FixedDimensionVector<T, 3> {
};

template<typename T1, typename T2, typename T3> double det(const FixedDimensionVector<T1, 3> &v0
                                                          ,const FixedDimensionVector<T2, 3> &v1
                                                          ,const FixedDimensionVector<T3, 3> &v2)
{
  return (double)((v0[0]*v1[1]*v2[2] + v0[1]*v2[0]*v1[2] + v1[0]*v0[2]*v2[1])
                - (v0[2]*v1[1]*v2[0] + v0[1]*v2[2]*v1[0] + v2[1]*v0[0]*v1[2]));
}

template<typename T1, typename T2> FixedDimensionVector<T1, 3> cross(const FixedDimensionVector<T1, 3> &a
                                                                    ,const FixedDimensionVector<T2, 3> &b) {
  return FixedDimensionVector<T1, 3>(a[1]*b[2] - a[2]*b[1]
                                    ,a[2]*b[0] - a[0]*b[2]
                                    ,a[0]*b[1] - a[1]*b[0]
                                    );
}
