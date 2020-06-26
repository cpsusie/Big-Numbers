#include "pch.h"
#include <MFCUtil/PixRect.h>
#include <MFCUtil/PixRectDevice.h>

PixRectDevice::PixRectDevice() {
  m_device       = NULL;
  m_renderTarget = NULL;
  resetException();
}

PixRectDevice::~PixRectDevice() {
  detach();
}

void PixRectDevice::attach(HWND hwnd, bool windowed, const CSize *size) {
  detach();

  resetException();
  CSize sz;
  if(!windowed) {
    sz = getScreenSize(false);
  } else if(size == NULL) {
    sz = getClientRect(hwnd).Size();
  } else {
    sz = *size;
  }

  HDC screenDC = getScreenDC();
  m_appScaleX = (float)(GetDeviceCaps(screenDC, LOGPIXELSX) / 96.0);
  m_appScaleY = (float)(GetDeviceCaps(screenDC, LOGPIXELSX) / 96.0);
  DeleteDC(screenDC);

  D3DPRESENT_PARAMETERS param;
  ZeroMemory(&param, sizeof(param));

  param.Windowed               = windowed ? TRUE : FALSE;
  param.MultiSampleType        = D3DMULTISAMPLE_NONE;
  param.SwapEffect             = D3DSWAPEFFECT_DISCARD;
  param.EnableAutoDepthStencil = FALSE;
  param.BackBufferFormat       = D3DFMT_X8R8G8B8;
  param.hDeviceWindow          = hwnd;
  param.Flags                  = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
  param.BackBufferCount        = 1;
  param.BackBufferWidth        = sz.cx;
  param.BackBufferHeight       = sz.cy;
  param.PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;

  m_device = DirectXDeviceFactory::getInstance().createDevice(hwnd, &param, D3DADAPTER_DEFAULT);

  D3DDISPLAYMODE displayMode;
  V(m_device->GetDisplayMode(0, &displayMode));
  m_defaultPixelFormat = displayMode.Format;
}

void PixRectDevice::detach() {
  releaseRenderTarget();
  SAFERELEASE(m_device);
  resetException();
}

void PixRectDevice::beginScene() {
  V(m_device->BeginScene());
}
void PixRectDevice::endScene() {
  V(m_device->EndScene());
}

void PixRectDevice::releaseRenderTarget() {
  SAFERELEASE(m_renderTarget);
}

void PixRectDevice::setRenderTargetSize(const CSize &size) {
  if((m_renderTarget == NULL) || (size != m_renderTargetSize)) {
    releaseRenderTarget();
    m_renderTarget = createRenderTarget(size);
    m_renderTargetSize = size;
  }
}

LPDIRECT3DTEXTURE PixRectDevice::createTexture(const CSize &size, D3DFORMAT format, D3DPOOL pool) {
  LPDIRECT3DTEXTURE texture;

  // Create an alpha texture
  if(format == D3DFMT_FORCE_DWORD) {
    format = getDefaultPixelFormat();
  }
  if(pool == D3DPOOL_FORCE_DWORD) {
    pool = D3DPOOL_SYSTEMMEM;
  }
  DWORD usage = D3DUSAGE_DYNAMIC; // (pool == D3DPOOL_MANAGED) ? 0 : D3DUSAGE_DYNAMIC;

  V(m_device->CreateTexture(size.cx, size.cy, 1, usage, format, pool, &texture, NULL));
  TRACE_CREATE(texture);
  return texture;
}

LPDIRECT3DSURFACE PixRectDevice::createRenderTarget(const CSize &size, D3DFORMAT format, bool lockable) {
  LPDIRECT3DSURFACE surface;

  LPDIRECT3DSURFACE oldRenderTarget;
  V(m_device->GetRenderTarget(0, &oldRenderTarget));
  TRACE_REFCOUNT(oldRenderTarget);
  D3DSURFACE_DESC desc;
  V(oldRenderTarget->GetDesc(&desc));
  releaseSurface(oldRenderTarget, PIXRECT_RENDERTARGET);
  if(format == D3DFMT_FORCE_DWORD) {
    format = desc.Format;
  }
  V(m_device->CreateRenderTarget(size.cx, size.cy, format, desc.MultiSampleType, desc.MultiSampleQuality, lockable, &surface, NULL));
  TRACE_CREATE(surface);
  return surface;
}

LPDIRECT3DSURFACE PixRectDevice::createOffscreenPlainSurface(const CSize &size, D3DFORMAT format, D3DPOOL pool) {
  if(format == D3DFMT_FORCE_DWORD) {
    format = getDefaultPixelFormat();
  }
  if(pool == D3DPOOL_FORCE_DWORD) {
    pool = D3DPOOL_SYSTEMMEM;
  }
  LPDIRECT3DSURFACE surface;
  V(m_device->CreateOffscreenPlainSurface(size.cx, size.cy, format, pool, &surface, NULL));
  TRACE_CREATE(surface);
  return surface;
}

LPDIRECT3DSURFACE PixRectDevice::getRenderTarget() {
  LPDIRECT3DSURFACE surface;
  V(m_device->GetRenderTarget(0,&surface));
  TRACE_CREATE(surface);
  return surface;
}

bool PixRectDevice::supportFormatConversion(D3DFORMAT srcFormat, D3DFORMAT dstFormat, UINT adapter) const {
  const D3DDEVTYPE deviceType = getDeviceCaps().DeviceType;
  return DirectXDeviceFactory::getInstance().supportFormatConversion(deviceType, srcFormat, dstFormat, adapter);
}

CompactArray<D3DDISPLAYMODE> PixRectDevice::getDisplayModes(UINT adapter) {
  return DirectXDeviceFactory::getInstance().getDisplayModes(adapter);
}

D3DCAPS PixRectDevice::getDeviceCaps() const {
  D3DCAPS caps;
  V(m_device->GetDeviceCaps(&caps));
  return caps;
}

/*
DDCAPS PixRect::getEmulatorCaps() { // static
  DDCAPS result;
  result.dwSize = sizeof(DDCAPS);
  V(directDraw->GetCaps(NULL, &result));
  return result;
}
*/

void PixRectDevice::render(const PixRect *pr) {
  if(!m_device) {
    return;  // Haven't been initialized yet!
  }

  LPDIRECT3DSURFACE oldRenderTarget = NULL;
  try {
    beginScene();
    V(m_device->SetRenderState(D3DRS_LIGHTING, FALSE));
    endScene();

    oldRenderTarget = getRenderTarget();
    if(pr->m_desc.Pool != D3DPOOL_DEFAULT) {
      setRenderTargetSize(pr->getSize());
      V(m_device->UpdateSurface(pr->m_surface, NULL, m_renderTarget, NULL));
      V(m_device->StretchRect(m_renderTarget, NULL, oldRenderTarget, NULL, D3DTEXF_NONE));
    } else {
      V(m_device->StretchRect(pr->m_surface, NULL, oldRenderTarget, NULL, D3DTEXF_NONE));
    }

    SAFERELEASE(oldRenderTarget);
    V(m_device->Present(NULL, NULL, NULL, NULL));
  } catch(...) {
    endScene();
    SAFERELEASE(oldRenderTarget);
    throw;
  }
}

void PixRectDevice::set2DTransform(const CSize &size) {
  D3DMATRIX projection;
  memset(&projection, 0, sizeof(D3DMATRIX));

  projection._11 = 2.0f * m_appScaleX / (float)size.cx;
  projection._41 = -m_appScaleX;
  projection._22 = -2.0f * m_appScaleY / (float)size.cy;
  projection._42 = m_appScaleY;

  projection._33 = 1;
  projection._43 = 0;

  projection._34 = 0;
  projection._44 = 1;

  V(m_device->SetTransform(D3DTS_PROJECTION, &projection));
/*
  const CSize sz = getSize(m_someTexture);
  const D3DXVECTOR2 rotationCenter(37,37);
  D3DXMATRIX matWorld;
  CHECKD3DRESULT(m_device->SetTransform( D3DTS_WORLD, D3DXMatrixAffineTransformation2D(&matWorld, 1, &rotationCenter, (float)GRAD2RAD(m_rotation), NULL)));
*/
}

void PixRectDevice::setWorldMatrix(const D3DXMATRIX &m) {
  V(m_device->SetTransform(D3DTS_WORLD, &m));
}
D3DXMATRIX &PixRectDevice::getWorldMatrix(D3DXMATRIX &m) const {
  V(m_device->GetTransform(D3DTS_WORLD, &m));
  return m;
}

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

  if(texture != pr) {
    SAFEDELETE(texture);
  }
}
