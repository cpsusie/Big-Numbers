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

// Sent to listener for id=SP_MATERIALPARAMETERS
class D3Material : public D3DMATERIAL {
private:
  // If < 0, material is undefined
  int m_id;
public:
  inline D3Material() {
    setUndefined();
  }
  inline explicit D3Material(UINT id) {
    setUndefined().m_id = id;
  }
  inline D3Material &operator=(const D3DMATERIAL &m) {
    *((D3DMATERIAL*)this) = m;
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
  static D3DMATERIAL createMaterialWithColor(D3DCOLOR color);
  static inline D3DMATERIAL createDefaultMaterial() {
    return D3Material().setDefault();
  }
  static D3DMATERIAL createMaterialForTexture() {
    return D3Material().setMaterialForTexture();
  }
  // v=0 => transparent, v=1 => opaque
  D3Material &setOpacity(float v);
  float getOpacity() const;
  String toString(int dec=3) const;
};

inline bool operator==(const D3Material &m1, const D3Material &m2) {
  return memcmp(&m1, &m2, sizeof(D3Material)) == 0;
}

inline bool operator!=(const D3Material &m1, const D3Material &m2) {
  return !(m1 == m2);
}

typedef CompactUIntHashMap<D3Material> MaterialMap;
