#include "stdafx.h"
#include <Math.h>
#include <float.h>
#include <Stack.h>
#include <PixRect.h>
#include <WinTools.h>

#pragma warning(disable : 4073)
#pragma init_seg(lib)

#pragma comment(lib,"ddraw.lib")

class InitPixRect {
public:
  InitPixRect();
  ~InitPixRect();
};

InitPixRect::InitPixRect() {
  PixRect::initialize();
}

InitPixRect::~InitPixRect() {
  PixRect::uninitialize();
}

static InitPixRect initPixRect;


LPDIRECTDRAW   PixRect::directDraw = NULL;
DDPIXELFORMAT  PixRect::defaultPixelFormat; // same format as the screen

void PixRect::initialize() { // static
  try {
    CHECKRESULT(DirectDrawCreate(NULL, &directDraw, NULL));
    CHECKRESULT(directDraw->SetCooperativeLevel(NULL, DDSCL_NORMAL));
    PixRect pr;
    defaultPixelFormat = pr.getPixelFormat();
  } catch(Exception e) {
    MessageBox(NULL, e.what(), _T("Error"), MB_OK|MB_ICONSTOP);
    abort();
  } catch(...) {
    MessageBox(NULL, _T("Unknown exception"), _T("Error"), MB_OK|MB_ICONSTOP);
    abort();
  }
}

void PixRect::uninitialize() { // static
  CHECKRESULT(directDraw->Release());
  directDraw = NULL;
}

void PixRect::reOpenDirectDraw() { // static 
  uninitialize();
  initialize();
}

#define ASM_OPTIMIZED

#define RGB565_GETRED(rgb)        (((rgb) >> 11) & 0x1f)
#define RGB565_GETGREEN(rgb)      (((rgb) >> 5)  & 0x3f)
#define RGB565_GETBLUE(rgb)       ( (rgb)        & 0x1f)

#define RGB555_GETRED(rgb)        (((rgb) >> 10) & 0x1f)
#define RGB555_GETGREEN(rgb)      (((rgb) >> 5)  & 0x1f)
#define RGB555_GETBLUE(rgb)       ( (rgb)        & 0x1f)

#define RGB565_MAKE(r, g, b)      ((unsigned short) (((r) << 11) | ((g) << 5) | (b)))
#define RGB555_MAKE(r, g, b)      ((unsigned short) (((r) << 10) | ((g) << 5) | (b)))

static WORD D3DColorToShortColor(D3DCOLOR color) {
#ifndef ASM_OPTIMIZED
  int r = RGB_GETRED(color)   >> 3;
  int g = RGB_GETGREEN(color) >> 2;
  int b = RGB_GETBLUE(color)  >> 3;
  return RGB565_MAKE(r,g,b);
#else
  WORD result;
  __asm {
	mov	eax, color
    mov ebx, eax
    mov ecx, eax
	shr	eax, 8
	and	eax, 0xf800
    shr ebx, 5
    and ebx, 0x07e0
    or  eax, ebx
    shr ecx, 3
    and ecx, 0x001f
	or  eax, ecx
    mov result,ax
  }
  return result;
#endif
}

static D3DCOLOR shortColorToD3DColor(WORD color) {
#ifndef ASM_OPTIMIZED
  int r = RGB565_GETRED(color)   << 3;
  int g = RGB565_GETGREEN(color) << 2;
  int b = RGB565_GETBLUE(color)  << 3;

  return RGB_MAKE(r,g,b);
#else
  D3DCOLOR result;
  __asm {
	mov	ax, color
	and	eax, 0xffff
    mov ebx, eax
    mov ecx, eax
	and eax, 0xf800
	shl eax, 8
	and ebx, 0x07e0
    shl	ebx, 5
	or  eax, ebx
    and ecx, 0x001f
    shl ecx, 3
    or  eax, ecx
    mov result,eax
  }
  return result;
#endif
}

static D3DCOLOR byteColorToD3DColor(BYTE color) {
  return 0;
}

static BYTE D3DColorToByteColor(D3DCOLOR color) {
  return 0;
}

double colorDistance(const D3DCOLOR &c1, const D3DCOLOR &c2) { // value between [0..1]
  int r = RGB_GETRED(  c1) - RGB_GETRED(  c2);
  int g = RGB_GETGREEN(c1) - RGB_GETGREEN(c2);
  int b = RGB_GETBLUE( c1) - RGB_GETBLUE( c2);
  return sqrt((double)r*r + g*g + b*b)/441.67295593;
}

class SimpleColorComparator : public ColorComparator {
public:
  bool equals(const D3DCOLOR &c1, const D3DCOLOR &c2);
};

bool SimpleColorComparator::equals(const D3DCOLOR &c1, const D3DCOLOR &c2) {
  return RGBA_TORGB(c1) == RGBA_TORGB(c2);
}

class ApproximateColorComparator : public ColorComparator {
  double m_tolerance;
public:
  ApproximateColorComparator(double tolerance);
  bool equals(const D3DCOLOR &c1, const D3DCOLOR &c2);
};

ApproximateColorComparator::ApproximateColorComparator(double tolerance) {
  m_tolerance = tolerance;
}

bool ApproximateColorComparator::equals(const D3DCOLOR &c1, const D3DCOLOR &c2) {
  return colorDistance(c1,c2) < m_tolerance;
}

PixelAccessor::PixelAccessor(PixRect *pixRect) : m_pixRect(pixRect) {
  ZeroMemory(&m_ddsd, sizeof(m_ddsd));
  m_ddsd.dwSize = sizeof(m_ddsd);

  CHECKRESULT(pixRect->m_surface->Lock(NULL, &m_ddsd, DDLOCK_WAIT, NULL));
}

PixelAccessor::~PixelAccessor() {
  CHECKRESULT(m_pixRect->m_surface->Unlock(NULL));
}

void PixelAccessor::fill(const CPoint &p, D3DCOLOR color) {
  fill(p,color,SimpleColorComparator());
}

void PixelAccessor::fill(const CPoint &p, D3DCOLOR color, ColorComparator &cmp) {
  if(!m_pixRect->contains(p)) {
    return;
  }
  D3DCOLOR oldColor = getPixel(p);
  if(cmp.equals(color,oldColor)) {
    return;
  }
  const int width  = m_pixRect->getWidth();
  const int height = m_pixRect->getHeight();
  Stack<CPoint> stack;
  stack.push(p);
  while(!stack.isEmpty()) {
    CPoint np = stack.pop();
    int x;
    bool n1Stacked = false;
    bool n2Stacked = false;
    int y1 = np.y-1;
    int y2 = np.y+1;
    for(x = np.x; x >= 0 && cmp.equals(getPixel(x,np.y),oldColor); x--) {
      setPixel(x,np.y,color);
      if(y1 >= 0) {
        if(!n1Stacked) {
          if(cmp.equals(getPixel(x,y1),oldColor)) {
            stack.push(CPoint(x,y1));
            n1Stacked = true;
          }
        } else if(!cmp.equals(getPixel(x,y1),oldColor)) {
          n1Stacked = false;
        }
      }
      if(y2 < height) {
        if(!n2Stacked) {
          if(cmp.equals(getPixel(x,y2),oldColor)) {
            stack.push(CPoint(x,y2));
            n2Stacked = true;
          }
        } else if(!cmp.equals(getPixel(x,y2),oldColor)) {
          n2Stacked = false;
        }
      }
    }
    n1Stacked = false;
    n2Stacked = false;
    for(x = np.x+1; x < width && cmp.equals(getPixel(x,np.y),oldColor); x++) {
      setPixel(x,np.y,color);
      if(y1 >= 0) {
        if(!n1Stacked) {
          if(cmp.equals(getPixel(x,y1),oldColor)) {
            stack.push(CPoint(x,y1));
            n1Stacked = true;
          }
        } else if(!cmp.equals(getPixel(x,y1),oldColor)) {
          n1Stacked = false;
        }
      }
      if(y2 < height) {
        if(!n2Stacked) {
          if(cmp.equals(getPixel(x,y2),oldColor)) {
            stack.push(CPoint(x,y2));
            n2Stacked = true;
          }
        } else if(!cmp.equals(getPixel(x,y2),oldColor)) {
          n2Stacked = false;
        }
      }
    }
  }
}

class BytePixelAccessor : public PixelAccessor {
private:
  BYTE *m_pixels;
  unsigned int m_pixelsPerLine;
public:
  BytePixelAccessor(PixRect *pixRect) : PixelAccessor(pixRect) {
    m_pixels = (BYTE*)m_ddsd.lpSurface;
    m_pixelsPerLine = m_ddsd.lPitch / sizeof(m_pixels[0]);
  }
  void setPixel(unsigned int x, unsigned int y, D3DCOLOR color);
  D3DCOLOR getPixel(unsigned int x, unsigned int y);
  void setPixel(const CPoint &p, D3DCOLOR color);
  D3DCOLOR getPixel(const CPoint &p);
};

void BytePixelAccessor::setPixel(unsigned int x, unsigned int y, D3DCOLOR color) {
#ifdef _DEBUG
  if(x >= m_ddsd.dwWidth || y >= m_ddsd.dwHeight) {
    throwException(_T("BytePixelAccessor::setPixel(%u,%u) outside pixrect. Size=(%d,%d)"),x,y,m_ddsd.dwWidth,m_ddsd.dwHeight);
  }
#endif
  m_pixels[m_pixelsPerLine * y + x] = D3DColorToByteColor(color);
}

D3DCOLOR BytePixelAccessor::getPixel(unsigned int x, unsigned int y) {
#ifdef _DEBUG
  if(x >= m_ddsd.dwWidth || y >= m_ddsd.dwHeight) {
    throwException(_T("BytePixelAccessor::getPixel(%u,%u) outside pixrect. Size=(%d,%d)"),x,y,m_ddsd.dwWidth,m_ddsd.dwHeight);
  }
#endif
  return byteColorToD3DColor(m_pixels[m_pixelsPerLine * y + x]);
}

void BytePixelAccessor::setPixel(const CPoint &p, D3DCOLOR color) {
#ifdef _DEBUG
  if((unsigned int)p.x >= m_ddsd.dwWidth || (unsigned int)p.y >= m_ddsd.dwHeight) {
    throwException(_T("BytePixelAccessor::setPixel(%d,%d) outside pixrect. Size=(%d,%d)"),p.x,p.y,m_ddsd.dwWidth,m_ddsd.dwHeight);
  }
#endif
  m_pixels[m_pixelsPerLine * p.y + p.x] = D3DColorToByteColor(color);
}

D3DCOLOR BytePixelAccessor::getPixel(const CPoint &p) {
#ifdef _DEBUG
  if(((unsigned int)p.x >= m_ddsd.dwWidth) || ((unsigned int)p.y >= m_ddsd.dwHeight)) {
    throwException(_T("BytePixelAccessor::getPixel(%d,%d) outside pixrect. Size=(%d,%d)"),p.x,p.y,m_ddsd.dwWidth,m_ddsd.dwHeight);
  }
#endif
  return byteColorToD3DColor(m_pixels[m_pixelsPerLine * p.y + p.x]);
}

class WordPixelAccessor : public PixelAccessor {
private:
  WORD *m_pixels;
  unsigned int m_pixelsPerLine;
public:
  WordPixelAccessor(PixRect *pixRect) : PixelAccessor(pixRect) {
    m_pixels = (WORD*)m_ddsd.lpSurface;
    m_pixelsPerLine = m_ddsd.lPitch / sizeof(m_pixels[0]);
  }
  void setPixel(unsigned int x, unsigned int y, D3DCOLOR color);
  D3DCOLOR getPixel(unsigned int x, unsigned int y);
  void setPixel(const CPoint &p, D3DCOLOR color);
  D3DCOLOR getPixel(const CPoint &p);
};

void WordPixelAccessor::setPixel(unsigned int x, unsigned int y, D3DCOLOR color) {
#ifdef _DEBUG
  if(x >= m_ddsd.dwWidth || y >= m_ddsd.dwHeight) {
    throwException(_T("WordPixelAccessor::setPixel(%u,%u) outside pixrect. Size=(%d,%d)"),x,y,m_ddsd.dwWidth,m_ddsd.dwHeight);
  }
#endif
  m_pixels[m_pixelsPerLine * y + x] = D3DColorToShortColor(color);
}

D3DCOLOR WordPixelAccessor::getPixel(unsigned int x, unsigned int y) {
#ifdef _DEBUG
  if(x >= m_ddsd.dwWidth || y >= m_ddsd.dwHeight) {
    throwException(_T("WordPixelAccessor::getPixel(%u,%u) outside pixrect. Size=(%d,%d)"),x,y,m_ddsd.dwWidth,m_ddsd.dwHeight);
  }
#endif
  return shortColorToD3DColor(m_pixels[m_pixelsPerLine * y + x]);
}

void WordPixelAccessor::setPixel(const CPoint &p, D3DCOLOR color) {
#ifdef _DEBUG
  if(((unsigned int)p.x >= m_ddsd.dwWidth) || ((unsigned int)p.y >= m_ddsd.dwHeight)) {
    throwException(_T("WordPixelAccessor::setPixel(%d,%d) outside pixrect. Size=(%d,%d)"),p.x,p.y,m_ddsd.dwWidth,m_ddsd.dwHeight);
  }
#endif
  m_pixels[m_pixelsPerLine * p.y + p.x] = D3DColorToShortColor(color);
}

D3DCOLOR WordPixelAccessor::getPixel(const CPoint &p) {
#ifdef _DEBUG
  if(((unsigned int)p.x >= m_ddsd.dwWidth) || ((unsigned int)p.y >= m_ddsd.dwHeight)) {
    throwException(_T("WordPixelAccessor::getPixel(%d,%d) outside pixrect. Size=(%d,%d)"),p.x,p.y,m_ddsd.dwWidth,m_ddsd.dwHeight);
  }
#endif
  return shortColorToD3DColor(m_pixels[m_pixelsPerLine * p.y + p.x]);
}

class DWordPixelAccessor : public PixelAccessor {
private:
  DWORD *m_pixels;
  unsigned int m_pixelsPerLine;
public:
  DWordPixelAccessor(PixRect *pixRect) : PixelAccessor(pixRect) {
    m_pixels = (DWORD*)m_ddsd.lpSurface;
    m_pixelsPerLine = m_ddsd.lPitch / sizeof(m_pixels[0]);
  }
  void     setPixel(unsigned int x, unsigned int y, D3DCOLOR color);
  D3DCOLOR getPixel(unsigned int x, unsigned int y);
  void     setPixel(const CPoint &p, D3DCOLOR color);
  D3DCOLOR getPixel(const CPoint &p);
};

void DWordPixelAccessor::setPixel(unsigned int x, unsigned int y, D3DCOLOR color) {
#ifdef _DEBUG
  if(x >= m_ddsd.dwWidth || y >= m_ddsd.dwHeight) {
    throwException(_T("DWordPixelAccessor::setPixel(%u,%u) outside pixrect. Size=(%d,%d)"),x,y,m_ddsd.dwWidth,m_ddsd.dwHeight);
  }
#endif
  m_pixels[m_pixelsPerLine * y + x] = color;
}

D3DCOLOR DWordPixelAccessor::getPixel(unsigned int x, unsigned int y) {
#ifdef _DEBUG
  if(x >= m_ddsd.dwWidth || y >= m_ddsd.dwHeight) {
    throwException(_T("DWordPixelAccessor::getPixel(%u,%u) outside pixrect. Size=(%d,%d)"),x,y,m_ddsd.dwWidth,m_ddsd.dwHeight);
  }
#endif
  return m_pixels[m_pixelsPerLine * y + x];
}

void DWordPixelAccessor::setPixel(const CPoint &p, D3DCOLOR color) {
#ifdef _DEBUG
  if(((unsigned int)p.x >= m_ddsd.dwWidth) || ((unsigned int)p.y >= m_ddsd.dwHeight)) {
    throwException(_T("DWordPixelAccessor::setPixel(%d,%d) outside pixrect. Size=(%d,%d)"),p.x,p.y,m_ddsd.dwWidth,m_ddsd.dwHeight);
  }
#endif
  m_pixels[m_pixelsPerLine * p.y + p.x] = color;
}

D3DCOLOR DWordPixelAccessor::getPixel(const CPoint &p) {
#ifdef _DEBUG
  if(((unsigned int)p.x >= m_ddsd.dwWidth) || ((unsigned int)p.y >= m_ddsd.dwHeight)) {
    throwException(_T("DWordPixelAccessor::getPixel(%d,%d) outside pixrect. Size=(%d,%d)"),p.x,p.y,m_ddsd.dwWidth,m_ddsd.dwHeight);
  }
#endif
  return m_pixels[m_pixelsPerLine * p.y + p.x];
}

PixelAccessor *PixelAccessor::createPixelAccessor(PixRect *pixRect) {
  switch(pixRect->m_ddsd.ddpfPixelFormat.dwRGBBitCount) {
  case 8 : return new BytePixelAccessor(pixRect);
  case 16: return new WordPixelAccessor(pixRect);
  case 32: return new DWordPixelAccessor(pixRect);
  default: throwException(_T("Unknown pixelsize:%d. Must be 8, 16 or 32"),pixRect->m_ddsd.ddpfPixelFormat.dwRGBBitCount);
           return NULL;
  }
}

PixRectOperator::PixRectOperator(PixRect *pr) {
  init();
  setPixRect(pr);
}

void PixRectOperator::init() {
  m_pixRect       = NULL;
  m_pixelAccessor = NULL;
}

void PixRectOperator::setPixRect(PixRect *pixRect) {
  m_pixRect = pixRect;
  if(m_pixelAccessor != NULL) {
    delete m_pixelAccessor;
    m_pixelAccessor = NULL;
  }
  
  if(m_pixRect != NULL) {
    m_pixelAccessor = PixelAccessor::createPixelAccessor(m_pixRect);
  }
}

PixRectOperator::~PixRectOperator() {
  setPixRect(NULL);
}

void PixRectFilter::init() {
  m_result              = NULL;
  m_resultPixelAccessor = NULL;
}

void PixRectFilter::setPixRect(PixRect *pixRect) {
  PixRectOperator::setPixRect(pixRect);
  m_result              = m_pixRect;
  m_resultPixelAccessor = m_pixelAccessor;
}

CRect PixRectFilter::getRect() const {
  return (m_pixRect == NULL) ? CRect(0,0,0,0) : CRect(0,0,m_pixRect->getWidth(),m_pixRect->getHeight());
}

PixRect::PixRect() {
  // Create primary surface
  memset(&m_ddsd, 0, sizeof(m_ddsd));
  m_ddsd.dwSize          = sizeof(m_ddsd);
  m_ddsd.dwFlags         = DDSD_CAPS;
  m_ddsd.ddsCaps.dwCaps  = DDSCAPS_PRIMARYSURFACE;

  CHECKRESULT(directDraw->CreateSurface(&m_ddsd, &m_surface, NULL));
  memset(&m_ddsd, 0, sizeof(m_ddsd));
  m_ddsd.dwSize = sizeof(m_ddsd);
  CHECKRESULT(m_surface->GetSurfaceDesc(&m_ddsd));
}

PixRect::PixRect(const CSize &size) {
  createSurface(size.cx,size.cy, defaultPixelFormat);
  fillRect(0,0,size.cx,size.cy,WHITE);
}

PixRect::PixRect(unsigned int width, unsigned int height) {
  createSurface(width, height, defaultPixelFormat);
  fillRect(0,0,width,height, WHITE);
}

PixRect::PixRect(unsigned int width, unsigned int height, const DDPIXELFORMAT &pixelFormat) {
  createSurface(width,height, pixelFormat);
  fillRect(0,0,width,height, WHITE);
}

PixRect::PixRect(const CSize &size, const DDPIXELFORMAT &pixelFormat) {
  createSurface(size.cx,size.cy,pixelFormat);
  fillRect(0,0,size.cx,size.cy,WHITE);
}

PixRect *PixRect::clone(bool cloneImage) const {
  PixRect *copy = new PixRect(getWidth(),getHeight(),getPixelFormat());
  if(cloneImage) {
    copy->fastCopy(CRect(0,0,getWidth(),getHeight()),this);
  }
  return copy;
}

LPDIRECTDRAWSURFACE PixRect::cloneSurface() const {
  DDSURFACEDESC ddsd = m_ddsd;;

  LPDIRECTDRAWSURFACE s = newSurface(ddsd);
  DDBLTFX ddbfx;
  ZeroMemory(&ddbfx, sizeof(DDBLTFX));
  ddbfx.dwSize = sizeof(DDBLTFX);
  ddbfx.dwROP  = SRCCOPY;
  CRect rect(0,0,getWidth(),getHeight());
  CHECKRESULT(s->Blt(&(CRect&)rect, m_surface, NULL, DDBLT_ROP | DDBLT_WAIT, &ddbfx));
  return s;
}

void PixRect::showPixRect(PixRect *pr) { // static
  HDC screenDC = getScreenDC();
  BitBlt(screenDC,0,0,pr->getWidth(),pr->getHeight(),NULL,0,0,WHITENESS);
  PixRect::bitBlt(screenDC, 0,0,pr->getWidth(),pr->getHeight(), SRCCOPY, pr, 0,0);
  DeleteDC(screenDC);
}

void PixRect::checkHasAlphaChannel() const { // throw Exception if no alpha-channel
}

DDCAPS PixRect::getDriverCaps() {   // static
  DDCAPS result;
  result.dwSize = sizeof(DDCAPS);
  CHECKRESULT(directDraw->GetCaps(&result, NULL));
  return result;
}

DDCAPS PixRect::getEmulatorCaps() { // static
  DDCAPS result;
  result.dwSize = sizeof(DDCAPS);
  CHECKRESULT(directDraw->GetCaps(NULL, &result));
  return result;
}


static HRESULT WINAPI displayModesCallBack(LPDDSURFACEDESC desc, void *context) {
  Array<DDSURFACEDESC> *a = (Array<DDSURFACEDESC>*)context;
  a->add(*desc);
  return DDENUMRET_OK;
}

Array<DDSURFACEDESC> PixRect::getDisplayModes() const {
  Array<DDSURFACEDESC> result;
  CHECKRESULT(directDraw->EnumDisplayModes(0, &((PixRect*)this)->m_ddsd, &result, displayModesCallBack));
  return result;
}

void PixRect::setSize(const CSize &size) {
//  if(m_ddsd.ddsCaps.dwCaps  = DDSCAPS_OFFSCREENPLAIN;

  const CSize oldSize = getSize();
  CSize areaToCopy;
  if(size == oldSize) {
    return;
  }
  areaToCopy.cx = min(oldSize.cx, size.cx);
  areaToCopy.cy = min(oldSize.cy, size.cy);
  PixRect copy(areaToCopy, getPixelFormat());
  copy.rop(0, 0, areaToCopy.cx, areaToCopy.cy, SRCCOPY, this, 0,0);
  destroySurface();
  createSurface(size.cx, size.cy, copy.getPixelFormat());
  fillRect(getRect(), WHITE);
  rop(0,0, areaToCopy.cx, areaToCopy.cy, SRCCOPY, &copy, 0, 0);
}

CSize PixRect::getSizeInMillimeters(HDC hdc) const {
  if(hdc != NULL) {
    return pixelsToMillimeters(hdc,getSize());
  } else {
    HDC screenDC = getScreenDC();
    CSize result = pixelsToMillimeters(screenDC,getSize());
    DeleteDC(screenDC);
    return result;
  }
}

PixRect::PixRect(HBITMAP src) {
  BITMAP bm;
  GetObject(src,sizeof(bm), &bm);
  init(src, getPixelFormat(bm));
}

PixRect::PixRect(HBITMAP src, const DDPIXELFORMAT &pixelFormat) {
  init(src, pixelFormat);
}

void PixRect::init(HBITMAP src, const DDPIXELFORMAT &pixelFormat) {
  String errMsg;
  BITMAP bm;
  GetObject(src,sizeof(bm),&bm);
  createSurface(bm.bmWidth,bm.bmHeight, pixelFormat);

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
    throwException(_T("PixRect::init failed:%s"), errMsg.cstr());
  }
}

PixRect::~PixRect() {
  destroySurface();
}

void PixRect::createSurface(int width, int height, const DDPIXELFORMAT &pixelFormat) {
  memset(&m_ddsd, 0, sizeof(m_ddsd));
  m_ddsd.dwSize          = sizeof(m_ddsd);
  m_ddsd.dwFlags         = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
  m_ddsd.ddsCaps.dwCaps  = DDSCAPS_OFFSCREENPLAIN;
  m_ddsd.dwWidth         = width;
  m_ddsd.dwHeight        = height;
  m_ddsd.ddpfPixelFormat = pixelFormat;

  m_surface = newSurface(m_ddsd);
}

LPDIRECTDRAWSURFACE PixRect::newSurface(DDSURFACEDESC &ddsd) { // static
  LPDIRECTDRAWSURFACE surface;
  CHECKRESULT(directDraw->CreateSurface(&ddsd, &surface, NULL));
  return surface;
}

void PixRect::destroySurface() {
  CHECKRESULT(m_surface->Release());
}

PixRect &PixRect::operator=(HBITMAP src) {
  String errMsg;
  BITMAP bm;
  GetObject(src,sizeof(bm),&bm);
  const DDPIXELFORMAT pixelFormat = getPixelFormat();

  destroySurface();
  createSurface(bm.bmWidth,bm.bmHeight, pixelFormat);

  HDC prDC = getDC();
  HDC bmDC = CreateCompatibleDC(prDC);;
  HGDIOBJ gdi1 = SelectObject(bmDC,src);
  BOOL ok = BitBlt(prDC,0,0,getWidth(),getHeight(),bmDC,0,0,SRCCOPY);
  if(!ok) {
    errMsg = getLastErrorText();
  }
  SelectObject(bmDC,gdi1);
  DeleteDC(bmDC);
  releaseDC(prDC);
  if(!ok) {
    throwException(_T("PixRect::operator=(HBITMAP) failed:%s"), errMsg.cstr());
  }

  return *this;
}

const DDPIXELFORMAT &PixRect::getPixelFormat() const {
  return m_ddsd.ddpfPixelFormat;
}

DDPIXELFORMAT PixRect::getPixelFormat(const BITMAP &bm) { // static
  DDPIXELFORMAT pf;
  pf.dwSize            = sizeof(DDPIXELFORMAT);
  pf.dwFlags           = 0x40; // DDPF_FOURCC;
  pf.dwFourCC          = bm.bmType;
  pf.dwRGBBitCount     = bm.bmBitsPixel;
  switch(bm.bmBitsPixel) {
  case 32:
    pf.dwRBitMask        = 0x00ff0000;
    pf.dwGBitMask        = 0x0000ff00;
    pf.dwBBitMask        = 0x000000ff;
    pf.dwRGBAlphaBitMask = 0;
    break;
  default:
    throwException(_T("PixRect::getPixelFormat():BITMAP.bmBitsPixel=%d. Must be 32"), bm.bmBitsPixel);
  }
  return pf;
}

PixRect::operator HBITMAP() const {
  String errMsg;
  DDPIXELFORMAT pf     = getPixelFormat();
  HBITMAP       result = CreateBitmap(getWidth(), getHeight(), 1, pf.dwRGBBitCount, NULL);

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
    throwException(_T("PixRect::operator HBITMAP failed:%s"), errMsg.cstr());
  }

  return result;
}

static bool operator==(const DDPIXELFORMAT &pf1, const DDPIXELFORMAT &pf2) {
  return pf1.dwRGBBitCount == pf2.dwRGBBitCount
      && pf1.dwRBitMask    == pf2.dwRBitMask
      && pf1.dwGBitMask    == pf2.dwGBitMask
      && pf1.dwBBitMask    == pf2.dwBBitMask
      && pf1.dwRGBZBitMask == pf2.dwRGBZBitMask;
}

static bool operator!=(const DDPIXELFORMAT &pf1, const DDPIXELFORMAT &pf2) {
  return !(pf1 == pf2);
}

void PixRect::fromBitmap(CBitmap &src) {
  String errMsg;
  BITMAP bm;
  src.GetObject(sizeof(bm),&bm);
  DDPIXELFORMAT bmpf = getPixelFormat(bm);
  DDPIXELFORMAT ppf  = getPixelFormat();

  if(bm.bmWidth != getWidth() || bm.bmHeight != getHeight() || bmpf != ppf) {
    destroySurface();
    createSurface(bm.bmWidth,bm.bmHeight,bmpf);
  }

  HDC     prDC = getDC();
  HDC     bmDC = CreateCompatibleDC(prDC);
  HGDIOBJ gdi1 = SelectObject(bmDC,(HBITMAP)src);
  BOOL    ok   = BitBlt(prDC,0,0,bm.bmWidth,bm.bmHeight,bmDC,0,0,SRCCOPY);
  if(!ok) {
    errMsg = getLastErrorText();
  }
  SelectObject(bmDC, gdi1);
  DeleteDC(bmDC);
  releaseDC(prDC);
  if(!ok) {
    throwException(_T("PixRect::fromBitmap failed:%s"), errMsg.cstr());
  }
}

void PixRect::toBitmap(CBitmap &dst) const {
  String errMsg;
  DDPIXELFORMAT ppf  = getPixelFormat();
  if(dst.m_hObject == NULL) {
    dst.CreateBitmap(getWidth(),getHeight(),1,ppf.dwRGBBitCount,NULL);
  } else {
    BITMAP bm;
    dst.GetObject(sizeof(bm),&bm);
    DDPIXELFORMAT bmpf = getPixelFormat(bm);
    if(bm.bmWidth != getWidth() || bm.bmHeight != getHeight() || bmpf != ppf) {
      dst.DeleteObject();
      dst.CreateBitmap(getWidth(),getHeight(),1,ppf.dwRGBBitCount,NULL);
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
    throwException(_T("PixRect::toBitmap failed:%s"), errMsg.cstr());
  }
}

bool PixRect::contains(const Point2D &p) const {
  return p.x >= 0 && p.x < m_ddsd.dwWidth && p.y >= 0 && p.y < m_ddsd.dwHeight;
}

bool PixRect::containsExtended(const Point2D &p) const {
  return p.x > -1 && p.x < m_ddsd.dwWidth && p.y > -1 && p.y < m_ddsd.dwHeight;
}

bool operator==(const DDSURFACEDESC &d1, const DDSURFACEDESC &d2) {
  return d1.dwWidth         == d2.dwWidth
      && d1.dwHeight        == d2.dwHeight
      && d1.ddpfPixelFormat == d2.ddpfPixelFormat;
}

bool operator!=(const DDSURFACEDESC &d1, const DDSURFACEDESC &d2) {
  return !(d1 == d2);
}

bool operator==(const PixRect &p1, const PixRect &p2) {
  if(&p1 == &p2) {
    return true;
  }
  if(p1.m_ddsd != p2.m_ddsd) {
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

void PixRect::setPixel(unsigned int x, unsigned int y, D3DCOLOR color) {
  if(!contains(x,y)) {
    return;
  }
  PixelAccessor *pixelAccessor = getPixelAccessor();
  pixelAccessor->setPixel(x,y,color);
  delete pixelAccessor;
}

D3DCOLOR PixRect::getPixel(unsigned int x, unsigned int y) const {
  if(!contains(x,y)) {
    return 0;
  }
  PixelAccessor *pixelAccessor = PixelAccessor::createPixelAccessor((PixRect*)this);
  D3DCOLOR result = pixelAccessor->getPixel(x,y);
  delete pixelAccessor;
  return result;
}

void PixRect::rop( const CRect  &dr, unsigned long op, const PixRect *src, const CPoint &sp) {
  rop(dr.left,dr.top,dr.Width(),dr.Height(), op, src,sp.x,sp.y);
}

void PixRect::rop( const CPoint &dp, const CSize &size, unsigned long op, const PixRect *src, const CPoint &sp) {
  rop(dp.x,dp.y,size.cx,size.cy, op, src,sp.x,sp.y);
}

void PixRect::rop(int x, int y, int w, int h, unsigned long op, const PixRect *src, int sx, int sy) {
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

void PixRect::rop(const CRect &dr, unsigned long op, const PixRect *src, const CRect &sr) {
  DDBLTFX bltFunc;
  memset(&bltFunc, 0, sizeof(DDBLTFX));
  bltFunc.dwSize = sizeof(DDBLTFX);
//  bltFunc.dwDDROP = 4;
  bltFunc.dwROP  = SRCCOPY;
  CRect dstRect  = dr;
  CRect srcRect  = sr;

  CHECKRESULT(m_surface->Blt(&dstRect, src->m_surface, &srcRect, DDBLT_ROP | DDBLT_WAIT, &bltFunc));
}

void PixRect::bitBlt(HDC dst, const CPoint &p, const CSize &size, unsigned long op, const PixRect *src, const CPoint &sp) { // static
  bitBlt(dst, p.x,p.y,size.cx,size.cy, op, src, sp.x,sp.y);
}

void PixRect::bitBlt(PixRect *dst, const CPoint &p, const CSize &size, unsigned long op, HDC src, const CPoint &sp) { // static
  bitBlt(dst, p.x,p.y,size.cx,size.cy, op, src, sp.x,sp.y);
}

void PixRect::bitBlt(HDC dst, int x, int y, int w, int h, unsigned long op, const PixRect *src, int sx, int sy) { // static
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
    throwException(_T("PixRect::bitBlt failed:%s"), errorMsg.cstr());
  }
}

void PixRect::bitBlt(PixRect *dst, int x, int y, int w, int h, unsigned long op, HDC src, int sx, int sy) { // static
  String errorMsg;

  HDC dstDC = dst->getDC();

  BOOL ok = BitBlt(dstDC,x,y,w,h,src,sx,sy,op);
  if(!ok) {
    errorMsg = getLastErrorText();
  }
  dst->releaseDC(dstDC);
  if(!ok) {
    throwException(_T("PixRect::bitBlt failed:%s"), errorMsg.cstr());
  }
}

void PixRect::stretchBlt(HDC dst, int x, int y, int w, int h, unsigned long op, const PixRect *src, int sx, int sy, int sw, int sh) { // static
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
    throwException(_T("PixRect::stretchBlt failed:%s"), errorMsg.cstr());
  }
}

void PixRect::stretchBlt(PixRect *dst, int x, int y, int w, int h, unsigned long op, const HDC src, int sx, int sy, int sw, int sh) { // static
  String errorMsg;

  HDC  dstDC = dst->getDC();
  BOOL ok    = StretchBlt(dstDC,x,y,w,h,src,sx,sy,sw,sh,op);
  if(!ok) {
    errorMsg = getLastErrorText();
  }
  dst->releaseDC(dstDC);
  if(!ok) {
    throwException(_T("PixRect::stretchBlt failed:%s"), errorMsg.cstr());
  }
}

void PixRect::stretchBlt(HDC dst, const CPoint &p, const CSize &size, unsigned long op, const PixRect *src, const CPoint &sp, const CSize &ssize) { // static
  stretchBlt(dst, p.x,p.y,size.cx,size.cy, op, src, sp.x,sp.y,ssize.cx,ssize.cy);
}

void PixRect::stretchBlt(PixRect *dst, const CPoint &p, const CSize &size, unsigned long op, const HDC src, const CPoint &sp, const CSize &ssize) { // static 
  stretchBlt(dst, p.x,p.y,size.cx,size.cy, op, src, sp.x,sp.y,ssize.cx,ssize.cy);
}

void PixRect::stretchBlt(HDC dst, const CRect &dstRect, unsigned long op, const PixRect *src, const CRect &sr) { // static
  const CSize dsz = dstRect.Size();
  const CSize ssz = sr.Size();
  stretchBlt(dst, dstRect.left,dstRect.top,dsz.cx,dsz.cy, op, src, sr.left,sr.top,ssz.cx,ssz.cy);
}

void PixRect::stretchBlt(PixRect *dst, const CRect &dstRect, unsigned long op, const HDC src, const CRect &sr) { // static
  const CSize dsz = dstRect.Size();
  const CSize ssz = sr.Size();
  stretchBlt(dst, dstRect.left,dstRect.top,dsz.cx,dsz.cy, op, src, sr.left,sr.top,ssz.cx,ssz.cy);
}

void PixRect::mask(const CRect  &dr, unsigned long op, const PixRect *src, const CPoint &sp, const PixRect *prMask) {
  mask(dr.left,dr.top,dr.Width(),dr.Height(), op, src,sp.x,sp.y, prMask);
}

void PixRect::mask(const CPoint &dp, const CSize &size, unsigned long op, const PixRect *src, const CPoint &sp, const PixRect *prMask) {
  mask(dp.x,dp.y,size.cx,size.cy, op, src, sp.x,sp.y, prMask);
}

// Performs rasteropreation op on this using src, for the pixels which are non-zero in prMask
void PixRect::mask(int x, int y, int w, int h, unsigned long op, const PixRect *src, int sx, int sy, const PixRect *prMask) {
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

void PixRect::fastCopy(const CRect &rect, const PixRect *src) {
  DDBLTFX ddbfx;
  ZeroMemory(&ddbfx, sizeof(DDBLTFX));
  ddbfx.dwSize = sizeof(DDBLTFX);
  ddbfx.dwROP  = SRCCOPY;

  if(getArea(rect) != 0) {
    CHECKRESULT(m_surface->Blt(&(CRect&)rect, src->m_surface, NULL, DDBLT_ROP | DDBLT_WAIT, &ddbfx));
  }
}

PixRectClipper::PixRectClipper(HWND hwnd) {
  CHECKRESULT(PixRect::directDraw->CreateClipper(0, &m_clipper, NULL));
  CHECKRESULT(m_clipper->SetHWnd(0, hwnd));
}

PixRectClipper::~PixRectClipper() {
  CHECKRESULT(m_clipper->Release());
}

void PixRect::setClipper(PixRectClipper *clipper) {
  if(clipper != NULL) {
    CHECKRESULT(m_surface->SetClipper(clipper->m_clipper));
  } else {
    CHECKRESULT(m_surface->SetClipper(NULL));
  }
}

void PixRect::copy(VIDEOHDR &videoHeader) {
  CHECKRESULT(m_surface->Lock(NULL, &m_ddsd, DDLOCK_WAIT | DDLOCK_WRITEONLY, NULL));
  memcpy(m_ddsd.lpSurface, videoHeader.lpData, videoHeader.dwBytesUsed); 
  CHECKRESULT(m_surface->Unlock(NULL));
}

void PixRect::fill(const CPoint &p, D3DCOLOR color, ColorComparator &cmp) {
  PixelAccessor *pa = getPixelAccessor();
  pa->fill(p, color, cmp);
  delete pa;
}

void PixRect::fill(const CPoint &p, D3DCOLOR color) {
  fill(p,color,SimpleColorComparator());
}

void PixRect::fillTransparent(const CPoint &p, unsigned char alpha) { // alpha = 0 => opaque, 255 = transparent
  if(!contains(p)) {
    return;
  }

  const int width  = getWidth();
  const int height = getHeight();
  PixRect *copy = clone(true);
  D3DCOLOR tmpColor = ~getPixel(p);
  copy->fill(p,tmpColor);
  copy->rop( 0,0,width,height,SRCINVERT,this,0,0);

  PixelAccessor *pa  = getPixelAccessor();
  PixelAccessor *cpa = copy->getPixelAccessor();

  CPoint sp;
  for(sp.x = 0; sp.x < width; sp.x++) {
    for(sp.y = 0; sp.y < height; sp.y++) {
      if(cpa->getPixel(p)) {
        pa->setPixel(sp,RGBA_SETALPHA(pa->getPixel(sp),alpha));
      }
    }
  }
  delete pa;
  delete cpa;
  delete copy;
}

void PixRect::approximateFill(const CPoint &p, D3DCOLOR color, double tolerance) {
  if(tolerance == 0) {
    tolerance = colorDistance(getPixel(p), color);
  }

  fill(p,color,ApproximateColorComparator(tolerance));
}

void PixRect::line(int x1, int y1, int x2, int y2, D3DCOLOR color, bool invert) {
  line(CPoint(x1,y1), CPoint(x2,y2), color, invert);
}

void PixRect::line(const CPoint &p1, const CPoint &p2, D3DCOLOR color, bool invert) {
  if(invert) {
    applyToLine(p1,p2,InvertColor(this));
  } else {
    applyToLine(p1,p2,SetColor(color, this));
  }
}

void PixRect::rectangle(int x1, int y1, int x2, int y2, D3DCOLOR color, bool invert) {
  rectangle(CRect(x1,y1,x2,y2),color,invert);
}

void PixRect::rectangle(const CPoint &p0, const CSize &size, D3DCOLOR color, bool invert) {
  rectangle(CRect(p0,size),color,invert);
}

void PixRect::rectangle(const CRect &rect, D3DCOLOR color, bool invert) {
  if(invert) {
    applyToRectangle(rect, InvertColor(this));
  } else {
    applyToRectangle(rect, SetColor(color, this));
  }
}

void PixRect::drawDragRect(CRect *newRect, const CSize &newSize, CRect *lastRect, const CSize &lastSize) {
  HDC hdc = getDC();
  CDC *dc = CDC::FromHandle(hdc);
  CRect tmp;
  if(lastRect) {
    tmp = makePositiveRect(*lastRect);
  }
  dc->DrawDragRect(&makePositiveRect(newRect), newSize,(lastRect!=NULL) ? tmp : NULL, lastSize);
  releaseDC(hdc);
}

void PixRect::polygon(const MyPolygon &polygon, D3DCOLOR color, bool invert, bool closingEdge) {
  if(invert) {
    polygon.applyToEdge(InvertColor(this), closingEdge);
  } else {
    polygon.applyToEdge(SetColor(color, this), closingEdge);
  }
}

void PixRect::ellipse(const CRect &rect, D3DCOLOR color, bool invert) {
  if(invert) {
    applyToEllipse(rect,InvertColor(this));
  } else {
    applyToEllipse(rect,SetColor(color, this));
  }
}

void PixRect::fillRect(const CRect &rect, D3DCOLOR color, bool invert) {
  if(rect.Width() == 0 || rect.Height() == 0) {
    return;
  }
  DDBLTFX ddbfx;
  ddbfx.dwSize = sizeof(DDBLTFX);
  ddbfx.dwFillColor = color;

  CRect dstRect = makePositiveRect(rect);
  dstRect.right  = min(dstRect.right ,(long)m_ddsd.dwWidth); 
  dstRect.bottom = min(dstRect.bottom,(long)m_ddsd.dwHeight);
  if(dstRect.Width() <= 0 ||dstRect.Height() <= 0) {
    return;
  }
  CHECKRESULT(m_surface->Blt(&dstRect, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbfx));
}

void PixRect::fillRect(const CPoint &p0, const CSize &size, D3DCOLOR color, bool invert) {
  fillRect(CRect(p0,size),color,invert);
}

void PixRect::fillRect(int x1, int y1, int x2, int y2, D3DCOLOR color, bool invert) {
  fillRect(CRect(x1,y1,x2-x1,y2-y1),color,invert);
}

void PixRect::replicate(int x, int y, int w, int h, const PixRect *src) {
  if(x < 0) {
    w += x;
    x = 0;
  }
  if(y < 0) {
    h += y;
    y = 0;
  }
  if(x >= getWidth() || y >= getHeight()) {
    return;
  }
  w = min(w,getWidth()-x);
  h = min(h,getHeight()-y);
  if(w == 0 || h == 0) {
    return;
  }
  for(int r = 0; r < h; r += src->getHeight()) {
    for(int c = 0; c < w; c += src->getWidth()) {
      rop(x+c,y+r,src->getWidth(),src->getHeight(),SRCCOPY, src,0,0);
    }
  }
}

PixRect &PixRect::apply(PixRectOperator &op) {
  op.setPixRect(this);
  applyToFullRectangle(CRect(CPoint(0,0), getSize()), op);
  op.setPixRect(NULL);
  return *this;
}

PixRect &PixRect::apply(PixRectFilter &filter) {
  filter.setPixRect(this);
  applyToFullRectangle(filter.getRect(),filter);
  if(filter.m_result != this) {
    rop(0,0,getWidth(),getHeight(),SRCCOPY,filter.m_result,0,0);
  }
  filter.setPixRect(NULL);
  return *this;
}

void applyToLine(const CPoint &p1, const CPoint &p2, PointOperator &op) {
  applyToLine(p1.x,p1.y,p2.x,p2.y,op);
}

void applyToLine(int x1, int y1, int x2, int y2, PointOperator &op) {
  if(x1 == x2) {
    if(y1 > y2) {
      swap(y1,y2);
    }
    for(CPoint p = CPoint(x1,y1); p.y <= y2; p.y++) {
      op.apply(p);
    }
  } else if(y1 == y2) {
    if(x1 > x2) {
      swap(x1,x2);
    }
    for(CPoint p = CPoint(x1,y1); p.x <= x2; p.x++) {
      op.apply(p);
    }
  } else {
    const int vx = x2 - x1;
    const int vy = y2 - y1;
    const int dx = sign(vx);
    const int dy = sign(vy);
    for(CPoint p = CPoint(x1,y1), p1 = CPoint(p.x-x1,p.y-y1);;) {
      op.apply(p);
      if(p.x == x2 && p.y == y2) {
        break;
      }
      if(abs((p1.x+dx)*vy - p1.y*vx) < abs(p1.x*vy - (p1.y+dy)*vx)) {
        p.x  += dx;
        p1.x += dx;
      } else {
        p.y  += dy;
        p1.y += dy;
      }
    }
  }
}

void applyToRectangle(const CRect &rect, PointOperator &op) {
  const CRect r = makePositiveRect(rect);
  if(r.left == r.right) {
    if(r.top == r.bottom) {
      op.apply(r.TopLeft());                                // rect contains only 1 pixel
    } else {
      applyToLine(r.left , r.top, r.left  , r.bottom , op); // rect is a vertical line
    }
  } else if(r.top == r.bottom) {
    applyToLine(r.left , r.top   , r.right, r.top    , op); // rect is a horizontal line
  } else {                                                  // rect is a "real" rectangle
    applyToLine(r.left , r.top   , r.right-1, r.top     , op); 
    applyToLine(r.right, r.top   , r.right  , r.bottom-1, op);
    applyToLine(r.right, r.bottom, r.left+1 , r.bottom  , op);
    applyToLine(r.left , r.bottom, r.left   , r.top+1   , op);
  }
}

void applyToFullRectangle(const CRect &rect, PointOperator &op) {
  const CRect r = makePositiveRect(rect);
  CPoint p;
  for(p.y = r.top; p.y < r.bottom; p.y++) {
    for(p.x = r.left; p.x < r.right; p.x++) {
      op.apply(p);
    }
  }
}

static void applyToEllipsePart(const CPoint &start, const CPoint &end, const CPoint &center, PointOperator &op) {
  const int    minx = min(start.x,end.x);
  const int    miny = min(start.y,end.y);
  const int    maxx = max(start.x,end.x);
  const int    maxy = max(start.y,end.y);
  const int    dx   = sign(end.x - start.x);
  const int    dy   = sign(end.y - start.y);
  const int    rx   = max(abs(start.x-center.x),abs(end.x-center.x));
  const int    ry   = max(abs(start.y-center.y),abs(end.y-center.y));
  const double rx2  = rx*rx;
  const double ry2  = ry*ry;
  const double rxy2 = rx2*ry2;

  if(rx == 0 || ry == 0) {
    return;
  }

  CPoint p  = start;
  CPoint pc = p - center;
  while(minx <= p.x && p.x <= maxx && miny <= p.y && p.y <= maxy) {
    if(p == end) {
      break;
    }
    op.apply(p);
    const double d1 = fabs(ry2*sqr(pc.x+dx) + rx2*sqr(pc.y   ) - rxy2);
    const double d2 = fabs(ry2*sqr(pc.x   ) + rx2*sqr(pc.y+dy) - rxy2);
    if(d1 < d2) {
      p.x  += dx;
      pc.x += dx;
    } else {
      p.y  += dy;
      pc.y += dy;
    }
  }
}

void applyToEllipse(const CRect &rect, PointOperator &op) {
  const CRect r = makePositiveRect(rect);
  const CPoint center = r.CenterPoint();
  applyToEllipsePart(CPoint(center.x,r.top   ),CPoint(r.right,center.y ),center, op);
  applyToEllipsePart(CPoint(r.right,center.y ),CPoint(center.x,r.bottom),center, op);
  applyToEllipsePart(CPoint(center.x,r.bottom),CPoint(r.left,center.y  ),center, op);
  applyToEllipsePart(CPoint(r.left,center.y  ),CPoint(center.x,r.top   ),center, op);
}

void applyToBezier(const Point2D &start, const Point2D &cp1, const Point2D &cp2, const Point2D &end, CurveOperator &op, bool applyStart) {
  Point2D p = start;
  if(applyStart) {
    op.apply(start);
  }
  for(float t = 0.1f; t < 1.01; t += 0.1f) {
    float tt = t*t;
    float s  = 1.0f - t;
    float ss = s*s;
    Point2D np = start*(ss*s) + cp1*(3*ss*t) + cp2*(3*s*tt) + end*(t*tt);
    op.apply(np);
    p = np;
  }
}

void SetColor::apply(const CPoint &p) {
  if(m_pixRect->contains(p)) {
    m_pixelAccessor->setPixel(p, m_color);
  }
}

void SetAlpha::apply(const CPoint &p) {
  if(m_pixRect->contains(p)) {
    const D3DCOLOR color  = RGBA_TORGB(m_pixelAccessor->getPixel(p));
    const D3DCOLOR aColor = RGBA_SETALPHA(color, m_alpha);
    m_pixelAccessor->setPixel(p, aColor);
  }
}

void SubstituteColor::apply(const CPoint &p) {
  if(m_pixelAccessor->getPixel(p) == m_from) {
    m_pixelAccessor->setPixel(p,m_to);
  }
}

void InvertColor::apply(const CPoint &p) {
  if(m_pixRect->contains(p)) {
    m_pixelAccessor->setPixel(p,~m_pixelAccessor->getPixel(p));
  }
}

void CurveOperator ::apply(const Point2D &p) {
  if(m_firstTime) {
    m_currentPoint = p;
    m_firstTime = false;
  } else {
    line(m_currentPoint,p);
    m_currentPoint = p;
  }
}

class PolygonFiller : public PixRectOperator {
private:
  MyPolygon &m_polygon;
  void checkAndFill(const CPoint &p);
public:
  Array<CPoint> m_pointsOutside;
  PolygonFiller(PixRect *pixRect, MyPolygon &polygon);
  void apply(const CPoint &p);
  void restoreBlackOutSideRegion();
};

PolygonFiller::PolygonFiller(PixRect *pixRect, MyPolygon &polygon) 
: m_polygon(polygon) {
  setPixRect(pixRect);
}

// Fills BLACK inside polygon, To speed up edge-scanner we fill RED outside, and collect all points used initially to fill 
// these regions. They are afterwards removed (filled whith BLACK) from mask, so mask finally contains WHITE inside (and edge),
// and BLACK outside.
void PolygonFiller::checkAndFill(const CPoint &p) {
  if(!m_pixRect->contains(p)) {
    return;
  }
  if(m_pixelAccessor->getPixel(p) == 0)
    switch(m_polygon.contains(p)) {
    case 1:
      m_pixelAccessor->fill(p,WHITE);
      break;
    case -1:
      m_pixelAccessor->fill(p,RGB_MAKE(255,0,0));
      m_pointsOutside.add(p);
      break;
    case 0:
      break;
  }
}

void PolygonFiller::restoreBlackOutSideRegion() {
  for(int i = 0; i < m_pointsOutside.size(); i++) {
    m_pixelAccessor->fill(m_pointsOutside[i],BLACK);
  }
}

void PolygonFiller::apply(const CPoint &p) {
  checkAndFill(CPoint(p.x-1,p.y));
  checkAndFill(CPoint(p.x+1,p.y));
  checkAndFill(CPoint(p.x,p.y-1));
  checkAndFill(CPoint(p.x,p.y+1));
}

void PixRect::fillPolygon(const MyPolygon &polygon, D3DCOLOR color, bool invert) {
  CRect rect = polygon.getBoundsRect();
  MyPolygon poly = polygon;
  poly.move(-rect.TopLeft());
  PixRect *psrc  = new PixRect(rect.Width(),rect.Height(),getPixelFormat());
  PixRect *pmask = new PixRect(rect.Width(),rect.Height(),getPixelFormat());
  psrc->fillRect(0,0,rect.Width(),rect.Height(),color);
  pmask->fillRect(0,0,rect.Width(),rect.Height(),0);   // set mask to black
  pmask->polygon(poly,WHITE);                          // draw white frame around polygon on mask

  PolygonFiller *polygonFiller = new PolygonFiller(pmask, poly);
  poly.applyToEdge(*polygonFiller);
  polygonFiller->restoreBlackOutSideRegion();
  delete polygonFiller;

//  rop(rect.left,rect.top,rect.Width(),rect.Height(),DSTINVERT,NULL,0,0);
//  mask(rect.left,rect.top,rect.Width(),rect.Height(), MAKEROP4(SRCCOPY,DSTINVERT), psrc, 0,0, pmask);
  mask(rect.left,rect.top,rect.Width(),rect.Height(), SRCCOPY, psrc, 0,0, pmask);
  delete pmask;
  delete psrc;
}

void PixRect::fillEllipse(const CRect &rect, D3DCOLOR color, bool invert) {
  CRect r = rect;
  if(rect.Width() == 0 || rect.Height() == 0) {
    return;
  }
  r -= rect.TopLeft();
  PixRect *psrc  = new PixRect(r.Width(),r.Height(),getPixelFormat());
  PixRect *pmask = new PixRect(r.Width(),r.Height(),getPixelFormat());
  psrc->fillRect(0,0,r.Width(),r.Height(),color);
  pmask->fillRect(0,0,r.Width(),r.Height(),BLACK);  // set mask to black
  pmask->ellipse(r,WHITE);                          // draw white ellipse on mask
  pmask->fill(r.CenterPoint(),WHITE);

//  rop(rect.left,rect.top,rect.Width(),rect.Height(),DSTINVERT,NULL,0,0);
//  mask(rect.left,rect.top,rect.Width(),rect.Height(), MAKEROP4(SRCCOPY,DSTINVERT), psrc, 0,0, pmask);
  mask(rect.left,rect.top,rect.Width(),rect.Height(), SRCCOPY, psrc, 0,0, pmask);

  delete pmask;
  delete psrc;
}

PixRect *PixRect::mirror(const PixRect *src, bool vertical) {
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
  m_pixRect->line(m_glyphPos%from,m_glyphPos%to,m_color,m_invert);
}

void PixRectTextMaker::beginGlyph(const Point2D &offset) {
  m_glyphPos = m_textPos + offset;
}

void PixRect::bezier(const Point2D &start, const Point2D &cp1, const Point2D &cp2, const Point2D &end, D3DCOLOR color, bool invert) {
  applyToBezier(start,cp1,cp2,end,PixRectTextMaker(this,Point2D(0,0), color,invert));
}

void MyPolygon::move(const CPoint &dp) {
  for(int i = 0; i < size(); i++) {
    (*this)[i] += dp;
  }
}

CRect MyPolygon::getBoundsRect() const {
  const Array<CPoint> &a = *this;
  if(a.size() == 0) {
    return CRect(0,0,0,0);
  }
  CRect result(a[0],a[0]);
  for(int i = 1; i < a.size(); i++) {
    const CPoint &p = a[i];
    if(p.x < result.left  ) result.left   = p.x;
    if(p.x > result.right ) result.right  = p.x;
    if(p.y < result.top   ) result.top    = p.y;
    if(p.y > result.bottom) result.bottom = p.y;
  }
  return CRect(result.left,result.top,result.right+1,result.bottom+1);
}

class Vector : public CPoint {
public:
  Vector(const CPoint &from, const CPoint &to);
  double length() const;
};

Vector::Vector(const CPoint &from, const CPoint &to) {
  x = to.x - from.x;
  y = to.y - from.y;
}

double operator*(const Vector &v1, const Vector &v2) {
  return v1.x*v2.x + v1.y*v2.y;
}

double Vector::length() const {
  return sqrt(sqr(x) + sqr(y));
}

static int det(const Vector &v1, const Vector &v2) {
  return v1.x*v2.y - v1.y*v2.x;
}

static double angle(const Vector &v1, const Vector &v2) {
  if(v1 == v2) {
    return 0;
  }
  double a = acos(v1*v2 / v1.length() / v2.length());
  return a * sign(det(v1,v2));
}

bool MyPolygon::add(const CPoint &p) {
  if(size() > 0 && p == last()) {
    return false;
  }
  return Array<CPoint>::add(p);
}

int MyPolygon::contains(const CPoint &p) const { // 1=inside, -1=outside, 0=edge
  unsigned int n = size();
  if(n < 3) {
    return -1;
  }
  Vector v(p,(*this)[0]);
  if(v.x == 0 && v.y == 0) {
    return 0; // edge
  }
  double d = 0;
  for(unsigned int i = 1; i <= n; i++) {
    Vector vnext(p,(*this)[i%n]);
    if(vnext.x == 0 && vnext.y == 0) {
      return 0; // edge
    }
    d += angle(v,vnext);
    v = vnext;
  }
  return fabs(d) > 1 ? 1 : -1; // d always +/- 2PI or 0.
}

class PolygonEdgeOperator : public PointOperator {
  PointOperator &m_op;
  const CPoint  &m_endPoint;
public:
  PolygonEdgeOperator(PointOperator &op, const CPoint &endPoint);
  void apply(const CPoint &p);
};

PolygonEdgeOperator::PolygonEdgeOperator(PointOperator &op, const CPoint &endPoint) : m_op(op), m_endPoint(endPoint) {
}

void PolygonEdgeOperator::apply(const CPoint &p) {
  if(p != m_endPoint) {
    m_op.apply(p);
  }
}

void MyPolygon::applyToEdge(PointOperator &op, bool closingEdge) const {
  int n = size();
  for(int i = 1; i < n; i++) {
    const CPoint &p1 = (*this)[i-1];
    const CPoint &p2 = (*this)[i];
    applyToLine(p1,p2,PolygonEdgeOperator(op,p2));
  }
  if(closingEdge && (n > 2)) {
    const CPoint &p1 = last();
    const CPoint &p2 = (*this)[0];
    applyToLine(p1,p2,PolygonEdgeOperator(op,p2));
  }
}
