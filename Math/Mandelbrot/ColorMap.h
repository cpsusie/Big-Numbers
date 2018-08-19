#pragma once

class ColorMapData {
public:
  UINT m_maxCount;
  UINT m_seed;
  BOOL m_randomSeed;

  ColorMapData() : m_maxCount(2000), m_randomSeed(TRUE), m_seed(127) {
  }
  bool operator==(const ColorMapData &d) const {
    return (m_maxCount   == d.m_maxCount    )
        && (m_randomSeed == d.m_randomSeed  )
        && (m_randomSeed || (m_seed == d.m_seed));
  }
  bool operator!=(const ColorMapData &d) const {
    return !(*this == d);
  }
  inline UINT getMaxCount() const {
    return m_maxCount;
  }
  inline String toString() const {
    return format(_T("ColorMapData(maxCount:%u, seed:%u, randomSeed:%s"), m_maxCount, m_seed, boolToStr(m_randomSeed));
  }
};

class ColorMapEntry {
public:
  D3DCOLOR m_d3c;
  COLORREF m_colorRef;
  inline ColorMapEntry() {
  }
  inline ColorMapEntry(D3DCOLOR d3c) : m_d3c(d3c), m_colorRef(D3DCOLOR2COLORREF(d3c)) {
  }
};

inline bool operator==(const ColorMapEntry &e1, const ColorMapEntry &e2) {
  return (e1.m_d3c == e2.m_d3c) && (e1.m_colorRef == e2.m_colorRef);
}

inline bool operator!=(const ColorMapEntry &e1, const ColorMapEntry &e2) {
  return !(e1 == e2);
}

// The last entry always BLACK
class ColorMap : public CompactArray<ColorMapEntry> {
public:
  inline ColorMap() {
  }
  ColorMap(const ColorMapData &d);
  inline UINT getMaxCount() const {
    return (UINT)size() - 1;
  }
};
