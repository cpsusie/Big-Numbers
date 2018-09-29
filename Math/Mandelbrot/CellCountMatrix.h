#pragma once

#include "ColorMap.h"
#include <Math/Rectangle2D.h>

#define EMPTYCELLVALUE 0xffffffff

class CellCountAccessor;
class CellCountMatrix;

class HistogramEntry {
public:
  UINT m_iterations;
  UINT m_pixelCount;
  inline HistogramEntry() : m_iterations(0), m_pixelCount(0) {
  }
  inline HistogramEntry(UINT iterations, UINT pixelCount) : m_iterations(iterations), m_pixelCount(pixelCount) {
  }
  inline bool isPresent() const {
    return m_pixelCount != 0;
  }
  inline String toString() const {
    return format(_T("%5d %6d"), m_iterations, m_pixelCount);
  }
};

class CellCountHistogram : public CompactArray<HistogramEntry> {
private:
  UINT                            m_totalPixelCount;
  UINT                            m_nonZeroEntryCount;

  inline void init() {
    m_totalPixelCount = m_nonZeroEntryCount = 0;
  }
public:
  CellCountHistogram() {
    init();
  }
  CellCountHistogram(const CellCountMatrix &ccm);
  // Return total number of pixels (=sum(entry.m_pixelCount) for all entries
  inline void clear() {
    __super::clear();
    init();
  }
  inline UINT getTotalPixelCount() const {
    return m_totalPixelCount;
  }
  // Return last().m_pixelCount
  inline UINT getBlackPixelCount() const {
    return isEmpty() ? 0 : last().m_pixelCount;
  }
  inline bool hasBlackPixels() const {
    return getBlackPixelCount() != 0;
  }
  // Return getTotalPixelCount() - getBlackPixelCount()
  inline UINT getNonBlackPixelCount() const {
    return getTotalPixelCount() - getBlackPixelCount();
  }
  // Return number of entries in histogram with non-zero m_pixelCount
  inline UINT getNonZeroEntryCount() const {
    return m_nonZeroEntryCount;
  }
  // Return max iteration. (=ccm.getMaxCount() of CellCountMatrix used as argument for constructor)
  inline UINT getMaxCount() const {
    return isEmpty() ? 0 : ((UINT)size() - 1);
  }

  // if(all==false), then build string with entries with nonzero count, else string will contain all entries
  String toString(bool all = false) const;
};

class CellCountMatrix : public PixRect {
private:
  UINT                       m_maxCount;
  UINT                       m_log10MaxCount;
  mutable CellCountHistogram m_histogram;
  inline void setMax(UINT maxCount) {
    m_maxCount      = maxCount;
    m_log10MaxCount = log10(maxCount);
  }
  static UINT log10(UINT n);
public:
  inline CellCountMatrix(const CSize &size, UINT maxCount)
  : PixRect(theApp.m_device, PIXRECT_PLAINSURFACE, size)
  {
    setMax(maxCount);
    clearRect(getRect());
  }
  // assume getSize() == src.getSize(). if not, an exception is thrown
  CellCountMatrix &operator=(const CellCountMatrix &src);
  CellCountMatrix          *clone() const;
  inline CellCountAccessor *getCCA() const {
    return (CellCountAccessor*)getPixelAccessor();
  }
  inline void               releaseCCA() const {
    __super::releasePixelAccessor();
  }
  inline void               clearRect(const CRect &r) {
    __super::fillColor(EMPTYCELLVALUE, &r);
    m_histogram.clear();
  }
  void                      setMaxCount(UINT maxCount);
  inline UINT               getMaxCount() const {
    return m_maxCount;
  }
  inline UINT               getCount(const CPoint &p) const {
    return getPixel(p);
  }
  PixRect                  *convertToPixRect(PixRect *dst, const ColorMap &cm) const;
  const CellCountHistogram &getHistogram() const;
  inline void clearHistogram() {
    m_histogram.clear();
  }
  inline bool hasHistogram() const {
    return getMaxCount() == m_histogram.getMaxCount();
  }
  String getPixelInfo(const CPoint &p) const;
};

class CellCountAccessor : protected DWordPixelAccessor {
private:
  const UINT m_maxCount;
public:
  CellCountAccessor(CellCountMatrix *m) : DWordPixelAccessor(m,0), m_maxCount(m->getMaxCount()) {
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
  inline bool isBlackCell(const CPoint &p) const {
    return getCount(p) == m_maxCount;
  }
};
