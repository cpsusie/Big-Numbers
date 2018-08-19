#include "stdafx.h"
#include "CellCountMatrix.h"

CellCountMatrix::CellCountMatrix(const CSize &size, UINT maxCount)
: PixRect(theApp.m_device, PIXRECT_PLAINSURFACE, size)
, m_maxCount(maxCount)
{
  clearRect(getRect());
}

CellCountAccessor *CellCountMatrix::getCCA() const {
  PixelAccessor *pa = getPixelAccessor();
  return (CellCountAccessor*)pa;
}

void CellCountMatrix::releaseCCA() const {
  __super::releasePixelAccessor();
}

void CellCountMatrix::clearRect(const CRect &r) {
  __super::fillColor(EMPTYCELLVALUE, &r);
}

CellCountMatrix *CellCountMatrix::clone() const {
  CellCountMatrix *result = new CellCountMatrix(getSize(), getMaxCount());
  result->rop(getRect(), SRCCOPY, this, ORIGIN);
  return result;
}

PixRect *CellCountMatrix::convertToPixRect(PixRect *dst, const ColorMap &cm) const {
  DEFINEMETHODNAME;
  const CSize size = getSize(), dstSize = dst->getSize();
  if(dstSize != size) {
    throwInvalidArgumentException(method, _T("size:(%d,%d). dst.size:(%d,%d)"), size.cx,size.cy, dstSize.cx, dstSize.cy);
  }
  if(cm.getMaxCount() != getMaxCount()) {
    throwInvalidArgumentException(method, _T("colorMap.maxCount=%u, matrix.maxCount=%u"), cm.getMaxCount(), getMaxCount());
  }
  CellCountAccessor *cca   = getCCA();
  PixelAccessor     *dstpa = dst->getPixelAccessor();
  CPoint p;
  const ColorMapEntry *ce = cm.getBuffer();
  UINT count;
  for(p.y = 0; p.y < size.cy; p.y++) {
    for(p.x = 0; p.x < size.cx; p.x++) {
      if((count = cca->getCount(p)) != EMPTYCELLVALUE) {
        dstpa->setPixel(p, ce[count].m_d3c);
      }
    }
  }
  dst->releasePixelAccessor();
  releaseCCA();
  return dst;
}
