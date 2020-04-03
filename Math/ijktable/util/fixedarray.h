#pragma once

#include "error.h"
#include "MemoryTrace.h"

template<typename T> class fixedarray {
private:
  T           *m_a;
  unsigned int m_size;
  inline void checkIndex(const char *method, uint index) const {
    if(index >= m_size) {
      throwInvalidArgumentException(method, "index=%u, size=%u", index, m_size);
    }
  }
  inline fixedarray &allocate() {
    m_a = m_size ? new T[m_size] : NULL; TRACE_NEW(m_a);
    return *this;
  }
  inline fixedarray &cleanup() {
    SAFEDELETEARRAY(m_a);
    m_a = NULL;
    return *this;
  }
  inline fixedarray &copy(const T *src) {
    if(m_size) {
      memcpy(m_a, src, m_size * sizeof(T));
    }
    return *this;
  }

public:
  explicit inline fixedarray(uint size) : m_size(size) {
    allocate();
  }
  fixedarray(const fixedarray<T> &src) : m_size(src.m_size) {
    allocate().copy(src.m_a);
  }
  fixedarray &operator=(const fixedarray<T> &src) {
    return setSize(src.m_size).copy(src.m_a);
  }
  virtual ~fixedarray() {
    cleanup();
  }
  fixedarray &setSize(uint size) {
    if(size != m_size) {
      cleanup();
      m_size = size;
      allocate();
    }
    return *this;
  }
  template<typename I> inline T &operator[](I index) {
    const uint i = (uint)index;
    checkIndex(__FUNCTION__, i);
    return m_a[i];
  }
  template<typename I> inline const T &operator[](I index) const {
    const uint i = (uint)index;
    checkIndex(__FUNCTION__, i);
    return m_a[i];
  }
  inline uint size() const {
    return m_size;
  }
};

typedef fixedarray<double> doublearray;