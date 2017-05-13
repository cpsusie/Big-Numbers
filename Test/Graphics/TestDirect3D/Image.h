#pragma once

#include <D3D9.h>

#define ORIGIN CPoint(0,0)

typedef enum {
  RESOURCE_BITMAP
 ,RESOURCE_JPEG
} ImageType;

class Image {
private:
  static IDirect3D9           *m_direct3D9;
  static IDirect3DDevice9     *m_device;
  static D3DPRESENT_PARAMETERS m_presentParameters;
  static D3DCAPS9              m_caps;

  IDirect3DTexture9 *m_texture;
  IDirect3DSurface9 *m_surface;
  CSize              m_size;
  bool               m_hasTransparentPixels;

  HDC  getDC() const;
  void releaseDC(HDC dc) const;
  void loadBMP(int resId);
  void loadJPG(int resId);
  void paintRotated(Image &dst, const CPoint &dstp, double rotation) const;
  static IDirect3DSurface9 *createSurface(     const CSize &size, D3DFORMAT format, D3DPOOL pool = D3DPOOL_DEFAULT);
  static IDirect3DSurface9 *createRenderTarget(const CSize &size, D3DFORMAT format);
  static IDirect3DTexture9 *createTexture(     const CSize &size, D3DFORMAT format);
  static void alphaBlend(IDirect3DTexture9 *texture, int x, int y, int w, int h);
  static void copySurfaceToTexture(IDirect3DTexture9 *dst, IDirect3DSurface9 *src);
  void drawLineList() const;
public:
  static void init(HWND wnd);

  static void beginScene();
  static void endScene();
  Image(const CSize &size, bool renderTarget=false);
  Image(int resId, ImageType type = RESOURCE_BITMAP, bool transparentWhite = false);
  ~Image();
  void makeWhiteTransparent();
  void makeOpaque();
  void paintSolidRect(const CRect &rect, D3DCOLOR color);
  void paint(         Image &dst, const CPoint &dstp, double rotation = 0) const;
  void paintRectangle(Image &dst, const CRect &rect) const;
//  void alphaBlend(int x, int y, int w, int h, Image &txtImage, D3DTEXTUREOP colorOp,= D3DTOP_ADD, IDirect3DTexture9 *texture, int x, int y, int w, int h);
  void render();
  const CSize &getSize() const {
    return m_size;
  }
};

#ifdef _DEBUG

void checkD3DResult(TCHAR *fileName, int line, HRESULT hr);

#define CHECKD3DRESULT(v) checkD3DResult(__TFILE__,__LINE__,v)

#else

void checkD3DResult(HRESULT hr);

#define CHECKD3DRESULT(v) checkD3DResult(v)

#endif

