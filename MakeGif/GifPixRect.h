#pragma once

#include "GifUtil.h"

// Conversion between D3DCOLOR <-> DimPoint
#define D3DCOLORTODP(dp,d3c) {            \
  (dp).m_x[0] = (BYTE)ARGB_GETRED(  d3c); \
  (dp).m_x[1] = (BYTE)ARGB_GETGREEN(d3c); \
  (dp).m_x[2] = (BYTE)ARGB_GETBLUE( d3c); \
}
#define DPTOD3DCOLOR(dp) D3DCOLOR_XRGB((BYTE)((dp).m_x[0]),(BYTE)((dp).m_x[1]),(BYTE)((dp).m_x[2]))

// Conversion between D3DCOLOR <-> GifColorType
#define D3DCOLORTOGC(gc,d3c) {            \
  (gc).Red    = ARGB_GETRED(  d3c);       \
  (gc).Green  = ARGB_GETGREEN(d3c);       \
  (gc).Blue   = ARGB_GETBLUE( d3c);       \
}
#define GCTOD3DCOLOR(gc) D3DCOLOR_XRGB((gc).Red, (gc).Green, (gc).Blue)

class GifPixRect : public PixRect {
private:
  void copyImage(const PixRect *src);
  void copyRasterBits(GifPixelType *rasterBits, const GifColorType *colorMap);
  static GraphicsControlBlock getDefaultGCB();
  void init();
public:
  CPoint               m_topLeft;
  CRect                m_winRect; // rectangle where this is painted. size may differ from PixRect.size
  GraphicsControlBlock m_gcb;
  GifPixRect(const CSize &size);
  GifPixRect(int w, int h);
  GifPixRect(GifFileType *gifFile, int i);
  GifPixRect(const SavedImage *image, const ColorMapObject *globalColorMap);
  GifPixRect *clone(double scaleFactor = 1.0) const;
  GifPixRect *clone(const CSize &newSize) const;
  static GifPixRect *GifPixRect::load(const String &fileName);
};
