#include "stdafx.h"
#include <Math/Double64.h>
#include "PointSet.h"
#include "CellCountMatrix.h"

CellCountMatrix *CellCountMatrix::clone() const {
  CellCountMatrix *result = new CellCountMatrix(getSize(), getMaxCount());
  result->rop(getRect(), SRCCOPY, this, ORIGIN);
  result->m_histogram = m_histogram;
  return result;
}

CellCountMatrix &CellCountMatrix::operator=(const CellCountMatrix &src) {
  if(src.getSize() != getSize()) {
    const CSize dsz = getSize(), ssz = src.getSize();
    throwInvalidArgumentException(__TFUNCTION__
                                 ,_T("size=(%d,%d), src.size=(%d,%d)")
                                 ,dsz.cx,dsz.cy
                                 ,ssz.cx,ssz.cy
                                 );
  }
  rop(getRect(), SRCCOPY, &src, ORIGIN);
  m_maxCount      = src.m_maxCount;
  m_log10MaxCount = src.m_log10MaxCount;
  m_histogram     = src.m_histogram;
  return *this;
}

void CellCountMatrix::setMaxCount(UINT maxCount) {
  if(maxCount != getMaxCount()) {
    clearRect(getRect());
    setMax(maxCount);
  }
}

PixRect *CellCountMatrix::convertToPixRect(PixRect *dst, const ColorMap &cm) const {
  DEFINEMETHODNAME;
  const CSize size = getSize(), dstSize = dst->getSize();
  if(dstSize != size) {
    throwInvalidArgumentException(method, _T("size:(%d,%d). dst.size:(%d,%d)"), size.cx,size.cy, dstSize.cx, dstSize.cy);
  }
  if(getMaxCount() != cm.getMaxCount()) {
    throwInvalidArgumentException(method, _T("this.getMaxCount()=%u, cm.getMaxCount()=%u"), getMaxCount(), cm.getMaxCount());
  }
  CellCountAccessor   *cca   = getCCA();
  PixelAccessor       *dstpa = dst->getPixelAccessor();
  const ColorMapEntry *cmp   = cm.getBuffer();
  CPoint p;
  for(p.y = 0; p.y < size.cy; p.y++) {
    for(p.x = 0; p.x < size.cx; p.x++) {
      UINT count;
      if((count = cca->getCount(p)) != EMPTYCELLVALUE) {
        dstpa->setPixel(p, cmp[count].getD3DColor());
      }
    }
  }
  dst->releasePixelAccessor();
  releaseCCA();
  return dst;
}

const CellCountHistogram &CellCountMatrix::getHistogram() const {
  if(m_histogram.isEmpty()) {
    m_histogram = CellCountHistogram(*this);
  }
  return m_histogram;
}

String CellCountMatrix::getPixelInfo(const CPoint &p) const {
  if(!contains(p)) return _T("");
  const UINT count = getCount(p);
  if(count == EMPTYCELLVALUE) {
    return _T("undefined");
  } else if(!hasHistogram()) {
    return format(_T("%u"),count);
  } else {
    return format(_T("(%*u,%u)"), m_log10MaxCount,count, getHistogram()[count].m_pixelCount);
  }
}

UINT CellCountMatrix::log10(UINT n) { // static
  UINT result = 0;
  for(; n; n /= 10) result++;
  return result;
}

CellCountHistogram::CellCountHistogram(const CellCountMatrix &ccm) {
  CompactArray<HistogramEntry> &h         = *this;
  const size_t                  histoSize = ccm.getMaxCount() + 1;
  const CSize                   size      = ccm.getSize();
  CellCountAccessor            *cca       = ccm.getCCA();

  init();
  setCapacity(histoSize);
  h.add(0,HistogramEntry(),histoSize);
  CPoint p;
  for(p.y = 0; p.y < size.cy; p.y++) {
    for(p.x = 0; p.x < size.cx; p.x++) {
      UINT count;
      if((count = cca->getCount(p)) != EMPTYCELLVALUE) {
        h[count].m_pixelCount++;
      }
    }
  }
  ccm.releaseCCA();

  const size_t n = h.size();
  for(UINT i = 0; i < n; i++) {
    HistogramEntry &e = h[i];
    e.m_iterations = i;
    if(e.isPresent()) {
      m_totalPixelCount += e.m_pixelCount;
      m_nonZeroEntryCount++;
    }
  }
}

String CellCountHistogram::toString(bool all) const {
  const CompactArray<HistogramEntry> &h = *this;
  String                              result;
  result += format(_T("Max count           :%8s\n"), format1000(getMaxCount()          ).cstr());
  result += format(_T("Non zero entry count:%8s\n"), format1000(getNonZeroEntryCount() ).cstr());
  result += format(_T("Total pixel count   :%8s\n"), format1000(getTotalPixelCount()   ).cstr());
  result += format(_T("Black pixel count   :%8s\n"), format1000(getBlackPixelCount()   ).cstr());
  result += format(_T("NonBlack pixel count:%8s\n"), format1000(getNonBlackPixelCount()).cstr());
  const size_t n = size();
  const double nonBlackPixelCount = getNonBlackPixelCount();
  double pixelSum = 0;
  for(size_t i = 0; i < n; i++) {
    const HistogramEntry &e = h[i];
    if(all || e.isPresent()) {
      result += e.toString();
      result += format(_T(" %10.3lf%%"), pixelSum/nonBlackPixelCount*100.0);
      result += _T("\n");
      pixelSum += e.m_pixelCount;
    }
  }
  return result;
}
