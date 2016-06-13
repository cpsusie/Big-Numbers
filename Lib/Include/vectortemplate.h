#pragma once

#include "MyUtil.h"
#include <Math.h>
#include "StreamParameters.h"

template <class T> class VectorTemplate {
private:
  T     *m_e;
  size_t m_dim;

  static void throwVectorException(const TCHAR *format, ...) {
    va_list argptr;
    va_start(argptr, format);
    throwException(_T("VectorTemplate:%s."), vformat(format, argptr).cstr());
    va_end(argptr);
  }

  void checkIndex(size_t index) const {
    if(index >= m_dim) {
      throwVectorException(_T("Index %s out of range. Dimension=%s")
                          ,format1000(index).cstr(), format1000(m_dim).cstr());
    }
  }

  static void throwOperatorException(const TCHAR *op, size_t dim1, size_t dim2) {
    throwException(_T("operator%s(VectorTemplate, VectorTemplate):Invalid dimension. Left.dimension=%s. Right.dimension=%s")
                  , op, format1000(dim1).cstr(), format1000(dim2).cstr());
  }

  static T *allocate(size_t dim, bool initialize) {
    if(dim == 0) {
      throwVectorException(_T("allocate:Dimension=0"));
    }
    T *v = new T[dim];
    if(initialize) {
      for(size_t i = 0; i < dim; i++) {
        v[i] = T(0);
      }
    }
    return v;
  }

  void cleanup() {
    delete[] m_e;
  }

protected:
  void init(size_t dim, bool initialize) {
    m_e   = allocate(dim, initialize);
    m_dim = dim;
  }

public:

  explicit VectorTemplate(size_t dim = 1) {
    init(dim, true);
  }

  VectorTemplate(const VectorTemplate<T> &src) {
    init(src.m_dim, false);
    for(size_t i = 0; i < m_dim; i++)
      m_e[i] = src.m_e[i];
  }

  VectorTemplate(const Array<T> &src) {
    init(src.size(), false);
    for(size_t i = 0; i < m_dim; i++) {
      m_e[i] = src[i];
    }
  }

  VectorTemplate<T> &operator=(const VectorTemplate<T> &src) {
    if(this == &src) {
      return *this;
    }
    if(src.m_dim != m_dim) {
      cleanup();
      init(src.m_dim, false);
    }
    for(size_t i = 0; i < m_dim; i++) {
      m_e[i] = src.m_e[i];
    }
    return *this;
  }

  virtual ~VectorTemplate() {
    cleanup();
  }

  VectorTemplate<T> &setDimension(size_t dim) {
    if(dim != m_dim) {
      T *newe = allocate(dim, true);
      size_t copyCount = __min(dim, m_dim);
      for(size_t i = 0; i < copyCount; i++) {
        newe[i] = m_e[i];
      }
      cleanup();
      m_dim = dim;
      m_e   = newe;
    }
    return *this;
  }

  size_t getDimension() const {
    return m_dim;
  }

  friend VectorTemplate<T> operator*(const T &d, const VectorTemplate<T> &rhs) {
    const size_t n = rhs.m_dim;
    VectorTemplate<T> result(n);
    for(size_t i = 0; i < n; i++) {
      result.m_e[i] = rhs.m_e[i] * d;
    }
    return result;
  }

  VectorTemplate<T> operator*(const T &d) const {
    const size_t n = m_dim;
    VectorTemplate<T> result(n);
    for(size_t i = 0; i < n; i++) {
      result.m_e[i] = m_e[i] * d;
    }
    return result;
  }
  
  VectorTemplate<T> operator/(const T &d) const {
    const size_t n = m_dim;
    VectorTemplate<T> result(n);
    for(size_t i = 0; i < n; i++) {
      result.m_e[i] = m_e[i] / d;
    }
    return result;
  }
  
  friend VectorTemplate<T> operator+(const VectorTemplate<T> &lts, const VectorTemplate<T> &rhs) {
    const size_t n = lts.m_dim;

    if(n != rhs.m_dim) {
      throwOperatorException(_T("+"), n, rhs.m_dim);
    }

    VectorTemplate<T> result(n);
    for(size_t i = 0; i < n; i++) {
      result.m_e[i] = lts.m_e[i] + rhs.m_e[i];
    }
    return result;
  }
  
  friend VectorTemplate<T> operator-(const VectorTemplate<T> &lts, const VectorTemplate<T> &rhs) {
    const size_t n = lts.m_dim;

    if(n != rhs.m_dim) {
      throwOperatorException(_T("-"), n, rhs.m_dim);
    }

    VectorTemplate<T> result(n);
    for(size_t i = 0; i < n; i++) {
      result.m_e[i] = lts.m_e[i] - rhs.m_e[i];
    }
    return result;
  }
  
  friend VectorTemplate<T> operator-(const VectorTemplate<T> &v) {
    const size_t n = v.m_dim;

    VectorTemplate<T> result(n);
    for(size_t i = 0; i < n; i++) {
      result.m_e[i] = -v.m_e[i];
    }
    return result;
  }

  friend T operator*(const VectorTemplate<T> &lts, const VectorTemplate<T> &rhs) {
    const size_t n = lts.m_dim;

    if(n != rhs.m_dim) {
      throwOperatorException(_T("*"), n, rhs.m_dim);
    }

    T sum = 0;
    T *lts_e = lts.m_e;
    T *rhs_e = rhs.m_e;
    for(size_t i = 0; i < n; i++) {
      sum += *(lts_e++) * *(rhs_e++);
    }
    return sum;
  }

  VectorTemplate<T> &operator*=(const T &d) {
    for(size_t i = 0; i < m_dim; i++) {
      m_e[i] *= d;
    }
    return *this;
  }

  VectorTemplate<T> &operator/=(const T &d) {
    for(size_t i = 0; i < m_dim; i++) {
      m_e[i] /= d;
    }
    return *this;
  }

  VectorTemplate<T> &operator+=(const VectorTemplate<T> &rhs) {
    if(m_dim != rhs.m_dim) {
      throwOperatorException(_T("+="), m_dim, rhs.m_dim);
    }
    for(size_t i = 0; i < m_dim; i++) {
      m_e[i] += rhs.m_e[i];
    }
    return *this;
  }

  VectorTemplate<T> &operator-=(const VectorTemplate<T> &rhs) {
    if(m_dim != rhs.m_dim) {
      throwOperatorException(_T("-="), m_dim, rhs.m_dim);
    }
    for(size_t i = 0; i < m_dim; i++) {
      m_e[i] -= rhs.m_e[i];
    }
    return *this;
  }

  T &operator[](size_t n) {
    checkIndex(n);
    return m_e[n];
  }

  const T &operator[](size_t n) const {
    checkIndex(n);
    return m_e[n];
  }

  T &operator()(size_t n) {
    checkIndex(n);
    return m_e[n];
  }

  const T &operator()(size_t n) const {
    checkIndex(n);
    return m_e[n];
  }

  T length() const {
    T sum = 0;
    for(size_t i = 0; i < m_dim; i++) {
      sum += m_e[i] * m_e[i];
    }
    return sqrt(sum);
  }

  bool operator==(const VectorTemplate<T> &v) {
    if(this == &v) {
      return true;
    }
    if(m_dim != v.m_dim) {
      return false;
    }
    for(size_t i = 0; i < m_dim; i++) {
      if(m_e[i] != v.m_e[i]) {
        return false;
      }
    }
    return true;
  }

  bool operator!=(const VectorTemplate<T> &v) {
    return !(*this == v);
  }

  virtual String toString() const {
    String result;
    for(size_t i = 0; i < m_dim; i++) {
      if(i > 0) {
        result += _T(" ");
      }
      result += ::toString(m_e[i]);
    }
    return result;
  }

  friend tostream &operator<<(tostream &out, const VectorTemplate<T> &v) {
    StreamParameters p(out);
    for(size_t i = 0; i < v.m_dim; i++) {
      if(i > 0) {
        out << _T(" ");
      }
      out << p << v.m_e[i];
    }
    return out;
  }

  friend tistream &operator>>(tistream &in, VectorTemplate<T> &v) {
    for(size_t i = 0; i < v.m_dim; i++) {
      in >> v[i];
    }
    return in;
  }

  String getDimensionString() const {
    return format(_T("Dimension=%s"), format1000(m_dim).cstr());
  }

};
