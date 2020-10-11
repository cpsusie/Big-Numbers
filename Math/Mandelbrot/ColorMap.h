#pragma once

class ColorMapInterpolationPoint {
private:
  RGBColor          m_rgb;
  float             m_pos;    // 0 <= m_pos <= 1
public:
  ColorMapInterpolationPoint() : m_pos(0) {
  }
  ColorMapInterpolationPoint(COLORREF colorRef, float pos)
    : m_rgb(COLORREF2D3DCOLOR(colorRef))
    , m_pos(  pos         )
  {
  }
  explicit ColorMapInterpolationPoint(const String &str);
  bool validate(bool throwOnError = true) const;
  inline const RGBColor &getRGB() const {
    return m_rgb;
  }
  inline void setColorRef(COLORREF colorRef) {
    m_rgb = COLORREF2D3DCOLOR(colorRef);
  }
  inline D3DCOLOR getD3DColor() const {
    return (D3DCOLOR)m_rgb;
  }
  inline COLORREF getColorRef() const {
    const D3DCOLOR d3c = getD3DColor();
    return D3DCOLOR2COLORREF(d3c);
  }
  inline float getPos() const {
    return m_pos;
  }
  inline void setPos(float pos) {
    m_pos = pos;
  }
  inline bool operator==(const ColorMapInterpolationPoint &cip) const {
    return (m_rgb == cip.m_rgb) && (m_pos == cip.m_pos);
  }
  inline bool operator!=(const ColorMapInterpolationPoint &cip) const {
    return !(*this == cip);
  }

  inline String toString() const {
    return format(_T("pos:%.5f C:(r:%.3f,g:%.3f,b:%.3f)"),m_pos,m_rgb.m_red, m_rgb.m_green, m_rgb.m_blue);
  }
};

inline std::wostream &operator<<(std::wostream &out, const ColorMapInterpolationPoint &p) {
  return out << p.toString();
}

class ColorMapData : public CompactArray<ColorMapInterpolationPoint> {
public:
  ColorMapData() {
  }
  bool validate(bool throwOnError = true) const;
  // Return index of position of inserted point, -1 if not inserted
  int  insertPoint(const ColorMapInterpolationPoint &cip);
  bool deletePoint(UINT index);
  inline String toString() const {
    return __super::toString(_T("\n"));
  }
  static ColorMapData getDefault();
  static ColorMapData loadDefaultFile();
  void setAsDefault() const;
};

class ColorMapEntry {
private:
  D3DCOLOR m_d3c;
  COLORREF m_colorRef;
  inline void init(D3DCOLOR d3c) {
    m_d3c      = d3c;
    m_colorRef = D3DCOLOR2COLORREF(d3c);
  }
public:
  inline ColorMapEntry() {
    init(0);
  }
  inline ColorMapEntry(D3DCOLOR d3c) {
    init(d3c);
  }
  inline ColorMapEntry(const RGBColor &rgb) {
    init(rgb);
  }
  inline COLORREF getColorRef() const {
    return m_colorRef;
  }
  inline D3DCOLOR getD3DColor() const {
    return m_d3c;
  }
  inline bool operator==(const ColorMapEntry &e) const {
    return m_d3c == e.m_d3c;
  }
  inline bool operator!=(const ColorMapEntry &e) const {
    return !(*this == e);
  }
  inline String toString() const {
    return format(_T("%3d,%3d,%3d"), ARGB_GETRED(m_d3c), ARGB_GETGREEN(m_d3c), ARGB_GETBLUE(m_d3c));
  }
};

inline std::wostream &operator<<(std::wostream &out, const ColorMapEntry &p) {
  return out << p.toString();
}

// The last entry always BLACK
class ColorMap : public CompactArray<ColorMapEntry> {
private:
  ColorMapData m_cdm;
  void initColors(UINT maxCount);
public:
  inline ColorMap() {
  }
  ColorMap(const ColorMapData &cdm, UINT maxCount);
  void clear(intptr_t capacity=0);
  inline const ColorMapData &getColorMapData() const {
    return m_cdm;
  }
  void setColorMapData(const ColorMapData &cdm);
  inline UINT getMaxCount() const {
    return isEmpty() ? 0 : ((UINT)size()-1);
  }
  void setMaxCount(UINT maxCount);
  inline UINT getBlackColorIndex() const {
    return getMaxCount();
  }
  inline String toString() const {
    return __super::toString(_T("\n"));
  }
};
