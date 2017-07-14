#include "pch.h"
#include <Math.h>
#include <float.h>
#include <MFCUtil/PixRect.h>
#include <MFCUtil/ColorSpace.h>

// --------------------------------- GrayScaleFilter ---------------------------------

void GrayScaleFilter::apply(const CPoint &p) {
  m_resultPixelAccessor->setPixel(p,getGrayColor(m_pixelAccessor->getPixel(p)));
}

// --------------------------------- SobelFilter ---------------------------------

SobelFilter::SobelFilter() {
  Gx[0][0] = -1; Gx[0][1] =  0; Gx[0][2] =  1;
  Gx[1][0] = -2; Gx[1][1] =  0; Gx[1][2] =  2;
  Gx[2][0] = -1; Gx[2][1] =  0; Gx[2][2] =  1;

  Gy[0][0] =  1; Gy[0][1] =  2; Gy[0][2] =  1;
  Gy[1][0] =  0; Gy[1][1] =  0; Gy[1][2] =  0;
  Gy[2][0] = -1; Gy[2][1] = -2; Gy[2][2] = -1;
}

void SobelFilter::setPixRect(PixRect *src) {
  __super::setPixRect(src);
  if(src != NULL) {
    m_result         = src->clone();
    const CSize size = src->getSize();
    m_result->rectangle(0,0,size.cx-1,size.cy-1,D3D_WHITE);
    m_resultPixelAccessor = m_result->getPixelAccessor();
  }
}

CRect SobelFilter::getRect() const {
  return (m_pixRect == NULL) ? CRect(0,0,0,0) : CRect(1,1,m_pixRect->getWidth()-2,m_pixRect->getHeight()-2);
}

void SobelFilter::apply(const CPoint &p) {
  int sumxR=0,sumxG=0,sumxB=0;
  int sumyR=0,sumyG=0,sumyB=0;
  for(int i = -1; i <= 1; i++) {
    for(int j = -1; j <= 1; j++) {
      const D3DCOLOR c = m_pixelAccessor->getPixel(p.x+i,p.y+j);
      const int fx = Gx[i+1][j+1];
      if(fx != 0) {
        sumxR += fx * ARGB_GETRED(c);
        sumxG += fx * ARGB_GETGREEN(c);
        sumxB += fx * ARGB_GETBLUE(c);
      }
      const int fy = Gy[i+1][j+1];
      if(fy != 0) {
        sumyR += fy * ARGB_GETRED(c);
        sumyG += fy * ARGB_GETGREEN(c);
        sumyB += fy * ARGB_GETBLUE(c);
      }
    }
  }

  const int SUMR = minMax(abs(sumxR) + abs(sumyR),0,255);
  const int SUMG = minMax(abs(sumxG) + abs(sumyG),0,255);
  const int SUMB = minMax(abs(sumxB) + abs(sumyB),0,255);
  m_resultPixelAccessor->setPixel(p,D3DCOLOR_XRGB(255-SUMR,255-SUMG,255-SUMB));
}

// --------------------------------- LaplaceFilter ---------------------------------

LaplaceFilter::LaplaceFilter() {
  MASK[0][0] = -1; MASK[0][1] = -1; MASK[0][2] = -1; MASK[0][3] = -1; MASK[0][4] = -1;
  MASK[1][0] = -1; MASK[1][1] = -1; MASK[1][2] = -1; MASK[1][3] = -1; MASK[1][4] = -1;
  MASK[2][0] = -1; MASK[2][1] = -1; MASK[2][2] = 24; MASK[2][3] = -1; MASK[2][4] = -1;
  MASK[3][0] = -1; MASK[3][1] = -1; MASK[3][2] = -1; MASK[3][3] = -1; MASK[3][4] = -1;
  MASK[4][0] = -1; MASK[4][1] = -1; MASK[4][2] = -1; MASK[4][3] = -1; MASK[4][4] = -1;
}

void LaplaceFilter::setPixRect(PixRect *src) {
  __super::setPixRect(src);
  if(src != NULL) {
    m_result         = src->clone();
    const CSize size = src->getSize();
    m_result->rectangle(0,0,size.cx-1,size.cy-1,D3D_WHITE);
    m_result->rectangle(1,1,size.cx-2,size.cy-2,D3D_WHITE);
    m_resultPixelAccessor = m_result->getPixelAccessor();
  }
}

CRect LaplaceFilter::getRect() const {
  return (m_pixRect == NULL) ? CRect(0,0,0,0) : CRect(2,2,m_pixRect->getWidth()-4,m_pixRect->getHeight()-4);
}

void LaplaceFilter::apply(const CPoint &p) {
  int SUMR=0,SUMG=0,SUMB=0;
  for(int i = -2; i <= 2; i++) {
    for(int j = -2; j <= 2; j++) {
      D3DCOLOR c = m_pixelAccessor->getPixel(p.x+i,p.y+j);
      int f = MASK[i+2][j+2];
      SUMR += f * ARGB_GETRED(c);
      SUMG += f * ARGB_GETGREEN(c);
      SUMB += f * ARGB_GETBLUE(c);
    }
  }
  SUMR = minMax(SUMR,0,255);
  SUMG = minMax(SUMG,0,255);
  SUMB = minMax(SUMB,0,255);
  m_resultPixelAccessor->setPixel(p,D3DCOLOR_XRGB(255-SUMR,255-SUMG,255-SUMB));
}

// --------------------------------- GaussFilter ---------------------------------

GaussFilter::GaussFilter() {
  MASK[0][0] =  2; MASK[0][1] =  4; MASK[0][2] =  5; MASK[0][3] =  4; MASK[0][4] =  2;
  MASK[1][0] =  4; MASK[1][1] =  9; MASK[1][2] = 12; MASK[1][3] =  9; MASK[1][4] =  4;
  MASK[2][0] =  5; MASK[2][1] = 12; MASK[2][2] = 15; MASK[2][3] = 12; MASK[2][4] =  5;
  MASK[3][0] =  4; MASK[3][1] =  9; MASK[3][2] = 12; MASK[3][3] =  9; MASK[3][4] =  4;
  MASK[4][0] =  2; MASK[4][1] =  4; MASK[4][2] =  5; MASK[4][3] =  4; MASK[4][4] =  2;
}

void GaussFilter::setPixRect(PixRect *src) {
  __super::setPixRect(src);
  if(src != NULL) {
    m_result         = src->clone();
    const CSize size = src->getSize();
    m_result->rectangle(0,0,size.cx-1,size.cy-1,D3D_WHITE);
    m_result->rectangle(1,1,size.cx-2,size.cy-2,D3D_WHITE);
    m_resultPixelAccessor = m_result->getPixelAccessor();
  }
}

CRect GaussFilter::getRect() const {
  return (m_pixRect == NULL) ? CRect(0,0,0,0) : CRect(2,2,m_pixRect->getWidth()-4,m_pixRect->getHeight()-4);
}

void GaussFilter::apply(const CPoint &p) {
  int SUMR=0,SUMG=0,SUMB=0;
  for(int i = -2; i <= 2; i++) {
    for(int j = -2; j <= 2; j++) {
      D3DCOLOR c = m_pixelAccessor->getPixel(p.x+i,p.y+j);
      int f = MASK[i+2][j+2];
      SUMR += f * ARGB_GETRED(c);
      SUMG += f * ARGB_GETGREEN(c);
      SUMB += f * ARGB_GETBLUE(c);
    }
  }
  SUMR /= 115; SUMG /= 115; SUMB /= 115;
  SUMR = minMax(SUMR,0,255);
  SUMG = minMax(SUMG,0,255);
  SUMB = minMax(SUMB,0,255);
  m_resultPixelAccessor->setPixel(p,D3DCOLOR_XRGB(SUMR,SUMG,SUMB));
}

// --------------------------------- EdgeDirectionFilter ---------------------------------


void EdgeDirectionFilter::setPixRect(PixRect *src) {
  if(src == NULL) {
    __super::setPixRect(NULL);
  } else {
    PixRectFilter::setPixRect(src); // NB Not __super::
    m_result         = src->clone();
    const CSize size = src->getSize();
    m_result->rectangle(0,0,size.cx-1,size.cy-1,D3D_WHITE);
    m_result->rectangle(1,1,size.cx-2,size.cy-2,D3D_WHITE);
    m_resultPixelAccessor = m_result->getPixelAccessor();
  }
}

CRect EdgeDirectionFilter::getRect() const {
  return (m_pixRect == NULL) ? CRect(0,0,0,0) : CRect(2,2,m_pixRect->getWidth()-4,m_pixRect->getHeight()-4);
}

void EdgeDirectionFilter::apply(const CPoint &p) {
  int sumxR=0,sumxG=0,sumxB=0;
  int sumyR=0,sumyG=0,sumyB=0;
  for(int i = -1; i <= 1; i++) {
    for(int j = -1; j <= 1; j++) {
      const D3DCOLOR c = m_pixelAccessor->getPixel(p.x+i,p.y+j);
      const int fx = Gx[i+1][j+1];
      if(fx != 0) {
        sumxR += fx * ARGB_GETRED(c);
        sumxG += fx * ARGB_GETGREEN(c);
        sumxB += fx * ARGB_GETBLUE(c);
      }
      const int fy = Gy[i+1][j+1];
      if(fx != 0) {
        sumyR += fy * ARGB_GETRED(c);
        sumyG += fy * ARGB_GETGREEN(c);
        sumyB += fy * ARGB_GETBLUE(c);
      }
    }
  }

  int SUMR = (abs(sumxR) + abs(sumyR))/2;
  int SUMG = (abs(sumxG) + abs(sumyG))/2;
  int SUMB = (abs(sumxB) + abs(sumyB))/2;
  if(SUMR > 255) SUMR = 255;
  if(SUMG > 255) SUMG = 255;
  if(SUMB > 255) SUMB = 255;
  const int intensity = (SUMR + SUMG + SUMB) / 3;

  const int sumX = sumxR + sumxG + sumxB;
  const int sumY = sumyR + sumyG + sumyB;

  const double theta = atan2(sumY,sumX);
  if(theta >= 0) {
    if(theta <= GRAD2RAD(22.5) || theta >= GRAD2RAD(157.5)) { // edge almost horizontal
      m_resultPixelAccessor->setPixel(p,D3DCOLOR_ARGB(0,intensity,0,0));
    } else if(theta <= GRAD2RAD(67.5)) {                      // 22.5 <  theta <= 67.5  => edge SW-NE
      m_resultPixelAccessor->setPixel(p,D3DCOLOR_ARGB(1, intensity,intensity,0));
    } else if(theta <= GRAD2RAD(112.5)) {                     // 67.5 <  theta <= 112.5 => edge almost vertical
      m_resultPixelAccessor->setPixel(p,D3DCOLOR_ARGB(2, 0,intensity,0));
    } else {                                                  // 112.5 < theta <  157.5 => edge NW-SE
      m_resultPixelAccessor->setPixel(p,D3DCOLOR_ARGB(3, 0,0,intensity));
    }
  } else { // theta < 0
    if(theta >= GRAD2RAD(-22.5) || theta <= GRAD2RAD(-157.5)) { // edge almost horizontal
      m_resultPixelAccessor->setPixel(p,D3DCOLOR_ARGB(0,intensity,0,0));
    } else if(theta >= GRAD2RAD(-67.5)) {                       // -67.5  <= theta < -22.5  => edge NW-SE
      m_resultPixelAccessor->setPixel(p,D3DCOLOR_ARGB(3,0,0,intensity));
    } else if(theta >= GRAD2RAD(-112.5)) {                      // -112.5 <= theta < -67.5  => edge almost vertical
      m_resultPixelAccessor->setPixel(p,D3DCOLOR_ARGB(2,0,intensity,0));
    } else {                                                    // -157.5 < theta  < -112.5 => edge SW-NE
      m_resultPixelAccessor->setPixel(p,D3DCOLOR_ARGB(1,intensity,intensity,0));
    }
  }
}

// --------------------------------- CannyEdgeFilter ---------------------------------

void CannyEdgeFilter::setPixRect(PixRect *src) {
  if (src == NULL) {
    __super::setPixRect(NULL);
  } else {
    src->apply(GaussFilter()).apply(EdgeDirectionFilter());
    __super::setPixRect(src);
    m_result              = src->clone();
    m_resultPixelAccessor = m_result->getPixelAccessor();
  }
}

CRect CannyEdgeFilter::getRect() const {
  return (m_pixRect == NULL) ? CRect(0,0,0,0) : CRect(2,2,m_pixRect->getWidth()-4,m_pixRect->getHeight()-4);
}

#define threshold 25

void CannyEdgeFilter::apply(const CPoint &p) {
  D3DCOLOR c = m_pixelAccessor->getPixel(p);
  switch(ARGB_GETALPHA(c)) {
  case 0: if(ARGB_GETRED(m_pixelAccessor->getPixel(p.x-1,p.y)) < threshold) {
            m_resultPixelAccessor->setPixel(p.x-1,p.y,BLACK);
          } else {
            m_resultPixelAccessor->setPixel(p.x-1,p.y,ARGB_SETALPHA(c,0));
          }
          break;
  case 1:if(ARGB_GETRED(m_pixelAccessor->getPixel(p.x-1,p.y-1)) < threshold)
            m_resultPixelAccessor->setPixel(p.x-1,p.y-1,BLACK);
          else
            m_resultPixelAccessor->setPixel(p.x-1,p.y-1,ARGB_SETALPHA(c,0));
          break;
  case 2:if(ARGB_GETGREEN(m_pixelAccessor->getPixel(p.x,p.y-1)) < threshold)
            m_resultPixelAccessor->setPixel(p.x,p.y-1,BLACK);
          else
            m_resultPixelAccessor->setPixel(p.x,p.y-1,ARGB_SETALPHA(c,0));
          break;
  case 3: if(ARGB_GETBLUE(m_pixelAccessor->getPixel(p.x+1,p.y-1)) < threshold)
            m_resultPixelAccessor->setPixel(p.x+1,p.y-1,BLACK);
          else
            m_resultPixelAccessor->setPixel(p.x+1,p.y-1,ARGB_SETALPHA(c,0));
          break;

  default: m_resultPixelAccessor->setPixel(p,D3DCOLOR_XRGB(255,0,255));
           break;

  }
}
