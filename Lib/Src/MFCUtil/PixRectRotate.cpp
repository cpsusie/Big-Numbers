#include "stdafx.h"
#include <Math.h>
#include <PixRect.h>

class RotationData {
public:
  const CRect  m_rect;
  const double m_degree;
  const double m_cos,m_sin;
  CRect        m_resultRect;
  Point2D      m_offset;

  RotationData(const CSize &size, double degree);

  Point2D rotateForw(double x, double y) const {
    return rotateForw(Point2D(x,y));
  }

  Point2D rotateBack(double x, double y) const {
    return rotateBack(Point2D(x,y));
  }

  Point2D rotateForw(const Point2D &p) const;
  Point2D rotateBack(const Point2D &p) const;
};

RotationData::RotationData(const CSize &size, double degree) 
: m_rect(0,0,size.cx, size.cy)
, m_degree(degree)
, m_cos(cos(GRAD2RAD(degree)))
, m_sin(sin(GRAD2RAD(degree)))
{
  m_offset.x = m_offset.y = 0;
  Point2DArray corners;
  corners.add(rotateForw(m_rect.TopLeft()));
  corners.add(rotateForw(CPoint(m_rect.right,m_rect.top)));
  corners.add(rotateForw(m_rect.BottomRight()));
  corners.add(rotateForw(CPoint(m_rect.left ,m_rect.bottom)));
  Rectangle2D fRect = corners.getBoundingBox();

  if(fmod(m_degree, 90) == 0) {
    m_offset = Point2D(0,0);
    switch(((int)m_degree)%360) {
    case 90:
    case -90:
    case 270:
    case -270:
      m_resultRect = CRect(0,0,m_rect.Height(),m_rect.Width());
      break;
    case 0   :
    case 180 :
    case -180:
      m_resultRect = m_rect;
      break;
    }
  } else {
    m_offset     = -Point2D(ceil(fRect.getX()),ceil(fRect.getY())) + Point2D(fraction(fRect.getBottomRight().x),fraction(fRect.getBottomRight().y));
    fRect        = Rectangle2D(0,0, fRect.getBottomRight().x + m_offset.x, fRect.getBottomRight().y + m_offset.y);
    m_resultRect = CRect(CPoint(0,0),CPoint((int)ceil(fRect.getBottomRight().x),(int)ceil(fRect.getBottomRight().y)));
  }
}

Point2D RotationData::rotateForw(const Point2D &p) const {
  return Point2D(m_cos * p.x - m_sin * p.y + m_offset.x
                ,m_sin * p.x + m_cos * p.y + m_offset.y);
}

Point2D RotationData::rotateBack(const Point2D &p) const {
  const double x1 = p.x - m_offset.x;
  const double y1 = p.y - m_offset.y;
  return Point2D(m_cos * x1 + m_sin * y1,-m_sin * x1 + m_cos * y1);
}

class Rotation : public RotationData {
private:
  const PixRect *m_srcPixRect;
  PixelAccessor *m_src;
  D3DCOLOR       m_background;

  D3DCOLOR getMixedPixel(const Point2D &p) const;
  void rotateAngle(PixRect *dstPixRect);
  void rotateRightAngle(PixRect *dstPixRect, int degree);
public:
  Rotation(const PixRect *srcPixRect, D3DCOLOR background, double degree);
  ~Rotation();
  PixRect *getResult();
};

Rotation::Rotation(const PixRect *srcPixRect, D3DCOLOR background, double degree) 
: m_srcPixRect(srcPixRect)
, RotationData(srcPixRect->getSize(), degree)
{
  m_src = PixelAccessor::createPixelAccessor((PixRect*)srcPixRect);
  m_background = background;
}

Rotation::~Rotation() {
  delete m_src;
}

static D3DCOLOR mixColor(D3DCOLOR c1, D3DCOLOR c2, double w2) {
  if(c1 == c2) {
    return c1;
  }
  const double w1 = 1.0 - w2;
  return RGB_MAKE((unsigned long)(w1*RGB_GETRED(  c1) + w2*RGB_GETRED(  c2))
                 ,(unsigned long)(w1*RGB_GETGREEN(c1) + w2*RGB_GETGREEN(c2))
                 ,(unsigned long)(w1*RGB_GETBLUE( c1) + w2*RGB_GETBLUE( c2)));
}

D3DCOLOR Rotation::getMixedPixel(const Point2D &p) const {
  const int x = (int)floor(p.x);
  const int y = (int)floor(p.y);

  const double wx = p.x - x;
  const double wy = p.y - y;

  if(wx == 0) {
    if(wy == 0) {
      return m_src->getPixel(x, y);
    } else {
      const int yp1 = y+1;
      const D3DCOLOR c1 = (y   >= 0            ) ? m_src->getPixel(x, y  ) : m_background;
      const D3DCOLOR c2 = (yp1 <  m_rect.bottom) ? m_src->getPixel(x, yp1) : m_background;
      return mixColor(c1, c2, wy);
    }
  } else if(wy == 0) {
    const int xp1 = x+1;
    const D3DCOLOR c1 = (x   >= 0           ) ? m_src->getPixel(x  ,y) : m_background;
    const D3DCOLOR c2 = (xp1 <  m_rect.right) ? m_src->getPixel(xp1,y) : m_background;
    return mixColor(c1, c2, wx);
  }
  const int xp1 = x+1;
  const int yp1 = y+1;
  D3DCOLOR c1,c2,c3,c4;
  if(x < 0) {
    c1 = c3 = m_background;
  } else {
    c1 = (y   >= 0            ) ? m_src->getPixel(x, y  ) : m_background;
    c3 = (yp1 <  m_rect.bottom) ? m_src->getPixel(x, yp1) : m_background;
  }
  if(xp1 >= m_rect.right) {
    c2 = c4 = m_background;
  } else {
    c2 = (y   >= 0            ) ? m_src->getPixel(xp1, y  ) : m_background;
    c4 = (yp1 <  m_rect.bottom) ? m_src->getPixel(xp1, yp1) : m_background;
  }
  if(c1 == c2) {
    return (c3==c4) ? mixColor(c1, c3, wy) : mixColor(c1, mixColor(c3, c4, wx), wy);
  } else if(c3==c4) {
    return mixColor(mixColor(c1, c2, wx), c3, wy);
  } else {

    const double wx1 = 1.0 - wx;
    const double wy1 = 1.0 - wy;

    const double r1 = wx1*RGB_GETRED(  c1) + wx*RGB_GETRED(  c2);
    const double g1 = wx1*RGB_GETGREEN(c1) + wx*RGB_GETGREEN(c2); 
    const double b1 = wx1*RGB_GETBLUE( c1) + wx*RGB_GETBLUE( c2); 
    const double r2 = wx1*RGB_GETRED(  c3) + wx*RGB_GETRED(  c4);
    const double g2 = wx1*RGB_GETGREEN(c3) + wx*RGB_GETGREEN(c4); 
    const double b2 = wx1*RGB_GETBLUE( c3) + wx*RGB_GETBLUE( c4); 

    return RGB_MAKE((unsigned long)(wy1*r1+wy*r2),(unsigned long)(wy1*g1+wy*g2),(unsigned long)(wy1*b1+wy*b2));
  }
}

void Rotation::rotateAngle(PixRect *dstPixRect) {
  PixelAccessor *dst = dstPixRect->getPixelAccessor();
  for(CPoint dstp(0,0); dstp.y < m_resultRect.bottom; dstp.y++) {
    Point2D srcp = rotateBack(0, dstp.y);
    for(dstp.x = 0; dstp.x < m_resultRect.right; dstp.x++, srcp.x += m_cos, srcp.y -= m_sin) {
      if(m_srcPixRect->containsExtended(srcp)) {
        dst->setPixel(dstp, getMixedPixel(srcp));
      }
    }
  }
  delete dst;
}

void Rotation::rotateRightAngle(PixRect *dstPixRect, int degree) {
  PixelAccessor *dst = dstPixRect->getPixelAccessor();
  CPoint srcp, dstp;
  switch(degree) {
  case   90:
  case -270:
    for(srcp.y = m_rect.top, dstp.x = m_resultRect.right-1; srcp.y < m_rect.bottom; srcp.y++, dstp.x--) {
      for(srcp.x = m_rect.left, dstp.y = 0; srcp.x < m_rect.right; srcp.x++, dstp.y++) {
        dst->setPixel(dstp, m_src->getPixel(srcp));
      }
    }
    break;

  case  270:
  case  -90:
    for(srcp.y = m_rect.top, dstp.x = 0; srcp.y < m_rect.bottom; srcp.y++, dstp.x++) {
      for(srcp.x = m_rect. left, dstp.y = m_resultRect.bottom-1; srcp.x < m_rect.right; srcp.x++, dstp.y--) {
        dst->setPixel(dstp, m_src->getPixel(srcp));
      }
    }
    break;

  case  180:
  case -180:
    for(srcp.y = m_rect.top, dstp.y = m_resultRect.bottom-1; srcp.y < m_rect.bottom; srcp.y++, dstp.y--) {
      for(srcp.x = m_rect.left, dstp.x = m_resultRect.right-1; srcp.x < m_rect.right; srcp.x++, dstp.x--) {
        dst->setPixel(dstp, m_src->getPixel(srcp));
      }
    }
    break;
  }
  delete dst;
}

PixRect *Rotation::getResult() {
  PixRect *result = new PixRect(m_resultRect.Width(),m_resultRect.Height(),m_srcPixRect->getPixelFormat());

  if(m_degree == 0) {
    result->rop(0,0,m_srcPixRect->getWidth(),m_srcPixRect->getHeight(),SRCCOPY,m_srcPixRect,0,0);
  } else if(fmod(m_degree,90.0) == 0) {
    rotateRightAngle(result,((int)m_degree)%360);
  } else {
    result->fillRect(m_resultRect, m_background);
    rotateAngle(result);
  }

  return result;
}

PixRect *PixRect::rotateImage(const PixRect *src, D3DCOLOR background, double degree) { // static
  return Rotation(src,background,degree).getResult();
}

CSize PixRect::getRotatedSize(const CSize &size, double degree) { // static 
  const RotationData data(size, degree);
  return data.m_resultRect.Size();
}
