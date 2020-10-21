#include "pch.h"
#include <plpngdec.h>
#include <plpngenc.h>
#include "BitmapCodec.h"

#pragma comment(lib, LIB_VERSION "libpng.lib")

// need to swap Red and Blue channel because PNG-encoder does the same
static HBITMAP swapRB(HBITMAP bm) {
  const BITMAP info = getBitmapInfo(bm);
  BITMAPINFO bmInfo;
  ZeroMemory(&bmInfo, sizeof(BITMAPINFO));
  bmInfo.bmiHeader.biSize        = sizeof(BITMAPINFO);
  bmInfo.bmiHeader.biWidth       = info.bmWidth;
  bmInfo.bmiHeader.biHeight      = info.bmHeight;
  bmInfo.bmiHeader.biPlanes      = 1;
  bmInfo.bmiHeader.biBitCount    = info.bmBitsPixel;
  bmInfo.bmiHeader.biCompression = BI_RGB;
  const int byteCount = info.bmWidthBytes * info.bmHeight;
  const int pixelCount = byteCount / sizeof(DWORD);
  DWORD *pixelArray = new DWORD[pixelCount]; TRACE_NEW(pixelArray);
  HDC hdc = getScreenDC();
  GetDIBits(hdc, bm, 0, info.bmHeight, pixelArray, &bmInfo, DIB_RGB_COLORS);
  DWORD *p = pixelArray;
  for(int i = pixelCount; i--;) {
    const DWORD c = *p;
    *(p++) = (c & 0xff00ff00) | ((c>>16)&0xff) | ((c<<16)&0x00ff0000);
  }
  SetDIBits(hdc, bm, 0, info.bmHeight, pixelArray, &bmInfo, DIB_RGB_COLORS);
  return bm;
}


void writeAsPNG(HBITMAP bm, ByteOutputStream &out) {
  const BITMAP info = getBitmapInfo(bm);
  HBITMAP bm1 = nullptr;
  PLPNGEncoder encoder;
  encoder.SetCompressionLevel(9);
  try {
    if(info.bmBitsPixel == 32) {
      bm1 = swapRB(cloneBitmap(bm));
      encodeBitmap(bm1, out, encoder);
      DeleteObject(bm1); bm1 = nullptr;
    } else {
      encodeBitmap(bm, out, encoder);
    }
  } catch(...) {
    if(bm1) DeleteObject(bm1);
    throw;
  }
}

HBITMAP decodeAsPNG(const ByteArray &bytes, bool &hasAlpha) {
  return decodeToBitmap(bytes, PLPNGDecoder(), hasAlpha);
}

