#pragma once

#include <MyUtil.h>
#include <D3D9.h>

inline bool operator==(const D3DMATERIAL &m1, const D3DMATERIAL &m2) {
  return memcmp(&m1, &m2, sizeof(D3DMATERIAL)) == 0;
}

inline bool operator!=(const D3DMATERIAL &m1, const D3DMATERIAL &m2) {
  return !(m1 == m2);
}

class MATERIAL : public D3DMATERIAL { // sent to listener for id=SP_MATERIALPARAMETERS
public:
  int m_index;   // if < 0, material is undefined.
  inline MATERIAL() : m_index(-1) {
  }
  inline bool isDefined() const {
    return m_index >= 0;
  }
  void setDefault();

  String toString(int dec=3) const;
};
