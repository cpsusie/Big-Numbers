#include "pch.h"

void textOutTransparentBackground(HDC hdc, const CPoint &p, const String &s, CFont &font, COLORREF color) {

  const int oldMode = SetBkMode(hdc, TRANSPARENT);
  if(oldMode == 0) {
    throwLastErrorOnSysCallException(_T("SetBkMode"));
  }
  HGDIOBJ  oldFont  = SelectObject(hdc, font);
  COLORREF oldColor = SetTextColor(hdc, color);
  
  BOOL ok = TextOut(hdc, p.x, p.y, s.cstr(), (int)s.length());
  String errorMsg;
  if(!ok) {
    errorMsg = getLastErrorText();
  }

  SetTextColor(hdc, oldColor);
  SelectObject(hdc, oldFont );
  SetBkMode(   hdc, oldMode );

  if(!ok) {
    throwException(_T("TextOut failed. %s"), errorMsg.cstr());
  }
}
