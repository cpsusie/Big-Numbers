#include "stdafx.h"
#include <MFCUtil/WinTools.h>
#include <MFCUtil/Picture.h>
#include "Image.h"

#pragma warning(disable : 4073)
#pragma init_seg(lib)

Image::Image(const CSize &size) : PixRect(theApp.m_device, PIXRECT_PLAINSURFACE, size) {
}

static D3DFORMAT getPixelFormat(ImageType type, bool transparentWhite) {
  switch (type) {
  case RESOURCE_BITMAP:
  case RESOURCE_JPEG  :
    return transparentWhite?D3DFMT_A8R8G8B8:D3DFMT_X8R8G8B8;
  case RESOURCE_PNG   :
    return D3DFMT_A8R8G8B8;
  default:
    return D3DFMT_X8R8G8B8;
  }
}

Image::Image(int resId, ImageType type, bool transparentWhite)
  : PixRect(theApp.m_device
           ,PIXRECT_PLAINSURFACE
           ,1, 1
           ,D3DPOOL_FORCE_DWORD
           ,::getPixelFormat(type,transparentWhite))
{
  DEFINEMETHODNAME;
  switch(type) {
  case RESOURCE_BITMAP:
    loadBMP(resId);
    break;
  case RESOURCE_JPEG  :
    loadJPG(resId);
    break;
  case RESOURCE_PNG   :
    loadPNG(resId);
    break;
  default:
    throwInvalidArgumentException(method, _T("Imagetype=%d"), type);
  }
  if(type == RESOURCE_PNG) {
    preMultiplyAlpha();
    m_hasTransparentPixels = true;
  } else {
    makeOpaque();
/*
    if(transparentWhite) {
      makeWhiteTransparent();
    }
*/
  }
}

void Image::makeOpaque() {
  apply(SetAlpha(255));
  m_hasTransparentPixels = false;
}

/*
#define OPAQUE_WHITE      D3DCOLOR_ARGB(255,255,255,255)
#define TRANSPARENT_WHITE D3DCOLOR_ARGB(  0,255,255,255)

class FadeEdgePixels : public PixRectOperator {
public:
  void apply(const CPoint &p);
};

void FadeEdgePixels::apply(const CPoint &p) {
  D3DCOLOR c;
  if((c = m_pixelAccessor->getPixel(p)) == TRANSPARENT_WHITE) {
    return;
  }
  CPoint np;
  int i,j;
  int countTransparent = 0;
  for(i = 3, np.x=p.x-1; i--; np.x++) {
    for(j = 3, np.y=p.y-1; j--; np.y++) {
      if(np == p) continue;
      if(m_pixRect->contains(np) && (m_pixelAccessor->getPixel(np) == TRANSPARENT_WHITE)) {
        countTransparent++;
      }
    }
  }

#define PM(c,alpha,channel) (int)ARGB_GET##channel(c)*(alpha)/255
#define SETALPHA(c,alpha)   D3DCOLOR_ARGB(alpha, PM(c,alpha,RED),PM(c,alpha,GREEN),PM(c,alpha,BLUE))

  if(countTransparent > 1) {
    const BYTE alpha = 256-24*countTransparent;
    m_pixelAccessor->setPixel(p,SETALPHA(c,alpha));
  }
}
void Image::makeWhiteTransparent() {
  apply(SubstituteColor(OPAQUE_WHITE, TRANSPARENT_WHITE));
  apply(FadeEdgePixels());
  m_hasTransparentPixels = true;
}
*/

void Image::loadBMP( int resId) {
  DEFINEMETHODNAME;
  CBitmap bmp;
  if(!bmp.LoadBitmap(resId)) {
    throwException(_T("%s(%d) failed"), method, resId);
  }
  ((PixRect&)*this) = bmp;
}

void Image::loadJPG(int resId) {
  CPicture picture;
  picture.loadFromResource(resId, _T("JPG"));
  const CSize size = picture.getSize();
  setSize(size);
  HDC dc = getDC();
  picture.show(dc, CRect(0,0,size.cx,size.cy));
  releaseDC(dc);
}

void Image::loadPNG(int resId) {
  CPicture picture;
  picture.loadFromResource(resId, _T("PNG"));
  const CSize size = picture.getSize();
  setSize(size);
  HDC dc = getDC();
  picture.show(dc, CRect(0,0,size.cx,size.cy));
  releaseDC(dc);
}

void Image::paintImage(PixRect &pr, const CPoint &dst, double scale, double rotation) const {
  HDC dc = pr.getDC();
  paintImage(dc, dst, getSize(), ORIGIN, scale, rotation);
  pr.releaseDC(dc);
}

void Image::paintImage(PixRect &pr, const CPoint &dst, const CSize &size, const CPoint &src, double scale, double rotation) const {
  HDC dc = pr.getDC();
  paintImage(dc, dst, size, src, scale, rotation);
  pr.releaseDC(dc);
}

void Image::paintImage(HDC dc, const CPoint &dst, double scale, double rotation) const {
  paintImage(dc, dst, getSize(), ORIGIN, scale, rotation);
}

CSize Image::getSize(double scale) const {
  const CSize sz1 = PixRect::getSize();
  return (scale == 1) ? sz1 : CSize((int)round(sz1.cx * scale), (int)round(sz1.cy * scale));
}

void Image::paintImage(HDC dc, const CPoint &dst, const CSize &size, const CPoint &src, double scale, double rotation) const {
  if(rotation != 0) {
    paintRotated(dc, dst, size, src, scale, rotation);
  } else {
    int dstWidth, dstHeight;
    if(scale == 1) {
      dstWidth  = size.cx;
      dstHeight = size.cy;
    } else {
      dstWidth  = (int)round(size.cx * scale);
      dstHeight = (int)round(size.cy * scale);
      SetStretchBltMode(dc, COLORONCOLOR /*HALFTONE*/);
    }
    if(m_hasTransparentPixels) {
      PixRect::alphaBlend(dc, dst.x,dst.y, dstWidth,dstHeight, *this, src.x,src.y, size.cx,size.cy, 255);
    } else if(scale == 1) {
      bitBlt(dc, dst.x,dst.y, dstWidth,dstHeight, SRCCOPY, this, src.x,src.y);
    } else {
      stretchBlt(dc,dst.x,dst.y, dstWidth,dstHeight, SRCCOPY, this, src.x,src.y, size.cx,size.cy);
    }
  }
}

void Image::paintRotated(HDC dc, const CPoint &dst, const CSize &size, const CPoint &src, double scale, double rotation) const {
  PixRect *rotatedImage = PixRect::rotateImage(this, rotation, D3DCOLOR_ARGB(0,255,0,0));
  const CSize rsize = rotatedImage->getSize();
  if(scale == 1) {
    PixRect::alphaBlend(dc, dst, rsize, *rotatedImage, ORIGIN, rsize, 255);
  } else {
    const CSize dstsize((int)(rsize.cx * scale), (int)(rsize.cy*scale));
    PixRect::alphaBlend(dc, dst, dstsize, *rotatedImage, ORIGIN, rsize, 255);
  }
  delete rotatedImage;
}
