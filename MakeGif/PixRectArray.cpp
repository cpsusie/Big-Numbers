#include "stdafx.h"
#include <Math.h>
#include <FileNameSplitter.h>
#include <MFCUtil/AnimatedImage.h>
#include <Math/Rectangle2D.h>
#include "PixRectArray.h"

#define ORIGIN CPoint(0,0)

PixRectArray::PixRectArray(const StringArray &fileNames, StringArray &errors) {
  USES_CONVERSION;
  m_updateCounter = 0;
  for(size_t i = 0; i < fileNames.size(); i++) {
    const String &name = fileNames[i];
    try {
      if(FileNameSplitter(name).getExtension().equalsIgnoreCase(_T(".gif"))) {
        int error;
        const char *namea = T2A(name.cstr());
        GifFileType *gifFile = DGifOpenFileName(namea, &error);
        if(gifFile == NULL) {
          throwException(_T("%s"), GifErrorString(error));
        }
        if(DGifSlurp(gifFile) != GIF_OK) {
          const String msg = GifErrorString(gifFile->Error);
          DGifCloseFile(gifFile, &error);
          throwException(_T("%s"), msg.cstr());
        }
        const int imageCount   = gifFile->ImageCount;

        for(int k = 0; k < imageCount; k++) {
          add(new GifPixRect(gifFile, k));
        }
        DGifCloseFile(gifFile, &error);
      } else {
        add(GifPixRect::load(name));
      }
    } catch(Exception e) {
      errors.add(e.what());
    }
  }
}

PixRectArray::~PixRectArray() {
  clear();
}

PixRectArray &PixRectArray::operator=(const PixRectArray &src) {
  if(this == &src) {
    return *this;
  }
  clear();
  for(size_t i = 0; i < src.size(); i++) {
    add(src[i]->clone());
  }
  incrUpdateCount();
  return *this;
}

bool PixRectArray::needUpdate(const PixRectArray &src) const {
  return (size() != src.size())
      || (getUpdateCount() < src.getUpdateCount());
}

void PixRectArray::incrUpdateCount() {
  unsigned int oldCounter = m_updateCounter;
  m_updateCounter++;
  notifyPropertyChanged(UPDATECOUNTER, &oldCounter, &m_updateCounter);
}

PixRectArray &PixRectArray::scale(double scaleFactor) {
  if(size() == 0) {
    return *this;
  }
  for(size_t i = 0; i < size(); i++) {
    const GifPixRect *pr = (*this)[i];
    GifPixRect *copy = pr->clone(scaleFactor);
    (*this)[i] = copy;
    delete pr;
  }
  incrUpdateCount();
  return *this;
}

PixRectArray &PixRectArray::quantize(int maxColorCount) {
  if(size() == 0) {
    return *this;
  }
  SavedImageArray tmp(*this, maxColorCount);
  clear();
  for(size_t i = 0; i < tmp.size(); i++) {
    add(new GifPixRect(tmp[i], NULL));
  }
  incrUpdateCount();
  return *this;
}

void PixRectArray::clear() {
  if(size() == 0) {
    return;
  }
  while(size() > 0) {
    delete last();
    removeLast();
  }
  incrUpdateCount();
}

void PixRectArray::append(const PixRectArray &a) {
  if(a.size() == 0) {
    return;
  }
  for(size_t i = 0; i < a.size(); i++) {
    add(a[i]->clone());
  }
  incrUpdateCount();
}

void PixRectArray::remove(unsigned int index, unsigned int count) {
  if(count == 0) {
    return;
  }
  for(size_t i = 0; i < count; i++) {
    PixRect *pr = (*this)[index];
    remove(index);
    delete pr;
  }
  incrUpdateCount();
}

CSize PixRectArray::getMinSize() const {
  if(size() == 0) {
    return CSize(0,0);
  }
  CSize result = (*this)[0]->getSize();
  for(size_t i = 1; i < size(); i++) {
    const CSize sz = (*this)[i]->getSize();
    result.cx = min(result.cx, sz.cx);
    result.cy = min(result.cy, sz.cy);
  }
  return result;
}

CSize PixRectArray::getMaxSize() const {
  if(size() == 0) {
    return CSize(0,0);
  }
  CSize result = (*this)[0]->getSize();
  for(size_t i = 1; i < size(); i++) {
    const CSize sz = (*this)[i]->getSize();
    result.cx = max(result.cx, sz.cx);
    result.cy = max(result.cy, sz.cy);
  }
  return result;
}

void PixRectArray::paintAll(HDC dc, const CRect &rect) const {
  const CSize rSize         = rect.Size();
  const CSize maxSize       = getMaxSize();
  const int   n             = (int)size();
  int         availableArea = getArea(rSize);

  double       scaleFactor;
  CSize        newSize;
  int          fpl;
  unsigned int maxArea = 0;

//  m_comment = format(_T("imageCount:%2d  rSize:(%3d,%3d)\r\n"), n, rSize.cx, rSize.cy);

  for(int framesPerLine = n; framesPerLine >= 1; framesPerLine--) {
    const unsigned int lineCount      = (n-1) / framesPerLine + 1;
    const int          lastFrameCount = n%framesPerLine;
    const unsigned int fullLines      = lineCount - (lastFrameCount?1:0);

    assert((fullLines >= 1) && (lastFrameCount >= 0) && (fullLines * framesPerLine + lastFrameCount == n));

    const double       sfx = min(1.0, (double)(rSize.cx-1) / ((maxSize.cx+1) * framesPerLine));
    double             sfy = min(1.0, (double)(rSize.cy-1) / ((maxSize.cy+1) * lineCount    ));
    double             sf  = min(sfx, sfy);

    const CSize        psz  = CSize((int)floor(sf * maxSize.cx) + 1, (int)floor(sf * maxSize.cy) + 1);
    const unsigned int area = getArea(psz);;
    const CSize        dstSz(framesPerLine * psz.cx-1, lineCount * psz.cy-1);
/*
    const CSize rightWaste( rSize.cx - dstSz.cx             , fullLines * psz.cy  );
    const CSize bottomWaste(rSize.cx                        , rSize.cy - dstSz.cy );
    const int   totalWaste = getArea(rightWaste) + getArea(bottomWaste);
*/
    if((dstSz.cx <= rSize.cx) && (dstSz.cy <= rSize.cy) && (area > maxArea)) {
      maxArea     = area;
      newSize     = psz;
      scaleFactor = sf;
      fpl         = framesPerLine;
    }
/*
    m_comment += format(_T("f/l %2d r:(%3d,%3d) a %6s, waste %8s (fx,fy,f):(%.3lf,%.3lf,%.6lf) sz:(%3d,%3d)\r\n")
                       , framesPerLine
                       , dstSz.cx, dstSz.cy
                       , format1000(area).cstr()
                       , format1000(totalWaste).cstr()
                       , sfx,sfy,sf
                       , psz.cx,psz.cy
                       );
*/
  }

  CPoint p = rect.TopLeft();
  for(int i = 0, lc = 0; i < n; i++) {
    const GifPixRect *pr = (*this)[i];
    if(lc == fpl) {
      p.x = rect.left;
      p.y += newSize.cy;
      lc  = 0;
    }

    const CPoint topLeft = pr->m_topLeft;
    const CSize  sz      = pr->getSize();
    CRect tmpRect = (scaleFactor == 1)
                  ? CRect(p + topLeft, sz)
                  : CRect(CPoint((int)(p.x + scaleFactor * topLeft.x), (int)(p.y + scaleFactor * topLeft.y))
                         ,CSize((int)(scaleFactor * sz.cx), (int)(scaleFactor * sz.cy))
                         );
    paintPixRect(pr, dc, tmpRect);
    p.x += newSize.cx;
    lc++;
  }
}

void PixRectArray::paintPixRect(const PixRect *pr, HDC dc, const CRect &rect) { // static
  CSize sz = pr->getSize();
  if(sz == rect.Size()) {
    PixRect::bitBlt(dc, rect.TopLeft(), sz, SRCCOPY, pr, CPoint(0,0));
  } else {
    const bool highQuality = false;
    SetStretchBltMode(dc, highQuality ? HALFTONE : COLORONCOLOR);
    PixRect::stretchBlt(dc, rect, SRCCOPY, pr, pr->getRect());
  }
}


GifPixRect::GifPixRect(const SavedImage *image, const ColorMapObject *globalColorMap)
: PixRect(theApp.m_device, PIXRECT_PLAINSURFACE, image->ImageDesc.Width, image->ImageDesc.Height) {
  copyRasterBits(image->RasterBits, image->ImageDesc.ColorMap ? image->ImageDesc.ColorMap->Colors : globalColorMap->Colors);
  m_topLeft = CPoint(image->ImageDesc.Left, image->ImageDesc.Top);
  m_gcb = getGCB(image);
}

GifPixRect::GifPixRect(const CSize &size) : PixRect(theApp.m_device, PIXRECT_PLAINSURFACE, size) {
  m_topLeft = ORIGIN;
  m_gcb     = getDefaultGCB();
}

GifPixRect::GifPixRect(int w, int h) : PixRect(theApp.m_device, PIXRECT_PLAINSURFACE, w, h) {
  m_topLeft = ORIGIN;
  m_gcb     = getDefaultGCB();
}

GifPixRect *GifPixRect::clone(double scaleFactor) const {
  const CSize   rawSize = getSize();
  const CSize   scaledSize((int)(scaleFactor * rawSize.cx), (int)(scaleFactor * rawSize.cy));
  GifPixRect   *copy = new GifPixRect(scaledSize);
  if(scaleFactor == 1) {
    copy->copyImage(this);
  } else {
    HDC dc = copy->getDC();
    bool highQuality = true;
    SetStretchBltMode(dc, highQuality?HALFTONE:COLORONCOLOR);
    PixRect::stretchBlt(dc, copy->getRect(), SRCCOPY, this, getRect());
    copy->releaseDC(dc);
  }
  copy->m_topLeft = CPoint((int)(scaleFactor * m_topLeft.x), (int)(scaleFactor * m_topLeft.y));
  copy->m_gcb     = m_gcb;
  return copy;
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
  PixRect *pr = PixRect::load(theApp.m_device, ByteInputFile(fileName));
  GifPixRect *result = new GifPixRect(pr->getSize());
  result->copyImage(pr);
  delete pr;
  return result;
}

void GifPixRect::copyImage(const PixRect *src) {
  rop(CRect(0,0,getWidth(),getHeight()), SRCCOPY, src, src->getRect());
}

void GifPixRect::copyRasterBits(GifPixelType *rasterBits, const GifColorType *colorMap) {
  const int w = getWidth();
  const int h = getHeight();
  PixelAccessor      *pa       = getPixelAccessor();
  for(CPoint p(0,0); p.y < h; p.y++) {
    for(p.x = 0; p.x < w; p.x++) {
      const GifColorType &c = colorMap[*(rasterBits++)];
      pa->setPixel(p, D3DCOLOR_ARGB(c.Red, c.Green, c.Blue, 0xff));
    }
  }
  delete pa;
}

GraphicsControlBlock GifPixRect::getDefaultGCB() { // static
  GraphicsControlBlock gcb;
  gcb.DelayTime        = 60;
  gcb.DisposalMode     = DISPOSE_DO_NOT;
  gcb.UserInputFlag    = false;
  gcb.TransparentColor = NO_TRANSPARENT_COLOR;
  return gcb;
}
