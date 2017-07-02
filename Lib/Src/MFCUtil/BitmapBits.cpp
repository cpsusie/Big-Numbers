#include "pch.h"

ByteArray &getBitmapBits(HBITMAP bm, ByteArray &bytes, BITMAPINFO &bmInfo) {
  const BITMAP info = getBitmapInfo(bm);
  ZeroMemory(&bmInfo, sizeof(BITMAPINFO));
  bmInfo.bmiHeader.biSize        = sizeof(BITMAPINFO);
  bmInfo.bmiHeader.biWidth       = info.bmWidth;
  bmInfo.bmiHeader.biHeight      = info.bmHeight;
  bmInfo.bmiHeader.biPlanes      = 1;
  bmInfo.bmiHeader.biBitCount    = info.bmBitsPixel;
  bmInfo.bmiHeader.biCompression = BI_RGB;

  const int byteCount  = info.bmWidthBytes * info.bmHeight;
  BYTE     *byteArray  = new BYTE[byteCount];
  HDC       hdc        = getScreenDC();
  try {
    if(!GetDIBits(hdc, bm, 0, info.bmHeight, byteArray, &bmInfo, DIB_RGB_COLORS)) {
      throwLastErrorOnSysCallException(_T("GetDIBits"));
    }
    bytes.clear();
    bytes.setData(byteArray, byteCount);
    delete[] byteArray;
    DeleteDC(hdc);
  } catch(...) {
    delete[] byteArray;
    DeleteDC(hdc);
    throw;
  }
  return bytes;
}

void setBitmapBits(HBITMAP bm, const ByteArray &bytes, const BITMAPINFO &bmInfo) {
  HDC hdc = getScreenDC();
  if(!SetDIBits(hdc, bm, 0, bmInfo.bmiHeader.biHeight, bytes.getData(), &bmInfo, DIB_RGB_COLORS)) {
    DeleteDC(hdc);
    throwLastErrorOnSysCallException(_T("SetDIBits"));
  }
  DeleteDC(hdc);
}
