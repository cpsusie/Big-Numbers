#include "pch.h"
#include <D3DGraphics/ColormapDialog.h>

D3DCOLORVALUE COLORREF2COLORVALUE(COLORREF c) {
  D3DCOLORVALUE result;
  result.r = ((float)(GetRValue(c))) / 255.0f;
  result.g = ((float)(GetGValue(c))) / 255.0f;
  result.b = ((float)(GetBValue(c))) / 255.0f;
  result.a = 1.0f;
  return result;
}
