#include "pch.h"
#include <plwinbmp.h>
#include <plexcept.h>
#include <MFCUtil/FileSink.h>
#include "BitmapCodec.h"

#pragma comment(lib, LIB_VERSION "common.lib")
#pragma comment(lib, LIB_VERSION "zlib.lib")

void encodeBitmap(HBITMAP bm, ByteOutputStream &out, PLPicEncoder &encoder) {
  PLWinBmp winBmp;
  winBmp.CreateFromHBitmap(bm);
  ByteStreamSink sink(out);
  encoder.SaveBmp(&winBmp, &sink);
}
//#define TEST_DECODEBITMAP
#if defined(TEST_DECODEBITMAP)
#include <MFCUtil/PixRect.h>

static void showAlphaBitmap(HDC dst, const CPoint &p, HBITMAP bm) {
  HWND hwnd = GetDesktopWindow();

  PixRectDevice device;
  device.attach(hwnd, true);
  PixRect pr(device, bm, D3DPOOL_DEFAULT);
  CRect r0(ORIGIN, pr.getSize());

  pr.preMultiplyAlpha();

  PixRect *tmpDst = pr.clone();
  PixRect::bitBlt(tmpDst, r0, SRCCOPY, dst, p);
  HDC tmpDC = tmpDst->getDC();
  PixRect::alphaBlend(tmpDC, r0, pr, r0, 255);
  tmpDst->releaseDC(tmpDC);
  PixRect::bitBlt(dst, p, r0.Size(), SRCCOPY, tmpDst, ORIGIN);
  SAFEDELETE(tmpDst);
}

static void showNormalBitmap(HDC dst, const CPoint &p, HBITMAP bm) {
  HDC srcDC = CreateCompatibleDC(dst);
  const CSize size = getBitmapSize(bm);
  HGDIOBJ oldGdi = SelectObject(srcDC, bm);
  BitBlt(dst, p.x,p.y,size.cx, size.cy, srcDC, 0,0, SRCCOPY);
  SelectObject(srcDC, oldGdi);
  delete srcDC;
}
#endif TEST_DECODEBITMAP

HBITMAP decodeToBitmap(const ByteArray &bytes, PLPicDecoder &decoder, bool &hasAlpha) {
  HDC     dc     = nullptr;
  HBITMAP bitmap = nullptr;
  try {
    try {
      PLWinBmp winBmp;
      decoder.MakeBmpFromMemory((unsigned char*)bytes.getData(), (int)bytes.size(),&winBmp);
      const int width      = winBmp.GetWidth();
      const int height     = winBmp.GetHeight();
      const int totalBytes = winBmp.GetBytesPerLine() * height;

      dc     = getScreenDC();

#if WINVER >= 0x0500

      BITMAPV5HEADER bmHeader;
      ZeroMemory(&bmHeader, sizeof(bmHeader));
      bmHeader.bV5Size        = sizeof(bmHeader);
      bmHeader.bV5Width       = width;
      bmHeader.bV5Height      = height;
      bmHeader.bV5Planes      = 1;
      bmHeader.bV5BitCount    = winBmp.GetBitsPerPixel();
      bmHeader.bV5Compression = BI_BITFIELDS;
      bmHeader.bV5SizeImage   = totalBytes;
      bmHeader.bV5RedMask     = 0x000000ff;
      bmHeader.bV5GreenMask   = 0x0000ff00;
      bmHeader.bV5BlueMask    = 0x00ff0000;
      bmHeader.bV5AlphaMask   = 0xff000000;
      bmHeader.bV5CSType      = LCS_sRGB; // INDOWS_COLOR_SPACE;

#else

      BITMAPV4HEADER bmHeader;
      ZeroMemory(&bmHeader, sizeof(bmHeader));
      bmHeader.bV4Size          = sizeof(bmHeader);
      bmHeader.bV4Width         = width;
      bmHeader.bV4Height        = height;
      bmHeader.bV4Planes        = 1;
      bmHeader.bV4BitCount      = winBmp.GetBitsPerPixel();
      bmHeader.bV4V4Compression = BI_BITFIELDS;
      bmHeader.bV4SizeImage     = totalBytes;
      bmHeader.bV4RedMask       = 0x000000ff;
      bmHeader.bV4GreenMask     = 0x0000ff00;
      bmHeader.bV4BlueMask      = 0x00ff0000;
      bmHeader.bV4AlphaMask     = 0xff000000;
      bmHeader.bV4CSType        = LCS_sRGB; // INDOWS_COLOR_SPACE;

#endif

      BITMAPINFO bmInfo;
      ZeroMemory(&bmInfo, sizeof(bmInfo));
      bmInfo.bmiHeader.biSize        = sizeof(bmInfo);
      bmInfo.bmiHeader.biWidth       = width;
      bmInfo.bmiHeader.biHeight      = height; // positive gives bottom up bitmap
      bmInfo.bmiHeader.biPlanes      = 1;
      bmInfo.bmiHeader.biBitCount    = winBmp.GetBitsPerPixel();
      bmInfo.bmiHeader.biCompression = BI_RGB;
      bmInfo.bmiHeader.biSizeImage   = totalBytes;

      bitmap = CreateDIBitmap(dc,  (BITMAPINFOHEADER*)&bmHeader, CBM_INIT, winBmp.GetBits(), &bmInfo, DIB_RGB_COLORS);
      if(bitmap == nullptr) {
        throwLastErrorOnSysCallException(_T("CreateDIBitmap"));
      }
      DeleteDC(dc);
      hasAlpha = winBmp.HasAlpha();

#if defined(TEST_DECODEBITMAP)
      HDC screenDC = getScreenDC();
      if(hasAlpha) {
        showAlphaBitmap(screenDC, CPoint(100,100), bitmap);
      } else {
        showNormalBitmap(screenDC, CPoint(100,100), bitmap);
      }
      DeleteDC(screenDC);
#endif
      return bitmap;
    } catch(PLTextException e) {
      const String errMsg = e;
      throwException(errMsg);
      return nullptr;
    }
  } catch(...) {
    if(bitmap) DeleteObject(bitmap);
    if(dc    ) DeleteDC(dc);
    throw;
  }
}
