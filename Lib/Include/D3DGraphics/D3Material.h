#pragma once

#include <MyUtil.h>
#include <CompactHashMap.h>
#include <MFCUtil/DirectXDeviceFactory.h>

inline bool operator==(const D3DMATERIAL &m1, const D3DMATERIAL &m2) {
  return memcmp(&m1, &m2, sizeof(D3DMATERIAL)) == 0;
}

inline bool operator!=(const D3DMATERIAL &m1, const D3DMATERIAL &m2) {
  return !(m1 == m2);
}

class D3MATERIAL : public D3DMATERIAL {
public:
  bool m_specularHighlights;
  inline bool operator==(const D3MATERIAL &m) const {
    return (m_specularHighlights == m.m_specularHighlights) && ((*(D3DMATERIAL*)this) == ((const D3DMATERIAL&)m));
  }
  inline bool operator!=(const D3MATERIAL &m) const {
    return !(*this == m);
  }
};

// Sent to listener for id=SP_MATERIALPARAMETERS
class D3Material : public D3MATERIAL {
private:
  // If m_id < 0, material is undefined
  int  m_id;
public:
  inline D3Material() {
    setUndefined();
  }
  inline explicit D3Material(UINT id) {
    setUndefined().m_id = id;
  }
  inline D3Material &operator=(const D3MATERIAL &m) {
    *((D3MATERIAL*)this) = m;
    return *this;
  }
  D3Material &setUndefined() {
    memset(this, 0xff, sizeof(D3Material));
    return *this;
  }
  inline bool isDefined() const {
    return m_id >= 0;
  }
  inline int getId() const {
    return m_id;
  }
  D3Material &setDefault();
  D3Material &setMaterialForTexture();

  // create a material with the specified diffuse and emissive color
  static D3MATERIAL        createMaterialWithColor(D3DCOLOR color, bool specularHighlights=true);
  static inline D3MATERIAL createDefaultMaterial() {
    return D3Material().setDefault();
  }
  static D3MATERIAL createMaterialForTexture() {
    return D3Material().setMaterialForTexture();
  }
  // v=0 => transparent, v=1 => opaque
  D3Material &setOpacity(float v);
  float getOpacity() const;

  inline bool operator==(const D3Material &m) const {
    return __super::operator==(m) && (m_id == m.m_id);
  }
  inline bool operator!=(const D3Material &m) const {
    return !(*this == m);
  }

  String toString(int dec=3) const;
};

typedef CompactUIntHashMap<D3Material,200> MaterialMap;
