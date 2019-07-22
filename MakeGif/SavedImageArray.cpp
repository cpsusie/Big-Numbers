#include "stdafx.h"
#include <MedianCut.h>
#include "SavedImageArray.h"

#define MAXCOLORCOUNT 0x1000000

//#define LAB_CONVERSION

#ifdef LAB_CONVERSION

//#define WHITEREFERENCE CIEXYZ1931Color::D65Normalized
#define WHITEREFERENCE CIEXYZ1931Color::monitorWhite

static DimPoint colorToPoint(D3DCOLOR c) {
  const CIELABColor lab(CIEXYZ1931Color(RGBColor(c)), WHITEREFERENCE);
  DimPoint p;
  p.x[0]    = lab.m_L;
  p.x[1]    = lab.m_a;
  p.x[2]    = lab.m_b;
  return p;
}

static D3DCOLOR pointToColor(const DimPoint &p) {
  const CIELABColor lab(p.x[0], p.x[1], p.x[2]);
  return RGBColor(lab.getCIEXYZ1931Color(WHITEREFERENCE));
}

#else

static DimPoint colorToPoint(D3DCOLOR c) {
  DimPoint p;
/*
  RGBColor rgb(c);
  p.x[0]    = rgb.m_red;
  p.x[1]    = rgb.m_green;
  p.x[2]    = rgb.m_blue;
*/
  p.m_x[0]    = (PointDimensionType)ARGB_GETRED(  c);
  p.m_x[1]    = (PointDimensionType)ARGB_GETGREEN(c);
  p.m_x[2]    = (PointDimensionType)ARGB_GETBLUE( c);
  return p;
}

static D3DCOLOR pointToColor(const DimPoint &p) {
//  return RGBColor(p.x[0],p.x[1],p.x[2]);
  const UINT r = (UINT)p.m_x[0];
  const UINT g = (UINT)p.m_x[1];
  const UINT b = (UINT)p.m_x[2];
  return D3DCOLOR_ARGB(0xff,r,g,b);
}

#endif

DimPointWithIndexArray colorArrayToPointArray(const CompactArray<D3DCOLOR> &colors) {
  DimPointWithIndexArray result;
  for(size_t i = 0; i < colors.size(); i++)  {
    result.add(colorToPoint(colors[i]));
  }
  return result;
}

CompactArray<D3DCOLOR> pointArrayToColorArray(const DimPointArray &pa) {
  CompactArray<D3DCOLOR> result;
  for(size_t i = 0; i < pa.size(); i++) {
    result.add(pointToColor(pa[i]));
  }
  return result;
}

SavedImageArray::~SavedImageArray() {
  clear();
}

void SavedImageArray::clear(intptr_t capacity) {
  for(size_t i = 0; i < size(); i++) {
    deallocateSavedImage((*this)[i]);
  }
  __super::clear(capacity);
}

class ReducedRasterMap {
private:
  CompactArray<GifColorType> m_colorMap;
  CompactArray<GifPixelType> m_rasterBits;
public:
  ReducedRasterMap(PixRect *pr, UINT colorCount);
  const CompactArray<GifColorType> &getColorMap() const {
    return m_colorMap;
  }
  const CompactArray<GifPixelType> &getRasterBits() const {
    return m_rasterBits;
  }
};

ReducedRasterMap::ReducedRasterMap(PixRect *pr, UINT colorCount) {
  const CSize            size = pr->getSize();
  const size_t           pixelCount = getArea(size);
  DimPointWithIndexArray a(pixelCount);
  PixelAccessor         *pa = NULL;
  try {
    pa = pr->getPixelAccessor();
    CPoint p;
    for(p.y = 0; p.y < size.cy; p.y++) {
      for (p.x = 0; p.x < size.cx; p.x++) {
        const D3DCOLOR c = pa->getPixel(p);
        DimPoint dp;
        D3DCOLORTODP(dp,c);
        a.add(dp);
      }
    }
    pr->releasePixelAccessor(); pa = NULL;
  } catch(...) {
    if(pa) {
      pr->releasePixelAccessor(); pa = NULL;
      throw;
    }
  }
  DimPointArray dpa = medianCut(a, colorCount);

  m_rasterBits.setCapacity(pixelCount);
  int counter = 0;
  CPoint p;
  for(p.y = 0; p.y < size.cy; p.y++) {
    for(p.x = 0; p.x < size.cx; p.x++, counter++) {
      m_rasterBits.add(a[counter].m_index);
    }
  }
  m_colorMap.setCapacity(colorCount);
  for(size_t i = 0; i < dpa.size(); i++) {
    const DimPoint &dp = dpa[i];
    const D3DCOLOR  c = DPTOD3DCOLOR(dp);
    GifColorType    gc;
    D3DCOLORTOGC(gc, c);
    m_colorMap.add(gc);
  }
}

SavedImageArray::SavedImageArray(const PixRectArray &prArray, int maxColorCount) {
  if(maxColorCount > 256) {
    throwInvalidArgumentException(__TFUNCTION__, _T("maxColorCount=%d. max=256"), maxColorCount);
  }
  try {
    const size_t n = prArray.size();
    for(size_t i = 0; i < n; i++) {
      GifPixRect    *pr   = prArray[i];
      const CSize    size = pr->getSize();

      const ReducedRasterMap reducedMap(pr, maxColorCount);
      SavedImage *image = allocateSavedImage(size.cx, size.cy, maxColorCount);
      add(image);

      GifPixelType                    *gifPixel     = image->RasterBits;
      const CompactArray<GifPixelType> &rasterBits  = reducedMap.getRasterBits();
      const size_t                      totalPixels = rasterBits.size();
      for(size_t i = 0; i < totalPixels; i++) {
        gifPixel[i] = rasterBits[i];
      }

      const CompactArray<GifColorType> &colorMap    = reducedMap.getColorMap();
      GifColorType                     *gifColor    = image->ImageDesc.ColorMap->Colors;
      const size_t                      totalColors = colorMap.size();
      for(size_t i = 0; i < totalColors; i++) {
        gifColor[i] = colorMap[i];
      }
      image->ImageDesc.Left = pr->m_topLeft.x;
      image->ImageDesc.Top  = pr->m_topLeft.y;
      setGCB(image, pr->m_gcb);
    }
  } catch(...) {
    clear();
    throw;
  }
}
