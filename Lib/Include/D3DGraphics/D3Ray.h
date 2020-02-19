#pragma once

#include "D3Math.h"

class D3Ray {
public:
  D3DXVECTOR3 m_orig; // Point in world space where ray starts
  D3DXVECTOR3 m_dir;  // direction of ray in world space
  inline D3Ray() : m_orig(D3DXORIGIN), m_dir(D3DXORIGIN) {
  }
  inline D3Ray(const D3DXVECTOR3 &orig, const D3DXVECTOR3 &dir) : m_orig(orig), m_dir(unitVector(dir)) {
  }
  D3DXVECTOR3 getHitPoint(float dist) const {
    return m_orig + m_dir * dist;
  }
  inline void clear() {
    m_dir = m_orig = D3DXORIGIN;
  }
  inline bool isEmpty() const {
    return length(m_dir) == 0;
  }
  inline String toString(int dec = 3) const {
    return format(_T("Orig:%s, Dir:%s"), ::toString(m_orig, dec).cstr(), ::toString(m_dir, dec).cstr());
  }
};
