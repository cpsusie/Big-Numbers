#include "stdafx.h"
#include <Math/Rectangle2D.h>
#include <D3DGraphics/D3AbstractTextureFactory.h>
#include "DrawTool.h"

#define D3DFVF_BLENDVERTEX ( D3DFVF_XYZ | D3DFVF_TEX1 )

class BlendVertex {
  float m_x, m_y, m_z, m_tu, m_tv;
public:
  BlendVertex() {
  }
  BlendVertex(int x, int y, float tu, float tv) : m_x((float)x), m_y((float)y), m_z(0), m_tu(tu), m_tv(tv) {
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

D3DXVECTOR2 operator*(const D3DXMATRIX &m, const D3DXVECTOR2 &v) {
  D3DXVECTOR4 v4;
  D3DXVec2Transform(&v4, &v, &m);
  return D3DXVECTOR2(v4.x, v4.y);
}

static CSize getSurfaceSize(LPDIRECT3DSURFACE surface) {
  D3DSURFACE_DESC desc;
  V(surface->GetDesc(&desc));
  return CSize(desc.Width, desc.Height);
}

static CSize getTextureSize(LPDIRECT3DTEXTURE texture) {
  LPDIRECT3DSURFACE9 surface;
  V(texture->GetSurfaceLevel(0, &surface)); TRACE_REFCOUNT(surface);
  const CSize sz = getSurfaceSize(surface);
  SAFERELEASE(surface);
  return sz;
}

static void set2DProjection(LPDIRECT3DDEVICE device, const CSize &winSize) {
  D3DMATRIX matProj;
  memset(&matProj, 0, sizeof(D3DMATRIX));
  const float app_scale_x = 1, app_scale_y = 1;

  matProj._11 = 2.0f * app_scale_x / (float)winSize.cx;
  matProj._41 = -app_scale_x;
  matProj._22 = -2.0f * app_scale_y / (float)winSize.cy;
  matProj._42 = app_scale_y;

  matProj._33 = 1;
  matProj._43 = 0;

  matProj._34 = 0;
  matProj._44 = 1;

  V(device->SetTransform(D3DTS_PROJECTION, &matProj));
}

static D3DXMATRIX create2DRotationWorld(const D3DXVECTOR2 &center, double rad) {
  D3DXMATRIX m;
  return *D3DXMatrixAffineTransformation2D(&m, 1, &center, (float)rad, NULL);
}

static void setWorldMatrix(LPDIRECT3DDEVICE device, const D3DXMATRIX &m) {
  V(device->SetTransform(D3DTS_WORLD, &m));
}

static void alphaBlend(LPDIRECT3DDEVICE device, LPDIRECT3DTEXTURE texture, const CRect &dstRect) {
  const CPoint topLeft     = dstRect.TopLeft();
  const CSize  size        = dstRect.Size();
  const CSize  textureSize = getTextureSize(texture);

/*
  debugLog("TextureSize:(%3d,%3d), DstRect:(%3d,%3d,%3d,%3d), Size:(%3d,%3d)\n"
          ,textureSize.cx, textureSize.cy
          ,dstRect.left,dstRect.top, dstRect.right, dstRect.bottom
          ,size.cx, size.cy);
*/

  const float qw = (size.cx == textureSize.cx) ? 1.0f : ((float)(size.cx) / (textureSize.cx));
  const float qh = (size.cy == textureSize.cy) ? 1.0f : ((float)(size.cy) / (textureSize.cy));

  BlendVertex p[4];
  p[0] = BlendVertex(dstRect.left  , dstRect.top     , 0 , 0 );
  p[1] = BlendVertex(dstRect.right , dstRect.top     , qw, 0 );
  p[2] = BlendVertex(dstRect.right , dstRect.bottom  , qw, qh);
  p[3] = BlendVertex(dstRect.left  , dstRect.bottom  , 0 , qh);

  V(device->SetRenderState( D3DRS_ALPHABLENDENABLE  , TRUE   ));
  V(device->SetRenderState( D3DRS_ALPHATESTENABLE   , TRUE   ));

  SetTextureColorStage(device, 0, D3DTA_TEXTURE, D3DTOP_BLENDTEXTUREALPHA, D3DTA_DIFFUSE);
  SetTextureAlphaStage(device, 0, D3DTA_TEXTURE, D3DTOP_MODULATE , D3DTA_DIFFUSE);
  V(device->SetRenderState( D3DRS_LIGHTING          , FALSE               ));
  V(device->SetRenderState( D3DRS_CULLMODE          , D3DCULL_NONE        ));
  V(device->SetRenderState( D3DRS_ZENABLE           , FALSE               ));
  V(device->SetRenderState( D3DRS_SRCBLEND          , D3DBLEND_SRCALPHA   ));
  V(device->SetRenderState( D3DRS_DESTBLEND         , D3DBLEND_INVSRCALPHA));
  V(device->SetTexture(0, texture ));
  V(device->SetFVF(D3DFVF_BLENDVERTEX));
  V(device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, p, sizeof(BlendVertex)));
}

static void render(LPDIRECT3DDEVICE device, LPDIRECT3DTEXTURE texture, const CSize &bmSize, HBITMAP result, const CSize &trSize) {
  ULONG clearColor = 0xffffffff;
  V(device->Clear(0, NULL, D3DCLEAR_TARGET, clearColor, 1.0f, 0));

  LPDIRECT3DSURFACE renderTarget;
  V(device->GetRenderTarget(0, &renderTarget)); TRACE_REFCOUNT(renderTarget);

  CPoint leftTop(0,0);
  set2DProjection(device, getSurfaceSize(renderTarget));

  V(device->BeginScene());

  alphaBlend(device, texture, CRect(leftTop, bmSize));

  V(device->EndScene());

  HDC renderDC;
  V(renderTarget->GetDC(&renderDC));

  HDC resultDC = CreateCompatibleDC(NULL);
  HGDIOBJ oldGdi = SelectObject(resultDC, result);
  BOOL ret = BitBlt(resultDC, 0,0,trSize.cx,trSize.cy, renderDC, 0,0, SRCCOPY);
  SelectObject(resultDC, oldGdi);
  DeleteDC(resultDC);

  V(renderTarget->ReleaseDC(renderDC));
  SAFERELEASE(renderTarget);

//  V(device->Present(NULL, NULL, NULL, NULL));
}

static Rectangle2D getTransformedRectangle(const D3DXMATRIX &m, const CSize bmSize) {
  D3DXVECTOR2 corner[4];
  corner[0] = D3DXVECTOR2(0               , 0               );
  corner[1] = D3DXVECTOR2((float)bmSize.cx, 0               );
  corner[2] = D3DXVECTOR2(0               , (float)bmSize.cy);
  corner[3] = D3DXVECTOR2((float)bmSize.cx, (float)bmSize.cy);

  Point2DArray trCorners(4);
  for(int i = 0; i < 4; i++) {
    const D3DXVECTOR2 tp = m * corner[i];
    trCorners.add(Point2D(tp.x,tp.y));
  }
  return trCorners.getBoundingBox();
}

HBITMAP rotateBitmap(LPDIRECT3DDEVICE device, HBITMAP bm, double degree) {
  const BITMAP      bmInfo          = getBitmapInfo(bm);
  const CSize       bmSize          = CSize(bmInfo.bmWidth, bmInfo.bmHeight);
  const D3DXVECTOR2 rotCenter       = D3DXVECTOR2((float)bmSize.cx/2, (float)bmSize.cy/2);
  const D3DXMATRIX  rotationMatrix  = create2DRotationWorld(rotCenter, GRAD2RAD(degree));
        Rectangle2D fRect           = getTransformedRectangle(rotationMatrix, bmSize);
  const Point2D     offset          = -Point2D(ceil(fRect.getX()),ceil(fRect.getY())) + Point2D(fraction(fRect.getBottomRight().x),fraction(fRect.getBottomRight().y));
  const D3DXMATRIX  translateMatrix = createTranslateMatrix(D3DXVECTOR3((float)offset.x, (float)offset.y, 0));
                    fRect           = Rectangle2D(0,0, fRect.getBottomRight().x + offset.x, fRect.getBottomRight().y + offset.y);
  const CRect       resultRect      = CRect(CPoint(0,0),CPoint((int)ceil(fRect.getBottomRight().x),(int)ceil(fRect.getBottomRight().y)));
  D3DXMATRIX        worldMatrix;
  CSize             trSize;

  if(fmod(degree, 90) == 0) {
    switch(((int)degree)%360) {
    case   90:
    case  -90:
    case  270:
    case -270:
      worldMatrix = rotationMatrix * translateMatrix;
      trSize      = CSize(bmSize.cy, bmSize.cx);
      break;
    case    0   :
    case  180 :
    case -180:
      worldMatrix = rotationMatrix;
      trSize      = bmSize;
      break;
    }
  } else {
    trSize        = resultRect.Size();
    worldMatrix   = rotationMatrix * translateMatrix;
  }

  HBITMAP result = CreateBitmap(trSize.cx, trSize.cy, bmInfo.bmPlanes, bmInfo.bmBitsPixel, NULL);

  setWorldMatrix(device, worldMatrix);

  LPDIRECT3DTEXTURE texture = AbstractTextureFactory::getTextureFromBitmap(device, bm);
  render(device, texture, bmSize, result, trSize);
  SAFERELEASE(texture);

  return result;
}
