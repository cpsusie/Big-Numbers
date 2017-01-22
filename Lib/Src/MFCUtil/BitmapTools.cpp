#include "pch.h"

BITMAP getBitmapInfo(HBITMAP bm) {
  BITMAP info;
  if(GetObject(bm, sizeof(info), &info) == 0) {
    throwException(_T("GetObject(HBITMAP) failed:%s. bm=%d"), getLastErrorText().cstr(), bm);
  }
  return info;
}

BITMAPINFOHEADER getBitmapInfoHeader(HBITMAP bm) {
  const BITMAP info = getBitmapInfo(bm);
  HDC hdc = NULL;
  try {
    hdc = CreateCompatibleDC(NULL);
    BITMAPINFOHEADER infoHeader;
    ZeroMemory(&infoHeader, sizeof(infoHeader));
    infoHeader.biSize = sizeof(infoHeader);
    if(!GetDIBits(hdc, bm, 0, info.bmHeight, NULL, (LPBITMAPINFO)&infoHeader, DIB_RGB_COLORS)) {
      throwLastErrorOnSysCallException(_T("GetDIBits"));
    }
    DeleteDC(hdc);
    return infoHeader;
  } catch(...) {
    if(hdc) DeleteDC(hdc);
    throw;
  }
}

CSize getBitmapSize(HBITMAP bm) {
  BITMAP info = getBitmapInfo(bm);
  return CSize(info.bmWidth, info.bmHeight);
}

CBitmap *createFromHandle(HBITMAP bm) {
  const BITMAP info = getBitmapInfo(bm);

  HDC srcDC = CreateCompatibleDC(NULL);
  HDC dstDC = CreateCompatibleDC(NULL);

  CBitmap *result = new CBitmap;
  result->CreateBitmap(info.bmWidth, info.bmHeight, info.bmPlanes, info.bmBitsPixel, NULL);
  HBITMAP bm1 = *result;

  SelectObject(srcDC, bm);
  SelectObject(dstDC, bm1);

  int ret = BitBlt(dstDC, 0,0,info.bmWidth,info.bmHeight, srcDC, 0,0, SRCCOPY);

  DeleteDC(dstDC);
  DeleteDC(srcDC);

  return result;
}

HBITMAP cloneBitmap(HBITMAP bm) {
  BITMAP info = getBitmapInfo(bm);

  HDC srcDC = CreateCompatibleDC(NULL);
  HDC dstDC = CreateCompatibleDC(NULL);

  HBITMAP result = CreateBitmap(info.bmWidth, info.bmHeight, info.bmPlanes, info.bmBitsPixel, NULL);

  SelectObject(srcDC, bm);
  SelectObject(dstDC, result);

  BitBlt(dstDC, 0,0,info.bmWidth,info.bmHeight, srcDC, 0,0, SRCCOPY);

  DeleteDC(dstDC);
  DeleteDC(srcDC);
  return result;
}
