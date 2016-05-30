#include "stdafx.h"
#include <MyUtil.h>
#include <shlobj.h>
#include <MfCUtil/WinTools.h>
#include <MFCUtil/Clipboard.h>

HBITMAP getClipboardBitmap() {
  if(!OpenClipboard(NULL)) {
    return NULL;
  }

  HANDLE h = GetClipboardData(CF_BITMAP);
  if(h == NULL) {
    CloseClipboard();
    return NULL;
  }
  HBITMAP bitmap1 = (HBITMAP)h;

  HDC screenDC = getScreenDC();
  HDC dc1 = CreateCompatibleDC(NULL);
  HDC dc2 = CreateCompatibleDC(NULL);
  BITMAP info;
  ::GetObject(bitmap1,sizeof(info),&info);

  HGDIOBJ oldgdi1 = ::SelectObject(dc1, bitmap1);
  HBITMAP result  = ::CreateCompatibleBitmap(screenDC, info.bmWidth, info.bmHeight);
  HGDIOBJ oldgdi2 = ::SelectObject(dc2, result);
  BitBlt(dc2, 0,0,info.bmWidth,info.bmHeight, dc1, 0,0, SRCCOPY);

  ::SelectObject(dc1, oldgdi1);
  ::SelectObject(dc2, oldgdi2);
  DeleteDC(dc1);
  DeleteDC(dc2);
  DeleteDC(screenDC);
  CloseClipboard();

  return result;
}

void putClipboard(HWND hwnd, HBITMAP bitmap) {
  if(!OpenClipboard(hwnd)) {
    throwLastErrorOnSysCallException(_T("OpenClipboard"));
  }

  try {
    if(!EmptyClipboard()) {
      throwLastErrorOnSysCallException(_T("EmptyClipboard"));
    }
    if(::SetClipboardData(CF_BITMAP, bitmap) == NULL) {
      throwLastErrorOnSysCallException(_T("SetClipboardData"));
    }
    CloseClipboard();
  } catch(...) {
    CloseClipboard();
    throw;
  }
}
