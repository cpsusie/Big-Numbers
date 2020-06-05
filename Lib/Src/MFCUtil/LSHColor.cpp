#include "pch.h"
#include <Math.h>
#include <MFCUtil/ColorSpace.h>

LSHColor::LSHColor(float hue, float saturation, float lumination) {
  m_hue        = hue;
  m_saturation = saturation;
  m_lumination = lumination;
}

bool operator==(const LSHColor &c1, const LSHColor &c2) {
  return (c1.m_hue == c2.m_hue) && (c1.m_saturation == c2.m_saturation) && (c1.m_lumination == c2.m_lumination);
}

bool operator!=(const LSHColor &c1, const LSHColor &c2) {
  return (c1.m_hue != c2.m_hue) || (c1.m_saturation != c2.m_saturation) || (c1.m_lumination != c2.m_lumination);
}

LSHColor getLSHColor(const RGBColor &c) { // getLSH(getColor(lsh)) = lsh; getColor(getLSH(color)) = color
  LSHColor lsh;
  const float maxColor   = c.getMax();
  const float minColor   = c.getMin();
  lsh.m_lumination       = (minColor+maxColor)/2;
  const float calib      = (float)(1.0 - 2*fabs(0.5-lsh.m_lumination));
  lsh.m_saturation       = calib ? ((maxColor-minColor) / calib) : 0;
  lsh.m_hue              = RGBColor::deSaturateColor(RGBColor::deLuminateColor(c, lsh.m_lumination), lsh.m_saturation).findHue();
  return lsh;
}
