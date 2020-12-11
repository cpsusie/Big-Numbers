#pragma once

#include <Math.h>
#include "Array.h"
#include "Exception.h"
#include "StreamParameters.h"

template<typename T> class VectorTemplate {
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
    if(dim == 0) return nullptr;
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

  class VectorIterator : public AbstractIterator {
  private:
    VectorTemplate &m_v;
    const size_t    m_dim;
    size_t          m_next;
    intptr_t        m_current;
  public:
    VectorIterator(const VectorTemplate *v)
      : m_v(*(VectorTemplate*)v)
      , m_dim(v->getDimension())
    {
      m_next        =  0;
      m_current     = -1;
    }
    AbstractIterator *clone()          override {
      return new VectorIterator(*this);
    }
    bool hasNext()               const override {
      return m_next < m_dim;
    }
    void *next()                       override {
      assert(m_dim == m_v.getDimension());
      if(m_next >= m_dim) {
        noNextElementError(__TFUNCTION__);
      }
      return &m_v[m_current = m_next++];
    }
    void remove()                      override {
      unsupportedOperationError(__TFUNCTION__);
    }
  };

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
  explicit VectorTemplate(AbstractVectorAllocator<T> *va = nullptr) : m_va(va), m_dim(0), m_e(nullptr) {
  }
  explicit VectorTemplate(size_t dim, AbstractVectorAllocator<T> *va = nullptr) : m_va(va) {
    init(dim, true);
  }

  VectorTemplate(const VectorTemplate &src, AbstractVectorAllocator<T> *va = nullptr) : m_va(va) {
    init(src.m_dim, false);
    if(m_dim) {
      for(T *dstp = m_e, *endp = dstp + m_dim, *srcp = src.m_e; dstp < endp;) {
        *(dstp++) = *(srcp++);
      }
    }
  }

  VectorTemplate(const Array<T> &src, AbstractVectorAllocator<T> *va = nullptr) : m_va(va) {
    init(src.size(), false);
    for(size_t i = 0; i < m_dim; i++) {
      m_e[i] = src[i];
    }
  }

  VectorTemplate &operator=(const VectorTemplate &src) {
    if(this == &src) {
      return *this;
    }
    if(src.m_dim != m_dim) {
      cleanup();
      init(src.m_dim, false);
    }
    if(m_dim) {
      for(T *dstp = m_e, *endp = dstp + m_dim, *srcp = src.m_e; dstp < endp;) {
        *(dstp++) = *(srcp++);
      }
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

  VectorTemplate &setDimension(size_t dim) {
    if(dim != m_dim) {
      T *newe = allocate(dim, true);
      const size_t copyCount = __min(dim, m_dim);
      for(T *dstp = newe, *endp = dstp + copyCount, *srcp = m_e; dstp < endp;) {
        *(dstp++) = *(srcp++);
      }
      cleanup();
      m_dim = dim;
      m_e   = newe;
    }
    return *this;
  }

  inline UINT getDimension() const {
    return (UINT)m_dim;
  }

  friend VectorTemplate operator*(const T &d, const VectorTemplate &rhs) {
    VectorTemplate result(rhs.getDimension());
    if(result.m_dim) {
      for(T *dstp = result.m_e, *endp = dstp + result.m_dim, *rhsp = rhs.m_e; dstp < endp;) {
        *(dstp++) = *(rhsp++) * d;
      }
    }
    return result;
  }

  VectorTemplate operator*(const T &d) const {
    VectorTemplate result(m_dim);
    if(m_dim) {
      for(T *dstp = result.m_e, *endp = dstp + m_dim, *srcp = m_e; dstp < endp;) {
        *(dstp++) = *(srcp++) * d;
      }
    }
    return result;
  }

  template<typename S> VectorTemplate operator/(const S &d) const {
    VectorTemplate result(m_dim);
    if(m_dim) {
      for(T *dstp = result.m_e, *endp = dstp + m_dim, *srcp = m_e; dstp < endp;) {
        *(dstp++) = *(srcp++) / d;
      }
    }
    return result;
  }

  friend VectorTemplate operator+(const VectorTemplate &lts, const VectorTemplate &rhs) {
    lts.checkSameDimension(__TFUNCSIG__, rhs);
    const size_t n = lts.m_dim;
    VectorTemplate result(n);
    if(n) {
      for(T *dstp = result.m_e, *endp = dstp + n, *lp = lts.m_e, *rp = rhs.m_e; dstp < endp;) {
        *(dstp++) = *(lp++) + *(rp++);
      }
    }
    return result;
  }

  friend VectorTemplate operator-(const VectorTemplate &lts, const VectorTemplate &rhs) {
    lts.checkSameDimension(__TFUNCSIG__, rhs);
    const size_t   n = lts.m_dim;
    VectorTemplate result(n);
    if(n) {
      for(T *dstp = result.m_e, *endp = dstp + n, *lp = lts.m_e, *rp = rhs.m_e; dstp < endp;) {
        *(dstp++) = *(lp++) - *(rp++);
      }
    }
    return result;
  }

  friend VectorTemplate operator-(const VectorTemplate &v) {
    const size_t n = v.m_dim;
    VectorTemplate result(n);
    if(n) {
      for(T *dstp = result.m_e, *endp = dstp + n, *srscp = v.m_e; dstp < endp;) {
        *(dstp++) = -(*(srcp++));
      }
    }
    return result;
  }

  friend T operator*(const VectorTemplate &lts, const VectorTemplate &rhs) {
    lts.checkSameDimension(__TFUNCSIG__, rhs);
    const size_t n = lts.m_dim;
    T sum = 0;
    if(n) {
      for(T *lp = lts.m_e, *endp = lp + n, *rp = rhs.m_e; lp < endp;) {
        sum += *(lp++) * *(rp++);
      }
    }
    return sum;
  }

  template<typename S> VectorTemplate &operator*=(const S &d) {
    if(m_dim) {
      for(T *vp = m_e, *endp = vp + m_dim; vp < endp;) {
        *(vp++) *= d;
      }
    }
    return *this;
  }

  template<typename S> VectorTemplate &operator/=(const S &d) {
    if(m_dim) {
      for(T *vp = m_e, *endp = vp + m_dim; vp < endp;) {
        *(vp++) /= d;
      }
    }
    return *this;
  }

  VectorTemplate &operator+=(const VectorTemplate &rhs) {
    checkSameDimension(__TFUNCSIG__, rhs);
    if(m_dim) {
      for(T *dstp = m_e, *endp = dstp + m_dim, *srcp = rhs.m_e; dstp < endp;) {
        *(dstp++) += *(srcp++);
      }
    }
    return *this;
  }

  VectorTemplate &operator-=(const VectorTemplate &rhs) {
    checkSameDimension(__TFUNCSIG__, rhs);
    if(m_dim) {
      for(T *dstp = m_e, *endp = dstp + m_dim, *srcp = rhs.m_e; dstp < endp;) {
        *(dstp++) -= *(srcp++);
      }
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
    if(m_dim) {
      for(T *vp = m_e, *endp = vp + m_dim; vp < endp; vp++) {
        sum += *vp * *vp;
      }
    }
    return sqrt(sum);
  }

  ConstIterator<T> getIterator() const {
    return ConstIterator<T>(new VectorIterator(this));
  }
  Iterator<T>      getIterator() {
    return Iterator<T>(new VectorIterator(this));
  }

  bool operator==(const VectorTemplate &rhs) const {
    if(this == &rhs) {
      return true;
    }
    if(m_dim != rhs.m_dim) {
      return false;
    }
    if(m_dim) {
      for(const T *lp = m_e, *endp = lp + m_dim, *rp = rhs.m_e; lp < endp;) {
        if(!(*(lp++) == *(rp++))) {
          return false;
        }
      }
    }
    return true;
  }

  inline bool operator!=(const VectorTemplate &v) const {
    return !(*this == v);
  }

  ULONG hashCode() const {
    ULONG sum = 0;
    if(m_dim) {
      for(const T *vp = m_e, *endp = vp + m_dim; vp < endp;) {
        sum = sum * 31 + (vp++)->hashCode();
      }
    }
    return sum;
  }

  inline T        *begin() {
    return m_e;
  }
  inline T        *end() {
    return m_e + m_dim;
  }
  inline const T  *begin()       const {
    return m_e;
  }
  inline const T  *end()         const {
    return m_e + m_dim;
  }

  virtual String toString(const TCHAR *delimiter = _T(" "), BracketType bracketType = BT_NOBRACKETS) const {
    return getIterator().toString(delimiter, bracketType);
  }

  inline String getDimensionString() const {
    return format(_T("Dimension=%u"), getDimension());
  }
};

template<typename OSTREAMTYPE, typename T> OSTREAMTYPE &operator<<(OSTREAMTYPE &out, const VectorTemplate<T> &v) {
  const StreamSize w = out.width();
  const UINT dim = v.getDimension();
  for(UINT i = 0; i < dim; i++) {
    if(i > 0) {
      out << " ";
    }
    out.width(w);
    out << v[i];
  }
  return out;
}

template<typename ISTREAMTYPE, typename T> ISTREAMTYPE &operator>>(ISTREAMTYPE &in, VectorTemplate<T> &v) {
  const FormatFlags flg = in.flags();
  in.flags(flg | std::ios::skipws);
  const UINT dim = v.getDimension();
  for(UINT i = 0; i < dim; i++) {
    in >> v[i];
  }
  in.flags(flg);
  return in;
}
