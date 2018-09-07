#include "pch.h"
#include <Math.h>
#include <MFCUtil/ColorSpace.h>

#pragma warning(disable : 4244)

String D3PCOLOR::toString(bool showAlpha) const {
  const D3DCOLOR cc = *this;
  if(showAlpha) {
    return format(_T("R:%3d G:%dd B:%3d A:%3d"), ARGB_GETRED(cc), ARGB_GETGREEN(cc), ARGB_GETBLUE(cc), ARGB_GETALPHA(cc));
  } else {
    return format(_T("R:%3d G:%dd B:%3d"), ARGB_GETRED(cc), ARGB_GETGREEN(cc), ARGB_GETBLUE(cc));
  }
}

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

bool operator==(const RGBColor &c1, const RGBColor &c2) {
  return (c1.m_red == c2.m_red) && (c1.m_green == c2.m_green) && (c1.m_blue == c2.m_blue);
}

bool operator!=(const RGBColor &c1, const RGBColor &c2) {
  return (c1.m_red != c2.m_red) || (c1.m_green != c2.m_green) || (c1.m_blue != c2.m_blue);
}

//#define VERSION2

static RGBColor &getBaseColor(RGBColor &c, float hue) {
#ifdef VERSION2
  switch((int)(hue*6)) {
  case 0 :
    c.m_red   = 1;
    c.m_green = (float)(6 * (hue - 0));
    c.m_blue  = 0;
    return c;
  case 1 :
    c.m_red   = (float)(6 * (2.0/6.0 - hue));
    c.m_green = 1;
    c.m_blue  = 0;
    return c;
  case 2 :
    c.m_red   = 0;
    c.m_green = 1;
    c.m_blue  = (float)(6 * (hue - 2.0/6));
    return c;
  case 3 :
    c.m_red   = 0;
    c.m_green = (float)(6 * (4.0/6 - hue));
    c.m_blue  = 1;
    return c;
  case 4 :
    c.m_red   = (float)(6 * (hue - 4.0/6));
    c.m_green = 0;
    c.m_blue  = 1;
    return c;
  default:
    c.m_red   = 1;
    c.m_green = 0;
    c.m_blue  = (float)(6 * (1-hue));
    return c;
  }

#else

  if(hue < 1.0/6) {
    c.m_red   = 1;
    c.m_green = (float)(6 * (hue - 0));
    c.m_blue  = 0;
  } else if(hue < 2.0/6) {
    c.m_red   = (float)(6 * (2.0/6.0 - hue));
    c.m_green = 1;
    c.m_blue  = 0;
  } else if(hue < 3.0/6) {
    c.m_red   = 0;
    c.m_green = 1;
    c.m_blue  = (float)(6 * (hue - 2.0/6));
  } else if(hue < 4.0/6) {
    c.m_red   = 0;
    c.m_green = (float)(6 * (4.0/6 - hue));
    c.m_blue  = 1;
  } else if(hue < 5.0/6) {
    c.m_red   = (float)(6 * (hue - 4.0/6));
    c.m_green = 0;
    c.m_blue  = 1;
  } else {
    c.m_red   = 1;
    c.m_green = 0;
    c.m_blue  = (float)(6 * (1-hue));
  }
  return c;
#endif
}

static float findHue(const RGBColor &c) {
  if(c.m_red   >= c.m_green && c.m_green >= c.m_blue ) {
    return (0+c.m_green) / 6;
  }
  if(c.m_green >= c.m_red   && c.m_red   >= c.m_blue ) {
    return (2-c.m_red) / 6;
  }
  if(c.m_green >= c.m_blue  && c.m_blue  >= c.m_red  ) {
    return (2+c.m_blue) / 6;
  }
  if(c.m_blue  >= c.m_green && c.m_green >= c.m_red  ) {
    return (4-c.m_green) / 6;
  }
  if(c.m_blue  >= c.m_red   && c.m_red   >= c.m_green) {
    return (4+c.m_red) / 6;
  }
  return (6-c.m_blue) / 6;
}

#define SATURATE(            color, saturation) (float)((saturation)*((color)-0.5)+0.5)
#define DESATURATE(          color, saturation) (((color)-0.5) / (saturation) + 0.5)

#define LUMINATE_LOWERHALF(  color, lumination) ((float)((color)*(lumination)*2))
#define LUMINATE_UPPERHALF(  color, lumination) ((float)(2*(1-(color))*(lumination) + 2*(color)-1))

#define DELUMINATE_LOWERHALF(color, lumination) ((color)/2/(lumination))
#define DELUMINATE_UPPERHALF(color, lumination) (((color)+1-2*(lumination))/(1-(lumination))/2)


static RGBColor &saturateColor(RGBColor &c, float saturation) {
  c.m_red   = SATURATE(c.m_red  , saturation);
  c.m_green = SATURATE(c.m_green, saturation);
  c.m_blue  = SATURATE(c.m_blue , saturation);
  return c;
}

RGBColor &luminateColor(RGBColor &c, float lumination) {
  if(lumination <= 0.5) {
    c.m_red   = LUMINATE_LOWERHALF(c.m_red  , lumination);
    c.m_green = LUMINATE_LOWERHALF(c.m_green, lumination);
    c.m_blue  = LUMINATE_LOWERHALF(c.m_blue , lumination);
  } else {
    c.m_red   = LUMINATE_UPPERHALF(c.m_red  , lumination);
    c.m_green = LUMINATE_UPPERHALF(c.m_green, lumination);
    c.m_blue  = LUMINATE_UPPERHALF(c.m_blue , lumination);
  }
  return c;
}

static RGBColor deSaturateColor(const RGBColor &c, float saturation) { // invers of saturateColor
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

static RGBColor deLuminateColor(const RGBColor &c, float lumination) { // invers of luminateColor
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
  RGBColor result;
  return luminateColor(saturateColor(getBaseColor(result, c.m_hue), c.m_saturation), c.m_lumination);
}

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
  lsh.m_hue              = findHue(deSaturateColor(deLuminateColor(c, lsh.m_lumination), lsh.m_saturation));
  return lsh;
}

LumaChromColor::LumaChromColor(D3DCOLOR c) {
  m_y = GETLUMINANCE(c);         // encodes luminance
  m_u = ARGB_GETBLUE(c) - m_y;    // u
  m_v = ARGB_GETRED(c)  - m_y;    // v
}

LumaChromColor::operator D3DCOLOR() const {
  const int r = m_y + m_v;
  const int g = m_y - 0.100951*m_u - 0.29726*m_v;
  const int b = m_y + m_u;
  return D3DCOLOR_XRGB(r, g, b);
}

D3DCOLOR getGrayColor(D3DCOLOR c) {
  const int lum = GETLUMINANCE(c);
  return D3DCOLOR_XRGB(lum, lum, lum);
}

D3DCOLORVALUE colorToColorValue(D3DCOLOR c) {
  D3DCOLORVALUE result;
  result.r = ((float)(ARGB_GETRED(  c))) / (float)255.0;
  result.g = ((float)(ARGB_GETGREEN(c))) / (float)255.0;
  result.b = ((float)(ARGB_GETBLUE( c))) / (float)255.0;
  result.a = 1.0;
  return result;
}

D3DCOLORVALUE COLORREF2COLORVALUE(COLORREF c) {
  D3DCOLORVALUE result;
  result.r = ((float)(GetRValue(c))) / 255.0f;
  result.g = ((float)(GetGValue(c))) / 255.0f;
  result.b = ((float)(GetBValue(c))) / 255.0f;
  result.a = 1.0f;
  return result;
}

bool SimpleColorComparator::equals(const D3DCOLOR &c1, const D3DCOLOR &c2) {
  return ARGB_TORGB(c1) == ARGB_TORGB(c2);
}

double colorDistance(D3DCOLOR c1, D3DCOLOR c2) { // value between [0..1]
  const int r = GetRValue(c1) - GetRValue( c2);
  const int g = GetGValue(c1) - GetGValue(c2);
  const int b = GetBValue(c1) - GetBValue(c2);
  return sqrt((double)r*r + g*g + b*b)/441.67295593;
}
