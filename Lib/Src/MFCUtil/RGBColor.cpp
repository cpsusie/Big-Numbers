#include "pch.h"
#include <Math.h>
#include <MFCUtil/ColorSpace.h>

#pragma warning(disable : 4244)

RGBColor::operator D3DCOLOR() const {
/*
  const int r = (int)round(round(m_red   * 255, 2));
  const int g = (int)round(round(m_green * 255, 2));
  const int b = (int)round(round(m_blue  * 255, 2));
*/
  const int r = (int)round(m_red   * 255);
  const int g = (int)round(m_green * 255);
  const int b = (int)round(m_blue  * 255);
  return D3DCOLOR_XRGB(r,g,b);
}

float RGBColor::getMax() const {
  if(m_green > m_red) {
    return m_blue > m_green ? m_blue : m_green;
  } else { // red >= green => either red og blue
    return m_red > m_blue ? m_red : m_blue;
  }
}

float RGBColor::getMin() const {
  if(m_green < m_red) {
    return m_blue < m_green ? m_blue : m_green;
  } else { // red <= green => either red og blue
    return m_red < m_blue ? m_red : m_blue;
  }
}

#define CHECKRANGE(c)                                                              \
if(((c) < 0) || ((c) > 1)) {                                                       \
  if(throwOnError) {                                                               \
    throwException(_T("%s:%s(=%f) outside valid range [0..1]"), method,_T(#c), c); \
  } else {                                                                         \
    return false;                                                                  \
  }                                                                                \
}

bool RGBColor::validate(bool throwOnError) const {
  DEFINEMETHODNAME;
  CHECKRANGE(m_red  );
  CHECKRANGE(m_green);
  CHECKRANGE(m_blue );
  return true;
}

bool operator==(const RGBColor &c1, const RGBColor &c2) {
  return (c1.m_red == c2.m_red) && (c1.m_green == c2.m_green) && (c1.m_blue == c2.m_blue);
}

bool operator!=(const RGBColor &c1, const RGBColor &c2) {
  return (c1.m_red != c2.m_red) || (c1.m_green != c2.m_green) || (c1.m_blue != c2.m_blue);
}

//#define VERSION2

RGBColor &RGBColor::getBaseColor(float hue) {
#ifdef VERSION2
  switch((int)(hue*6)) {
  case 0 :
    m_red   = 1;
    m_green = (float)(6 * (hue - 0));
    m_blue  = 0;
    return c;
  case 1 :
    m_red   = (float)(6 * (2.0/6.0 - hue));
    m_green = 1;
    m_blue  = 0;
    return c;
  case 2 :
    m_red   = 0;
    m_green = 1;
    m_blue  = (float)(6 * (hue - 2.0/6));
    return c;
  case 3 :
    m_red   = 0;
    m_green = (float)(6 * (4.0/6 - hue));
    m_blue  = 1;
    return c;
  case 4 :
    m_red   = (float)(6 * (hue - 4.0/6));
    m_green = 0;
    m_blue  = 1;
    return c;
  default:
    m_red   = 1;
    m_green = 0;
    m_blue  = (float)(6 * (1-hue));
    return c;
  }

#else

  if(hue < 1.0/6) {
    m_red   = 1;
    m_green = (float)(6 * (hue - 0));
    m_blue  = 0;
  } else if(hue < 2.0/6) {
    m_red   = (float)(6 * (2.0/6.0 - hue));
    m_green = 1;
    m_blue  = 0;
  } else if(hue < 3.0/6) {
    m_red   = 0;
    m_green = 1;
    m_blue  = (float)(6 * (hue - 2.0/6));
  } else if(hue < 4.0/6) {
    m_red   = 0;
    m_green = (float)(6 * (4.0/6 - hue));
    m_blue  = 1;
  } else if(hue < 5.0/6) {
    m_red   = (float)(6 * (hue - 4.0/6));
    m_green = 0;
    m_blue  = 1;
  } else {
    m_red   = 1;
    m_green = 0;
    m_blue  = (float)(6 * (1-hue));
  }
  return *this;
#endif
}

float RGBColor::findHue() const {
  if(m_red   >= m_green && m_green >= m_blue ) {
    return (0+m_green) / 6;
  }
  if(m_green >= m_red   && m_red   >= m_blue ) {
    return (2-m_red) / 6;
  }
  if(m_green >= m_blue  && m_blue  >= m_red  ) {
    return (2+m_blue) / 6;
  }
  if(m_blue  >= m_green && m_green >= m_red  ) {
    return (4-m_green) / 6;
  }
  if(m_blue  >= m_red   && m_red   >= m_green) {
    return (4+m_red) / 6;
  }
  return (6-m_blue) / 6;
}

#define SATURATE(            color, saturation) (float)((saturation)*((color)-0.5)+0.5)
#define DESATURATE(          color, saturation) (((color)-0.5) / (saturation) + 0.5)

#define LUMINATE_LOWERHALF(  color, lumination) ((float)((color)*(lumination)*2))
#define LUMINATE_UPPERHALF(  color, lumination) ((float)(2*(1-(color))*(lumination) + 2*(color)-1))

#define DELUMINATE_LOWERHALF(color, lumination) ((color)/2/(lumination))
#define DELUMINATE_UPPERHALF(color, lumination) (((color)+1-2*(lumination))/(1-(lumination))/2)


RGBColor &RGBColor::saturateColor(float saturation) {
  m_red   = SATURATE(m_red  , saturation);
  m_green = SATURATE(m_green, saturation);
  m_blue  = SATURATE(m_blue , saturation);
  return *this;
}

RGBColor &RGBColor::luminateColor(float lumination) {
  if(lumination <= 0.5) {
    m_red   = LUMINATE_LOWERHALF(m_red  , lumination);
    m_green = LUMINATE_LOWERHALF(m_green, lumination);
    m_blue  = LUMINATE_LOWERHALF(m_blue , lumination);
  } else {
    m_red   = LUMINATE_UPPERHALF(m_red  , lumination);
    m_green = LUMINATE_UPPERHALF(m_green, lumination);
    m_blue  = LUMINATE_UPPERHALF(m_blue , lumination);
  }
  return *this;
}

RGBColor RGBColor::deSaturateColor(const RGBColor &c, float saturation) { // static, invers of saturateColor
  RGBColor result;
  if(saturation == 0) {
    result.m_red   = 1;
    result.m_green = 0;
    result.m_blue  = 0;
  } else {
    result.m_red   = DESATURATE(c.m_red  , saturation);
    result.m_green = DESATURATE(c.m_green, saturation);
    result.m_blue  = DESATURATE(c.m_blue , saturation);
  }
  return result;
}

RGBColor RGBColor::deLuminateColor(const RGBColor &c, float lumination) { // static, invers of luminateColor
  RGBColor result;
  if(lumination == 0) {
    result.m_red = result.m_green = result.m_blue = 0;
  } else if(lumination == 1) {
    result.m_red = result.m_green = result.m_blue = 1;
  } else if(lumination <= 0.5) {
    result.m_red   = DELUMINATE_LOWERHALF(c.m_red  , lumination);
    result.m_green = DELUMINATE_LOWERHALF(c.m_green, lumination);
    result.m_blue  = DELUMINATE_LOWERHALF(c.m_blue , lumination);
  } else {
    result.m_red   = DELUMINATE_UPPERHALF(c.m_red  , lumination);
    result.m_green = DELUMINATE_UPPERHALF(c.m_green, lumination);
    result.m_blue  = DELUMINATE_UPPERHALF(c.m_blue , lumination);
  }
  return result;
}

RGBColor getRGBColor(const LSHColor &c) {
  return RGBColor().getBaseColor(c.m_hue).saturateColor(c.m_saturation).luminateColor(c.m_lumination);
}
