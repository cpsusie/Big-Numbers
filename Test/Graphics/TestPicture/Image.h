#pragma once

#include <MFCUtil/Picture.h>

class Image : public CPicture {
private:
  String m_fileName;
  double m_zoomFactor, m_minZoomFactor;
  CPoint m_offset;    // (non zoomed) offset of lower,left visible picture-pixel
  CRect  m_visiblePart;
  CRect  m_lastClientRect;
  CRect  m_imageRect; // area in m_lastClientRect where pixels from image are shown
  void init();
  CPoint getMaxOffset(  const CRect &r) const;
  CRect  getVisiblePart(const CRect &r, double zoomFactor) const;
  double getRectangles( const CRect &r, double zoomFactor, CRect &visiblePart, CRect &imageRect) const; // return zoomFactor
  CPoint getImagePoint( const CPoint &p, const CRect &visiblePart, const CRect &imageRect) const;
  bool   isMinimalZoom(double zoomFactor) const {
    return (m_minZoomFactor == 0) || (zoomFactor == m_minZoomFactor);
  }
  bool   isMinimalZoom() const {
    return isMinimalZoom(m_zoomFactor);
  }
  double findtMinimalZoom(const CRect &r) const;
public:
  Image();
  void load(const String &fileName);
  void unload();
  void show(HDC dc, const CRect &r, bool keepAspectRatio);
  bool zoom(const CPoint &point, bool in, double step = 1.05);
  inline double getZoomFactor() const {
    return m_zoomFactor;
  }
  bool setOffset(const CPoint &p);
  inline const CPoint &getOffset() const {
    return m_offset;
  }
  inline const CRect &getImageRect() const {
    return m_imageRect;
  }
  inline CPoint getImagePoint(const CPoint &p) const {
    return getImagePoint(p, m_visiblePart, m_imageRect);
  }
  CSize getZoomedSize() const;
  inline CPoint getMaxOffset() const {
    return getMaxOffset(m_lastClientRect);
  }
  inline const CRect &getVisiblePart() const {
    return m_visiblePart;
  }
  inline double getMinZoomFactor() const {
    return m_minZoomFactor;
  }
  inline const String &getFileName() const {
    return m_fileName;
  }
};

inline double getAspectRatio(const CSize &s) {
  return (double)s.cx/s.cy;
}

inline double getAspectRatio(const CRect &r) {
  return getAspectRatio(r.Size());
}

