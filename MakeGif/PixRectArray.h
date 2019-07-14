#pragma once

#include <PropertyContainer.h>
#include "GifPixRect.h"

typedef enum {
  UPDATECOUNTER // size_t
} PixRectArrayProperty;

class PixRectArray : public CompactArray<GifPixRect*>, public PropertyContainer {
private:
  size_t m_updateCounter;
  void incrUpdateCount();
  PixRectArray(const PixRectArray &src); // not defined
public:
  PixRectArray() {
    m_updateCounter = 0;
  }
  bool needUpdate(const PixRectArray &src) const;
  PixRectArray(const StringArray &fileNames, StringArray &errors);
  ~PixRectArray();
  PixRectArray &operator=(const PixRectArray &src);
  PixRectArray &scale(double scaleFactor);
  PixRectArray &setAllToSize(const CSize &newSize);
  PixRectArray &quantize(int maxColorCount);
  BitSet needsResize(const CSize &newSize) const;
  void clear(intptr_t capacity = 0);
  void append(const PixRectArray &a);
  void remove(UINT index, UINT count = 1);
  void paintAll(HDC dc, const CRect &rect) const;
  static void paintPixRect(const PixRect *pr, HDC dc, const CRect &rect);
  CSize getMinSize() const;
  CSize getMaxSize() const;
  size_t getUpdateCount() const {
    return m_updateCounter;
  }
  int findImageIndexByPoint(const CPoint &p) const; // return index of the image containing p, -1 if none
};
