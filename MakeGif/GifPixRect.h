#pragma once

#include "GifUtil.h"

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
