#include "stdafx.h"
#include <Math.h>
#include <MFCUtil/AnimatedImage.h>
#include <Math/Rectangle2D.h>
#include "GifPixRect.h"

GifPixRect::GifPixRect(const SavedImage *image, const ColorMapObject *globalColorMap)
  : PixRect(theApp.m_device, PIXRECT_PLAINSURFACE, image->ImageDesc.Width, image->ImageDesc.Height)
{
  copyRasterBits(image->RasterBits, image->ImageDesc.ColorMap ? image->ImageDesc.ColorMap->Colors : globalColorMap->Colors);
  init();
  m_topLeft = CPoint(image->ImageDesc.Left, image->ImageDesc.Top);
  m_gcb = getGCB(image);
}

GifPixRect::GifPixRect(const CSize &size) : PixRect(theApp.m_device, PIXRECT_PLAINSURFACE, size) {
  init();
}

GifPixRect::GifPixRect(int w, int h) : PixRect(theApp.m_device, PIXRECT_PLAINSURFACE, w, h) {
  init();
}

GifPixRect *GifPixRect::clone(double scaleFactor) const {
  const CSize   rawSize = getSize();
  const CSize   scaledSize((int)(scaleFactor * rawSize.cx), (int)(scaleFactor * rawSize.cy));
  GifPixRect   *copy = new GifPixRect(scaledSize); TRACE_NEW(copy);
  if (scaleFactor == 1) {
    copy->copyImage(this);
  }
  else {
    HDC dc = copy->getDC();
    bool highQuality = true;
    SetStretchBltMode(dc, highQuality ? HALFTONE : COLORONCOLOR);
    PixRect::stretchBlt(dc, copy->getRect(), SRCCOPY, this, getRect());
    copy->releaseDC(dc);
  }
  copy->m_topLeft = CPoint((int)(scaleFactor * m_topLeft.x), (int)(scaleFactor * m_topLeft.y));
  copy->m_gcb = m_gcb;
  return copy;
}

GifPixRect *GifPixRect::clone(const CSize &newSize) const {
  const CSize   rawSize = getSize();
  GifPixRect   *copy = new GifPixRect(newSize); TRACE_NEW(copy);
  if (newSize == rawSize) {
    copy->copyImage(this);
  }
  else {
    HDC dc = copy->getDC();
    bool highQuality = true;
    SetStretchBltMode(dc, highQuality ? HALFTONE : COLORONCOLOR);
    PixRect::stretchBlt(dc, copy->getRect(), SRCCOPY, this, getRect());
    copy->releaseDC(dc);
  }
  copy->m_topLeft = CPoint((int)(((double)newSize.cx / rawSize.cx) * m_topLeft.x)
                          ,(int)(((double)newSize.cy / rawSize.cy) * m_topLeft.y));
  copy->m_gcb = m_gcb;
  return copy;
}

void GifPixRect::init() {
  m_topLeft = ORIGIN;
  m_winRect = CRect(m_topLeft, getSize());
  m_gcb     = getDefaultGCB();
}

static CSize getImageSize(const GifFileType *gifFile, int index) {
  const SavedImage &image = gifFile->SavedImages[index];
  return CSize(image.ImageDesc.Width, image.ImageDesc.Height);
}

GifPixRect::GifPixRect(GifFileType *gifFile, int i)
  : PixRect(theApp.m_device, PIXRECT_PLAINSURFACE, getImageSize(gifFile, i)) {
  const SavedImage &image = gifFile->SavedImages[i];
  copyRasterBits(image.RasterBits, image.ImageDesc.ColorMap ? image.ImageDesc.ColorMap->Colors : gifFile->SColorMap->Colors);
  m_topLeft = CPoint(image.ImageDesc.Left, image.ImageDesc.Top);

  if(DGifSavedExtensionToGCB(gifFile, i, &m_gcb) != GIF_OK) {
    m_gcb = getDefaultGCB();
  }
}

GifPixRect *GifPixRect::load(const String &fileName) { // static
  PixRect    *pr = PixRect::load(theApp.m_device, ByteInputFile(fileName));
  GifPixRect *result = new GifPixRect(pr->getSize()); TRACE_NEW(result);
  result->copyImage(pr);
  SAFEDELETE(pr);
  return result;
}

void GifPixRect::copyImage(const PixRect *src) {
  rop(CRect(0, 0, getWidth(), getHeight()), SRCCOPY, src, src->getRect());
}

void GifPixRect::copyRasterBits(GifPixelType *rasterBits, const GifColorType *colorMap) {
  const int      w  = getWidth();
  const int      h  = getHeight();
  PixelAccessor *pa = NULL;
  try {
    PixelAccessor *pa = getPixelAccessor();
    for (CPoint p(0, 0); p.y < h; p.y++) {
      for (p.x = 0; p.x < w; p.x++) {
        const GifColorType &gc = colorMap[*(rasterBits++)];
        pa->setPixel(p, GCTOD3DCOLOR(gc));
      }
    }
    releasePixelAccessor();
  } catch(...) {
    if(pa) {
      releasePixelAccessor();
    }
    throw;
  }
}

GraphicsControlBlock GifPixRect::getDefaultGCB() { // static
  GraphicsControlBlock gcb;
  gcb.DelayTime        = 60;
  gcb.DisposalMode     = DISPOSE_DO_NOT;
  gcb.UserInputFlag    = false;
  gcb.TransparentColor = NO_TRANSPARENT_COLOR;
  return gcb;
}
