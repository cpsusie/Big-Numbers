#include "stdafx.h"
#include <MFCUtil/ColorSpace.h>
#include "GridParameters.h"
#include <MedianCut.h>

static void reduceColors(PixRect *image, int colorCount) {
  if(colorCount < 2) return;
  const CSize    size = image->getSize();
  PixelAccessor *pa   = image->getPixelAccessor();
  DimPointWithIndexArray a(size.cx*size.cy);
  CPoint p;
  for(p.y = 0; p.y < size.cy; p.y++) {
    for(p.x = 0; p.x < size.cx; p.x++) {
      const D3DCOLOR c = pa->getPixel(p);
      DimPoint dp;
      dp.m_x[0] = (BYTE)ARGB_GETRED(c);
      dp.m_x[1] = (BYTE)ARGB_GETGREEN(c);
      dp.m_x[2] = (BYTE)ARGB_GETBLUE(c);
      a.add(dp);
    }
  }
  DimPointArray dpa = medianCut(a, colorCount);
  int pixelCount = 0;
  for(p.y = 0; p.y < size.cy; p.y++) {
    for(p.x = 0; p.x < size.cx; p.x++) {
      const DimPoint &dp = dpa[a[pixelCount++].m_index];
      pa->setPixel(p, D3DCOLOR_XRGB((BYTE)dp.m_x[0],(BYTE)dp.m_x[1],(BYTE)dp.m_x[2]));
    }
  }
  delete pa;
}

PixRect *GridParameters::calculateImage(const PixRect *image) const {
  const CSize imageSize = image->getSize();
  const CSize cellCount = findCellCount(imageSize);
  PixRect *result;
  if(cellCount == imageSize) {
    result = image->clone(true);
    reduceColors(result, m_colorCount);
  } else {
    result = theApp.fetchPixRect(imageSize     );
    PixRect *tmp = theApp.fetchPixRect(cellCount);
    HDC      hdc = tmp->getDC();
    PixRect::stretchBlt(hdc, ORIGIN, cellCount, SRCCOPY, image, ORIGIN, imageSize);
    tmp->releaseDC(hdc);

    reduceColors(tmp, m_colorCount);

    hdc = result->getDC();
    PixRect::stretchBlt(hdc, ORIGIN, imageSize, SRCCOPY, tmp, ORIGIN, tmp->getSize());
    result->releaseDC(hdc);
    delete tmp;
  }
  return result;
}

CSize GridParameters::findCellCount(const CSize &imageSize) const {
  CSize result;
  result.cx = (UINT)round((double)imageSize.cx / m_cellSize);
  result.cy = (UINT)round((double)imageSize.cy / m_cellSize);
  return result;
}
