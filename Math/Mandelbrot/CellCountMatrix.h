#pragma once

#include "ColorMap.h"

#define EMPTYCELLVALUE 0xffffffff

class CellCountAccessor;

class CellCountMatrix : public PixRect {
private:
  const UINT m_maxCount;
public:
  CellCountMatrix(const CSize &size, UINT maxCount);
  CellCountAccessor *getCCA() const;
  void               releaseCCA() const;
  void               clearRect(const CRect &r);
  UINT               getMaxCount() const {
    return m_maxCount;
  }
  CellCountMatrix *clone() const;
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
