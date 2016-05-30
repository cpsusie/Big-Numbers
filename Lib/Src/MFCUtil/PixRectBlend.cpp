#include "stdafx.h"
#include <PixRect.h>

#pragma warning(disable : 4244)

void PixRect::alphaBlend(PixRect &dst, const CRect &dstRect, const PixRect &src, const CRect &srcRect, int srcConstAlpha) { // static 
  alphaBlend(dst, dstRect.left, dstRect.top, dstRect.Width(), dstRect.Height(), src, srcRect.left, srcRect.top, srcRect.Width(), srcRect.Height(), srcConstAlpha);
}

#define AC_SRC_ALPHA                0x01

#pragma comment(lib, "MSIMG32.lib")

void PixRect::alphaBlend(PixRect &dst, int x, int y, int w, int h,  const PixRect &src, int sx, int sy, int sw, int sh, int srcConstAlpha) {  // static 
  HDC dc = NULL;
  try {
    dc = dst.getDC();
    alphaBlend(dc, x, y, w, h, src, sx, sy, sw, sh, srcConstAlpha);
    dst.releaseDC(dc);
  } catch(...) {
    if(dc) dst.releaseDC(dc);
    throw;
  }
}

void PixRect::alphaBlend(HDC dst, const CRect &dstRect, const PixRect &src, const CRect &srcRect, int srcConstAlpha) { // static 
  alphaBlend(dst, dstRect.left, dstRect.top, dstRect.Width(), dstRect.Height(), src, srcRect.left, srcRect.top, srcRect.Width(), srcRect.Height(), srcConstAlpha);
}

void PixRect::alphaBlend(HDC dst, int x, int y, int w, int h,  const PixRect &src, int sx, int sy, int sw, int sh, int srcConstAlpha) {  // static 
  String errMsg;
  HDC srcDC = src.getDC();

  BLENDFUNCTION blendFunction;
  blendFunction.BlendOp             = AC_SRC_OVER;
  blendFunction.BlendFlags          = 0;
  blendFunction.SourceConstantAlpha = srcConstAlpha;
  blendFunction.AlphaFormat         = AC_SRC_ALPHA;

  BOOL ok = AlphaBlend(dst, x,y,w,h, srcDC, sx,sy,sw,sh, blendFunction);
  if(!ok) {
    errMsg = getLastErrorText();
  }
  src.releaseDC(srcDC);
  if(!ok) {
    throwException(_T("AlphaBlend failed:%s"), errMsg.cstr());
  }
}

#define PM(c, alpha, channel) (int)RGB_GET##channel(c)*(alpha)/255
#define SETALPHA(c, alpha)    RGBA_MAKE(PM(c, alpha, RED), PM(c, alpha, GREEN), PM(c, alpha, BLUE), alpha)

void PixRect::preMultiplyAlpha() {
  const CSize size = getSize();

  PixelAccessor *pa = getPixelAccessor();
  for(CPoint sp(0,0); sp.y < size.cy; sp.y++) {
    for(sp.x = 0; sp.x < size.cx; sp.x++) {
      D3DCOLOR pixel = pa->getPixel(sp);
      const BYTE alpha = RGBA_GETALPHA(pixel);
      if(alpha != 255) {
        pixel = SETALPHA(pixel, alpha);
        pa->setPixel(sp, pixel);
      }
    }
  }
  delete pa;
}
