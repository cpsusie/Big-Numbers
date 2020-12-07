#include "pch.h"

#define CHECK(syscall) if(!syscall) throwLastErrorOnSysCallException(__TFUNCTION__,_T(#syscall))

WINDOWPLACEMENT getWindowPlacement(HWND hwnd) {
  WINDOWPLACEMENT wpl;
  CHECK(GetWindowPlacement(hwnd, &wpl));
  return wpl;
}

void setWindowPlacement(HWND hwnd, const WINDOWPLACEMENT &wpl) {
  CHECK(SetWindowPlacement(hwnd, &wpl));
}

CRect getWindowRect(HWND hwnd) {
  CRect rect;
  CHECK(::GetWindowRect(hwnd, &rect));
  return rect;
}

CPoint getWindowPosition(HWND hwnd) {
  return CRect(getWindowPlacement(hwnd).rcNormalPosition).TopLeft();
}

void setWindowPosition(HWND hwnd, const CPoint &pos) {
  WINDOWPLACEMENT wpl = getWindowPlacement(hwnd);
  const int dx = pos.x - wpl.rcNormalPosition.left;
  const int dy = pos.y - wpl.rcNormalPosition.top;
  if(dx | dy) {
    wpl.rcNormalPosition.left   += dx;
    wpl.rcNormalPosition.right  += dx;
    wpl.rcNormalPosition.top    += dy;
    wpl.rcNormalPosition.bottom += dy;
    setWindowPlacement(hwnd, wpl);
  }
}

CSize getWindowSize(HWND hwnd) {
  return getWindowRect(hwnd).Size();
}

void setWindowSize(HWND hwnd, const CSize &size) {
  WINDOWPLACEMENT wpl = getWindowPlacement(hwnd);
  wpl.rcNormalPosition.right  = wpl.rcNormalPosition.left + size.cx;
  wpl.rcNormalPosition.bottom = wpl.rcNormalPosition.top  + size.cy;
  setWindowPlacement(hwnd, wpl);
}

CRect getClientRect(HWND hwnd) {
  CRect r;
  CHECK(GetClientRect(hwnd, &r));
  return r;
}
