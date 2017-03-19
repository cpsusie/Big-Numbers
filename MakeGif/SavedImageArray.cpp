#include "stdafx.h"
#include "MedianCut.h"
#include "PixRectArray.h"

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

void SavedImageArray::clear() {
  for(size_t i = 0; i < size(); i++) {
    deallocateSavedImage((*this)[i]);
  }
  CompactArray<SavedImage*>::clear();
}

SavedImageArray::SavedImageArray(const PixRectArray &prArray, int maxColorCount) {
  if(maxColorCount > 256) {
    throwInvalidArgumentException(_T("SavedImageArray"), _T("maxColorCount=%d. max=256"), maxColorCount);
  }
  PixelAccessor *pa          = NULL;
  BYTE          *bigColorMap = NULL;
  try {
    BitSet usedColors(MAXCOLORCOUNT);
    for(size_t i = 0; i < prArray.size(); i++) {
      const GifPixRect *pr = prArray[i];
      pa = ((GifPixRect*)pr)->getPixelAccessor();
      const int w = pr->getWidth();
      const int h = pr->getHeight();
      for(CPoint p(0,0); p.y < h; p.y++) {
        for(p.x = 0; p.x < w; p.x++) {
          usedColors.add(pa->getPixel(p) & 0xffffff);
        }
      }
      delete pa; pa = NULL;
    }
    CompactArray<D3DCOLOR> allColors;
    for(Iterator<size_t> it = usedColors.getIterator(); it.hasNext();) {
      const UINT c = (UINT)it.next();
      allColors.add(c);
    }

    const int colorCount = min((int)allColors.size(), maxColorCount);

    DimPointWithIndexArray allPoints = colorArrayToPointArray(allColors);
    DimPointArray          mc        = medianCut(allPoints, colorCount);

    bigColorMap = new BYTE[MAXCOLORCOUNT];
    memset(bigColorMap, 0, MAXCOLORCOUNT);
    for(intptr_t i = allPoints.size(); i--;) {
      bigColorMap[allColors[i]] = allPoints[i].m_index;
    }

    for(size_t i = 0; i < prArray.size(); i++) {
      const GifPixRect *pr = prArray[i];
      const int w = pr->getWidth();
      const int h = pr->getHeight();

      SavedImage *image = allocateSavedImage(w, h, colorCount);
      add(image);

      pa = ((GifPixRect*)pr)->getPixelAccessor();
      GifPixelType  *gifPixel = image->RasterBits;
      for(CPoint p(0,0); p.y < h; p.y++) {
        for(p.x = 0; p.x < w; p.x++) {
          const UINT srcPixel = pa->getPixel(p) & 0xffffff;
          *(gifPixel++) = bigColorMap[srcPixel];
        }
      }
      delete pa; pa = NULL;

      CompactArray<D3DCOLOR> newColors = pointArrayToColorArray(mc);
      GifColorType          *gifColor  = image->ImageDesc.ColorMap->Colors;
      const D3DCOLOR        *cp        = &newColors[0];
      for(int j = colorCount; j--; gifColor++, cp++) {
        gifColor->Red   = ARGB_GETRED(  *cp);
        gifColor->Green = ARGB_GETGREEN(*cp);
        gifColor->Blue  = ARGB_GETBLUE( *cp);
      }
      image->ImageDesc.Left = pr->m_topLeft.x;
      image->ImageDesc.Top  = pr->m_topLeft.y;
      setGCB(image, pr->m_gcb);
    }
    delete[] bigColorMap; bigColorMap = NULL;
  } catch(...) {
    if(pa)          { delete pa;          pa          = NULL; }
    if(bigColorMap) { delete bigColorMap; bigColorMap = NULL; }
    clear();
    throw;
  }
}
