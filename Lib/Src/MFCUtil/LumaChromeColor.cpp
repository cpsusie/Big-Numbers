#include "pch.h"
#include <Math.h>
#include <MFCUtil/ColorSpace.h>

#pragma warning(disable : 4244)

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
