#include "pch.h"
#include <MFCUtil/ColorSpace.h>

#pragma comment(lib, "MSIMG32.lib")

void alphaBlend(HDC dst, int x, int y, int w, int h,  HDC src, int sx, int sy, int sw, int sh, int srcConstAlpha) {
  BLENDFUNCTION blendFunction;
  blendFunction.BlendOp             = AC_SRC_OVER;
  blendFunction.BlendFlags          = 0;
  blendFunction.SourceConstantAlpha = srcConstAlpha;
  blendFunction.AlphaFormat         = AC_SRC_ALPHA;

  if(!AlphaBlend(dst, x,y,w,h, src, sx,sy,sw,sh, blendFunction)) {
    throwException(_T("AlphaBlend failed:%s"), getLastErrorText().cstr());
  }
}

void preMultiplyAlpha(HBITMAP bm) {
  ByteArray  bits;
  BITMAPINFO bmInfo;
  getBitmapBits(bm, bits, bmInfo);

  if(bmInfo.bmiHeader.biBitCount != 32) {
    throwInvalidArgumentException(__TFUNCTION__,_T("bitmap has %d bits/pixel. Must be 32")
                                 ,bmInfo.bmiHeader.biBitCount);
  }
#define PM(c, alpha, channel) (int)ARGB_GET##channel(c)*(alpha)/255
#define SETALPHA(c, alpha)    D3DCOLOR_ARGB(alpha, PM(c, alpha, RED), PM(c, alpha, GREEN), PM(c, alpha, BLUE))

  // we treat pixels as D3DCOLOR, even it is COLORREF
  // Doesn't matter, because alpha-channel
  // is at the same position

  D3DCOLOR *pixels = (D3DCOLOR*)bits.getData();
  const int count  = bmInfo.bmiHeader.biWidth*bmInfo.bmiHeader.biHeight;

  assert(bits.size() == count * sizeof(D3DCOLOR));

  const D3DCOLOR *last = pixels + count;
  for(D3DCOLOR *p = pixels; p < last; p++) {
    const D3DCOLOR pixel = *p;
    const BYTE     alpha = ARGB_GETALPHA(pixel);
    if(alpha && (alpha != 255)) {
      *p = SETALPHA(pixel, alpha);
    }
  }
  setBitmapBits(bm, bits, bmInfo);
}
