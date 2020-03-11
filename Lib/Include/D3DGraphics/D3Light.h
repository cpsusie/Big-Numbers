#pragma once

#include <MyUtil.h>
#include <MFCUtil/DirectXDeviceFactory.h>

// Sent to listener for id=SP_LIGHTPARAMETERS
class D3Light : public D3DLIGHT {
private:
  int   m_index;
  bool  m_enabled;

  D3Light &setDefaultDirectional();
  D3Light &setDefaultPoint();
  D3Light &setDefaultSpot();

public:
  inline D3Light() : m_index(-1) {
  }
  explicit inline D3Light(int index) : m_index(index) {
  }

  inline D3Light &operator=(const D3DLIGHT &light) {
    (*(D3DLIGHT*)this) = light;
    return *this;
  }
  D3Light &setUndefined() {
    memset(this, 0xff, sizeof(D3Light));
    return *this;
  }
  inline bool isDefined() const {
    return m_index >= 0;
  }
  inline int getIndex() const {
    return m_index;
  }
  D3Light &setDirection(const D3DXVECTOR3 &dir) {
    Direction = unitVector(dir);
    return *this;
  }
  D3Light &setPosition(const D3DXVECTOR3 &pos) {
    Position = pos;
    return *this;
  }

  // Only valid for spot lights
  // Maintain invariant:0 <= Theta <= Phi <= pi
  D3Light &setInnerAngle(float rad);
  // Only valid for spot lights
  // Maintain invariant:0 <= Theta <= Phi <= pi
  D3Light &setOuterAngle(float rad);
  inline float getInnerAngle() const {
    return Theta;
  }
  inline float getOuterAngle() const {
    return Phi;
  }

  D3Light &setEnabled(bool enabled) {
    m_enabled = enabled;
    return *this;
  }
  inline bool isEnabled() const {
    return m_enabled;
  }

  D3Light &setDefaultColors();
  D3Light &setDefault(D3DLIGHTTYPE type);
  static D3DLIGHT createDefaultLight(D3DLIGHTTYPE type = D3DLIGHT_DIRECTIONAL) {
    return D3Light().setDefault(type);
  }
  String getName() const;
  String toString(int dec=3) const;
};

inline bool operator==(const D3Light &l1, const D3Light &l2) {
  return memcmp(&l1, &l2, sizeof(D3Light)) == 0;
}

inline bool operator!=(const D3Light &l1, const D3Light &l2) {
  return !(l1 == l2);
}

typedef CompactArray<D3Light> LightArray;
