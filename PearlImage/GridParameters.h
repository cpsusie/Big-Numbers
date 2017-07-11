#pragma once

#include <PersistentData.h>
#include <MatrixTemplate.h>

class CellSpec {
private:
  static const TCHAR *s_digitChar;
  static const int    s_radix;
  mutable String      m_symStr;
public:
  int      m_symbol;
  D3DCOLOR m_color;
  UINT     m_count;
  CellSpec() : m_symbol(0), m_color(0), m_count(0) {
  }
  CellSpec(int symbol, COLORREF color) : m_symbol(symbol), m_color(color), m_count(1) {
  }
  String toString() const;
  const String &getSymbolStr() const;
  static inline TCHAR  getSingleCharSymbol(int symbol) {
    assert(symbol < s_radix);
    return s_digitChar[symbol];
  }
  static inline int getRadix() {
    return s_radix;
  }
};

class PearlDiagram {
public:
  Array<CellSpec>        m_cellSpecArray;
  MatrixTemplate<int>    m_cellMatrix;
  void clear(const CSize &size, size_t mapSize = 0) {
    m_cellSpecArray.clear();
    m_cellSpecArray.setCapacity(mapSize);
    m_cellMatrix.setDimension(size.cy,size.cx);
  }
  String toString() const;
};

class GridParameters : public PersistentData {
public:
  double  m_cellSize;   // in pixels
  CSize   m_cellCount;
  UINT    m_colorCount; // if 0, then no color-reduction
  double  m_cellSizeMM;
  String  m_imageFileName;
  time_t  m_fileTime;

  GridParameters() {
    reset();
  }
  void putDataToDoc(  XMLDoc &doc);
  void getDataFromDoc(XMLDoc &doc);
  GridParameters &reset() {
    m_cellSize   = 1;
    m_cellCount  = CSize(0,0);
    m_colorCount = 0;
    m_cellSizeMM = 3;
    return *this;
  }
  CSize findCellCount(const CSize &imageSize) const;
  PixRect *calculateImage(const PixRect *image, PearlDiagram *diagram = NULL) const;
  inline bool operator==(const GridParameters &rhs) const {
    return (m_cellCount == rhs.m_cellCount) && (m_colorCount == rhs.m_colorCount);
  }
  inline bool operator!=(const GridParameters &rhs) const {
    return !(*this == rhs);
  }
  Size2DS getImageSize() const {
    return Size2DS(m_cellCount) * m_cellSizeMM;
  }
  int getType() const {
    return 0;
  }
  void setFileTime();
  void checkFileTime() const;
};

