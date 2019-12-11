#pragma once

#include <Math.h>
#include "Array.h"
#include "Exception.h"
#include "StreamParameters.h"


template <typename T> class VectorTemplate {
private:
  AbstractVectorAllocator<T> *m_va;
  T                          *m_e;
  size_t                      m_dim;

  static void throwVectorException(const TCHAR *method, _In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
    va_list argptr;
    va_start(argptr, format);
    const String msg = vformat(format, argptr);
    va_end(argptr);
    throwException(_T("%s:%s."), method, msg.cstr());
  }
  void throwIndexError(const TCHAR *method, const size_t index) const {
    throwVectorException(method,_T("Index %s out of range. %s")
                               ,format1000(index).cstr(), getDimensionString().cstr());
  }

#define CHECKVECTORTEMPLATEINDEX(index) if((index) >= m_dim) throwIndexError(__TFUNCTION__, index)

  T *allocate(size_t dim, bool initialize) const {
    if(dim == 0) return NULL;
    T *v = m_va ? m_va->allocVector(dim) : new T[dim]; TRACE_NEW(v);
    if(initialize) {
      for(size_t i = 0; i < dim; i++) {
        v[i] = T(0);
      }
    }
    return v;
  }

  inline void cleanup() {
    SAFEDELETEARRAY(m_e);
  }

protected:
  void init(size_t dim, bool initialize) {
    m_e   = allocate(dim, initialize);
    m_dim = dim;
  }

  inline AbstractVectorAllocator<T> *getVectorAllocator() const {
    return m_va;
  }

  static void throwDimensionException(const TCHAR *method, const String &dim1, const String &dim2) {
    throwVectorException(method, _T("Invalid dimension. Left.%s. Right.%s"), dim1.cstr(), dim2.cstr());
  }

public:
  explicit VectorTemplate(AbstractVectorAllocator<T> *va = NULL) : m_va(va), m_dim(0), m_e(NULL) {
  }
  explicit VectorTemplate(size_t dim, AbstractVectorAllocator<T> *va = NULL) : m_va(va) {
    init(dim, true);
  }

  VectorTemplate(const VectorTemplate<T> &src, AbstractVectorAllocator<T> *va = NULL) : m_va(va) {
    init(src.m_dim, false);
    for(size_t i = 0; i < m_dim; i++) {
      m_e[i] = src.m_e[i];
    }
  }

  VectorTemplate(const Array<T> &src, AbstractVectorAllocator<T> *va = NULL) : m_va(va) {
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

  void checkSameDimension(const TCHAR *method, const VectorTemplate &v) const {
    if(getDimension() != v.getDimension()) {
      throwDimensionException(method, getDimensionString(), v.getDimensionString());
    }
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

  inline size_t getDimension() const {
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
    lts.checkSameDimension(__TFUNCSIG__, rhs);
    const size_t n = lts.m_dim;
    VectorTemplate<T> result(n);
    for(size_t i = 0; i < n; i++) {
      result.m_e[i] = lts.m_e[i] + rhs.m_e[i];
    }
    return result;
  }

  friend VectorTemplate<T> operator-(const VectorTemplate<T> &lts, const VectorTemplate<T> &rhs) {
    lts.checkSameDimension(__TFUNCSIG__, rhs);
    const size_t n = lts.m_dim;
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
    lts.checkSameDimension(__TFUNCSIG__, rhs);
    const size_t n = lts.m_dim;
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
    checkSameDimension(__TFUNCSIG__, rhs);
    for(size_t i = 0; i < m_dim; i++) {
      m_e[i] += rhs.m_e[i];
    }
    return *this;
  }

  VectorTemplate<T> &operator-=(const VectorTemplate<T> &rhs) {
    checkSameDimension(__TFUNCSIG__, rhs);
    for(size_t i = 0; i < m_dim; i++) {
      m_e[i] -= rhs.m_e[i];
    }
    return *this;
  }

  inline T &operator[](size_t n) {
    CHECKVECTORTEMPLATEINDEX(n);
    return m_e[n];
  }

  inline const T &operator[](size_t n) const {
    CHECKVECTORTEMPLATEINDEX(n);
    return m_e[n];
  }

  inline T &operator()(size_t n) {
    CHECKVECTORTEMPLATEINDEX(n);
    return m_e[n];
  }

  inline const T &operator()(size_t n) const {
    CHECKVECTORTEMPLATEINDEX(n);
    return m_e[n];
  }

  inline T &select(RandomGenerator &rnd = *RandomGenerator::s_stdGenerator) {
    return m_e[randSizet(m_dim, rnd)];
  }
  inline const T &select(RandomGenerator &rnd = *RandomGenerator::s_stdGenerator) const {
    return m_e[randSizet(m_dim, rnd)];
  }

  T length() const {
    T sum = 0;
    for(size_t i = 0; i < m_dim; i++) {
      sum += m_e[i] * m_e[i];
    }
    return sqrt(sum);
  }

  bool operator==(const VectorTemplate<T> &v) const {
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

  inline bool operator!=(const VectorTemplate<T> &v) const {
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

  template<typename OSTREAMTYPE> friend OSTREAMTYPE &operator<<(OSTREAMTYPE &out, const VectorTemplate<T> &v) {
    const StreamSize w = out.width();
    for(size_t i = 0; i < v.m_dim; i++) {
      if(i > 0) {
        out << " ";
      }
      out.width(w);
      out << v.m_e[i];
    }
    return out;
  }

  template<typename ISTREAMTYPE> friend ISTREAMTYPE &operator>>(ISTREAMTYPE &in, VectorTemplate<T> &v) {
    const FormatFlags flg = in.flags();
    in.flags(flg | std::ios::skipws);
    for(size_t i = 0; i < v.m_dim; i++) {
      in >> v[i];
    }
    in.flags(flg);
    return in;
  }

  inline String getDimensionString() const {
    return format(_T("Dimension=%s"), formatSize(m_dim).cstr());
  }
};
