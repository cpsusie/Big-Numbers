#include "stdafx.h"
#include <MyUtil.h>
#include <Stack.h>
#include <MFCUtil/PixRect.h>

PixelAccessor::PixelAccessor(PixRect *pixRect, DWORD flags) : m_pixRect(*pixRect) {
  m_desc       = m_pixRect.m_desc;
  m_lockedRect = m_pixRect.lockRect(flags);
}

PixelAccessor::~PixelAccessor() {
  m_pixRect.unlockRect();
}

PixelAccessor *PixelAccessor::createPixelAccessor(PixRect *pixRect, DWORD flags) {
  DEFINEMETHODNAME;
  switch(pixRect->getPixelFormat()) {
    //  case 8 : return new BytePixelAccessor(pixRect);
    //  case 16: return new WordPixelAccessor(pixRect);
  case D3DFMT_A8R8G8B8:
  case D3DFMT_X8R8G8B8:
    return new DWordPixelAccessor(pixRect, flags);
  default:
    throwException(_T("%s:Unknown pixel format:%d. Must be 8, 16 or 32"),method, pixRect->getPixelFormat());
    return NULL;
  }
}

void PixelAccessor::fill(const CPoint &p, D3DCOLOR color) {
  fill(p, color, SimpleColorComparator());
}

void PixelAccessor::fill(const CPoint &p, D3DCOLOR color, ColorComparator &cmp) {
  if(!m_pixRect.contains(p)) {
    return;
  }
  D3DCOLOR oldColor = getPixel(p);
  if(cmp.equals(color, oldColor)) {
    return;
  }
  const int width  = m_pixRect.getWidth();
  const int height = m_pixRect.getHeight();
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

#ifdef __NEVER__

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

  return D3DCOLOR_XRGB(r,g,b);
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
#endif

DEFINECLASSNAME(DWordPixelAccessor);

void DWordPixelAccessor::setPixel(UINT x, UINT y, D3DCOLOR color) {
#ifdef _DEBUG
  checkPoint(s_className, __TFUNCTION__, x, y);
#endif
  m_pixels[m_pixelsPerLine * y + x] = color;
}

D3DCOLOR DWordPixelAccessor::getPixel(UINT x, UINT y) {
#ifdef _DEBUG
  checkPoint(s_className, __TFUNCTION__, x, y);
#endif
  return m_pixels[m_pixelsPerLine * y + x];
}

void DWordPixelAccessor::setPixel(const CPoint &p, D3DCOLOR color) {
#ifdef _DEBUG
  checkPoint(s_className, __TFUNCTION__, p.x, p.y);
#endif
  m_pixels[m_pixelsPerLine * p.y + p.x] = color;
}

D3DCOLOR DWordPixelAccessor::getPixel(const CPoint &p) {
#ifdef _DEBUG
  checkPoint(s_className, __TFUNCTION__, p.x, p.y);
#endif
  return m_pixels[m_pixelsPerLine * p.y + p.x];
}
