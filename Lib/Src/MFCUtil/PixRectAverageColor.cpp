#include "pch.h"
#include <Math.h>
#include <float.h>
#include <MFCUtil/PixRect.h>

class Scale {
private:
  const PixRect         *m_srcPixRect;
  const ScaleParameters &m_param;
  PixelAccessor         *m_srcPA;
  CRect                  m_srcRect;
public:
  Scale(const PixRect *srcPixRect, const ScaleParameters &param);
  ~Scale();
  D3DCOLOR getAverageColor(const Rectangle2D &rect) const;
};

Scale::Scale(const PixRect *srcPixRect, const ScaleParameters &param)
: m_srcPixRect(srcPixRect)
, m_param(param)
{
  m_srcRect.left   = m_srcRect.top = 0;
  m_srcRect.right  = srcPixRect->getWidth();
  m_srcRect.bottom = srcPixRect->getHeight();
  m_srcPA          = srcPixRect->getPixelAccessor();
}

Scale::~Scale() {
  m_srcPixRect->releasePixelAccessor();
}

D3DCOLOR Scale::getAverageColor(const Rectangle2D &rect) const {
  Point2D bottomRight = rect.getBottomRight();
  const int x1 = (int)ceil( rect.m_x);
  const int y1 = (int)ceil( rect.m_y);
  const int x2 = (int)floor(bottomRight.x);
  const int y2 = (int)floor(bottomRight.y);

  const double wxL = x1 - rect.m_x;
  const double wxR = bottomRight.x - x2;
  const double wyT = y1 - rect.m_y;
  const double wyB = bottomRight.y - y2;

  double redTotal = 0, greenTotal = 0, blueTotal = 0;

  if(x1 > x2) {
    if(y1 > y2) {
      return m_srcPA->getPixel(x2,y2);
    } else { // x1 > x2 && y1 <= y2
      for(int y = y1; y < y2; y++) { // single column
        const D3DCOLOR c = m_srcPA->getPixel(x2,y);
        redTotal   += ARGB_GETRED(c);
        greenTotal += ARGB_GETGREEN(c);
        blueTotal  += ARGB_GETBLUE(c);
      }
      if(wyT > 0 && y1 > 0) { // top pixel
        const D3DCOLOR c = m_srcPA->getPixel(x2,y1-1);
        redTotal += wyT * ARGB_GETRED(c); greenTotal += wyT * ARGB_GETGREEN(c); blueTotal += wyT * ARGB_GETBLUE(c);
      }
      if(wyB > 0 && y2 < m_srcRect.bottom) { // bottom pixel
        const D3DCOLOR c = m_srcPA->getPixel(x2,y2);
        redTotal += wyB * ARGB_GETRED(c); greenTotal += wyB * ARGB_GETGREEN(c); blueTotal += wyB * ARGB_GETBLUE(c);
      }
      double height = rect.getHeight();
      return D3DCOLOR_XRGB((int)round(redTotal/height),(int)round(greenTotal/height),(int)round(blueTotal/height));
    }
  } else if(y1 > y2) { // && x1 <= x2
    for(int x = x1; x < x2; x++) { // single row
      const D3DCOLOR c = m_srcPA->getPixel(x,y2);
      redTotal   += ARGB_GETRED(c);
      greenTotal += ARGB_GETGREEN(c);
      blueTotal  += ARGB_GETBLUE(c);
    }
    if(wxL > 0 && x1 > 0) { // left pixel
      const D3DCOLOR c = m_srcPA->getPixel(x1-1,y2);
      redTotal += wxL * ARGB_GETRED(c); greenTotal += wxL * ARGB_GETGREEN(c); blueTotal += wxL * ARGB_GETBLUE(c);
    }
    if(wxR > 0 && x2 < m_srcRect.right) { // right pixel
      const D3DCOLOR c = m_srcPA->getPixel(x2,y2);
      redTotal += wxR * ARGB_GETRED(c); greenTotal += wxR * ARGB_GETGREEN(c); blueTotal += wxR * ARGB_GETBLUE(c);
    }
    const double width = rect.getWidth();
    return D3DCOLOR_XRGB((int)round(redTotal/width),(int)round(greenTotal/width),(int)round(blueTotal/width));
  } else { // x1 <= x2 && y1 <= y2
    int red = 0, green = 0, blue  = 0;
    for(int x = x1; x < x2; x++) { // inner box
      for(int y = y1; y < y2; y++) {
        D3DCOLOR c = m_srcPA->getPixel(x,y);
        red   += ARGB_GETRED(c);
        green += ARGB_GETGREEN(c);
        blue  += ARGB_GETBLUE(c);
      }
    }
    redTotal += red; greenTotal += green; blueTotal += blue;

    if(wxL > 0) {
      const int xL = x1 - 1;
      int red = 0, green = 0, blue = 0;
      for(int y = y1; y < y2; y++) { // left edge
        D3DCOLOR c = m_srcPA->getPixel(xL,y);
        red   += ARGB_GETRED(c);
        green += ARGB_GETGREEN(c);
        blue  += ARGB_GETBLUE(c);
      }
      redTotal += wxL * red; greenTotal += wxL * green; blueTotal += wxL * blue;
    }

    if(wxR > 0) {
      int red = 0, green = 0, blue = 0;
      for(int y = y1; y < y2; y++) { // right edge
        const D3DCOLOR c = m_srcPA->getPixel(x2,y);
        red   += ARGB_GETRED(c);
        green += ARGB_GETGREEN(c);
        blue  += ARGB_GETBLUE(c);
      }
      redTotal += wxR * red; greenTotal += wxR * green; blueTotal += wxR * blue;
    }

    if(wyT > 0 && y1 > 0) {
      const int yT = y1 - 1;
      int red = 0, green = 0, blue = 0;
      for(int x = x1; x < x2; x++) { // top edge
        const D3DCOLOR c = m_srcPA->getPixel(x,yT);
        red   += ARGB_GETRED(c);
        green += ARGB_GETGREEN(c);
        blue  += ARGB_GETBLUE(c);
      }
      redTotal += wyT * red; greenTotal += wyT * green; blueTotal += wyT * blue;

      if(wxL > 0) { // top-left corner
        const D3DCOLOR c = m_srcPA->getPixel(x1-1,yT);
        const double   f = wxL * wyT;
        redTotal += f * ARGB_GETRED(c); greenTotal += f * ARGB_GETGREEN(c); blueTotal += f * ARGB_GETBLUE(c);
      }
      if(wxR > 0) { // top-right corner
        const D3DCOLOR c = m_srcPA->getPixel(x2,yT);
        const double   f = wxR * wyT;
        redTotal += f * ARGB_GETRED(c); greenTotal += f * ARGB_GETGREEN(c); blueTotal += f * ARGB_GETBLUE(c);
      }
    }

    if(wyB > 0 && y2 < m_srcRect.bottom) {
      int red = 0, green = 0, blue = 0;
      for(int x = x1; x < x2; x++) { // bottom edge
        const D3DCOLOR c = m_srcPA->getPixel(x,y2);
        red   += ARGB_GETRED(c);
        green += ARGB_GETGREEN(c);
        blue  += ARGB_GETBLUE(c);
      }
      redTotal += wyB * red; greenTotal += wyB * green; blueTotal += wyB * blue;

      if(wxL > 0 && x1 > 0) { // bottom-left corner
        const D3DCOLOR c = m_srcPA->getPixel(x1-1,y2);
        const double   f = wxL * wyB;
        redTotal += f * ARGB_GETRED(c); greenTotal += f * ARGB_GETGREEN(c); blueTotal += f * ARGB_GETBLUE(c);
      }
      if(wxR > 0 && x2 < m_srcRect.right) { // bottom-right corner
        const D3DCOLOR c = m_srcPA->getPixel(x2,y2);
        const double   f = wxR * wyB;
        redTotal += f * ARGB_GETRED(c); greenTotal += f * ARGB_GETGREEN(c); blueTotal += f * ARGB_GETBLUE(c);
      }
    }
    const double area = rect.getWidth() * rect.getHeight();
    return D3DCOLOR_XRGB((int)round(redTotal/area),(int)round(greenTotal/area),(int)round(blueTotal/area));
  }
}

D3DCOLOR PixRect::getAverageColor(const Rectangle2D &rect) const {
  if(rect.m_w <= 0 || rect.m_h < 0) {
    return BLACK;
  }
  Rectangle2D r(rect);
  if(r.m_x < 0)           r.m_x = 0;
  if(r.m_y < 0)           r.m_y = 0;
  if(r.m_w > getWidth())  r.m_w = getWidth();
  if(r.m_h > getHeight()) r.m_h = getHeight();
  return Scale(this, ScaleParameters()).getAverageColor(r);
}

D3DCOLOR PixRect::getAverageColor() const {
  return getAverageColor(Rectangle2DR(getRect()));
}
