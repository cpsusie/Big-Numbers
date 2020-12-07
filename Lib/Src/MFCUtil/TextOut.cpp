#include "pch.h"

void textOut(HDC hdc, const CPoint &p, const String &s) {
  textOut(hdc, p.x, p.y, s);
}

void textOut(HDC hdc, int x, int y, const String &s) {
  const BOOL ok = TextOut(hdc, x, y, s.cstr(), (int)s.length());
  if(!ok) {
    throwLastErrorOnSysCallException(__TFUNCTION__, _T("textOut"));
  }
}
