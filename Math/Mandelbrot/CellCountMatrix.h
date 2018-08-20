#pragma once

#include "ColorMap.h"

#define EMPTYCELLVALUE 0xffffffff

class CellCountAccessor;

class CellCountMatrix : public PixRect {
private:
  const UINT m_maxCount;
public:
  inline CellCountMatrix(const CSize &size, UINT maxCount)
  : PixRect(theApp.m_device, PIXRECT_PLAINSURFACE, size)
  , m_maxCount(maxCount)
  {
    clearRect(getRect());
  }
  inline CellCountAccessor *getCCA() const {
    return (CellCountAccessor*)getPixelAccessor();
  }
  inline void               releaseCCA() const {
    __super::releasePixelAccessor();
  }
  inline void               clearRect(const CRect &r) {
    __super::fillColor(EMPTYCELLVALUE, &r);
  }
  inline UINT               getMaxCount() const {
    return m_maxCount;
  }
  inline UINT               getCount(const CPoint &p) const {
    return getPixel(p);
  }
  CellCountMatrix          *clone() const;
  PixRect *convertToPixRect(PixRect *dst, const ColorMap &cm) const;
};

class CellCountAccessor : protected DWordPixelAccessor {
public:
  CellCountAccessor(CellCountMatrix *m) : DWordPixelAccessor(m,0) {
  }
  inline void setCount(UINT x, UINT y , UINT count) {
    setPixel(x,y,count);
  }
  inline void setCount(const CPoint &p, UINT count) {
    setPixel(p.x,p.y,count);
  }
  inline UINT getCount(UINT x, UINT y) const {
    return getPixel(x,y);
  }
  inline UINT getCount(const CPoint &p) const {
    return getPixel(p.x,p.y);
  }
  inline bool isEmptyCell(const CPoint &p) const {
    return getCount(p) == EMPTYCELLVALUE;
  }
};
