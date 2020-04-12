#include "pch.h"
#include <CompactStack.h>
#include <MFCUtil/PixRect.h>

#ifdef _DEBUG
#define CHECKPOINTINSIDE(x,y) checkPoint(__TFUNCTION__, x, y);
#else
#define CHECKPOINTINSIDE(x,y)
#endif

PixelAccessor::PixelAccessor(PixRect *pixRect, DWORD flags) : m_pixRect(*pixRect) {
  m_desc       = m_pixRect.m_desc;
  m_lockedRect = m_pixRect.lockRect(flags);
}

PixelAccessor::~PixelAccessor() {
  m_pixRect.unlockRect();
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
  CompactStack<CPoint> stack(1000);
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
  UINT  m_pixelsPerLine;
public:
  BytePixelAccessor(PixRect *pixRect) : PixelAccessor(pixRect) {
    m_pixels = (BYTE*)m_ddsd.lpSurface;
    m_pixelsPerLine = m_ddsd.lPitch / sizeof(m_pixels[0]);
  }
  void setPixel(UINT x, UINT y, D3DCOLOR color);
  D3DCOLOR getPixel(UINT x, UINT y) const;
};

void BytePixelAccessor::setPixel(UINT x, UINT y, D3DCOLOR color) {
  CHECKPOINTINSIDE(x, y);
  m_pixels[m_pixelsPerLine * y + x] = D3DColorToByteColor(color);
}

D3DCOLOR BytePixelAccessor::getPixel(UINT x, UINT y) const {
  CHECKPOINTINSIDE(x, y);
  return byteColorToD3DColor(m_pixels[m_pixelsPerLine * y + x]);
}

class WordPixelAccessor : public PixelAccessor {
private:
  WORD *m_pixels;
  UINT  m_pixelsPerLine;
public:
  WordPixelAccessor(PixRect *pixRect) : PixelAccessor(pixRect) {
    m_pixels = (WORD*)m_ddsd.lpSurface;
    m_pixelsPerLine = m_ddsd.lPitch / sizeof(m_pixels[0]);
  }
  void setPixel(UINT x, UINT y, D3DCOLOR color);
  D3DCOLOR getPixel(UINT x, UINT y) const;
};

void WordPixelAccessor::setPixel(UINT x, UINT y, D3DCOLOR color) {
  CHECKPOINTINSIDE(x, y);
  m_pixels[m_pixelsPerLine * y + x] = D3DColorToShortColor(color);
}

D3DCOLOR WordPixelAccessor::getPixel(UINT x, UINT y) const {
  CHECKPOINTINSIDE(x, y);
  return shortColorToD3DColor(m_pixels[m_pixelsPerLine * y + x]);
}
#endif

void DWordPixelAccessor::setPixel(UINT x, UINT y, D3DCOLOR color) {
  CHECKPOINTINSIDE(x, y);
  *getPixelAddr(x,y) = color;
}

D3DCOLOR DWordPixelAccessor::getPixel(UINT x, UINT y) const {
  CHECKPOINTINSIDE(x, y);
  return *getPixelAddr(x,y);
}

void DWordPixelAccessor::fillRect(D3DCOLOR color, const CRect *r) {
  UINT left,right,top,bottom;
  if(r == NULL) {
    left   = 0;
    right  = m_desc.Width;
    top    = 0;
    bottom = m_desc.Height;
  } else {
    left   = min((UINT)r->left  , m_desc.Width );
    right  = min((UINT)r->right , m_desc.Width );
    top    = min((UINT)r->top   , m_desc.Height);
    bottom = min((UINT)r->bottom, m_desc.Height);
  }
  for(UINT y = top; y < bottom; y++) {
    for(DWORD *pp = getPixelAddr(left,y), *end = getPixelAddr(right,y); pp < end;) {
      *(pp++) = color;
    }
  }
}
