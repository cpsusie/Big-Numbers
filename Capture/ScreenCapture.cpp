#include "stdafx.h"
#include <Math/Rectangle2D.h>
#include "ScreenCapture.h"

static HBITMAP captureDC(HDC dc, int x, int y, const CSize &size) {
  HDC     memDC  = NULL;
  HBITMAP bitmap = NULL;

  try {
    if((memDC = CreateCompatibleDC(dc)) == NULL) {
      throwException(_T("CreateCompatibleDC(%08x) failed:%s"), dc, getLastErrorText().cstr());
    }

    if((bitmap = CreateCompatibleBitmap(dc, size.cx, size.cy)) == NULL) {
      throwException(_T("CreateCompatibleBitmap(%08x) failed:%s"), dc, getLastErrorText().cstr());
    }

    if(!SelectObject(memDC, bitmap)) {
      throwException(_T("SelecetObject failed:%s"), getLastErrorText().cstr());
    }

    if(!BitBlt(memDC, 0, 0, size.cx, size.cy, dc, x,y, SRCCOPY)) {
      throwException(_T("BitBlt failed:%s"), getLastErrorText().cstr());
    }
    DeleteDC(memDC);
    return bitmap;
  } catch(...) {
    if(bitmap != NULL) {
      DeleteObject(bitmap);
    }
    if(memDC != NULL) {
      DeleteDC(memDC);
    }
    throw;
  }
}

HBITMAP captureScreen(const CRect &rect) {
  const CRect r = makePositiveRect(rect);
  return captureScreen(r.left, r.top, r.Size());
}

HBITMAP captureScreen(int x, int y, const CSize &size) {
  HDC screenDC = getScreenDC();
  try {
    HBITMAP result = captureDC(screenDC, x, y, size);
    DeleteDC(screenDC);
    return result;
  } catch(...) {
    DeleteDC(screenDC);
    throw;
  }
}

HBITMAP captureWindow(HWND hwnd) {
  HDC winDC = ::GetWindowDC(hwnd);
  try {
    HBITMAP result = captureDC(winDC, 0, 0, getWindowSize(hwnd));
    ::ReleaseDC(hwnd, winDC);
    return result;
  } catch(...) {
    ::ReleaseDC(hwnd, winDC);
    throw;
  }
}

/*
// giver forkert resultat
CSize getScreenSizeInMillimeters1() {
  HDC screenDC = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
  int w = GetDeviceCaps(screenDC,HORZSIZE);
  int h = GetDeviceCaps(screenDC,VERTSIZE);
  DeleteDC(screenDC);
  return CSize(w,h);
}
*/

