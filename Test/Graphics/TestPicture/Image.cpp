#include "stdafx.h"
#include <Math.h>
#include "Image.h"

Image::Image() {
  init();
}

void Image::load(const String &fileName) {
  CPicture pic;
  pic.load(fileName);
  (CPicture&)*this = pic;
  init();
  m_fileName = fileName;
}

void Image::unload() {
  CPicture::unload();
  init();
}

void Image::init() {
  m_zoomFactor     = 1;
  m_minZoomFactor  = 0;
  m_offset         = CPoint(0,0);
  m_fileName       = EMPTYSTRING;
  m_lastClientRect = CRect(0,0,0,0);
  m_visiblePart    = m_imageRect = m_lastClientRect;
}

void Image::show(HDC dc, const CRect &r, bool keepAspectRatio) {
  if(!keepAspectRatio) {
    m_lastClientRect = m_imageRect = r;
    m_visiblePart = CRect(CPoint(0,0), getSize());
  } else {
    const bool adjustMinimalZoom = (r != m_lastClientRect) ? true : false;
    m_lastClientRect = r;
    setOffset(m_offset);
    m_zoomFactor = getRectangles(r, m_zoomFactor, m_visiblePart, m_imageRect);
    if(adjustMinimalZoom) {
      m_minZoomFactor = findtMinimalZoom(r);
    }
  }
  CPicture::show(dc, m_imageRect, m_visiblePart);
}

bool Image::zoom(const CPoint &point, bool in, double step) {
  const double oldZoomFactor = m_zoomFactor;
  const CPoint ip = getImagePoint(point);

  if(in) {
    m_zoomFactor *= step;
  } else {
    m_zoomFactor /= step;
  }
  if(m_zoomFactor < m_minZoomFactor) {
    m_zoomFactor = m_minZoomFactor;
  }
  if(m_zoomFactor == oldZoomFactor) {
    return false;
  } else {
    CRect visiblePart, imageRect;
    getRectangles(m_lastClientRect, m_zoomFactor, visiblePart, imageRect);
    const CPoint np = getImagePoint(point, visiblePart, imageRect);
    if(np != ip) {
      setOffset(getOffset() - np + ip);
    }
    return true;
  }
}

bool Image::setOffset(const CPoint &p) {
  const CPoint oldOffset = m_offset;
  const CPoint maxOffset = getMaxOffset();
  m_offset.x = minMax(p.x, 0L, maxOffset.x);
  m_offset.y = minMax(p.y, 0L, maxOffset.y);
  return (m_offset != oldOffset) ? true : false;
}

CPoint Image::getMaxOffset(const CRect &r) const {
  if(isMinimalZoom()) {
    return CPoint(0,0);
  } else {
    const CSize size       = getSize();
    const int   maxXOffset = (int)(size.cx - (double)r.Width() / m_zoomFactor);
    const int   maxYOffset = (int)(size.cy - (double)r.Height()/ m_zoomFactor);
    return CPoint(max(0, maxXOffset), max(0, maxYOffset));
  }
}

double Image::findtMinimalZoom(const CRect &r) const {
  CRect visiblePart, imageRect;
  return getRectangles(r, m_minZoomFactor, visiblePart, imageRect);
}

double Image::getRectangles(const CRect &r, double zoomFactor, CRect &visiblePart, CRect &imageRect) const {
  visiblePart = getVisiblePart(r, zoomFactor);
  if(isMinimalZoom(zoomFactor)) {
    const CPoint rCenter = r.CenterPoint();
    const CSize  size    = getSize();
    if((size.cx <= r.Width()) && (size.cy <= r.Height())) { // Image smaller than r. put it at the center
      imageRect.left   = rCenter.x      - size.cx/2;
      imageRect.right  = imageRect.left + size.cx;
      imageRect.top    = rCenter.y      - size.cy/2;
      imageRect.bottom = imageRect.top  + size.cy;
      return 1;
    } else {                                                // Image too big for window. downscale
      const double imageAspect  = getAspectRatio(size);
      const double windowAspect = getAspectRatio(r);
      imageRect = r;
      if(imageAspect > windowAspect) {                      // Image too wide
        const int h      = (int)(r.Width() / imageAspect);
        imageRect.top    = rCenter.y - h/2;
        imageRect.bottom = imageRect.top + h;
        return (double)r.Width() / size.cx;
      } else {                                              // Image too high
        const int w      = (int)(r.Height() * imageAspect);
        imageRect.left   = rCenter.x - w/2;
        imageRect.right  = imageRect.left + w;
        return (double)r.Height() / size.cy;
      }
    }
  } else {                                                  // zoomFactor > minimalZoom
    CSize irSize;
    irSize.cx = (int)(zoomFactor * visiblePart.Size().cx);
    irSize.cy = (int)(zoomFactor * visiblePart.Size().cy);
    imageRect = CRect(r.left, r.top, min(r.right, r.left + irSize.cx), min(r.bottom, r.top + irSize.cy));
    const CPoint offset(max(0, r.Width() - imageRect.Width())/2, max(0, r.Height() - imageRect.Height())/2);
    imageRect += offset;
    return zoomFactor;
  }
}

CRect Image::getVisiblePart(const CRect &r, double zoomFactor) const {
  const CSize size = getSize();
  if(isMinimalZoom(zoomFactor)) {
    return CRect(0,0,size.cx, size.cy);
  } else {
    const CSize  zSize   = size * zoomFactor;
    const int    left    = m_offset.x;
    const int    top     = m_offset.y;
    int          zRight  = left + zSize.cx;
    int          zBottom = top  + zSize.cy;
    const double ia      = getAspectRatio(zSize);
    const double ra      = getAspectRatio(r);

    if(((ia >= ra) && (zRight <= r.Width())) || ((ia < ra) && (zBottom <= r.Height()))) {
      return CRect(left,top,left + size.cx,top + size.cy); // zoomed image fit into r
    } else {
      const CSize visibleSize = r.Size() / zoomFactor;
      return CRect(left, top, min(size.cx, left + visibleSize.cx), min(size.cy, top + visibleSize.cy));
    }
  }
}

CSize Image::getZoomedSize() const {
  CSize zs;
  zs.cx = (int)(m_zoomFactor * getSize().cx);
  zs.cy = (int)(m_zoomFactor * getSize().cy);
  return zs;
}

CPoint Image::getImagePoint(const CPoint &p, const CRect &visiblePart, const CRect &imageRect) const {
  const double ax = (double)imageRect.Width() / visiblePart.Width();
  const double bx = (double)imageRect.left - ax * visiblePart.left;
  const int    x  = (int)((p.x - bx)/ax);

  const double ay = (double)imageRect.Height() / visiblePart.Height();
  const double by = (double)imageRect.top - ay * visiblePart.top;
  const int    y  = (int)(((m_lastClientRect.bottom-p.y) - by)/ay);

  return CPoint(x,y);
}
