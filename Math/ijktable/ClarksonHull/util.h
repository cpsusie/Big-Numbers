#pragma once

#include <string>

using namespace std;

typedef unsigned long u_long;
typedef unsigned int  u_int;

#ifdef _DEBUG
void xassert(const char *fileName, int line, const char *exp);
#define Assert(exp) (void)( (exp) || (xassert(__FILE__, __LINE__, #exp), 0) )
#else
#define Assert(exp)
#endif

class Exception {
private:
  const string m_what;
public:
  Exception(const string &msg) : m_what(msg) {
  }
  const char *what() const {
    return m_what.c_str();
  }
};

void throwException(                                   _In_z_ _Printf_format_string_ char const * const format, ...);
void throwInvalidArgumentException(const char *method, _In_z_ _Printf_format_string_ char const * const format, ...);
void throwPROCEDURE_ERROR(         const char *method, const string &msg);
void throwPROCEDURE_ERROR(         const char *method, _In_z_ _Printf_format_string_ char const * const format, ...);

template<typename T> class fixedarray {
private:
  T           *m_a;
  unsigned int m_size;
  inline void checkIndex(const char *method, u_int index) const {
    if(index >= m_size) {
      throwInvalidArgumentException(method, "index=%u, size=%u", index, m_size);
    }
  }
  inline fixedarray &allocate() {
    m_a = m_size ? new T[m_size] : NULL;
    return *this;
  }
  inline fixedarray &cleanup() {
    delete[] m_a;
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
  explicit inline fixedarray(u_int size) : m_size(size) {
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
  fixedarray &setSize(u_int size) {
    if(size != m_size) {
      cleanup();
      m_size = size;
      allocate();
    }
    return *this;
  }
  template<typename I> inline T &operator[](I index) {
    const u_int i = (u_int)index;
    checkIndex(__FUNCTION__,i);
    return m_a[i];
  }
  template<typename I> inline const T &operator[](I index) const {
    const u_int i = (u_int)index;
    checkIndex(__FUNCTION__, i);
    return m_a[i];
  }
  inline u_int size() const {
    return m_size;
  }
};

typedef fixedarray<double> doublearray;