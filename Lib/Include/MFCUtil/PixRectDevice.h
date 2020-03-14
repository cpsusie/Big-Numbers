#pragma once

#include <BasicIncludes.h>
#include <afxwin.h>         // MFC core and standard components
#include "PixRectType.h"
#include "DirectXDeviceFactory.h"

class PixRect;

class PixRectDevice {
private:
  LPDIRECT3DDEVICE   m_device;
  LPDIRECT3DSURFACE  m_renderTarget;
  CSize              m_renderTargetSize;
  D3DFORMAT          m_defaultPixelFormat; // same format as the screen
  float              m_appScaleX, m_appScaleY;
  mutable bool       m_exceptionInProgress;

  LPDIRECT3DSURFACE getRenderTarget();
  void releaseRenderTarget();
  void setRenderTargetSize(const CSize &size);
public:
  PixRectDevice();
  ~PixRectDevice();
  void attach(HWND hwnd, bool windowed = true, const CSize *size = NULL);
  void detach();
  void beginScene();
  void endScene();
  void render(const PixRect *pr);
  LPDIRECT3DTEXTURE createTexture(const CSize &size, D3DFORMAT format, D3DPOOL pool);
  LPDIRECT3DSURFACE createRenderTarget(const CSize &size, D3DFORMAT format = D3DFMT_FORCE_DWORD, bool lockable = false); // always in D3DPOOL_DEFAULT
  LPDIRECT3DSURFACE createOffscreenPlainSurface(const CSize &size, D3DFORMAT format, D3DPOOL pool);
  inline void releaseTexture(LPDIRECT3DTEXTURE &texture) {
    SAFERELEASE(texture);
  }
  inline void releaseSurface(LPDIRECT3DSURFACE &surface, PixRectType type) {
    SAFERELEASE(surface)
  }

  inline D3DFORMAT getDefaultPixelFormat() const {
    return m_defaultPixelFormat;
  }
  LPDIRECT3DDEVICE &getD3Device() {
    return m_device;
  }
  bool supportFormatConversion(D3DFORMAT srcFormat, D3DFORMAT dstFormat, UINT adapter = D3DADAPTER_DEFAULT) const;
  void setWorldMatrix(const D3DXMATRIX &m);
  D3DXMATRIX &getWorldMatrix(D3DXMATRIX &m) const;
  void set2DTransform(const CSize &size);
  static CompactArray<D3DDISPLAYMODE> getDisplayModes(UINT adapter = D3DADAPTER_DEFAULT);
  D3DCAPS getDeviceCaps() const;
  inline void resetException() {
    m_exceptionInProgress = false;
  }
  void alphaBlend(const PixRect *texture, const CRect &dstRect);
  void draw(const PixRect *pr, const CRect &dstRect);
};
