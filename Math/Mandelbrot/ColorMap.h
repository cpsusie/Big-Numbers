#pragma once

typedef enum {
  CD_LINEAR
 ,CD_SQRROOT
 ,CD_LOG
} ColorDistribution;

class ColorMapData {
public:
  UINT              m_maxCount;
  ColorDistribution m_distribution;
  UINT              m_seed;
  BOOL              m_randomSeed;
  ColorMapData() : m_maxCount(2000), m_distribution(CD_LINEAR), m_randomSeed(TRUE), m_seed(127) {
  }
  bool operator==(const ColorMapData &d) const {
    return (m_maxCount     == d.m_maxCount    )
        && (m_distribution == d.m_distribution)
        && (m_randomSeed   == d.m_randomSeed  )
        && (m_randomSeed || (m_seed == d.m_seed));
  }
  bool operator!=(const ColorMapData &d) const {
    return !(*this == d);
  }
  inline UINT getMaxCount() const {
    return m_maxCount;
  }
  static String getDistributionName(ColorDistribution cd);
  inline String getDistributionName() const {
    return getDistributionName(m_distribution);
  }

  inline String toString() const {
    return format(_T("ColorMapData(maxCount:%u, distribution:%s, seed:%u, randomSeed:%s")
                 ,m_maxCount
                 ,getDistributionName().cstr()
                 ,m_seed
                 ,boolToStr(m_randomSeed));
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
