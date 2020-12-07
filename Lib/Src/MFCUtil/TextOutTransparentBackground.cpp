#include "pch.h"

void textOutTransparentBackground(HDC hdc, const CPoint &p, const String &s, CFont &font, COLORREF color) {
  const int oldMode = SetBkMode(hdc, TRANSPARENT);
  if(oldMode == 0) {
    throwLastErrorOnSysCallException(__TFUNCTION__, _T("SetBkMode"));
  }
  HGDIOBJ  oldFont  = SelectObject(hdc, font );
  COLORREF oldColor = SetTextColor(hdc, color);

  try {
    textOut(hdc, p, s);
    SetTextColor(hdc, oldColor);
    SelectObject(hdc, oldFont );
    SetBkMode(   hdc, oldMode );
  } catch(...) {
    SetTextColor(hdc, oldColor);
    SelectObject(hdc, oldFont );
    SetBkMode(   hdc, oldMode );
    throw;
  }
}
