#include "stdafx.h"
#include <MyUtil.h>
#include <MFCUtil/WinTools.h>

void DebugBitmap::showBitmap(HBITMAP bitmap) { // static
  HDC screenDC = NULL, tmpDC = NULL;
  try {
    screenDC = getScreenDC();
    BITMAP info;
    if(GetObject(bitmap, sizeof(info),&info) == 0) {
      throwLastErrorOnSysCallException(_T("GetObject"));
    }

    BitBlt(screenDC,0,0,info.bmWidth,info.bmHeight,NULL,0,0,WHITENESS);
    tmpDC      = CreateCompatibleDC(screenDC);
    HGDIOBJ oldgdi = SelectObject(tmpDC, bitmap);
    if(BitBlt(screenDC, 0, 0, info.bmWidth, info.bmHeight, tmpDC, 0, 0, SRCCOPY) == 0) {
      throwLastErrorOnSysCallException(_T("BitBlt"));
    }
    SelectObject(tmpDC, oldgdi);
    DeleteDC(tmpDC);
    DeleteDC(screenDC);
  } catch(Exception e) {
    if(screenDC != NULL) {
      TextOut(screenDC,0,0,e.what(), _tcsclen(e.what()));
    } else {
      throw;
    }
    if(tmpDC   ) DeleteDC(tmpDC   );
    if(screenDC) DeleteDC(screenDC);
  }
}

