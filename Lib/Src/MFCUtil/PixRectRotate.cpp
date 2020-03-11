#include "pch.h"
#include <Math.h>
#include <MFCUtil/PixRectDevice.h>
#include <MFCUtil/Viewport2D.h>
#include <MFCUtil/PixRect.h>

D3DXVECTOR2 p2DToD3V3(const Point2DP &p) {
  D3DXVECTOR2 result;
  result.x = (float)p.x;
  result.y = (float)p.y;
  return result;
}

static inline D3DXMATRIX createTransformedWorld(double scale, const Point2DP *rotationCenter, double rad, const Point2DP *translate) { // static
  D3DXMATRIX m;
  D3DXVECTOR2 d3C,d3T;
  if(rotationCenter) d3C = p2DToD3V3(*rotationCenter);
  if(translate     ) d3T = p2DToD3V3(*translate);
  return *D3DXMatrixAffineTransformation2D(&m, (float)scale, rotationCenter?&d3C:NULL, (float)rad, translate?&d3T:NULL);
}

static inline D3DXMATRIX createIdWorld() {
  return createTransformedWorld(1, NULL, 0, NULL);
}
static inline D3DXMATRIX createScaleWorld(double scale) {
  return createTransformedWorld(scale, NULL, 0, NULL);
}
static inline D3DXMATRIX createTranslateWorld(const Point2DP &translate) {
  return createTransformedWorld(1, NULL, 0, &translate);
}

D3DXVECTOR2 operator*(const D3DXMATRIX &m, const D3DXVECTOR2 &v) {
  D3DXVECTOR4 v4;
  D3DXVec2Transform(&v4, &v, &m);
  return D3DXVECTOR2(v4.x, v4.y);
}

Rectangle2DR getTransformedRectangle(const D3DXMATRIX &m, const Rectangle2DR &r) {
  D3DXVECTOR2 corner[4];
  corner[0] = p2DToD3V3(r.getTopLeft());
  corner[1] = p2DToD3V3(r.getTopRight());
  corner[2] = p2DToD3V3(r.getBottomRight());
  corner[3] = p2DToD3V3(r.getBottomLeft());

  Point2DArray trCorners;
  for(int i = 0; i < 4; i++) {
    const D3DXVECTOR2 tp = m * corner[i];
    trCorners.add(Point2D(tp.x,tp.y));
  }
  return trCorners.getBoundingBox();
}

void PixRect::drawRotated(const PixRect *src, const CPoint &dst, double degree, const Point2DP &rotationCenter) {
  const CSize           dstSize = getSize();
  LPDIRECT3DDEVICE     &device  = m_device.getD3Device();
  LPD3DXRENDERTOSURFACE tmpRender;

  V(D3DXCreateRenderToSurface(device
                             ,dstSize.cx, dstSize.cy
                             ,getPixelFormat()
                             ,FALSE
                             ,(D3DFORMAT)0
                             ,&tmpRender));
  TRACE_CREATE(tmpRender);
  D3DVIEWPORT vp;
  vp.X = vp.Y = 0;
  vp.Width  = dstSize.cx;
  vp.Height = dstSize.cy;
  vp.MinZ   = 0;
  vp.MaxZ   = 1;

//showPixRect(src);
//showPixRect(this);
  V(tmpRender->BeginScene(m_surface, &vp));
  TRACE_REFCOUNT(m_surface);
  m_device.set2DTransform(dstSize);
  m_device.setWorldMatrix(createIdWorld());
  m_device.draw(this, getRect());
//showPixRect(this);
  const Point2DP   dp = Point2DP(dst)-rotationCenter;
  const D3DXMATRIX tr = createTransformedWorld(1,&rotationCenter,GRAD2RAD(degree),&dp);
  m_device.setWorldMatrix(tr);
  const PixRect *texture = (src->getType() == PIXRECT_TEXTURE) ? src  : src->clone(true, PIXRECT_TEXTURE, D3DPOOL_DEFAULT);
//showPixRect(texture);

//  V(device->Clear(0, NULL, D3DCLEAR_TARGET, background, 1.0f, 0));
  m_device.alphaBlend(texture, src->getRect());
  V(tmpRender->EndScene(D3DX_DEFAULT));
  TRACE_REFCOUNT(m_surface);
  SAFERELEASE(tmpRender);

  V(device->SetTexture(0, NULL));

  if(texture != src) {
    SAFEDELETE(texture);
  }
//showPixRect(this);
}

PixRect *PixRect::rotateImage(const PixRect *src, double degree, D3DCOLOR background) { // static
  const CSize resultSize = getRotatedSize(src->getSize(), degree);
  PixRect    *result     = new PixRect(src->getDevice(), src->getType(), resultSize, src->getPool(), src->getPixelFormat()); TRACE_NEW(result);
  result->fillColor(background);
  result->drawRotated(src, resultSize/2, degree, src->getRect().CenterPoint());
  return result;
}

CSize PixRect::getRotatedSize(const CSize &size, double degree) { // static
  const Point2DP center = ORIGIN;
  const CSize result = Size2DS(getTransformedRectangle(createTransformedWorld(1, &center, GRAD2RAD(degree), NULL)
                                                      ,CRect(ORIGIN,size)).getSize());
  return result;
}
