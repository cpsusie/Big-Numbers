#include "pch.h"
#include <MFCUtil/PixRect.h>

ScaleParameters::ScaleParameters() {
  m_toSize = false;
  m_scale  = Point2D(1, 1);
}

ScaleParameters::ScaleParameters(bool toSize, const Point2D &scale) {
  m_toSize = toSize;
  m_scale  = scale;
}

ScaleParameters::ScaleParameters(bool toSize, double x, double y) {
  m_toSize = toSize;
  m_scale  = Point2D(x,y);
}

PixRect *PixRect::scaleImage(const PixRect *src, const ScaleParameters &param) { // static}
  CRect srcRect = src->getRect();
  CRect resultRect;
  resultRect.left = resultRect.top = 0;
  if(param.m_toSize) {
    resultRect.right  = (int)param.m_scale[0];
    resultRect.bottom = (int)param.m_scale[1];
  } else {
    resultRect.right  = (int)ceil(srcRect.right  * param.m_scale[0]);
    resultRect.bottom = (int)ceil(srcRect.bottom * param.m_scale[1]);
  }

  if(resultRect == srcRect) {
    return src->clone(true);
  } else {
    PixRect *result = new PixRect(src->getDevice(), src->getType(), resultRect.Size(), src->getPool(), src->getPixelFormat()); TRACE_NEW(result);
    HDC dc = result->getDC();
    stretchBlt(dc, resultRect, SRCCOPY, src, src->getRect());
    result->releaseDC(dc);
    return result;
  }
}
