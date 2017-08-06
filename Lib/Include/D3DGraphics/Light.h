#pragma once

#include <MyUtil.h>
#include <MFCUtil/D3DeviceFactory.h>

// Sent to listener for id=SP_LIGHTPARAMETERS
class LIGHT : public D3DLIGHT {
private:
  void setDefaultDirectional();
  void setDefaultPoint();
  void setDefaultSpot();

public:
  int   m_index;
  bool  m_enabled;

  // Only valid for spot lights
  // Maintain invariant:0 <= Theta <= Phi <= pi
  void setInnerAngle(float rad);
  // Only valid for spot lights
  // Maintain invariant:0 <= Theta <= Phi <= pi
  void setOuterAngle(float rad);
  inline float getInnerAngle() const {
    return Theta;
  }
  inline float getOuterAngle() const {
    return Phi;
  }
  inline bool isEnabled() const {
    return m_enabled;
  }
  void setDefaultColors();
  void setDefault(D3DLIGHTTYPE type);
  String toString(int dec=3) const;
};

inline bool operator==(const LIGHT &l1, const LIGHT &l2) {
  return memcmp(&l1, &l2, sizeof(LIGHT)) == 0;
}

inline bool operator!=(const LIGHT &l1, const LIGHT &l2) {
  return !(l1 == l2);
}

