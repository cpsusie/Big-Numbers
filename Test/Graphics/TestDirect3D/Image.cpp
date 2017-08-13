#include "stdafx.h"
#include <MFCUtil/picture.h>
#include "Image.h"

#pragma comment(lib,"d3d9.lib")

IDirect3D9            *Image::m_direct3D9;
IDirect3DDevice9      *Image::m_device;
D3DPRESENT_PARAMETERS  Image::m_presentParameters;
D3DCAPS9               Image::m_caps;

void Image::init(HWND wnd) { // static
  m_direct3D9 = Direct3DCreate9(D3D_SDK_VERSION);

  if(m_direct3D9 == NULL) {
    throwException("Cannot initialize Direct3D9");
  }

  CHECKD3DRESULT(m_direct3D9->GetDeviceCaps(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL, &m_caps));

  D3DDEVTYPE deviceType = D3DDEVTYPE_HAL;

   if((m_caps.TextureOpCaps & D3DTOP_DOTPRODUCT3) == 0) {
     showWarning(_T("Current Direct3D driver does not support the "
                      "D3DTOP_DOTPRODUCT3 texture-blending operation! \n\nSwitching to reference rasterizer!"));
     deviceType = D3DDEVTYPE_REF;
  }


  D3DDISPLAYMODE mode;

  m_direct3D9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode);

  CSize sz = getClientRect(wnd).Size();

  D3DPRESENT_PARAMETERS param;
  ZeroMemory(&param, sizeof(param));

  param.Windowed               = TRUE;
  param.MultiSampleType        = D3DMULTISAMPLE_NONE;
  param.SwapEffect             = D3DSWAPEFFECT_DISCARD;
  param.BackBufferFormat       = D3DFMT_A8R8G8B8; // mode.Format;
  param.EnableAutoDepthStencil = FALSE;
  param.PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;
  param.hDeviceWindow          = wnd;
  param.Flags                  = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
  param.BackBufferCount        = 1;
  param.BackBufferWidth        = sz.cx;
  param.BackBufferHeight       = sz.cy;
  param.PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;

  CHECKD3DRESULT(m_direct3D9->CreateDevice(D3DADAPTER_DEFAULT
                                          ,deviceType
                                          ,wnd
                                          ,D3DCREATE_MIXED_VERTEXPROCESSING
                                         | D3DCREATE_FPU_PRESERVE
                                         | D3DCREATE_MULTITHREADED
                                          ,&param
                                          ,&m_device));
  CHECKD3DRESULT(m_device->Reset(&param));
  CHECKD3DRESULT(m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE  ));
  CHECKD3DRESULT(m_device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE   ));
  CHECKD3DRESULT(m_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID ));

  m_device->SetRenderState(D3DRS_LIGHTING, FALSE);

  m_presentParameters = param;
}

void Image::beginScene() { // static
  CHECKD3DRESULT(m_device->BeginScene());
}

void Image::endScene() { // static
  CHECKD3DRESULT(m_device->EndScene());
}

void Image::render() {
/*
  m_device->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER
                    , D3DCOLOR_COLORVALUE(0.35f, 0.53f, 0.7, 1.0f), 1.0f, 0 );
*/

  IDirect3DSurface9 *renderTarget;
  CHECKD3DRESULT(m_device->GetRenderTarget(0, &renderTarget));
  CHECKD3DRESULT(m_device->StretchRect(m_surface, NULL, renderTarget, NULL, D3DTEXF_NONE));
  CHECKD3DRESULT(m_device->Present(NULL, NULL, NULL, NULL));
  renderTarget->Release();
}

Image::Image(const CSize &size, bool renderTarget) {
  m_size                 = size;
  if(renderTarget) {
    m_surface            = createRenderTarget(size, D3DFMT_A8R8G8B8);
  } else {
    m_surface            = createSurface(m_size, D3DFMT_A8R8G8B8);
  }
  m_hasTransparentPixels = false;
}

Image::Image(int resId, ImageType type, bool transparentWhite) {
  m_surface = NULL;
  m_texture = NULL;
  switch(type) {
  case RESOURCE_BITMAP:
    loadBMP(resId);
    break;
  case RESOURCE_JPEG  :
    loadJPG(resId);
    break;
  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("type=%d, resId=%d"), type, resId);
  }
  if(transparentWhite) {
    makeWhiteTransparent();
  } else {
    makeOpaque();
  }
}

Image::~Image() {
  if(m_surface) {
    m_surface->Release();
  }
  if(m_texture) {
    m_texture->Release();
  }
}

void Image::loadBMP(int resId) {
  CBitmap cbm;
  if(!cbm.LoadBitmap(resId)) {
    throwException(_T("LoadBitmap(%d) failed"), resId);
  }
  m_size    = getBitmapSize(cbm);
  m_surface = createSurface(m_size, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM);

  HDC srcDC = CreateCompatibleDC(NULL);
  HBITMAP bm = cbm;
  HGDIOBJ oldObj = SelectObject(srcDC, bm);
  HDC dstDC = getDC();
  BitBlt(dstDC, 0, 0, m_size.cx, m_size.cy, srcDC, 0,0, SRCCOPY);
  releaseDC(dstDC);
  DeleteDC(srcDC);
}

void Image::loadJPG(int resId) {
  CPicture picture;
  picture.loadFromResource(resId, _T("JPG"));
  m_size    = picture.getSize();
  m_surface = createSurface(m_size, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM);
  HDC dc = getDC();
  picture.show(dc, CRect(0,0,m_size.cx,m_size.cy));
  releaseDC(dc);
}

IDirect3DSurface9 *Image::createSurface(const CSize &size, D3DFORMAT format, D3DPOOL pool) { // static
  IDirect3DSurface9 *surface;
  CHECKD3DRESULT(m_device->CreateOffscreenPlainSurface(size.cx, size.cy, format, pool, &surface, NULL));
  return surface;
}

IDirect3DSurface9 *Image::createRenderTarget(const CSize &size, D3DFORMAT format) { // static
  IDirect3DSurface9 *surface;

  IDirect3DSurface9 *oldRenderTarget;
  CHECKD3DRESULT(m_device->GetRenderTarget(0, &oldRenderTarget));
  D3DSURFACE_DESC desc;
  CHECKD3DRESULT(oldRenderTarget->GetDesc(&desc));
  oldRenderTarget->Release();

  CHECKD3DRESULT(m_device->CreateRenderTarget(size.cx, size.cy, format, desc.MultiSampleType, desc.MultiSampleQuality, false, &surface, NULL));
//  D3DSURFACE_DESC newDesc;
//  CHECKD3DRESULT(surface->GetDesc(&newDesc));

  return surface;
}

IDirect3DTexture9 *Image::createTexture(const CSize &size, D3DFORMAT format) { // static
  IDirect3DTexture9 *texture;

// Create an alpha texture
  CHECKD3DRESULT(m_device->CreateTexture(size.cx, size.cy, 1, D3DUSAGE_AUTOGENMIPMAP, format, D3DPOOL_DEFAULT, &texture, NULL));
  int levels = texture->GetLevelCount();
  D3DSURFACE_DESC desc;
  CHECKD3DRESULT(texture->GetLevelDesc(0, &desc));
  return texture;
}


HDC Image::getDC() const {
  HDC dc;
  CHECKD3DRESULT(m_surface->GetDC(&dc));
  return dc;
}

void Image::releaseDC(HDC dc) const {
  CHECKD3DRESULT(m_surface->ReleaseDC(dc));
}

void Image::makeWhiteTransparent() {
  D3DLOCKED_RECT lockedRect;

  CHECKD3DRESULT(m_surface->LockRect(&lockedRect, NULL, D3DLOCK_NOOVERWRITE/*D3DLOCK_DISCARD*/));
  D3DCOLOR *pixelRow = (D3DCOLOR*)lockedRect.pBits;
  const int PixelPerRow = lockedRect.Pitch / sizeof(D3DCOLOR);
  for(int y = 0; y < m_size.cy; y++, pixelRow += PixelPerRow) {
    D3DCOLOR *pixel = pixelRow;
    for(int x = 0; x < m_size.cx; x++, pixel++) {
      if((*pixel & 0xffffff) == 0xffffff) {
        *pixel &= 0x00ff0000;
      }
    }
  }
  CHECKD3DRESULT(m_surface->UnlockRect());
  m_hasTransparentPixels = true;
}

void Image::makeOpaque() {
  D3DLOCKED_RECT lockedRect;

  CHECKD3DRESULT(m_surface->LockRect(&lockedRect, NULL, D3DLOCK_DISCARD));
  D3DCOLOR *pixelRow = (D3DCOLOR*)lockedRect.pBits;
  const int PixelPerRow = lockedRect.Pitch / sizeof(D3DCOLOR);
  for(int y = 0; y < m_size.cy; y++, pixelRow += PixelPerRow) {
    D3DCOLOR *pixel = pixelRow;
    for(int x = 0; x < m_size.cx; x++, pixel++) {
      *pixel |= 0xff000000;
    }
  }
  CHECKD3DRESULT(m_surface->UnlockRect());
  m_hasTransparentPixels = false;
}

#define DOALPHABLEND

void Image::paint(Image &dst, const CPoint &dstp, double rotation) const {
  if(rotation == 0) {
    CRect r(ORIGIN, m_size);
    if(m_hasTransparentPixels) {
#ifdef DOALPHABLEND
      IDirect3DTexture9 *texture = createTexture(m_size, D3DFMT_A8R8G8B8);
      copySurfaceToTexture(texture, m_surface);

      IDirect3DSurface9 *oldRenderTarget;
      CHECKD3DRESULT(m_device->GetRenderTarget(0, &oldRenderTarget));

      CHECKD3DRESULT(m_device->SetRenderTarget(0, dst.m_surface));

      alphaBlend(texture, 0,0,m_size.cx, m_size.cy);

      CHECKD3DRESULT(m_device->SetRenderTarget(0, oldRenderTarget));
      oldRenderTarget->Release();
      texture->Release();
#else

      IDirect3DSurface9 *oldRenderTarget;
      CHECKD3DRESULT(m_device->GetRenderTarget(0, &oldRenderTarget));
      CHECKD3DRESULT(m_device->SetRenderTarget(0, dst.m_surface));

      drawLineList();

      CHECKD3DRESULT(m_device->SetRenderTarget(0, oldRenderTarget));
      oldRenderTarget->Release();

#endif

// CHECKD3DRESULT(m_device->UpdateSurface(m_surface, &r, dst.m_surface , &dstp));
    } else {
      CHECKD3DRESULT(m_device->UpdateSurface(m_surface, &r, dst.m_surface , &dstp));
    }
  } else {
  }
}

void Image::paintRotated(Image &dst, const CPoint &dstp, double rotation) const {
}

void Image::paintRectangle(Image &dst, const CRect &rect) const {
}


void Image::copySurfaceToTexture(IDirect3DTexture9 *dst, IDirect3DSurface9 *src) { // static
  IDirect3DSurface9 *dstSurface;
  CHECKD3DRESULT(dst->GetSurfaceLevel(0, &dstSurface));

  CHECKD3DRESULT(m_device->UpdateSurface(src, NULL, dstSurface, NULL));
  dstSurface->Release();

//  D3DLOCKED_RECT srcRect;
//  D3DLOCKED_RECT dstRect;
//  CHECKD3DRESULT(src->LockRect(   &srcRect, NULL, D3DLOCK_READONLY));
//  CHECKD3DRESULT(dst->LockRect(0, &dstRect, NULL, D3DLOCK_NOOVERWRITE));

//  CHECKD3DRESULT(dst->UnlockRect(0));
//  CHECKD3DRESULT(src->UnlockRect());
}

#define SetTextureColorStage( dev, i, arg1, op, arg2 )                      \
    CHECKD3DRESULT(dev->SetTextureStageState( i, D3DTSS_COLOROP  , op   )); \
    CHECKD3DRESULT(dev->SetTextureStageState( i, D3DTSS_COLORARG1, arg1 )); \
    CHECKD3DRESULT(dev->SetTextureStageState( i, D3DTSS_COLORARG2, arg2 ));

#define SetTextureAlphaStage( dev, i, arg1, op, arg2 )                      \
    CHECKD3DRESULT(dev->SetTextureStageState( i, D3DTSS_ALPHAOP  , op   )); \
    CHECKD3DRESULT(dev->SetTextureStageState( i, D3DTSS_ALPHAARG1, arg1 )); \
    CHECKD3DRESULT(dev->SetTextureStageState( i, D3DTSS_ALPHAARG2, arg2 ));


void Image::alphaBlend(IDirect3DTexture9 *texture, int x, int y, int w, int h) { // static

  typedef struct {
    float x,y, z;
  } CUSTOMVERTEX;
  CHECKD3DRESULT(m_device->SetFVF(D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE2(0))); // D3DFVF_XYZ | D3DFVF_DIFFUSE ));
  CHECKD3DRESULT(m_device->SetRenderState(D3DRS_BLENDOP  , D3DBLENDOP_ADD      ));
  CHECKD3DRESULT(m_device->SetRenderState(D3DRS_SRCBLEND , D3DBLEND_INVSRCCOLOR)); // D3DBLEND_SRCALPHA   ));
  CHECKD3DRESULT(m_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO )); // D3DBLEND_INVSRCALPHA));

  CHECKD3DRESULT(m_device->SetTexture(0, texture));
//  CHECKD3DRESULT(m_device->SetTextureStageState(0, D3DTSS_CONSTANT, D3DCOLOR_ARGB(0,255,0,0)));
  SetTextureColorStage(m_device, 0, D3DTA_TEXTURE, D3DTOP_MODULATE , D3DTA_DIFFUSE);
  SetTextureAlphaStage(m_device, 0, D3DTA_TEXTURE, D3DTOP_MODULATE , D3DTA_DIFFUSE);

//  CHECKD3DRESULT(m_device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_BLENDTEXTUREALPHA));
//  CHECKD3DRESULT(m_device->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION | 1));
#define CV(x,y,z) (float)(x),(float)(y),(float)(z)

CUSTOMVERTEX vertices[] = {
    CV(x  , y  ,0)
   ,CV(x+w, y  ,0)
   ,CV(x+w, y+h,0)
   ,CV(x,   y+h,0)
  };

  CHECKD3DRESULT(m_device->DrawPrimitiveUP(D3DPT_LINELIST/*D3DPT_TRIANGLESTRIP*/, 4, vertices, sizeof(vertices[0])));
}

void Image::paintSolidRect(const CRect &rect, D3DCOLOR color) {
  CHECKD3DRESULT(m_device->ColorFill(m_surface, &rect, color));
}


void Image::drawLineList() const {
  struct CUSTOMVERTEX {
    float x,y, z;
  };

CUSTOMVERTEX vertices[] = {
  {-5.0, -5.0, 0.0},
  { 0.0,  5.0, 0.0},
  { 5.0, -5.0, 0.0},
  {10.0,  5.0, 0.0},
  {15.0, -5.0, 0.0},
  {20.0,  5.0, 0.0}
  };

  CHECKD3DRESULT(m_device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE )); // D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE2(0)));
  CHECKD3DRESULT(m_device->DrawPrimitiveUP(D3DPT_LINELIST, 4, vertices, sizeof(vertices[0])));
}

String getD3DErrorMsg(HRESULT hr) {

#define casepr(v) case v: return #v

  switch(hr) {
  casepr(D3DERR_WRONGTEXTUREFORMAT               );
  casepr(D3DERR_UNSUPPORTEDCOLOROPERATION        );
  casepr(D3DERR_UNSUPPORTEDCOLORARG              );
  casepr(D3DERR_UNSUPPORTEDALPHAOPERATION        );
  casepr(D3DERR_UNSUPPORTEDALPHAARG              );
  casepr(D3DERR_TOOMANYOPERATIONS                );
  casepr(D3DERR_CONFLICTINGTEXTUREFILTER         );
  casepr(D3DERR_UNSUPPORTEDFACTORVALUE           );
  casepr(D3DERR_CONFLICTINGRENDERSTATE           );
  casepr(D3DERR_UNSUPPORTEDTEXTUREFILTER         );
  casepr(D3DERR_CONFLICTINGTEXTUREPALETTE        );
  casepr(D3DERR_DRIVERINTERNALERROR              );
  casepr(D3DERR_NOTFOUND                         );
  casepr(D3DERR_MOREDATA                         );
  casepr(D3DERR_DEVICELOST                       );
  casepr(D3DERR_DEVICENOTRESET                   );
  casepr(D3DERR_NOTAVAILABLE                     );
  casepr(D3DERR_OUTOFVIDEOMEMORY                 );
  casepr(D3DERR_INVALIDDEVICE                    );
  casepr(D3DERR_INVALIDCALL                      );
  casepr(D3DERR_DRIVERINVALIDCALL                );
  casepr(D3DERR_WASSTILLDRAWING                  );
  casepr(D3DERR_DEVICEREMOVED                    );
  casepr(D3DOK_NOAUTOGEN                         );
  casepr(S_NOT_RESIDENT                          );
  casepr(S_RESIDENT_IN_SHARED_MEMORY             );
  casepr(S_PRESENT_MODE_CHANGED                  );
  casepr(S_PRESENT_OCCLUDED                      );
  casepr(D3DERR_DEVICEHUNG                       );
  default: return format(_T("Unknown D3D-error:%x"), hr);
  }
}

#ifdef _DEBUG

void checkD3DResult(TCHAR *fileName, int line, HRESULT hr) {
  if(hr != D3D_OK) {
    showWarning(_T("D3D-error %s in %s, line %d"), getD3DErrorMsg(hr).cstr(), fileName, line);
    return;
  }
}

#else

void checkD3DResult(HRESULT hr) {
  if(hr != D3D_OK) {
    showWarning(_T("D3D-error %s"), getD3DErrorMsg(hr).cstr());
    return;
  }
}

#endif
