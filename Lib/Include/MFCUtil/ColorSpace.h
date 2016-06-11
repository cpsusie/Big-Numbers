#pragma once

#include <MyUtil.h>
#include "WinTools.h"

class RGBColor {
public:
  float m_red;        // 0..1
  float m_green;      // 0..1
  float m_blue;       // 0..1
  RGBColor() : m_red(0), m_green(0), m_blue(0) {
  }
  RGBColor(float red, float green, float blue);
  RGBColor(const D3DCOLOR c);
  operator D3DCOLOR() const;
  float getMax() const;
  float getMin() const;
};

class sRGBColor {
public:
  float m_R;       // 0..1
  float m_G;       // 0..1
  float m_B;       // 0..1
  sRGBColor(const RGBColor &c);
  operator RGBColor() const;
};

bool operator==(const RGBColor &c1, const RGBColor &c2);
bool operator!=(const RGBColor &c1, const RGBColor &c2);

class LSHColor {
public:
  float m_lumination; // 0..1
  float m_saturation; // 0..1
  float m_hue;        // 0..1
  LSHColor() : m_lumination(0), m_saturation(0), m_hue(0) {
  }
  LSHColor(float hue, float saturation, float lumination);
};

bool operator==(const LSHColor &c1, const LSHColor &c2);
bool operator!=(const LSHColor &c1, const LSHColor &c2);

class LumaChromColor {
public:
  float m_y;
  float m_u;
  float m_v;
  LumaChromColor(D3DCOLOR c);
  operator D3DCOLOR() const;
};

LSHColor getLSHColor(const RGBColor &c);
RGBColor getRGBColor(const LSHColor &c);
D3DCOLOR getGrayColor(     D3DCOLOR  c);

// COLORREF has layout 0x00bbggrr  (lower to the right)
// D3DCOLOR has layout 0xaarrggbb  (    ---- ""------ )

//#define RGB(r,g,b)          ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
//#define D3DCOLOR_ARGB(a,r,g,b) \
//    ((D3DCOLOR)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))

#define ARGB_GETALPHA(d3c)      ( (d3c) >> 24)
#define ARGB_GETRED(  d3c)      (((d3c) >> 16) & 0xff)
#define ARGB_GETGREEN(d3c)      (((d3c) >>  8) & 0xff)
#define ARGB_GETBLUE( d3c)      ( (d3c)        & 0xff)

#define ARGB_TORGB(d3c) ((d3c) & 0x00ffffff)

#define GETLUMINANCE(d3c) (0.2126*ARGB_GETRED(d3c) + 0.7152*ARGB_GETGREEN(d3c) + 0.0722*ARGB_GETBLUE(d3c))

#define D3DCOLOR2COLORREF(d3c) RGB(ARGB_GETRED(d3c), ARGB_GETGREEN(d3c), ARGB_GETBLUE(d3c))
#define COLORREF2D3DCOLOR(cr) D3DCOLOR_XRGB(GetRValue(cr), GetGValue(cr), GetBValue(cr))

class ColorComparator {
public:
  virtual bool equals(const D3DCOLOR &c1, const D3DCOLOR &c2) = 0;
};

double colorDistance(D3DCOLOR c1, D3DCOLOR c2);

class SimpleColorComparator : public ColorComparator {
public:
  bool equals(const D3DCOLOR &c1, const D3DCOLOR &c2);
};


class CIERGB1931Color {
public:
  float m_R,m_G,m_B;
  CIERGB1931Color(float R, float G, float B) : m_R(R), m_G(G), m_B(B) {
  }
  CIERGB1931Color(D3DCOLOR c);
  operator D3DCOLOR() const;
};

class CIEChromaticity {
public:
  float m_x, m_y;
  CIEChromaticity(float x, float y) : m_x(x), m_y(y) {
  }
};

class CIEXYZ1931Color {
public:
  static const CIEChromaticity D65StandardObserver, D65SupplementaryObserver;
  static const CIEXYZ1931Color D65Normalized;      // normalized white
  static const CIEXYZ1931Color monitorWhite;       // CIEXYZ1931Color(RGBColor(RGB_MAKE(255,255,255)))

  float m_X,m_Y,m_Z;
  CIEXYZ1931Color(float X, float Y, float Z) : m_X(X), m_Y(Y), m_Z(Z) {
  }
  CIEXYZ1931Color(const CIERGB1931Color &c);
  operator CIERGB1931Color() const ;
  CIEXYZ1931Color(const RGBColor &c);
  operator RGBColor() const;
  String toString() const;
};

class CIELABColor {
private:
  static double f(   double x);
  static double finv(double x);
public:
  float m_L;                     // [0..100]
  float m_a,m_b;
  CIELABColor(float L, float a, float b) : m_L(L), m_a(a), m_b(b) {
  }
  CIELABColor(const CIEXYZ1931Color &c, const CIEXYZ1931Color &white);
  CIEXYZ1931Color getCIEXYZ1931Color(const CIEXYZ1931Color &white) const;
  String toString() const;
};
