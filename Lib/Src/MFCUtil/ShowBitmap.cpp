#include "pch.h"

void DebugBitmap::showBitmap(HBITMAP bitmap) { // static
  HDC screenDC = nullptr, tmpDC = nullptr;
  try {
    screenDC = getScreenDC();
    BITMAP info;
    if(GetObject(bitmap, sizeof(info),&info) == 0) {
      throwLastErrorOnSysCallException(__TFUNCTION__, _T("GetObject"));
    }

    BitBlt(screenDC,0,0,info.bmWidth,info.bmHeight,nullptr,0,0,WHITENESS);
    tmpDC      = CreateCompatibleDC(screenDC);
    HGDIOBJ oldgdi = SelectObject(tmpDC, bitmap);
    if(BitBlt(screenDC, 0, 0, info.bmWidth, info.bmHeight, tmpDC, 0, 0, SRCCOPY) == 0) {
      throwLastErrorOnSysCallException(__TFUNCTION__, _T("BitBlt"));
    }
    SelectObject(tmpDC, oldgdi);
    DeleteDC(tmpDC);
    DeleteDC(screenDC);
  } catch(Exception e) {
    if(screenDC != nullptr) {
      TextOut(screenDC,0,0,e.what(), (int)_tcsclen(e.what()));
    } else {
      throw;
    }
    if(tmpDC   ) DeleteDC(tmpDC   );
    if(screenDC) DeleteDC(screenDC);
  }
}

