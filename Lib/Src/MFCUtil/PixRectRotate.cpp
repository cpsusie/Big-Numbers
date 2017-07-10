#include "pch.h"
#include <Math.h>
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

#define V CHECK3DRESULT

class TextureVertex2D {
  float m_x, m_y, m_z, m_tu, m_tv;
public:
  enum FVF {
    FVF_Flags = D3DFVF_XYZ | D3DFVF_TEX1
  };
  TextureVertex2D() {
  }
  TextureVertex2D(int x, int y, float tu, float tv) : m_x((float)x), m_y((float)y), m_z(0), m_tu(tu), m_tv(tv) {
  }
  TextureVertex2D(const CPoint &p, const Point2DP &tp) : m_x((float)p.x), m_y((float)p.y), m_z(0), m_tu((float)tp.x), m_tv((float)tp.y) {
  }
};

class TextureTriangleFan2D {
  DECLARERESULTCHECKER;
  PixRectDevice &m_device;
public:
  TextureVertex2D m_vtx[4];

  TextureTriangleFan2D(PixRectDevice *device, const CRect &dstRect, const CSize &textureSize);
  void draw() const;
};

TextureTriangleFan2D::TextureTriangleFan2D(PixRectDevice *device, const CRect &dstRect, const CSize &textureSize)
: m_device(*device)
{
  const CSize size = dstRect.Size();
  const float qw   = (size.cx == textureSize.cx) ? 1.0f : ((float)(size.cx) / (textureSize.cx));
  const float qh   = (size.cy == textureSize.cy) ? 1.0f : ((float)(size.cy) / (textureSize.cy));

  m_vtx[0] = TextureVertex2D(dstRect.left  , dstRect.top     , 0 , 0 );
  m_vtx[1] = TextureVertex2D(dstRect.right , dstRect.top     , qw, 0 );
  m_vtx[2] = TextureVertex2D(dstRect.right , dstRect.bottom  , qw, qh);
  m_vtx[3] = TextureVertex2D(dstRect.left  , dstRect.bottom  , 0 , qh);
}

void TextureTriangleFan2D::draw() const {
  LPDIRECT3DDEVICE &device = m_device.getD3Device();
  V(device->SetFVF(TextureVertex2D::FVF_Flags));
  V(device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_vtx, sizeof(TextureVertex2D)));
}

#ifdef _DEBUG
void TextureTriangleFan2D::check3DResult(TCHAR *fileName, int line, HRESULT hr) const {
  m_device.check3DResult(fileName, line, hr);
}
#else
void TextureTriangleFan2D::check3DResult(HRESULT hr) const {
  m_device.check3DResult(hr);
}
#endif

#define SetTextureColorStage(dev, stage, arg1, op, arg2)        \
  V(dev->SetTextureStageState(stage, D3DTSS_COLOROP  , op   )); \
  V(dev->SetTextureStageState(stage, D3DTSS_COLORARG1, arg1 )); \
  V(dev->SetTextureStageState(stage, D3DTSS_COLORARG2, arg2 ));

#define SetTextureAlphaStage(dev, stage, arg1, op, arg2)        \
  V(dev->SetTextureStageState(stage, D3DTSS_ALPHAOP  , op   )); \
  V(dev->SetTextureStageState(stage, D3DTSS_ALPHAARG1, arg1 )); \
  V(dev->SetTextureStageState(stage, D3DTSS_ALPHAARG2, arg2 ));

void PixRectDevice::alphaBlend(const PixRect *texture, const CRect &dstRect) {
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
  TextureTriangleFan2D(this, dstRect, texture->getSize()).draw();

  V(m_device->SetRenderState( D3DRS_ALPHABLENDENABLE  , FALSE));
}

void PixRectDevice::draw(const PixRect *pr, const CRect &dstRect) {
  const PixRect *texture = (pr->getType() == PIXRECT_TEXTURE) ? pr  : pr->clone(true, PIXRECT_TEXTURE, D3DPOOL_DEFAULT);

  V(m_device->SetRenderState( D3DRS_ALPHABLENDENABLE  , TRUE   ));
  V(m_device->SetRenderState( D3DRS_ALPHATESTENABLE   , TRUE   ));

  SetTextureColorStage(m_device, 0, D3DTA_TEXTURE, D3DTOP_BLENDTEXTUREALPHA, D3DTA_DIFFUSE);
  SetTextureAlphaStage(m_device, 0, D3DTA_TEXTURE, D3DTOP_MODULATE         , D3DTA_DIFFUSE);

  V(m_device->SetRenderState( D3DRS_LIGHTING          , FALSE               ));
  V(m_device->SetRenderState( D3DRS_CULLMODE          , D3DCULL_NONE        ));
  V(m_device->SetRenderState( D3DRS_ZENABLE           , FALSE               ));
  V(m_device->SetRenderState( D3DRS_SRCBLEND          , D3DBLEND_SRCALPHA   ));
  V(m_device->SetRenderState( D3DRS_DESTBLEND         , D3DBLEND_ZERO       ));
  V(m_device->SetTexture(0, ((PixRect*)texture)->getTexture()));
  TextureTriangleFan2D(this, dstRect, texture->getSize()).draw();

  V(m_device->SetRenderState( D3DRS_ALPHABLENDENABLE  , FALSE));

  if(texture != pr) delete texture;
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
  D3DVIEWPORT vp;
  vp.X = vp.Y = 0;
  vp.Width  = dstSize.cx;
  vp.Height = dstSize.cy;
  vp.MinZ   = 0;
  vp.MaxZ   = 1;

//showPixRect(src);
//showPixRect(this);
  V(tmpRender->BeginScene(m_surface, &vp));
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
  tmpRender->Release();

  V(device->SetTexture(0, NULL));

  if(texture != src)  delete texture;

//showPixRect(this);
}

PixRect *PixRect::rotateImage(const PixRect *src, double degree, D3DCOLOR background) { // static
  const CSize resultSize = getRotatedSize(src->getSize(), degree);
  PixRect    *result     = new PixRect(src->getDevice(), src->getType(), resultSize, src->getPool(), src->getPixelFormat());
  result->fillColor(background);
  result->drawRotated(src, resultSize/2, degree, src->getRect().CenterPoint());
  return result;
}

CSize PixRect::getRotatedSize(const CSize &size, double degree) { // static
  const Point2DP center = ORIGIN;
  return Size2DS(getTransformedRectangle(createTransformedWorld(1, &center, GRAD2RAD(degree), NULL)
                                ,CRect(ORIGIN,size)).getSize());
}
