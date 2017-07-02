#include "pch.h"
#include <Math.h>
#include <float.h>
#include <MFCUtil/PixRect.h>
#include <MFCUtil/D3DeviceFactory.h>

#pragma warning(disable : 4073)
#pragma init_seg(lib)

#ifdef assert
#undef assert
#endif

#ifdef _DEBUG
#define assert(exp) (void)( (exp) || (xassert(__TFILE__, __LINE__, _T(#exp)), 0) )
#else
#define assert(exp)
#endif

CSize getSurfaceSize(LPDIRECT3DSURFACE surface) {
  D3DSURFACE_DESC desc;
  surface->GetDesc(&desc);
  return CSize(desc.Width, desc.Height);
}

CSize getTextureSize(LPDIRECT3DTEXTURE texture) {
  D3DSURFACE_DESC desc;
  texture->GetLevelDesc(0, &desc);
  return CSize(desc.Width, desc.Height);
}

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

  m_device = D3DeviceFactory::createDevice(hwnd, &param, D3DADAPTER_DEFAULT);

  D3DDISPLAYMODE displayMode;
  CHECK3DRESULT(m_device->GetDisplayMode(0, &displayMode));
  m_defaultPixelFormat = displayMode.Format;
}

void PixRectDevice::detach() {
  if (m_device != NULL) {
    m_device->Release();
    m_device = NULL;
  }
  releaseRenderTarget();
  resetException();
}

void PixRectDevice::releaseRenderTarget() {
  if (m_renderTarget != NULL) {
    m_renderTarget->Release();
    m_renderTarget = NULL;
  }
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
  if (format == D3DFMT_FORCE_DWORD) {
    format = getDefaultPixelFormat();
  }
  if (pool == D3DPOOL_FORCE_DWORD) {
    pool = D3DPOOL_SYSTEMMEM;
  }
  DWORD usage = D3DUSAGE_DYNAMIC; // (pool == D3DPOOL_MANAGED) ? 0 : D3DUSAGE_DYNAMIC;

  CHECK3DRESULT(m_device->CreateTexture(size.cx, size.cy, 1, usage, format, pool, &texture, NULL));
  return texture;
}

LPDIRECT3DSURFACE PixRectDevice::createRenderTarget(const CSize &size, D3DFORMAT format, bool lockable) {
  LPDIRECT3DSURFACE surface;

  LPDIRECT3DSURFACE oldRenderTarget;
  CHECK3DRESULT(m_device->GetRenderTarget(0, &oldRenderTarget));
  D3DSURFACE_DESC desc;
  CHECK3DRESULT(oldRenderTarget->GetDesc(&desc));
  releaseSurface(oldRenderTarget, PIXRECT_RENDERTARGET);
  if(format == D3DFMT_FORCE_DWORD) {
    format = desc.Format;
  }
  CHECK3DRESULT(m_device->CreateRenderTarget(size.cx, size.cy, format, desc.MultiSampleType, desc.MultiSampleQuality, lockable, &surface, NULL));
  return surface;
}

LPDIRECT3DSURFACE PixRectDevice::createOffscreenPlainSurface(const CSize &size, D3DFORMAT format, D3DPOOL pool) {
  if (format == D3DFMT_FORCE_DWORD) {
    format = getDefaultPixelFormat();
  }
  if (pool == D3DPOOL_FORCE_DWORD) {
    pool = D3DPOOL_SYSTEMMEM;
  }
  LPDIRECT3DSURFACE surface;
  CHECK3DRESULT(m_device->CreateOffscreenPlainSurface(size.cx, size.cy, format, pool, &surface, NULL));
  return surface;
}

void PixRectDevice::releaseTexture(LPDIRECT3DTEXTURE texture) {
  texture->Release();
}

void PixRectDevice::releaseSurface(LPDIRECT3DSURFACE surface, PixRectType type) {
  surface->Release();
}

LPDIRECT3DSURFACE PixRectDevice::getRenderTarget() {
  LPDIRECT3DSURFACE surface;
  CHECK3DRESULT(m_device->GetRenderTarget(0,&surface));
  return surface;
}

bool PixRectDevice::supportFormatConversion(D3DFORMAT srcFormat, D3DFORMAT dstFormat, UINT adapter) const {
  const D3DDEVTYPE deviceType = getDeviceCaps().DeviceType;
  return D3DeviceFactory::supportFormatConversion(deviceType, srcFormat, dstFormat, adapter);
}

D3DCAPS PixRectDevice::getDeviceCaps() const {
  D3DCAPS caps;
  CHECK3DRESULT(m_device->GetDeviceCaps(&caps));
  return caps;
}

/*
DDCAPS PixRect::getEmulatorCaps() { // static
  DDCAPS result;
  result.dwSize = sizeof(DDCAPS);
  CHECK3DRESULT(directDraw->GetCaps(NULL, &result));
  return result;
}
*/

#define SAFERELEASE(s) { if(s) (s)->Release(); s = NULL; }

void PixRectDevice::render(const PixRect *pr) {
  if(!m_device) {
    return;  // Haven't been initialized yet!
  }

  LPDIRECT3DSURFACE oldRenderTarget = NULL;
  try {
    beginScene();
    CHECK3DRESULT(m_device->SetRenderState(D3DRS_LIGHTING, FALSE));
    endScene();

    oldRenderTarget = getRenderTarget();
    if (pr->m_desc.Pool != D3DPOOL_DEFAULT) {
      setRenderTargetSize(pr->getSize());
      CHECK3DRESULT(m_device->UpdateSurface(pr->m_surface, NULL, m_renderTarget, NULL));
      CHECK3DRESULT(m_device->StretchRect(m_renderTarget, NULL, oldRenderTarget, NULL, D3DTEXF_NONE));
    } else {
      CHECK3DRESULT(m_device->StretchRect(pr->m_surface, NULL, oldRenderTarget, NULL, D3DTEXF_NONE));
    }

    SAFERELEASE(oldRenderTarget);
    CHECK3DRESULT(m_device->Present(NULL, NULL, NULL, NULL));
  } catch (...) {
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

  CHECK3DRESULT(m_device->SetTransform(D3DTS_PROJECTION, &projection));
/*
  const CSize sz = getSize(m_someTexture);
  const D3DXVECTOR2 rotationCenter(37,37);
  D3DXMATRIX matWorld;
  CHECKD3DRESULT(m_device->SetTransform( D3DTS_WORLD, D3DXMatrixAffineTransformation2D(&matWorld, 1, &rotationCenter, (float)GRAD2RAD(m_rotation), NULL)));
*/
}

const TCHAR *PixRect::s_typeName[] {
  _T("PIXRECT_TEXTURE")
 ,_T("PIXRECT_RENDERTARGET")
 ,_T("PIXRECT_PLAINSURFACE")
};

#ifdef _DEBUG
void PixRectDevice::check3DResult(TCHAR *fileName, int line, HRESULT hr) const {
  if((hr != D3D_OK) && !m_exceptionInProgress) {
    m_exceptionInProgress = true;
    throwException(_T("D3D-error %s in %s, line %d"), get3DErrorMsg(hr).cstr(), fileName, line);
  }
}
void PixRect::check3DResult(TCHAR *fileName, int line, HRESULT hr) const {
  m_device.check3DResult(fileName, line, hr);
}
#else
void PixRectDevice::check3DResult(HRESULT hr) const {
  if((hr != D3D_OK) && !m_exceptionInProgress) {
    m_exceptionInProgress = true;
    AfxMessageBox(format(_T("D3D-error %s"), get3DErrorMsg(hr).cstr()).cstr(), MB_ICONSTOP);
    exit(-1);
  }
}
void PixRect::check3DResult(HRESULT hr) const {
  m_device.check3DResult(hr);
}
#endif

void PixRect::reOpenDirectX() { // static
//  uninitialize();
//  initialize();
}

PixRect::PixRect(PixRectDevice &device) : m_device(device) {
  initSurfaces();
}

PixRect::PixRect(PixRectDevice &device, PixRectType type, UINT width, UINT height, D3DPOOL pool, D3DFORMAT pixelFormat) : m_device(device) {
  const CSize sz(width, height);
  initSurfaces();
  create(type, sz, pixelFormat, pool);
  fillColor(D3D_WHITE);
/*
  if(isValidGDIFormat()) {
    fillRect(0,0,width,height, D3D_WHITE);
  }
*/
}

PixRect::PixRect(PixRectDevice &device, PixRectType type, const CSize &size, D3DPOOL pool, D3DFORMAT pixelFormat) : m_device(device) {
  initSurfaces();
  create(type, size, pixelFormat, pool);
  fillColor(D3D_WHITE);
/*
  if(isValidGDIFormat()) {
    fillRect(0,0,size.cx,size.cy,D3D_WHITE);
  }
*/
}

PixRect::PixRect(PixRectDevice &device, HBITMAP src, D3DPOOL pool, D3DFORMAT pixelFormat) : m_device(device) {
  initSurfaces();
  init(src, pixelFormat, pool);
}

PixRect::~PixRect() {
  destroy();
}

PixRect *PixRect::clone(bool cloneImage, PixRectType type, D3DPOOL pool) const {
  if (type == PiXRECT_FORCE_DWORD) {
    type = getType();
  }
  if (type == PIXRECT_RENDERTARGET) {
    pool = D3DPOOL_DEFAULT;
  }
  if (pool == D3DPOOL_FORCE_DWORD) {
    pool = getPool();
  }
  PixRect *copy = new PixRect(m_device, type, getSize(), pool, getPixelFormat());
  if(cloneImage) {
    if ((getPool() == D3DPOOL_SYSTEMMEM) && (pool == D3DPOOL_DEFAULT)) {
      LPDIRECT3DSURFACE srcSurface = NULL, dstSurface = NULL;
      try {
        srcSurface = getSurface();
        dstSurface = copy->getSurface();
        CHECK3DRESULT(m_device.getD3Device()->UpdateSurface(srcSurface, NULL, dstSurface, NULL));
        SAFERELEASE(srcSurface);
        SAFERELEASE(dstSurface);
      } catch(...) {
        SAFERELEASE(srcSurface);
        SAFERELEASE(dstSurface);
        delete copy;
        throw;
      }
    } else {
      copy->rop(getRect(), SRCCOPY, this, ORIGIN);
    }
  }
  return copy;
}

void PixRect::moveToPool(D3DPOOL pool) {
  DEFINEMETHODNAME;
  if (pool == getPool()) return;
  if (getType() == PIXRECT_RENDERTARGET) {
    throwException(_T("%s:RenderTargets cannot be moved from D3DPOOL_DEFAULT"), method);
  }
  PixRect *tmp = clone(true, getType(), pool);
  destroy();
  m_type = tmp->m_type;
  m_desc = tmp->m_desc;
  switch(getType()) {
  case PIXRECT_TEXTURE:
    m_texture = tmp->m_texture;
    tmp->m_texture = NULL;
    break;
  case PIXRECT_PLAINSURFACE:
    m_surface = tmp->m_surface;
    tmp->m_surface = NULL;
    break;
  default:
    unknownTypeError(method);
  }
  delete tmp;
}

LPDIRECT3DSURFACE PixRect::cloneSurface(D3DPOOL pool) const {
  LPDIRECT3DSURFACE dstSurface = m_device.createOffscreenPlainSurface(getSize(), getPixelFormat(), pool);
  LPDIRECT3DSURFACE srcSurface = getSurface();
  CHECK3DRESULT(m_device.getD3Device()->UpdateSurface(srcSurface, NULL, dstSurface, NULL));
  m_device.releaseSurface(srcSurface, m_type);
  return dstSurface;
}

void PixRect::showPixRect(const PixRect *pr) { // static
  HDC screenDC = getScreenDC();
  if (pr->hasAlphaChannel()) {
    alphaBlend(screenDC,0,0,pr->getWidth(),pr->getHeight(), *pr, 0, 0, pr->getWidth(),pr->getHeight(), 255);
  } else {
    BitBlt(screenDC,0,0,pr->getWidth(),pr->getHeight(),NULL,0,0,WHITENESS);
    PixRect::bitBlt(screenDC, 0,0,pr->getWidth(),pr->getHeight(), SRCCOPY, pr, 0,0);
  }
  DeleteDC(screenDC);
}

bool PixRect::canUseColorFill() const {
  switch (getType()) {
  case PIXRECT_RENDERTARGET: return true;
  case PIXRECT_PLAINSURFACE: return getPool() == D3DPOOL_DEFAULT;
  default                  : return false;
  }
}

void PixRect::fillColor(D3DCOLOR color, const CRect *r) {
  if(canUseColorFill()) {
    LPDIRECT3DSURFACE surface = getSurface();
    CHECK3DRESULT(m_device.getD3Device()->ColorFill(surface, r, color));
    surface->Release();
  } else {
    CRect tmpr;
    if (r == NULL) {
      tmpr = CRect(CPoint(0,0), getSize());
      r = &tmpr;
    }
    HBRUSH hbr = CreateSolidBrush(D3DCOLOR2COLORREF(color));
    HDC hdc = getDC();
    FillRect(hdc, r, hbr);
    DeleteObject(hbr);
    releaseDC(hdc);
  }
}

void PixRect::checkType(const TCHAR *method, PixRectType expectedType) const {
  if (getType() != expectedType) {
    throwException(_T("%s::Type is %s. Expected type:%s"), method, s_typeName[m_type], s_typeName[expectedType]);
  }
}

LPDIRECT3DSURFACE &PixRect::getPlainSurface() {
  checkType(__TFUNCTION__, PIXRECT_PLAINSURFACE);
  return m_surface;
}

LPDIRECT3DSURFACE &PixRect::getRenderTarget() {
  checkType(__TFUNCTION__, PIXRECT_RENDERTARGET);
  return m_surface;
}
LPDIRECT3DTEXTURE &PixRect::getTexture() {
  checkType(__TFUNCTION__, PIXRECT_TEXTURE);
  return m_texture;
}

void PixRect::checkHasAlphaChannel() const { // throw Exception if no alpha-channel
  if (!hasAlphaChannel()) {
    throwException(_T("PixRect has no alpha-channel. PixelFormat=%s"), getFormatName(getPixelFormat()));
  }
}

bool PixRect::hasAlphaChannel(D3DFORMAT format) { //static
  switch (format) {
  case D3DFMT_A8R8G8B8      :
  case D3DFMT_A1R5G5B5      :
  case D3DFMT_A4R4G4B4      :
  case D3DFMT_A8            :
  case D3DFMT_A8R3G3B2      :
  case D3DFMT_A2B10G10R10   :
  case D3DFMT_A8B8G8R8      :
  case D3DFMT_A2R10G10B10   :
  case D3DFMT_A16B16G16R16  :
  case D3DFMT_A8P8          :
  case D3DFMT_A8L8          :
  case D3DFMT_A4L4          :
  case D3DFMT_A2W10V10U10   :
  case D3DFMT_MULTI2_ARGB8  :
  case D3DFMT_A16B16G16R16F :
  case D3DFMT_A32B32G32R32F :
    return true;
  default:
    return false;
  }
}

bool PixRect::isValidGDIFormat(D3DFORMAT format) { // static
  switch(format) {
  case D3DFMT_R5G6B5  :
  case D3DFMT_X1R5G5B5:
  case D3DFMT_R8G8B8  :
  case D3DFMT_X8R8G8B8:
    return true;
  default:
    return false;
  }
}

void PixRect::setSize(const CSize &size) {
  const CSize oldSize = getSize();
  CSize areaToCopy;
  if(size == oldSize) {
    return;
  }
  areaToCopy.cx = min(oldSize.cx, size.cx);
  areaToCopy.cy = min(oldSize.cy, size.cy);
  PixRect copy(m_device, getType(), areaToCopy, getPool(), getPixelFormat());
  copy.rop(0, 0, areaToCopy.cx, areaToCopy.cy, SRCCOPY, this, 0,0);
  destroy();
  createPlainSurface(size, copy.getPixelFormat(), getPool());
  fillRect(getRect(), D3D_WHITE);
  rop(0,0, areaToCopy.cx, areaToCopy.cy, SRCCOPY, &copy, 0, 0);
}

CSize PixRect::getSizeInMillimeters(HDC hdc) const {
  if(hdc != NULL) {
    return pixelsToMillimeters(hdc,getSize());
  } else {
    HDC screenDC = getScreenDC();
    const CSize result = pixelsToMillimeters(screenDC,getSize());
    DeleteDC(screenDC);
    return result;
  }
}

void PixRect::init(HBITMAP src, D3DFORMAT pixelFormat, D3DPOOL pool) {
  String errMsg;
  const CSize sz = getBitmapSize(src);
  createPlainSurface(sz, pixelFormat, pool);

  HDC prDC = getDC();
  HDC bmDC = CreateCompatibleDC(prDC);
  HGDIOBJ oldGDI = SelectObject(bmDC, src);
  BOOL ok = BitBlt(prDC,0,0,getWidth(),getHeight(), bmDC,0,0,SRCCOPY);
  if(!ok) {
    errMsg = getLastErrorText();
  }

  SelectObject(bmDC, oldGDI);

  DeleteDC(bmDC);
  releaseDC(prDC);
  if(!ok) {
    throwException(_T("%s failed:%s"), __TFUNCTION__, errMsg.cstr());
  }
}

void PixRect::create(PixRectType type, const CSize &sz, D3DFORMAT pixelFormat, D3DPOOL pool) {
  destroy();
  switch (type) {
  case PIXRECT_TEXTURE:
    createTexture(sz, pixelFormat, pool);
    break;
  case PIXRECT_RENDERTARGET:
    createRenderTarget(sz, pixelFormat, true);
    break;
  case PIXRECT_PLAINSURFACE:
    createPlainSurface(sz, pixelFormat, pool);
    break;
  default:
    unknownTypeError(__TFUNCTION__, type);
  }
}

void PixRect::createTexture(const CSize &sz, D3DFORMAT pixelFormat, D3DPOOL pool) {
  m_texture = m_device.createTexture(sz, pixelFormat, pool);
  m_type    = PIXRECT_TEXTURE;
  const int levels = m_texture->GetLevelCount();
  CHECK3DRESULT(m_texture->GetLevelDesc(0, &m_desc));
}

void PixRect::createRenderTarget(const CSize &sz, D3DFORMAT pixelFormat, bool lockable) { // always in D3DPOOL_DEFAULT
  m_surface = m_device.createRenderTarget(sz, pixelFormat, lockable);
  m_type    = PIXRECT_RENDERTARGET;
  CHECK3DRESULT(m_surface->GetDesc(&m_desc));
}

void PixRect::createPlainSurface(const CSize &sz, D3DFORMAT pixelFormat, D3DPOOL pool) {
  m_surface = m_device.createOffscreenPlainSurface(sz, pixelFormat, pool);
  m_type    = PIXRECT_PLAINSURFACE;
  CHECK3DRESULT(m_surface->GetDesc(&m_desc));
}

void PixRect::destroy() {
  if (m_surface == NULL) return;
  switch (getType()) {
  case PIXRECT_TEXTURE     :
    destroyTexture();
    break;
  case PIXRECT_RENDERTARGET:
  case PIXRECT_PLAINSURFACE:
    destroySurface();
    break;
  default:
    unknownTypeError(__TFUNCTION__);
  }
}

void PixRect::destroyTexture() {
  assert((getType() == PIXRECT_TEXTURE) && (m_texture != NULL));
  m_device.releaseTexture(m_texture);
  m_texture = NULL;
}

void PixRect::destroySurface() {
  assert(((getType() == PIXRECT_RENDERTARGET) || (getType() == PIXRECT_PLAINSURFACE)) && (m_surface != NULL));
  m_device.releaseSurface(m_surface, m_type);
  m_surface = NULL;
}

PixRect &PixRect::operator=(HBITMAP src) {
  const D3DFORMAT pixelFormat = getPixelFormat();
  const D3DPOOL   pool        = getPool();
  destroy();
  init(src, pixelFormat, pool);
  return *this;

/*
  String errMsg;
  const CSize     bmSize      = getBitmapSize(src);
  const D3DFORMAT pixelFormat = getPixelFormat(src);
  const D3DPOOL   pool        = getPool();
  destroy();
  create(PIXRECT_TEXTURE,  bmSize, pixelFormat, pool);

  HDC prDC = getDC();
  HDC bmDC = CreateCompatibleDC(prDC);
  HGDIOBJ gdi1 = SelectObject(bmDC,src);
  BOOL ok = BitBlt(prDC,0,0,getWidth(),getHeight(),bmDC,0,0,SRCCOPY);
  if(!ok) {
    errMsg = getLastErrorText();
  }
  SelectObject(bmDC,gdi1);
  DeleteDC(bmDC);
  releaseDC(prDC);
  if(!ok) {
    throwException(_T("%s(HBITMAP) failed:%s"), __TFUNCTION__, errMsg.cstr());
  }
  return *this;
*/
}

D3DFORMAT PixRect::getPixelFormat(HBITMAP bm) { // static
  const BITMAP info = getBitmapInfo(bm);
  switch(info.bmBitsPixel) {
  case 32:
    return D3DFMT_X8B8G8R8;
  default:
    throwException(_T("%s(HBITMAP):BITMAP.bmBitsPixel=%d. Must be 32"), __TFUNCTION__, info.bmBitsPixel);
    return D3DFMT_X8B8G8R8;
  }
}

PixRect::operator HBITMAP() const {
  String errMsg;
  HBITMAP result = CreateBitmap( getWidth(), getHeight(), 1, 32, NULL);

  HDC     prDC = getDC();
  HDC     bmDC = CreateCompatibleDC(prDC);
  HGDIOBJ gdi1 = SelectObject(bmDC,result);
  BOOL    ok   = BitBlt(bmDC,0,0,getWidth(),getHeight(),prDC,0,0,SRCCOPY);
  if(!ok) {
    errMsg = getLastErrorText();
  }
  DeleteDC(bmDC);
  releaseDC(prDC);
  if(!ok) {
    throwException(_T("%s failed:%s"), __TFUNCTION__, errMsg.cstr());
  }

  return result;
}

void PixRect::fromBitmap(CBitmap &src) {
  String errMsg;
  const CSize   bmSize = getBitmapSize(src);
  const CSize   prSize = getSize();

  if(bmSize != prSize) {
    const D3DFORMAT pixelFormat = getPixelFormat();
    const D3DPOOL   pool        = getPool();
    destroy();
    createPlainSurface(bmSize, pixelFormat, pool);
  }

  HDC     prDC = getDC();
  HDC     bmDC = CreateCompatibleDC(prDC);
  HGDIOBJ gdi1 = SelectObject(bmDC,(HBITMAP)src);
  BOOL    ok   = BitBlt(prDC,0,0,bmSize.cx,bmSize.cy,bmDC,0,0,SRCCOPY);
  if(!ok) {
    errMsg = getLastErrorText();
  }
  SelectObject(bmDC, gdi1);
  DeleteDC(bmDC);
  releaseDC(prDC);
  if(!ok) {
    throwException(_T("%s failed:%s"), __TFUNCTION__, errMsg.cstr());
  }
}

void PixRect::toBitmap(CBitmap &dst) const {
  String errMsg;
  if(dst.m_hObject == NULL) {
    dst.CreateBitmap(getWidth(),getHeight(),1,32,NULL);
  } else {
    const CSize bmSize = getBitmapSize(dst);
    const CSize prSize = getSize();

    if(bmSize != prSize) {
      dst.DeleteObject();
      dst.CreateBitmap(prSize.cx, prSize.cy, 1, 32, NULL);
    }
  }
  HDC     prDC = getDC();
  HDC     bmDC = CreateCompatibleDC(prDC);
  HGDIOBJ gdi1 = SelectObject(bmDC,(HBITMAP)dst);
  BOOL    ok   = BitBlt(bmDC,0,0,getWidth(),getHeight(),prDC,0,0,SRCCOPY);
  if(!ok) {
    errMsg = getLastErrorText();
  }
  SelectObject(bmDC, gdi1);

  DeleteDC(bmDC);
  releaseDC(prDC);
  if(!ok) {
    throwException(_T("%s failed:%s"), __TFUNCTION__, errMsg.cstr());
  }
}

HDC PixRect::getDC() const {
  assert(m_DCSurface == NULL);
  HDC dc;
  m_DCSurface = getSurface();
  CHECK3DRESULT(m_DCSurface->GetDC(&dc));
  return dc;
}

void PixRect::releaseDC(HDC dc) const {
  assert(m_DCSurface != NULL);
  m_DCSurface->ReleaseDC(dc);
  m_device.releaseSurface(m_DCSurface, m_type);
  m_DCSurface = NULL;
}

LPDIRECT3DSURFACE PixRect::getSurface() const {
  LPDIRECT3DSURFACE surface = NULL;
  switch (getType()) {
  case PIXRECT_TEXTURE:
    CHECK3DRESULT(m_texture->GetSurfaceLevel(0, &surface));
    break;
  case PIXRECT_RENDERTARGET:
  case PIXRECT_PLAINSURFACE:
    surface = m_surface;
    surface->AddRef();
    break;
  default:
    unknownTypeError(__TFUNCTION__);
  }
  return surface;
}

D3DLOCKED_RECT PixRect::lockRect(DWORD flags, const CRect *rect) {
  D3DLOCKED_RECT lr;
  switch (getType()) {
  case PIXRECT_TEXTURE:
    CHECK3DRESULT(m_texture->LockRect(0, &lr, rect, flags));
    break;
  case PIXRECT_RENDERTARGET:
  case PIXRECT_PLAINSURFACE:
    CHECK3DRESULT(m_surface->LockRect(&lr, rect, flags));
    break;
  default:
    unknownTypeError(__TFUNCTION__);
    break;
  }
  return lr;
}

void PixRect::unlockRect() {
  switch (getType()) {
  case PIXRECT_TEXTURE:
    CHECK3DRESULT(m_texture->UnlockRect(0));
    break;
  case PIXRECT_RENDERTARGET:
  case PIXRECT_PLAINSURFACE:
    CHECK3DRESULT(m_surface->UnlockRect());
    break;
  default:
    unknownTypeError(__TFUNCTION__);
  }
}

void PixRect::unknownTypeError(const TCHAR *method) const {
  unknownTypeError(method, getType());
}

/*static*/ void PixRect::unknownTypeError(const TCHAR *method, PixRectType type) {
  throwException(_T("%s:Unknown type:%d"), method, type);
}


bool PixRect::contains(const Point2D &p) const {
  return p.x >= 0 && p.x < m_desc.Width && p.y >= 0 && p.y < m_desc.Height;
}

bool PixRect::containsExtended(const Point2D &p) const {
  return p.x > -1 && p.x < m_desc.Width && p.y > -1 && p.y < m_desc.Height;
}

bool operator==(const D3DSURFACE_DESC &d1, const D3DSURFACE_DESC &d2) {
  return d1.Width  == d2.Width
      && d1.Height == d2.Height
      && d1.Format == d2.Format;
}

bool operator!=(const D3DSURFACE_DESC &d1, const D3DSURFACE_DESC &d2) {
  return !(d1 == d2);
}

bool operator==(const PixRect &p1, const PixRect &p2) {
  if(&p1 == &p2) {
    return true;
  }
  if(p1.m_desc != p2.m_desc) {
    return false;
  }
  PixelAccessor *pa1 = PixelAccessor::createPixelAccessor(&(PixRect&)p1);
  PixelAccessor *pa2 = PixelAccessor::createPixelAccessor(&(PixRect&)p2);
  const int width  = p1.getWidth();
  const int height = p1.getHeight();
  bool equals = true;
  CPoint pt;
  for(pt.x = 0; pt.x < width; pt.x++) {
    for(pt.y = 0; pt.y < height; pt.y++) {
      if(pa1->getPixel(pt) != pa2->getPixel(pt)) {
        equals = false;
        goto done;
      }
    }
  }
done:
  delete pa1;
  delete pa2;
  return equals;
}

bool operator!=(const PixRect &p1, const PixRect &p2) {
  return !(p1 == p2);
}

void PixRect::setPixel(const CPoint &p, D3DCOLOR color) {
  setPixel(p.x,p.y,color);
}

D3DCOLOR PixRect::getPixel(const CPoint &p) const {
  return getPixel(p.x,p.y);
}

void PixRect::setPixel(UINT x, UINT y, D3DCOLOR color) {
  if(!contains(x,y)) {
    return;
  }
  PixelAccessor *pixelAccessor = getPixelAccessor();
  pixelAccessor->setPixel(x,y,color);
  delete pixelAccessor;
}

D3DCOLOR PixRect::getPixel(UINT x, UINT y) const {
  if(!contains(x,y)) {
    return 0;
  }
  PixelAccessor *pixelAccessor = PixelAccessor::createPixelAccessor((PixRect*)this);
  D3DCOLOR result = pixelAccessor->getPixel(x,y);
  delete pixelAccessor;
  return result;
}

void PixRect::rop( const CRect  &dr, ULONG op, const PixRect *src, const CPoint &sp) {
  rop(dr.left,dr.top,dr.Width(),dr.Height(), op, src,sp.x,sp.y);
}

void PixRect::rop( const CPoint &dp, const CSize &size, ULONG op, const PixRect *src, const CPoint &sp) {
  rop(dp.x,dp.y,size.cx,size.cy, op, src,sp.x,sp.y);
}

void PixRect::rop(int x, int y, int w, int h, ULONG op, const PixRect *src, int sx, int sy) {
  String errorMsg;

  HDC srcDC = src ? src->getDC() : NULL;
  HDC dstDC = getDC();

  BOOL ok = BitBlt(dstDC,x,y,w,h,srcDC,sx,sy,op);
  if(!ok) {
    errorMsg = getLastErrorText();
  }
  if(src != NULL) {
    src->releaseDC(srcDC);
  }
  releaseDC(dstDC);
  if(!ok) {
    throwException(_T("PixRect::rop failed:%s"), errorMsg.cstr());
  }
}

void PixRect::rop(const CRect &dr, ULONG op, const PixRect *src, const CRect &sr) {
  HDC dstDC = getDC();
  try {
    const CSize dsz = dr.Size(), ssz = sr.Size();
    if(ssz == dsz) {
      bitBlt(dstDC, dr.left, dr.top, dsz.cx,dsz.cy, op, src, sr.left,sr.top);
    } else {
      stretchBlt(dstDC, dr, op, src, sr);
    }
    releaseDC(dstDC);
  }
  catch (...) {
    releaseDC(dstDC);
    throw;
  }
}

void PixRect::bitBlt(PixRect *dst, int x, int y, int w, int h, ULONG op, HDC src, int sx, int sy) { // static
  String errorMsg;

  HDC dstDC = dst->getDC();

  BOOL ok = BitBlt(dstDC,x,y,w,h,src,sx,sy,op);
  if(!ok) {
    errorMsg = getLastErrorText();
  }
  dst->releaseDC(dstDC);
  if(!ok) {
    throwException(_T("%s failed:%s"), __TFUNCTION__, errorMsg.cstr());
  }
}

void PixRect::bitBlt(PixRect *dst, const CPoint &dp, const CSize &ds, ULONG op, HDC src, const CPoint &sp) { // static
  bitBlt(dst, dp.x,dp.y,ds.cx,ds.cy, op, src, sp.x,sp.y);
}

void PixRect::bitBlt(PixRect *dst, const CRect &dr, ULONG op, HDC src, const CPoint &sp) {
  bitBlt(dst, dr.left,dr.top,dr.Width(),dr.Height(), op, src, sp.x,sp.y);
}

void PixRect::bitBlt(HDC dst, int x, int y, int w, int h, ULONG op, const PixRect *src, int sx, int sy) { // static
  String errorMsg;

  HDC srcDC = src ? src->getDC() : NULL;
  BOOL ok = BitBlt(dst,x,y,w,h,srcDC,sx,sy,op);
  if(!ok) {
    errorMsg = getLastErrorText();
  }
  if(src != NULL) {
    src->releaseDC(srcDC);
  }
  if(!ok) {
    throwException(_T("%s failed:%s"), __TFUNCTION__, errorMsg.cstr());
  }
}

void PixRect::bitBlt(HDC dst, const CPoint &dp, const CSize &ds, ULONG op, const PixRect *src, const CPoint &sp) { // static
  bitBlt(dst, dp.x,dp.y,ds.cx,ds.cy, op, src, sp.x,sp.y);
}

void PixRect::bitBlt(HDC dst, const CRect &dr, ULONG op, const PixRect *src, const CPoint &sp) { // static
  bitBlt(dst, dr.left,dr.top,dr.Width(),dr.Height(), op, src, sp.x,sp.y);
}

void PixRect::stretchBlt(HDC dst, int x, int y, int w, int h, ULONG op, const PixRect *src, int sx, int sy, int sw, int sh) { // static
  String errorMsg;

  HDC  srcDC = src ? src->getDC() : NULL;
  BOOL ok    = StretchBlt(dst,x,y,w,h,srcDC,sx,sy,sw,sh,op);
  if(!ok) {
    errorMsg = getLastErrorText();
  }
  if(src) {
    src->releaseDC(srcDC);
  }
  if(!ok) {
    throwException(_T("%s failed:%s"), __TFUNCTION__, errorMsg.cstr());
  }
}

void PixRect::stretchBlt(PixRect *dst, int x, int y, int w, int h, ULONG op, const HDC src, int sx, int sy, int sw, int sh) { // static
  String errorMsg;

  HDC  dstDC = dst->getDC();
  BOOL ok    = StretchBlt(dstDC,x,y,w,h,src,sx,sy,sw,sh,op);
  if(!ok) {
    errorMsg = getLastErrorText();
  }
  dst->releaseDC(dstDC);
  if(!ok) {
    throwException(_T("%s failed:%s"), __TFUNCTION__, errorMsg.cstr());
  }
}

void PixRect::stretchBlt(HDC dst, const CPoint &dp, const CSize &ds, ULONG op, const PixRect *src, const CPoint &sp, const CSize &ss) { // static
  stretchBlt(dst, dp.x,dp.y,ds.cx,ds.cy, op, src, sp.x,sp.y,ss.cx,ss.cy);
}

void PixRect::stretchBlt(PixRect *dst, const CPoint &dp, const CSize &ds, ULONG op, const HDC src, const CPoint &sp, const CSize &ss) { // static
  stretchBlt(dst, dp.x,dp.y,ds.cx,ds.cy, op, src, sp.x,sp.y,ss.cx,ss.cy);
}

void PixRect::stretchBlt(HDC dst, const CRect &dr, ULONG op, const PixRect *src, const CRect &sr) { // static
  stretchBlt(dst, dr.left,dr.top,dr.Width(),dr.Height(), op, src, sr.left,sr.top,sr.Width(),sr.Height());
}

void PixRect::stretchBlt(PixRect *dst, const CRect &dr, ULONG op, const HDC src, const CRect &sr) { // static
  stretchBlt(dst, dr.left,dr.top,dr.Width(),dr.Height(), op, src, sr.left,sr.top,sr.Width(),sr.Height());
}

void PixRect::mask(const CRect  &dr, ULONG op, const PixRect *src, const CPoint &sp, const PixRect *prMask) {
  mask(dr.left,dr.top,dr.Width(),dr.Height(), op, src,sp.x,sp.y, prMask);
}

void PixRect::mask(const CPoint &dp, const CSize &size, ULONG op, const PixRect *src, const CPoint &sp, const PixRect *prMask) {
  mask(dp.x,dp.y,size.cx,size.cy, op, src, sp.x,sp.y, prMask);
}

// Performs rasteropreation op on this using src, for the pixels which are non-zero in prMask
void PixRect::mask(int x, int y, int w, int h, ULONG op, const PixRect *src, int sx, int sy, const PixRect *prMask) {
  String errorMsg;

  HDC srcDC  = src ? src->getDC() : NULL;
  HDC dstDC  = getDC();
  HDC maskDC = prMask->getDC();

  HBITMAP maskBitmap = CreateBitmap(prMask->getWidth(),prMask->getHeight(),1,1,NULL);
  HDC bmDC = CreateCompatibleDC(maskDC);
  SelectObject(bmDC,maskBitmap);

  BOOL ok = BitBlt(bmDC,0,0,prMask->getWidth(),prMask->getHeight(),maskDC,0,0,SRCCOPY);
  if(!ok) {
    errorMsg = getLastErrorText();
  }

//  BitBlt(dstDC,x,y,w,h,maskDC,0,0,SRCCOPY);

  if(ok) {
    BitBlt(dstDC,x,y,w,h,NULL,0,0,DSTINVERT); // this is only necessary, because windows has no DSTCOPY-rasteroperation to use for the
                                              // background-operation (used for 0-bits in maskBitmap), so we have to invert dst twice. #!&$?
    ok = MaskBlt(dstDC,x,y,w,h,srcDC,sx,sy,maskBitmap,0,0,MAKEROP4(op,DSTINVERT));
    if(!ok) {
      errorMsg = getLastErrorText();
    }
  }

  if(src != NULL) {
    src->releaseDC(srcDC);
  }
  releaseDC(dstDC);
  prMask->releaseDC(maskDC);
  DeleteDC(bmDC);
  DeleteObject(maskBitmap);

  if(!ok) {
    throwException(_T("PixRect::mask failed:%s"), errorMsg.cstr());
  }
}

/*
PixRectClipper::PixRectClipper(HWND hwnd) {
  CHECK3DRESULT(PixRect::directDraw->CreateClipper(0, &m_clipper, NULL));
  CHECK3DRESULT(m_clipper->SetHWnd(0, hwnd));
}

PixRectClipper::~PixRectClipper() {
  m_clipper->Release();
}

void PixRect::setClipper(PixRectClipper *clipper) {
  if(clipper != NULL) {
    CHECK3DRESULT(m_surface->SetClipper(clipper->m_clipper));
  } else {
    CHECK3DRESULT(m_surface->SetClipper(NULL));
  }
}
*/
void PixRect::copy(VIDEOHDR &videoHeader) {
  D3DLOCKED_RECT lr;
  CHECK3DRESULT(m_surface->LockRect(&lr, NULL, D3DLOCK_NOSYSLOCK));
  memcpy(lr.pBits, videoHeader.lpData, videoHeader.dwBytesUsed);
  CHECK3DRESULT(m_surface->UnlockRect());
}

void PixRect::formatConversion(const PixRect &pr) {
  CHECKRESULT(m_device.getD3Device()->StretchRect(pr.m_surface, NULL, m_surface, NULL, D3DTEXF_NONE));
}


PixRect *PixRect::mirror(const PixRect *src, bool vertical) { // static
  PixRect *result = src->clone();
  const int width  = result->getWidth();
  const int height = result->getHeight();
  PixelAccessor *srcPA = PixelAccessor::createPixelAccessor((PixRect*)src);
  PixelAccessor *dstPA = result->getPixelAccessor();
  if(vertical) {
    for(int x1 = 0, x2=width-1; x1 < width; x1++, x2--) {
      for(int y = 0; y < height; y++) {
        dstPA->setPixel(x1,y,srcPA->getPixel(x2,y));
      }
    }
  } else {
    for(int y1 = 0, y2=height-1; y1 < height; y1++, y2--) {
      for(int x = 0; x < width; x++) {
        dstPA->setPixel(x,y1,srcPA->getPixel(x,y2));
      }
    }
  }
  delete srcPA;
  delete dstPA;
  return result;
}

PixRectTextMaker::PixRectTextMaker(PixRect *pixRect, const Point2D &textPos, D3DCOLOR color, bool invert) {
  m_pixRect = pixRect;
  m_textPos = textPos;
  m_color   = color;
  m_invert  = invert;
}

void PixRectTextMaker::line(const Point2D &from, const Point2D &to) {
//  m_pixRect->line(m_glyphPos%from,m_glyphPos%to,m_color,m_invert);
}

void PixRectTextMaker::beginGlyph(const Point2D &offset) {
  m_glyphPos = m_textPos + offset;
}
