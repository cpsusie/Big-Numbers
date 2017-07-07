#pragma once

class GridParameters {
public:
  double  m_cellSize; // in pixels
  CSize   m_cellCount;
  UINT    m_colorCount; // if 0, then no color-reduction

  GridParameters() {
    reset();
  }
  GridParameters &reset() {
    m_cellSize   = 1;
    m_cellCount  = CSize(0,0);
    m_colorCount = 0;
    return *this;
  }
  CSize findCellCount(const CSize &imageSize) const;
  PixRect *calculateImage(const PixRect *image) const;
  inline bool operator==(const GridParameters &rhs) const {
    return (m_cellCount  == rhs.m_cellCount)
        && (m_colorCount == rhs.m_colorCount);
  }
  inline bool operator!=(const GridParameters &rhs) const {
    return !(*this == rhs);
  }
};
