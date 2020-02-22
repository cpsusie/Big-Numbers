#include "pch.h"
#include <D3DGraphics/D3Ray.h>

String D3Ray::toString(int dec) const {
  return format(_T("Orig:%s, Dir:%s"), ::toString(m_orig, dec).cstr(), ::toString(m_dir, dec).cstr());
}
