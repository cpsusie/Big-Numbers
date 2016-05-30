#include "stdafx.h"
#include <math.h>
#include <Stack.h>
#include <PixRect.h>

#pragma warning(disable : 4244)

ScaleParameters::ScaleParameters() {
  m_toSize  = false;
  m_scale.x = 1;
  m_scale.y = 1;
}

ScaleParameters::ScaleParameters(bool toSize, const Point2D &scale) {
  m_toSize = toSize;
  m_scale  = scale;
}

ScaleParameters::ScaleParameters(bool toSize, double x, double y) {
  m_toSize = toSize;
  m_scale.x = x;
  m_scale.y = y;
}

class Scale {
private:
  const PixRect         *m_srcPixRect;
  const ScaleParameters &m_param;
  PixelAccessor         *m_src;
  CRect                  m_srcRect;
public:
  Scale(const PixRect *srcPixRect, const ScaleParameters &param);
  ~Scale();
  void allocatePixelAccessor();
  D3DCOLOR getAverageColor(const Rectangle2D &rect) const;
  PixRect *getResult();
};

Scale::Scale(const PixRect *srcPixRect, const ScaleParameters &param)
: m_srcPixRect(srcPixRect)
, m_param(param)
{
  m_srcRect.left   = m_srcRect.top = 0;
  m_srcRect.right  = srcPixRect->getWidth();
  m_srcRect.bottom = srcPixRect->getHeight();
  m_src            = NULL;
}

Scale::~Scale() {
  if(m_src != NULL) {
    delete m_src;
  }
}

void Scale::allocatePixelAccessor() {
  if(m_src == NULL) {
    m_src = PixelAccessor::createPixelAccessor((PixRect*)m_srcPixRect);
  }
}

D3DCOLOR Scale::getAverageColor(const Rectangle2D &rect) const {
  Point2D bottomRight = rect.getBottomRight();
  const int x1 = (int)ceil( rect.m_x);
  const int y1 = (int)ceil( rect.m_y);
  const int x2 = (int)floor(bottomRight.x);
  const int y2 = (int)floor(bottomRight.y);

  const float wxL = x1 - rect.m_x;
  const float wxR = bottomRight.x - x2;
  const float wyT = y1 - rect.m_y;
  const float wyB = bottomRight.y - y2;

  float redTotal = 0, greenTotal = 0, blueTotal = 0;

  if(x1 > x2) {
    if(y1 > y2) {
      return m_src->getPixel(x2,y2);
    } else { // x1 > x2 && y1 <= y2
      for(int y = y1; y < y2; y++) { // single column
        const D3DCOLOR c = m_src->getPixel(x2,y);
        redTotal   += RGB_GETRED(c);
        greenTotal += RGB_GETGREEN(c);
        blueTotal  += RGB_GETBLUE(c);
      }
      if(wyT > 0 && y1 > 0) { // top pixel
        const D3DCOLOR c = m_src->getPixel(x2,y1-1);
        redTotal += wyT * RGB_GETRED(c); greenTotal += wyT * RGB_GETGREEN(c); blueTotal += wyT * RGB_GETBLUE(c);
      }
      if(wyB > 0 && y2 < m_srcRect.bottom) { // bottom pixel
        const D3DCOLOR c = m_src->getPixel(x2,y2);
        redTotal += wyB * RGB_GETRED(c); greenTotal += wyB * RGB_GETGREEN(c); blueTotal += wyB * RGB_GETBLUE(c);
      }
      float height = rect.getHeight();
      return RGB_MAKE((int)round(redTotal/height),(int)round(greenTotal/height),(int)round(blueTotal/height));
    }
  } else if(y1 > y2) { // && x1 <= x2
    for(int x = x1; x < x2; x++) { // single row
      const D3DCOLOR c = m_src->getPixel(x,y2);
      redTotal   += RGB_GETRED(c);
      greenTotal += RGB_GETGREEN(c);
      blueTotal  += RGB_GETBLUE(c);
    }
    if(wxL > 0 && x1 > 0) { // left pixel
      const D3DCOLOR c = m_src->getPixel(x1-1,y2);
      redTotal += wxL * RGB_GETRED(c); greenTotal += wxL * RGB_GETGREEN(c); blueTotal += wxL * RGB_GETBLUE(c);
    }
    if(wxR > 0 && x2 < m_srcRect.right) { // right pixel
      const D3DCOLOR c = m_src->getPixel(x2,y2);
      redTotal += wxR * RGB_GETRED(c); greenTotal += wxR * RGB_GETGREEN(c); blueTotal += wxR * RGB_GETBLUE(c);
    }
    const float width = rect.getWidth();
    return RGB_MAKE((int)round(redTotal/width),(int)round(greenTotal/width),(int)round(blueTotal/width));
  } else { // x1 <= x2 && y1 <= y2
    int red = 0, green = 0, blue  = 0;
    for(int x = x1; x < x2; x++) { // inner box
      for(int y = y1; y < y2; y++) {
        D3DCOLOR c = m_src->getPixel(x,y);
        red   += RGB_GETRED(c);
        green += RGB_GETGREEN(c);
        blue  += RGB_GETBLUE(c);
      }
    }
    redTotal += red; greenTotal += green; blueTotal += blue;

    if(wxL > 0) {
      const int xL = x1 - 1;
      int red = 0, green = 0, blue = 0;
      for(int y = y1; y < y2; y++) { // left edge
        D3DCOLOR c = m_src->getPixel(xL,y);
        red   += RGB_GETRED(c);
        green += RGB_GETGREEN(c);
        blue  += RGB_GETBLUE(c);
      }
      redTotal += wxL * red; greenTotal += wxL * green; blueTotal += wxL * blue;
    }

    if(wxR > 0) {
      int red = 0, green = 0, blue = 0;
      for(int y = y1; y < y2; y++) { // right edge
        const D3DCOLOR c = m_src->getPixel(x2,y);
        red   += RGB_GETRED(c);
        green += RGB_GETGREEN(c);
        blue  += RGB_GETBLUE(c);
      }
      redTotal += wxR * red; greenTotal += wxR * green; blueTotal += wxR * blue;
    }

    if(wyT > 0 && y1 > 0) {
      const int yT = y1 - 1;
      int red = 0, green = 0, blue = 0;
      for(int x = x1; x < x2; x++) { // top edge
        const D3DCOLOR c = m_src->getPixel(x,yT);
        red   += RGB_GETRED(c);
        green += RGB_GETGREEN(c);
        blue  += RGB_GETBLUE(c);
      }
      redTotal += wyT * red; greenTotal += wyT * green; blueTotal += wyT * blue;

      if(wxL > 0) { // top-left corner
        const D3DCOLOR c = m_src->getPixel(x1-1,yT);
        const float f = wxL * wyT;
        redTotal += f * RGB_GETRED(c); greenTotal += f * RGB_GETGREEN(c); blueTotal += f * RGB_GETBLUE(c);
      }
      if(wxR > 0) { // top-right corner
        const D3DCOLOR c = m_src->getPixel(x2,yT);
        const float f = wxR * wyT;
        redTotal += f * RGB_GETRED(c); greenTotal += f * RGB_GETGREEN(c); blueTotal += f * RGB_GETBLUE(c);
      }
    }

    if(wyB > 0 && y2 < m_srcRect.bottom) {
      int red = 0, green = 0, blue = 0;
      for(int x = x1; x < x2; x++) { // bottom edge
        const D3DCOLOR c = m_src->getPixel(x,y2);
        red   += RGB_GETRED(c);
        green += RGB_GETGREEN(c);
        blue  += RGB_GETBLUE(c);
      }
      redTotal += wyB * red; greenTotal += wyB * green; blueTotal += wyB * blue;

      if(wxL > 0 && x1 > 0) { // bottom-left corner
        const D3DCOLOR c = m_src->getPixel(x1-1,y2);
        const float f = wxL * wyB;
        redTotal += f * RGB_GETRED(c); greenTotal += f * RGB_GETGREEN(c); blueTotal += f * RGB_GETBLUE(c);
      }
      if(wxR > 0 && x2 < m_srcRect.right) { // bottom-right corner
        const D3DCOLOR c = m_src->getPixel(x2,y2);
        const float f = wxR * wyB;
        redTotal += f * RGB_GETRED(c); greenTotal += f * RGB_GETGREEN(c); blueTotal += f * RGB_GETBLUE(c);
      }
    }
    const float area = rect.getWidth() * rect.getHeight();
    return RGB_MAKE((int)round(redTotal/area),(int)round(greenTotal/area),(int)round(blueTotal/area));
  }
}

PixRect *Scale::getResult() {
  CRect resultRect;
  resultRect.left = resultRect.top = 0;
  if(m_param.m_toSize) {
    resultRect.right  = (int)m_param.m_scale.x;
    resultRect.bottom = (int)m_param.m_scale.y;
  } else {
    resultRect.right  = (int)ceil(m_srcRect.right  * m_param.m_scale.x);
    resultRect.bottom = (int)ceil(m_srcRect.bottom * m_param.m_scale.y);
  }

  if(resultRect == m_srcRect) {
    return m_srcPixRect->clone(true);
  } else {
    allocatePixelAccessor();
    PixRect       *result = new PixRect(resultRect.Width(), resultRect.Height(), m_srcPixRect->getPixelFormat());
    PixelAccessor *dst    = result->getPixelAccessor();
    const float    stepx  = (float)(m_srcRect.Width()-1)  / resultRect.Width();
    const float    stepy  = (float)(m_srcRect.Height()-1) / resultRect.Height();

    Rectangle2D rect;
    rect.m_h = stepy;
    rect.m_w = stepx;
    rect.m_y = 0;
    const int rh = resultRect.Height();
    const int rw = resultRect.Width();
    for(int y = 0; y < rh; y++, rect.m_y += stepy) {
      rect.m_x = 0;
      for(int x = 0; x < rw; x++, rect.m_x += stepx) {
        dst->setPixel(x,y,getAverageColor(rect));
      }
    }
    delete dst;
    return result;
  }
}

PixRect *PixRect::scaleImage(const PixRect *src, const ScaleParameters &param) {
  return Scale(src,param).getResult();
}

D3DCOLOR PixRect::getAverageColor(const Rectangle2D &rect) const {
  if(rect.m_w <= 0 || rect.m_h < 0) {
    return BLACK;
  }
  Scale scale(this, ScaleParameters());
  Rectangle2D r(rect);
  if(r.m_x < 0)           r.m_x = 0;
  if(r.m_y < 0)           r.m_y = 0;
  if(r.m_w > getWidth())  r.m_w = getWidth();
  if(r.m_h > getHeight()) r.m_h = getHeight();
  scale.allocatePixelAccessor();
  return scale.getAverageColor(r);
}

D3DCOLOR PixRect::getAverageColor() const {
  return getAverageColor(Rectangle2D(0,0,getWidth(),getHeight()));
}
