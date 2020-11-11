#include "stdafx.h"
#include <Math.h>
#include <FileNameSplitter.h>
#include <MFCUtil/AnimatedImage.h>
#include <Math/Rectangle2D.h>
#include "SavedImageArray.h"

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
        if(gifFile == nullptr) {
          const String msg = GifErrorString(error);
          throwException(_T("%s"), msg.cstr());
        }
        if(DGifSlurp(gifFile) != GIF_OK) {
          const String msg = GifErrorString(gifFile->Error);
          DGifCloseFile(gifFile, &error);
          throwException(_T("%s"), msg.cstr());
        }
        const int imageCount   = gifFile->ImageCount;

        for(int k = 0; k < imageCount; k++) {
          GifPixRect *gp = new GifPixRect(gifFile, k); TRACE_NEW(gp);
          add(gp);
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
  return (size() != src.size()) || (getUpdateCount() < src.getUpdateCount());
}

void PixRectArray::incrUpdateCount() {
  setProperty(UPDATECOUNTER, m_updateCounter, m_updateCounter+1);
}

PixRectArray &PixRectArray::scale(double scaleFactor) {
  if(isEmpty()) {
    return *this;
  }
  for(size_t i = 0; i < size(); i++) {
    const GifPixRect *pr = (*this)[i];
    GifPixRect *copy = pr->clone(scaleFactor);
    (*this)[i] = copy;
    SAFEDELETE(pr);
  }
  incrUpdateCount();
  return *this;
}

PixRectArray &PixRectArray::setAllToSize(const CSize &newSize) {
  BitSet resizingSet = needsResize(newSize);
  if(resizingSet.isEmpty()) {
    return *this;
  }
  for(auto it = resizingSet.getIterator(); it.hasNext();) {
    const size_t i = it.next();
    const GifPixRect *pr = (*this)[i];
    GifPixRect *copy = pr->clone(newSize);
    (*this)[i] = copy;
    SAFEDELETE(pr);
  }
  incrUpdateCount();
  return *this;
}

BitSet PixRectArray::needsResize(const CSize &newSize) const {
  BitSet result(size() + 1);
  for(size_t i = 0; i < size(); i++) {
    const GifPixRect *pr = (*this)[i];
    if(pr->getSize() != newSize) {
      result += i;
    }
  }
  return result;
}

PixRectArray &PixRectArray::quantize(int maxColorCount) {
  if(isEmpty()) {
    return *this;
  }
  SavedImageArray tmp(*this, maxColorCount);
  clear();
  for(size_t i = 0; i < tmp.size(); i++) {
    GifPixRect *gp = new GifPixRect(tmp[i], nullptr); TRACE_NEW(gp);
    add(gp);
  }
  incrUpdateCount();
  return *this;
}

void PixRectArray::clear(intptr_t capacity) {
  if(isEmpty()) {
    return;
  }
  for(size_t i = 0; i < size(); i++) {
    SAFEDELETE((*this)[i]);
  }
  __super::clear(capacity);
  incrUpdateCount();
}

void PixRectArray::append(const PixRectArray &a) {
  if(a.isEmpty()) {
    return;
  }
  for(size_t i = 0; i < a.size(); i++) {
    add(a[i]->clone());
  }
  incrUpdateCount();
}

void PixRectArray::remove(UINT index, UINT count) {
  if(count == 0) {
    return;
  }
  for(size_t i = 0; i < count; i++) {
    PixRect *pr = (*this)[index];
    SAFEDELETE(pr);
  }
  __super::remove(index, count);
  incrUpdateCount();
}

CSize PixRectArray::getMinSize() const {
  const int n = (int)size();
  if(n == 0) {
    return CSize(0,0);
  }
  CSize result = first()->getSize();
  for(size_t i = 1; i < n; i++) {
    const CSize sz = (*this)[i]->getSize();
    result.cx = min(result.cx, sz.cx);
    result.cy = min(result.cy, sz.cy);
  }
  return result;
}

CSize PixRectArray::getMaxSize() const {
  const int   n = (int)size();
  if(n == 0) {
    return CSize(0,0);
  }
  CSize result = first()->getSize();
  for(size_t i = 1; i < n; i++) {
    const CSize sz = (*this)[i]->getSize();
    result.cx = max(result.cx, sz.cx);
    result.cy = max(result.cy, sz.cy);
  }
  return result;
}

void PixRectArray::paintAll(HDC dc, const CRect &rect) const {
  const int   n = (int)size();
  if(n == 0) {
    return;
  }
  const CSize  rSize         = rect.Size();
  const CSize  maxSize       = getMaxSize();
  int          availableArea = getArea(rSize);

  double       scaleFactor;
  CSize        newSize;
  int          fpl;
  UINT         maxArea = 0;

//  m_comment = format(_T("imageCount:%2d  rSize:(%3d,%3d)\r\n"), n, rSize.cx, rSize.cy);

  for(int framesPerLine = n; framesPerLine >= 1; framesPerLine--) {
    const UINT lineCount      = (n-1) / framesPerLine + 1;
    const int  lastFrameCount = n%framesPerLine;
    const UINT fullLines      = lineCount - (lastFrameCount?1:0);

    assert((fullLines >= 1) && (lastFrameCount >= 0) && (fullLines * framesPerLine + lastFrameCount == n));

    const double sfx = min(1.0, (double)(rSize.cx-1) / ((maxSize.cx+1) * framesPerLine));
    double       sfy = min(1.0, (double)(rSize.cy-1) / ((maxSize.cy+1) * lineCount    ));
    double       sf  = min(sfx, sfy);

    const CSize  psz  = CSize((int)floor(sf * maxSize.cx) + 1, (int)floor(sf * maxSize.cy) + 1);
    const UINT   area = getArea(psz);;
    const CSize  dstSz(framesPerLine * psz.cx-1, lineCount * psz.cy-1);
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
    GifPixRect *pr = (*this)[i];
    if(lc == fpl) {
      p.x = rect.left;
      p.y += newSize.cy;
      lc  = 0;
    }

    const CPoint topLeft = pr->m_topLeft;
    const CSize  sz      = pr->getSize();
    const CRect  tmpRect = (scaleFactor == 1)
                  ? CRect(p + topLeft, sz)
                  : CRect(CPoint((int)(p.x + scaleFactor * topLeft.x), (int)(p.y + scaleFactor * topLeft.y))
                         ,CSize((int)(scaleFactor * sz.cx), (int)(scaleFactor * sz.cy))
                         );
    paintPixRect(pr, dc, tmpRect);
    pr->m_winRect = tmpRect;
    p.x += newSize.cx;
    lc++;
  }
}

void PixRectArray::paintPixRect(const PixRect *pr, HDC dc, const CRect &rect) { // static
  CSize sz = pr->getSize();
  if(sz == rect.Size()) {
    PixRect::bitBlt(dc, rect.TopLeft(), sz, SRCCOPY, pr, CPoint(0,0));
  } else {
//    const bool highQuality = false;
//    SetStretchBltMode(dc, highQuality ? HALFTONE : COLORONCOLOR);
    PixRect::stretchBlt(dc, rect, SRCCOPY, pr, pr->getRect());
  }
}

int PixRectArray::findImageIndexByPoint(const CPoint &p) const {
  for(UINT i = 0; i < size(); i++) {
    const GifPixRect *gp = (*this)[i];
    if(gp->m_winRect.PtInRect(p)) {
      return i;
    }
  }
  return -1;
}
