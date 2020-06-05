#pragma once

#include "WinTools.h"
#include <MyUtil.h>

class D3PCOLOR {
public:
  D3DCOLOR m_color;
  inline D3PCOLOR() {}
  inline D3PCOLOR(D3DCOLOR c) : m_color(c) {}
  inline operator D3DCOLOR() const { return m_color; }
  String toString(bool showAlpha = false) const;
};

inline String toString(D3PCOLOR c, bool showAlpha=false) {
  return c.toString(showAlpha);
}

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

#define ARGB_SETALPHA(argb, x) (((x) << 24) | ((argb) & 0x00ffffff))

#define GETLUMINANCE(d3c) (0.2126*ARGB_GETRED(d3c) + 0.7152*ARGB_GETGREEN(d3c) + 0.0722*ARGB_GETBLUE(d3c))

#define D3DCOLOR2COLORREF(d3c) RGB(ARGB_GETRED(d3c), ARGB_GETGREEN(d3c), ARGB_GETBLUE(d3c))
#define COLORREF2D3DCOLOR(cr) D3DCOLOR_XRGB(GetRValue(cr), GetGValue(cr), GetBValue(cr))

#define D3DCOLORVALUE2COLORREF(c) RGB((DWORD)((c.r)*255.f),(DWORD)((c.g)*255.f),(DWORD)((c.b)*255.f))
D3DCOLORVALUE COLORREF2COLORVALUE(COLORREF c);

class RGBColor {
public:
  float m_red;        // 0..1
  float m_green;      // 0..1
  float m_blue;       // 0..1
  inline RGBColor() : m_red(0), m_green(0), m_blue(0) {
  }
  inline RGBColor(float red, float green, float blue) : m_red(red), m_green(green), m_blue(blue) {
  }

  inline RGBColor(const D3DCOLOR c)
    : m_red(  (float)ARGB_GETRED(  c)/255)
    , m_green((float)ARGB_GETGREEN(c)/255)
    , m_blue( (float)ARGB_GETBLUE( c)/255)
  {}
  operator D3DCOLOR() const;
  float getMax() const;
  float getMin() const;
  bool  validate(bool throwOnError = true) const;

  // 0 <= fc2 <= 1
  // Return RGBColor, where each component C = c1.C*(1-fc2) + c2.C*fc2; C=[m_red,m_green,m_blue]
  static inline RGBColor blendColor(const RGBColor &c1, const RGBColor &c2, float fc2) {
    const float fc1=1.0f-fc2;
    return RGBColor(c1.m_red  *fc1 + c2.m_red  *fc2
                   ,c1.m_green*fc1 + c2.m_green*fc2
                   ,c1.m_blue *fc1 + c2.m_blue *fc2);
  }
  static RGBColor deSaturateColor(const RGBColor &c, float saturation); // invers of saturateColor
  static RGBColor deLuminateColor(const RGBColor &c, float lumination); // invers of luminateColor
  float findHue() const;
  RGBColor &luminateColor(float lumination); // return *this
  RGBColor &getBaseColor( float hue       ); // return *this
  RGBColor &saturateColor(float saturation); // return *this

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

D3DCOLORVALUE   colorToColorValue(D3DCOLOR c);
inline D3DCOLOR colorValueToD3DColor(const D3DCOLORVALUE &cv) {
  return D3DCOLOR_COLORVALUE(cv.r, cv.g, cv.b, cv.a);
}

String toString(const D3DCOLORVALUE &c,bool showAlpha=false);

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
  static const CIEXYZ1931Color monitorWhite;       // CIEXYZ1931Color(RGBColor(D3DCOLOR_XRGB(255,255,255)))

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

// D3DCOLOR constants
#define D3D_BLACK      D3DCOLOR_XRGB(  0,  0,  0)
#define D3D_WHITE      D3DCOLOR_XRGB(255,255,255)
#define D3D_RED        D3DCOLOR_XRGB(255,  0,  0)
#define D3D_LIGHTRED   D3DCOLOR_XRGB(255,140,140)
#define D3D_LIGHTGREEN D3DCOLOR_XRGB(140,255,140)
#define D3D_GREEN      D3DCOLOR_XRGB(0  ,255,  0)
#define D3D_LIGHTBLUE  D3DCOLOR_XRGB(198,214,253)
#define D3D_BLUE       D3DCOLOR_XRGB(0  ,0  ,255)
#define D3D_GREY       D3DCOLOR_XRGB(192,192,192)
#define D3D_YELLOW     D3DCOLOR_XRGB(255,255,  0)
#define D3D_CYAN       D3DCOLOR_XRGB(255,  0,255)

// COLORREF constants
#define BLACK          RGB(0  ,0  ,0  )
#define WHITE          RGB(255,255,255)
#define LIGHTRED       RGB(255,140,140)
#define RED            RGB(255,0  ,0  )
#define LIGHTGREEN     RGB(140,255,140)
#define GREEN          RGB(0  ,255,0  )
#define LIGHTBLUE      RGB(198,214,253)
#define BLUE           RGB(0  ,0  ,255)
#define LIGHTGREY      RGB(235,235,235)
#define GREY           RGB(192,192,192)
#define YELLOW         RGB(255,255,0  )
#define CYAN           RGB(255,0  ,255)
