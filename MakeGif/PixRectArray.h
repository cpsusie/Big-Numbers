#pragma once

#include <CompactArray.h>
#include <PropertyContainer.h>
#include "GifUtil.h"

class GifPixRect : public PixRect {
private:
  void copyImage(const PixRect *src);
  void copyRasterBits(GifPixelType *rasterBits, const GifColorType *colorMap);
  static GraphicsControlBlock getDefaultGCB();
public:
  CPoint               m_topLeft;
  GraphicsControlBlock m_gcb;
  GifPixRect(const CSize &size);
  GifPixRect(int w, int h);
  GifPixRect(GifFileType *gifFile, int i);
  GifPixRect(const SavedImage *image, const ColorMapObject *globalColorMap);
  GifPixRect *clone(double scaleFactor = 1.0) const;
  static GifPixRect *GifPixRect::load(const String &fileName);
};

typedef enum {
  UPDATECOUNTER
} PixRectArrayProperty;

class PixRectArray : public CompactArray<GifPixRect*>, public PropertyContainer {
private:
  unsigned int m_updateCounter;
  void incrUpdateCount();
public:
  PixRectArray() {
    m_updateCounter = 0;
  }
  bool needUpdate(const PixRectArray &src) const;
  PixRectArray(const StringArray &fileNames, StringArray &errors);
  PixRectArray(const PixRectArray &src); // not defined
  ~PixRectArray();
  PixRectArray &operator=(const PixRectArray &src);
  PixRectArray &scale(double scaleFactor);
  PixRectArray &quantize(int maxColorCount);
  void clear();
  void append(const PixRectArray &a);
  void remove(unsigned int index, unsigned int count = 1);
  void paintAll(HDC dc, const CRect &rect) const;
  static void paintPixRect(const PixRect *pr, HDC dc, const CRect &rect);
  CSize getMinSize() const;
  CSize getMaxSize() const;
  unsigned int getUpdateCount() const {
    return m_updateCounter;
  }
};

class SavedImageArray : public CompactArray<SavedImage*> {
public:
  SavedImageArray(const PixRectArray &prArray, int maxColorCount);
  SavedImageArray(const SavedImageArray &src);            // not defined
  SavedImageArray &operator=(const SavedImageArray &src); // not defined
  ~SavedImageArray();
  void clear();
};
