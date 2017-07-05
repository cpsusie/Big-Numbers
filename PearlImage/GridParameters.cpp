#include "stdafx.h"
#include <MFCUtil/ColorSpace.h>
#include "GridParameters.h"
#include <MedianCut.h>

static void reduceColors(PixRect *image, int colorCount) {
  const CSize    size = image->getSize();
  PixelAccessor *pa   = image->getPixelAccessor();
  DimPointWithIndexArray a(size.cx*size.cy);
  CPoint p;
  for(p.y = 0; p.y < size.cy; p.y++) {
    for(p.x = 0; p.x < size.cx; p.x++) {
      const D3DCOLORVALUE cv = colorToColorValue(pa->getPixel(p));
      DimPoint dp;
      dp.m_x[0] = cv.r;
      dp.m_x[1] = cv.g;
      dp.m_x[2] = cv.b;
      a.add(dp);
    }
  }
  DimPointArray dpa = medianCut(a, colorCount);
  int pixelCount = 0;
  for(p.y = 0; p.y < size.cy; p.y++) {
    for(p.x = 0; p.x < size.cx; p.x++) {
      const DimPoint &dp = dpa[a[pixelCount++].m_index];
      D3DCOLORVALUE cv;
      cv.r = dp.m_x[0];
      cv.g = dp.m_x[1];
      cv.b = dp.m_x[2];
      pa->setPixel(p, colorValueToD3DColor(cv));
    }
  }
  delete pa;
}

PixRect *GridParameters::calculateImage(const PixRect *image) const {
  const CSize size      = image->getSize();
  CSize       cellCount = m_cellCount;
  if(getArea(cellCount) == 0) {
    cellCount.cx = (UINT)round((double)size.cx / m_cellSize);
    cellCount.cy = (UINT)round((double)size.cy / m_cellSize);
  }
  PixRect *result = theApp.fetchPixRect(size     );
  PixRect *tmp    = theApp.fetchPixRect(cellCount);
  HDC hdc = tmp->getDC();
  PixRect::stretchBlt(hdc, ORIGIN, cellCount, SRCCOPY, image, ORIGIN, size);
  tmp->releaseDC(hdc);
  reduceColors(tmp, m_colorCount);
  hdc = result->getDC();
  PixRect::stretchBlt(hdc, ORIGIN, size, SRCCOPY, tmp, ORIGIN, tmp->getSize());
  result->releaseDC(hdc);
  delete tmp;
  return result;
}
