#include "pch.h"

String toString(const CPoint &p) {
  return format(_T("(%3d,%3d)"), p.x, p.y);
}

String toString(const CSize &s) {
  return format(_T("(%3d,%3d)"), s.cx, s.cy);
}
