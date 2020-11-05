#include "stdafx.h"
#include <Math/Double80.h>
#include <MyString.h>

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
  inline FixedDimensionVector() {
  }
  // copy constructor
  template<typename S> FixedDimensionVector(const FixedDimensionVector<S, dimension> &src) {
    const S *sp   = src.begin();
    const T *endp = end();
    for(T *dp = begin(); dp < endp;) {
      *(dp++) = (T)(*(sp++));
    }
  }
  // copy constructor
  template<typename S> FixedDimensionVector(const S *src) {
    const S *sp   = src;
    const T *endp = end();
    for(T *dp = begin(); dp < endp;) {
      *(dp++) = (T)(*(sp++));
    }
  }

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

  inline T &operator[](UINT i) {
    assert(i < dimension);
    return m_v[i];
  }
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
  inline UINT getDim() const {
    return dimension;
  }

  // Return *this * k
  template<typename S> FixedDimensionVector<T, dimension> operator*(const S &k) const {
    FixedDimensionVector<T, dimension> result;
    const T                               tmp = (T)k;
    T *dp = result.begin();
    for(const T vk : m_v) {
      *(dp++) = vk * tmp;
    }
    return result;
  }
  template<typename S> friend inline FixedDimensionVector<T, dimension> operator*(const S &k, const FixedDimensionVector<T, dimension> &s) {
    return s * k;
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
  template<typename S> FixedDimensionVector<T, dimension> operator-(const FixedDimensionVector<S, dimension> &v) const {
    FixedDimensionVector<T, dimension> result;
    const S *sp = s.begin();
    T       *dp = result.begin();
    for(const T vk : m_v) {
      *(dp++) = vk - (T)(*(sp++));
    }
    return result;
  }

  // Unary mius
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

  // Return dot product = *this * s
  template<typename S> T operator*(const FixedDimensionVector<S, dimension> &s) const {
    T sum = 0;
    const S *sp = s.begin();
    for(const T vk : m_v) {
      sum += vk * (T)(*(sp++));
    }
    return sum;
  }

  // Return euclidian length
  T length() const {
    T sum = 0;
    for(const T vk : m_v) {
      sum += vk * vk;
    }
    return (T)sqrt(sum);
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

  T volume() const {
    const T v = (T)1;
    for(const T vk : m_v) {
      v *= vk;
    }
    return v;
  }

  inline bool operator==(const FixedDimensionVector<T, dimension> &s) const {
    const S *sp = s.begin();
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

  inline String toString(int precision = 3) const {
    String result;
    TCHAR delim = '(';
    for(UINT i = 0; i < dimension; i++, delim = ',') {
      result += delim;
      result += ::toString(m_v[i], precision);
    }
    result += ')';
    return result;
  }
};

typedef FixedDimensionVector<float   , 3> FVector3D;
typedef FixedDimensionVector<double  , 3> DVector3D;
typedef FixedDimensionVector<Double80, 3> D80Vector3D;

int main(int argc, TCHAR **argv) {
  const DVector3D v1(DBL80_MAX, DBL80_MIN, DBL80_EPSILON);
/*
  int ssi3[] = { 2,4,8 };
  double ssd3[] = { 2.6,-4.4,8.3 };
  double ssd2[] = { 2.6,-4.4 };
  const Vector3D v2(5, 6, 10);
  const Vector3D v3(ssi3);
  const Vector3D v4(ssd3);
  Vector3D v5(ssd2);
  const Vector3D v6(v2);
  v5 = v1;
  Vector3D s = v1 + v2;
  FVector3D fv(v1);
  double p1 = v1 * v2;
  double p2 = fv * v2;
  FVector3D fv1 = 2 * v1;
  FVector3D fv2 = -v1;
*/
  String str = v1.toString();

  return 0;
}
