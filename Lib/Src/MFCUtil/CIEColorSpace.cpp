#include "pch.h"
#include <Math.h>
#include <MFCUtil/ColorSpace.h>

#pragma warning(disable : 4244)

sRGBColor::sRGBColor(const RGBColor &c) {

#define LINEARTOSRGB(cl) (((cl) <= 0.00331308) ? (12.92*(cl)) : ((1.0+0.055)*pow(cl,1.0/2.4)-0.055))

  m_R = LINEARTOSRGB(c.m_red  );
  m_G = LINEARTOSRGB(c.m_green);
  m_B = LINEARTOSRGB(c.m_blue );
}

sRGBColor::operator RGBColor() const {

#define SRGBTOLINEAR(cs) (((cs) <= 0.04045)? ((cs)/12.92) : pow(((cs) + 0.055) / (1.0 + 0.055), 2.4))

  return RGBColor(SRGBTOLINEAR(m_R)
                 ,SRGBTOLINEAR(m_G)
                 ,SRGBTOLINEAR(m_B)
                 );
}

/*
CIERGB1931Color::CIERGB1931Color(D3DCOLOR c) {
}

CIERGB1931Color::operator D3DCOLOR() const {
}
*/

// see http://en.wikipedia.org/wiki/Illuminant_D65

const CIEChromaticity CIEXYZ1931Color::D65StandardObserver(     0.31271f, 0.32902f);
const CIEChromaticity CIEXYZ1931Color::D65SupplementaryObserver(0.31382f, 0.33100f);
const CIEXYZ1931Color CIEXYZ1931Color::D65Normalized(95.047f, 100.00f, 108.883f);
const CIEXYZ1931Color CIEXYZ1931Color::monitorWhite(RGBColor(D3DCOLOR_XRGB(255,255,255)));

CIEXYZ1931Color::CIEXYZ1931Color(const CIERGB1931Color &c) {
  m_X = (0.49 * c.m_R +  0.31   * c.m_G + 0.2     * c.m_B) / 0.17697;
  m_Y =         c.m_R + (0.8123 * c.m_G + 0.01063 * c.m_B) / 0.17697;
  m_Z =                 (0.01   * c.m_G + 0.99    * c.m_B) / 0.17697;
}

CIEXYZ1931Color::operator CIERGB1931Color() const {
  return CIERGB1931Color( 0.41847    * m_X - 0.15866   * m_Y - 0.082835 * m_Z
                        ,-0.091169   * m_X + 0.25243   * m_Y + 0.015708 * m_Z
                        , 0.00092090 * m_X - 0.0025498 * m_Y + 0.17860  * m_Z
                        );
}

// see http://en.wikipedia.org/wiki/SRGB

CIEXYZ1931Color::CIEXYZ1931Color(const RGBColor &c) {
  m_X = 0.4121 * c.m_red + 0.3576 * c.m_green + 0.1805 * c.m_blue;
  m_Y = 0.2126 * c.m_red + 0.7152 * c.m_green + 0.0722 * c.m_blue;
  m_Z = 0.0192 * c.m_red + 0.1192 * c.m_green + 0.9505 * c.m_blue;
}

CIEXYZ1931Color::operator RGBColor() const {
  return RGBColor(3.2406 * m_X - 1.5372 * m_Y - 0.4986 * m_Z
                ,-0.9689 * m_X + 1.8758 * m_Y + 0.0415 * m_Z
                , 0.0557 * m_X - 0.2040 * m_Y + 1.0570 * m_Z
                );
}

String CIEXYZ1931Color::toString() const {
  return format(_T("X:%f, Y:%f, Z:%f"), m_Z,m_Y,m_Z);
}

// See http://en.wikipedia.org/wiki/Lab_color_space

CIELABColor::CIELABColor(const CIEXYZ1931Color &c, const CIEXYZ1931Color &white) {
  const double fY = f(c.m_Y / white.m_Y);
  m_L =  116.0 * fY- 16;
  m_a =  500.0 * (f(c.m_X / white.m_X) - fY);
  m_b = -200.0 * (f(c.m_Z / white.m_Z) - fY);
}

CIEXYZ1931Color CIELABColor::getCIEXYZ1931Color(const CIEXYZ1931Color &white) const {
  const double L16 = (m_L + 16) / 116.0;
  return CIEXYZ1931Color(white.m_X * finv(L16 + m_a/500.0)
                        ,white.m_Y * finv(L16)
                        ,white.m_Z * finv(L16 - m_b/200.0)
                        );
}

double CIELABColor::f(double x) { // static 
#define X0 0.008856451679035630817   // (6/29)^3
  if(x > X0) {
    return pow(x, 1.0/3.0);
  } else {
    return 7.7870370370370370370 * x + (4.0/29); // (29/6)^2 / 3 * x + 4/29
  }
}

double CIELABColor::finv(double x) { // static 
  if(x > 6.0/29) {
    return x*x*x;
  } else {
    return 0.1284185493460166 * (x-4.0/29);
  }
}

String CIELABColor::toString() const {
  return format(_T("L:%f, a:%f, b:%f"), m_L,m_a,m_b);
}
