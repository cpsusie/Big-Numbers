#include "stdafx.h"

#include <MyUtil.h>
#include <ByteArray.h>
#include <MFCUtil/WinTools.h>
#include <MFCUtil/FileSink.h>
#include <MFCUtil/ColorSpace.h>
#include <plbmpenc.h>
#include <plbmpdec.h>
#include <pljpegenc.h>
#include <pljpegdec.h>
#include <plpngdec.h>
#include <plpngenc.h>
#include <pltiffenc.h>
#include <pltiffdec.h>
#include <plwinbmp.h>
#include <plexcept.h>

#pragma comment(lib, LIB_VERSION "libjpeg.lib")
#pragma comment(lib, LIB_VERSION "libtiff.lib")
#pragma comment(lib, LIB_VERSION "libpng.lib")
#pragma comment(lib, LIB_VERSION "common.lib")
#pragma comment(lib, LIB_VERSION "zlib.lib")

static void encodeBitmap(HBITMAP bm, FILE *f, PLPicEncoder &encoder) {
  PLWinBmp winBmp;
  winBmp.CreateFromHBitmap(bm);
  FileSink sink(f);
  encoder.SaveBmp(&winBmp,&sink);
}

#ifdef __NEVER__
static void showAlphaBitmap(HDC dst, const CPoint &p, HBITMAP bm) {
  PixRect pr(bm);
  const CSize size = pr.getSize();

  pr.preMultiplyAlpha();

  PixRect *tmpDst = pr.clone();
  PixRect::bitBlt(tmpDst, CPoint(0,0), size, SRCCOPY, dst, p);
  HDC tmpDC = tmpDst->getDC();
  PixRect::alphaBlend(tmpDC, 0, 0, size.cx, size.cy,  pr, 0, 0, size.cx, size.cy, 255);
  tmpDst->releaseDC(tmpDC);
  PixRect::bitBlt(dst, p, size, SRCCOPY, tmpDst, CPoint(0,0));
  delete tmpDst;
}
#endif

static void showNormalBitmap(HDC dst, const CPoint &p, HBITMAP bm) {
  HDC srcDC = CreateCompatibleDC(dst);
  const CSize size = getBitmapSize(bm);
  HGDIOBJ oldGdi = SelectObject(srcDC, bm);
  BitBlt(dst, p.x,p.y,size.cx, size.cy, srcDC, 0,0, SRCCOPY);
  SelectObject(srcDC, oldGdi);
  delete srcDC;
}

static HBITMAP decodeToBitmap(const ByteArray &bytes, PLPicDecoder &decoder, bool &hasAlpha) {
  HDC     dc     = NULL;
  HBITMAP bitmap = NULL;
  try {
    try {
      PLWinBmp winBmp;
      decoder.MakeBmpFromMemory((unsigned char*)bytes.getData(), bytes.size(),&winBmp);
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
      if(bitmap == NULL) {
        throwLastErrorOnSysCallException(_T("CreateDIBitmap"));
      }
      DeleteDC(dc);
      hasAlpha = winBmp.HasAlpha();

/*
      HDC screenDC = getScreenDC();
      if() {
        showAlphaBitmap(screenDC, CPoint(100,100), bitmap);
      } else {
        showNormalBitmap(screenDC, CPoint(100,100), bitmap);
      }
      DeleteDC(screenDC);
*/
      return bitmap;
    } catch(PLTextException e) {
      const String errMsg = e;
      throwException(errMsg);
      return NULL;
    }
  } catch(...) {
    if(bitmap) DeleteObject(bitmap);
    if(dc    ) DeleteDC(dc);
    throw;
  }
}

static void encodeBitmap(HBITMAP bm, const String &fileName, PLPicEncoder &encoder) {
  FILE *file = FOPEN(fileName,"wb");
  try {
    encodeBitmap(bm, file, encoder);
    fclose(file);
  } catch(...) {
    fclose(file);
    throw;
  }
}

// ----------------------------------------- BMP -----------------------------------

void writeAsBMP(HBITMAP bm, const String &fileName) {
  encodeBitmap(bm, fileName, PLBmpEncoder());
}

void writeAsBMP(HBITMAP bm, FILE *f) {
  encodeBitmap(bm, f, PLBmpEncoder());
}

// ----------------------------------------- JPG -----------------------------------

void writeAsJPG(HBITMAP bm, const String &fileName) {
  encodeBitmap(bm, fileName, PLJPEGEncoder());
}

void writeAsJPG(HBITMAP bm, FILE *f) {
  encodeBitmap(bm, f, PLJPEGEncoder());
}

// ----------------------------------------- PNG ------------------------------------

void writeAsPNG(HBITMAP bm, const String &fileName) {
  FILE *file = FOPEN(fileName,"wb");
  try {
    writeAsPNG(bm, file);
    fclose(file);
  } catch(...) {
    fclose(file);
    throw;
  }
}

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
  DWORD *pixelArray = new DWORD[pixelCount];
  HDC hdc = getScreenDC();
  GetDIBits(hdc, bm, 0, info.bmHeight, pixelArray, &bmInfo, DIB_RGB_COLORS);
  DWORD *p = pixelArray;
  for(int i = pixelCount; i--;) {
    const DWORD c = *p;
    *(p++) = COLORREF2D3DCOLOR(c) | (c&0xff000000);
  }
  SetDIBits(hdc, bm, 0, info.bmHeight, pixelArray, &bmInfo, DIB_RGB_COLORS);
  return bm;
}


void writeAsPNG(HBITMAP bm, FILE *f) {
  const BITMAP info = getBitmapInfo(bm);
  HBITMAP bm1 = NULL;
  PLPNGEncoder encoder;
  encoder.SetCompressionLevel(9);
  try {
    if(info.bmBitsPixel == 32) {
      bm1 = swapRB(cloneBitmap(bm));
      encodeBitmap(bm1, f, encoder);
      DeleteObject(bm1); bm1 = NULL;
    } else {
      encodeBitmap(bm, f, encoder);
    }
  } catch(...) {
    if(bm1) DeleteObject(bm1);
    throw;
  }
}

HBITMAP decodeAsPNG(const ByteArray &bytes, bool &hasAlpha) {
  return decodeToBitmap(bytes, PLPNGDecoder(), hasAlpha);
}

// ----------------------------------------- TIFF-----------------------------------

void writeAsTIFF(HBITMAP bm, const String &fileName) {
  encodeBitmap(bm, fileName, PLTIFFEncoder());
}

void writeAsTIFF(HBITMAP bm, FILE *f) {
  encodeBitmap(bm, f, PLTIFFEncoder());
}

HBITMAP decodeAsTIFF(const ByteArray &bytes, bool &hasAlpha) {
  return decodeToBitmap(bytes, PLTIFFDecoder(), hasAlpha);
}

