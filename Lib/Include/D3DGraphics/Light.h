#pragma once

#include <MyUtil.h>
#include <MFCUtil/D3DeviceFactory.h>

// Sent to listener for id=SP_LIGHTPARAMETERS
class LIGHT : public D3DLIGHT {
private:
  int   m_index;
  bool  m_enabled;

  LIGHT &setDefaultDirectional();
  LIGHT &setDefaultPoint();
  LIGHT &setDefaultSpot();

public:
  inline LIGHT(int index = -1) : m_index(index) {
  }

  inline LIGHT &operator=(const D3DLIGHT &light) {
    (*(D3DLIGHT*)this) = light;
    return *this;
  }
  LIGHT &setUndefined() {
    memset(this, 0xff, sizeof(LIGHT));
    return *this;
  }
  inline bool isDefined() const {
    return m_index >= 0;
  }
  inline int getIndex() const {
    return m_index;
  }
  LIGHT &setDirection(const D3DXVECTOR3 &dir) {
    Direction = unitVector(dir);
    return *this;
  }
  LIGHT &setPosition(const D3DXVECTOR3 &pos) {
    Position = pos;
    return *this;
  }

  // Only valid for spot lights
  // Maintain invariant:0 <= Theta <= Phi <= pi
  LIGHT &setInnerAngle(float rad);
  // Only valid for spot lights
  // Maintain invariant:0 <= Theta <= Phi <= pi
  LIGHT &setOuterAngle(float rad);
  inline float getInnerAngle() const {
    return Theta;
  }
  inline float getOuterAngle() const {
    return Phi;
  }

  LIGHT &setEnabled(bool enabled) {
    m_enabled = enabled;
    return *this;
  }
  inline bool isEnabled() const {
    return m_enabled;
  }

  LIGHT &setDefaultColors();
  LIGHT &setDefault(D3DLIGHTTYPE type);
  static D3DLIGHT createDefaultLight(D3DLIGHTTYPE type = D3DLIGHT_DIRECTIONAL) {
    return LIGHT(-1).setDefault(type);
  }
  String toString(int dec=3) const;
};

inline bool operator==(const LIGHT &l1, const LIGHT &l2) {
  return memcmp(&l1, &l2, sizeof(LIGHT)) == 0;
}

inline bool operator!=(const LIGHT &l1, const LIGHT &l2) {
  return !(l1 == l2);
}

typedef CompactArray<LIGHT> LightArray;
