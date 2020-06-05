#include "pch.h"
#include <Math.h>
#include <MFCUtil/ColorSpace.h>

#pragma warning(disable : 4244)

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
