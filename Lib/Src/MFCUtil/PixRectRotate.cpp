#include "pch.h"
#include <Math.h>
#include <MFCUtil/Viewport2D.h>
#include <MFCUtil/PixRect.h>

class RotationData {
public:
  const CRect  m_rect;
  const double m_degree;
  CRect        m_resultRect;
  Point2D      m_offset;
  D3DXMATRIX   m_world;

  static D3DXMATRIX create2DRotationWorld(const D3DXVECTOR2 &center, double rad);
  Rectangle2DR getTransformedRectangle(const D3DXMATRIX &m, const CSize bmSize);

  RotationData(const CSize &size, double degree);
};

RotationData::RotationData(const CSize &size, double degree)
: m_rect(0,0,size.cx, size.cy)
, m_degree(degree)
{
  m_offset.x = m_offset.y = 0;
  const Point2DP rcenter = m_rect.CenterPoint();
  D3DXVECTOR2 center;
  center.x     = (float)rcenter.x;
  center.y     = (float)rcenter.y;
  m_world      = create2DRotationWorld(center, GRAD2RAD(m_degree));
  m_resultRect = getTransformedRectangle(m_world, size);
}

D3DXMATRIX RotationData::create2DRotationWorld(const D3DXVECTOR2 &center, double rad) { // static
  D3DXMATRIX m;
  return *D3DXMatrixAffineTransformation2D(&m, 1, &center, (float)rad, NULL);
}

D3DXVECTOR2 operator*(const D3DXMATRIX &m, const D3DXVECTOR2 &v) {
  D3DXVECTOR4 v4;
  D3DXVec2Transform(&v4, &v, &m);
  return D3DXVECTOR2(v4.x, v4.y);
}

Rectangle2DR RotationData::getTransformedRectangle(const D3DXMATRIX &m, const CSize bmSize) {
  D3DXVECTOR2 corner[4];
  corner[0] = D3DXVECTOR2(0               , 0               );
  corner[1] = D3DXVECTOR2((float)bmSize.cx, 0               );
  corner[2] = D3DXVECTOR2(0               , (float)bmSize.cy);
  corner[3] = D3DXVECTOR2((float)bmSize.cx, (float)bmSize.cy);

  Point2DArray trCorners;
  for(int i = 0; i < 4; i++) {
    const D3DXVECTOR2 tp = m * corner[i];
    trCorners.add(Point2D(tp.x,tp.y));
  }
  return trCorners.getBoundingBox();
}

#define V CHECK3DRESULT

class BlendVertex {
  float m_x, m_y, m_z, m_tu, m_tv;
public:
  enum FVF {
    FVF_Flags = D3DFVF_XYZ | D3DFVF_TEX1
  };
  BlendVertex() {
  }
  BlendVertex(int x, int y, float tu, float tv) : m_x((float)x), m_y((float)y), m_z(0), m_tu(tu), m_tv(tv) {
  }
  BlendVertex(const CPoint &p, const Point2DP &tp) : m_x((float)p.x), m_y((float)p.y), m_z(0), m_tu((float)tp.x), m_tv((float)tp.y) {
  }
};

#define SetTextureColorStage(dev, stage, arg1, op, arg2)        \
  V(dev->SetTextureStageState(stage, D3DTSS_COLOROP  , op   )); \
  V(dev->SetTextureStageState(stage, D3DTSS_COLORARG1, arg1 )); \
  V(dev->SetTextureStageState(stage, D3DTSS_COLORARG2, arg2 ));

#define SetTextureAlphaStage(dev, stage, arg1, op, arg2)        \
  V(dev->SetTextureStageState(stage, D3DTSS_ALPHAOP  , op   )); \
  V(dev->SetTextureStageState(stage, D3DTSS_ALPHAARG1, arg1 )); \
  V(dev->SetTextureStageState(stage, D3DTSS_ALPHAARG2, arg2 ));

void PixRectDevice::alphaBlend(const PixRect *texture, const CRect &dstRect) {
  const CPoint     topLeft     = dstRect.TopLeft();
  const CSize      size        = dstRect.Size();
  const CSize      textureSize = texture->getSize();

  const float qw = (size.cx == textureSize.cx) ? 1.0f : ((float)(size.cx) / (textureSize.cx));
  const float qh = (size.cy == textureSize.cy) ? 1.0f : ((float)(size.cy) / (textureSize.cy));

  BlendVertex vtx[4];
  vtx[0] = BlendVertex(dstRect.left  , dstRect.top     , 0 , 0 );
  vtx[1] = BlendVertex(dstRect.right , dstRect.top     , qw, 0 );
  vtx[2] = BlendVertex(dstRect.right , dstRect.bottom  , qw, qh);
  vtx[3] = BlendVertex(dstRect.left  , dstRect.bottom  , 0 , qh);

  V(m_device->SetRenderState( D3DRS_ALPHABLENDENABLE  , TRUE   ));
  V(m_device->SetRenderState( D3DRS_ALPHATESTENABLE   , TRUE   ));

  SetTextureColorStage(m_device, 0, D3DTA_TEXTURE, D3DTOP_BLENDTEXTUREALPHA, D3DTA_DIFFUSE);
  SetTextureAlphaStage(m_device, 0, D3DTA_TEXTURE, D3DTOP_MODULATE         , D3DTA_DIFFUSE);

  V(m_device->SetRenderState( D3DRS_LIGHTING          , FALSE               ));
  V(m_device->SetRenderState( D3DRS_CULLMODE          , D3DCULL_NONE        ));
  V(m_device->SetRenderState( D3DRS_ZENABLE           , FALSE               ));
  V(m_device->SetRenderState( D3DRS_SRCBLEND          , D3DBLEND_SRCALPHA   ));
  V(m_device->SetRenderState( D3DRS_DESTBLEND         , D3DBLEND_INVSRCALPHA));
  V(m_device->SetTexture(0, ((PixRect*)texture)->getTexture()));
  V(m_device->SetFVF(BlendVertex::FVF_Flags));
  V(m_device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vtx, sizeof(BlendVertex)));

  V(m_device->SetRenderState( D3DRS_ALPHABLENDENABLE  , FALSE));
}

void PixRect::drawRotated(const PixRect *src, const CPoint &dst, double degree) {
  const RotationData    rotation(src->getSize(), degree);
  const CSize           dstSize = getSize();
  LPD3DXRENDERTOSURFACE tmpRender;
  LPDIRECT3DDEVICE      device  = m_device.getD3Device();
  V(D3DXCreateRenderToSurface(device
                             ,dstSize.cx, dstSize.cy
                             ,getPixelFormat()
                             ,FALSE
                             ,(D3DFORMAT)0
                             ,&tmpRender));
  D3DVIEWPORT9 vp;
  vp.X = vp.Y = 0;
  vp.Width  = dstSize.cx;
  vp.Height = dstSize.cy;
  vp.MinZ   = 0;
  vp.MaxZ   = 1;
  V(tmpRender->BeginScene(m_surface, &vp));
  m_device.set2DTransform(dstSize);
  m_device.setWorldMatrix(rotation.m_world);
  const PixRect *texture = (src->getType() == PIXRECT_TEXTURE) ? src  : src->clone(true, PIXRECT_TEXTURE, D3DPOOL_DEFAULT);

  unsigned long clearColor = 0x00ff0000;
  V(device->Clear(0, NULL, D3DCLEAR_TARGET, clearColor, 1.0f, 0));
  m_device.alphaBlend(texture, CRect(dst, src->getSize()));
  V(tmpRender->EndScene(D3DX_DEFAULT));
  tmpRender->Release();

  V(m_device.getD3Device()->SetTexture(0, NULL));

  if(texture != src)  delete texture;
}

PixRect *PixRect::rotateImage(const PixRect *src, double degree) { // static
  RotationData rot(src->getSize(), degree);
  PixRect     *result = new PixRect(src->getDevice(), PIXRECT_PLAINSURFACE, rot.m_resultRect.Size(),D3DPOOL_FORCE_DWORD, D3DFMT_A8R8G8B8);
  result->drawRotated(src, ORIGIN, degree);
  return result;
}

CSize PixRect::getRotatedSize(const CSize &size, double degree) { // static
  const RotationData data(size, degree);
  return data.m_resultRect.Size();
}
