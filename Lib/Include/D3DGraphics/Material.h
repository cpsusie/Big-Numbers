#pragma once

#include <MyUtil.h>
#include <CompactHashMap.h>
#include <MFCUtil/D3DeviceFactory.h>

inline bool operator==(const D3DMATERIAL &m1, const D3DMATERIAL &m2) {
  return memcmp(&m1, &m2, sizeof(D3DMATERIAL)) == 0;
}

inline bool operator!=(const D3DMATERIAL &m1, const D3DMATERIAL &m2) {
  return !(m1 == m2);
}

// Sent to listener for id=SP_MATERIALPARAMETERS
class MATERIAL : public D3DMATERIAL {
private:
  // If < 0, material is undefined
  int m_id;
public:
  inline MATERIAL() : m_id(-1) {
  }
  inline explicit MATERIAL(int id) : m_id(id) {
  }
  inline MATERIAL &operator=(const D3DMATERIAL &m) {
    *((D3DMATERIAL*)this) = m;
    return *this;
  }
  MATERIAL &setUndefined() {
    memset(this, 0xff, sizeof(MATERIAL));
    return *this;
  }
  inline bool isDefined() const {
    return m_id >= 0;
  }
  inline int getId() const {
    return m_id;
  }
  MATERIAL &setDefault();
  // create a material with the specified diffuse and emissive color
  static D3DMATERIAL createMaterialWithColor(D3DCOLOR color);
  static inline D3DMATERIAL createDefaultMaterial() {
    return MATERIAL().setDefault();
  }
  String toString(int dec=3) const;
};

typedef CompactUIntHashMap<MATERIAL> MaterialMap;
inline int materialCmp(const MATERIAL &m1, const MATERIAL &m2) {
  return m1.getId() - m2.getId();
}
