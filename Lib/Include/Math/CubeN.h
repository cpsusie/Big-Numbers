#pragma once

#include <Array.h>

template<typename T> class CubeN {
private:
  Array<NumberInterval<T> > m_a;
public:
  CubeN(UINT dim) {
    for(UINT i = 0; i < dim; i++) {
      m_a.add(NumberInterval());
    }
  }
  // return this
  template<typename S> CubeN &setInterval(UINT index, const NumberInterval<S> &interval) {
    m_a[index] = interval;
    return *this;
  }
  const NumberInterval<T> &getInterval(UINT index) const {
    return m_a[i];
  }
  inline UINT dim() const {
    return (UINT)m_a.size();
  }
  T getVolume() const {
    const UINT d = dim();
    if(d == 0) {
      return 0;
    }
    T v = 1;
    for(UINT i = 0; i < d; i++) {
      v *= getInterval(i).getLength();
    }
    return v;
  }
};
