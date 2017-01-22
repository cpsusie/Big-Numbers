#include "pch.h"
#include <MFCUtil/PixRect.h>

void PixRect::fill(const CPoint &p, D3DCOLOR color, ColorComparator &cmp) {
  PixelAccessor *pa = getPixelAccessor();
  pa->fill(p, color, cmp);
  delete pa;
}

void PixRect::fill(const CPoint &p, D3DCOLOR color) {
  fill(p,color,SimpleColorComparator());
}

void PixRect::fillTransparent(const CPoint &p, unsigned char alpha) { // alpha = 0 => opaque, 255 = transparent
  if(!contains(p)) {
    return;
  }

  const int width  = getWidth();
  const int height = getHeight();
  PixRect *copy = clone(getType(), true);
  D3DCOLOR tmpColor = ~getPixel(p);
  copy->fill(p,tmpColor);
  copy->rop( 0,0,width,height,SRCINVERT,this,0,0);

  PixelAccessor *pa  = getPixelAccessor();
  PixelAccessor *cpa = copy->getPixelAccessor();

  CPoint sp;
  const D3DCOLOR alphaMask = D3DCOLOR_ARGB(alpha, 0, 0, 0);
  for(sp.x = 0; sp.x < width; sp.x++) {
    for(sp.y = 0; sp.y < height; sp.y++) {
      if(cpa->getPixel(p)) {
        D3DCOLOR c = pa->getPixel(sp) & 0x00ffffff;
        pa->setPixel(sp, c | alphaMask);
      }
    }
  }
  delete pa;
  delete cpa;
  delete copy;
}

class ApproximateColorComparator : public ColorComparator {
  double m_tolerance;
public:
  ApproximateColorComparator(double tolerance);
  bool equals(const D3DCOLOR &c1, const D3DCOLOR &c2);
};

ApproximateColorComparator::ApproximateColorComparator(double tolerance) {
  m_tolerance = tolerance;
}

bool ApproximateColorComparator::equals(const D3DCOLOR &c1, const D3DCOLOR &c2) {
  return colorDistance(c1,c2) < m_tolerance;
}


void PixRect::approximateFill(const CPoint &p, D3DCOLOR color, double tolerance) {
  if(tolerance == 0) {
    tolerance = colorDistance(getPixel(p), color);
  }

  fill(p,color,ApproximateColorComparator(tolerance));
}

