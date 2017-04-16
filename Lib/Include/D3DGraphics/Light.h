#pragma once

#include <MyUtil.h>
#include <MFCUtil/D3DeviceFactory.h>

class LIGHT : public D3DLIGHT { // sent to listener for id=SP_LIGHTPARAMETERS
private:
  void setDefaultDirectional();
  void setDefaultPoint();
  void setDefaultSpot();

public:
  int   m_index;
  bool  m_enabled;

  // only valid for spot lights
  void setInnerAngle(float rad); // maintain invariant:0 <= Theta <= Phi <= pi
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

