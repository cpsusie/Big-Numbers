#include "stdafx.h"
#include <CompactHashMap.h>
#include <Math/MathXML.h>
#include <MFCUtil/MFCXml.h>
#include "GridParameters.h"
#include <MedianCut.h>

static void createFullColorDiagram(PixRect &image, PearlDiagram &diagram) {
  const CSize size = image.getSize();
  diagram.clear(size);
  Array<CellSpec>       &sa = diagram.m_cellSpecArray;
  MatrixTemplate<int>   &cm = diagram.m_cellMatrix;
  CompactIntHashMap<int> colorMap;
  CPoint p;
  PixelAccessor *pa = image.getPixelAccessor();
  for(p.y = 0; p.y < size.cy; p.y++) {
    for(p.x = 0; p.x < size.cx; p.x++) {
      const D3DCOLOR c      = pa->getPixel(p);
      const int     *indexp = colorMap.get(c);
      int            index;
      if(indexp) {
        index = *indexp;
        sa[index].m_count++;
      } else {
        index = (int)sa.size();
        sa.add(CellSpec(index, c));
        colorMap.put(c, index);
      }
      cm(p.y,p.x) = index;
    }
  }
  image.releasePixelAccessor();
}

static void reduceColors(PixRect &image, int colorCount, PearlDiagram *diagram) {
  if(colorCount < 2) {
    if(diagram) {
      createFullColorDiagram(image, *diagram);
    }
    return;
  }
  const CSize    size = image.getSize();
  PixelAccessor *pa   = image.getPixelAccessor();
  DimPointWithIndexArray a(getArea(size));
  CPoint p;
  for(p.y = 0; p.y < size.cy; p.y++) {
    for(p.x = 0; p.x < size.cx; p.x++) {
      const D3DCOLOR c = pa->getPixel(p);
      DimPoint dp;
      dp.m_x[0] = (BYTE)ARGB_GETRED(c);
      dp.m_x[1] = (BYTE)ARGB_GETGREEN(c);
      dp.m_x[2] = (BYTE)ARGB_GETBLUE(c);
      a.add(dp);
    }
  }
  DimPointArray dpa = medianCut(a, colorCount);

  #define GETCOLOR(dp) D3DCOLOR_XRGB((BYTE)dp.m_x[0],(BYTE)dp.m_x[1],(BYTE)dp.m_x[2])

  int pixelCount = 0;
  for(p.y = 0; p.y < size.cy; p.y++) {
    for(p.x = 0; p.x < size.cx; p.x++) {
      const DimPoint &dp = dpa[a[pixelCount++].m_index];
      pa->setPixel(p, GETCOLOR(dp));
    }
  }
  if(diagram) {
    const size_t         n  = dpa.size();
    diagram->clear(size,n);
    Array<CellSpec>     &sa = diagram->m_cellSpecArray;
    MatrixTemplate<int> &cm = diagram->m_cellMatrix;
    sa.insert(0,CellSpec(),n);
    int pixelCount = 0;
    for(p.y = 0; p.y < size.cy; p.y++) {
      for(p.x = 0; p.x < size.cx; p.x++) {
        const int index = a[pixelCount++].m_index;
        const DimPoint &dp = dpa[index];
        CellSpec       &cs = sa[index];
        if(cs.m_count == 0) {
          cs.m_color  = GETCOLOR(dp);
          cs.m_count  = 1;
          cs.m_symbol = index;
        } else {
          cs.m_count++;
        }
        cm(p.y,p.x) = index;
      }
    }
  }
  image.releasePixelAccessor();
}

PixRect *GridParameters::calculateImage(const PixRect *image, PearlDiagram *diagram) const {
  const CSize imageSize = image->getSize();
  const CSize cellCount = findCellCount(imageSize);
  PixRect *result;
  if(cellCount == imageSize) {
    result = image->clone(true);
    reduceColors(*result, m_colorCount, diagram);
  } else {
    result = theApp.fetchPixRect(imageSize     );
    PixRect *tmp = theApp.fetchPixRect(cellCount);
    HDC      hdc = tmp->getDC();
    PixRect::stretchBlt(hdc, ORIGIN, cellCount, SRCCOPY, image, ORIGIN, imageSize);
    tmp->releaseDC(hdc);

    reduceColors(*tmp, m_colorCount, diagram);

    hdc = result->getDC();
    PixRect::stretchBlt(hdc, ORIGIN, imageSize, SRCCOPY, tmp, ORIGIN, tmp->getSize());
    result->releaseDC(hdc);
    SAFEDELETE(tmp);
  }
  return result;
}

CSize GridParameters::findCellCount(const CSize &imageSize) const {
  CSize result;
  result.cx = (UINT)round((double)imageSize.cx / m_cellSize);
  result.cy = (UINT)round((double)imageSize.cy / m_cellSize);
  return result;
}

void GridParameters::putDataToDoc(XMLDoc &doc) {
  XMLNodePtr root = doc.createRoot(_T("PearlGrid"));
  setValue(doc, root, _T("cellsize"  ), m_cellSize     );
  setValue(doc, root, _T("cellcount" ), m_cellCount    );
  setValue(doc, root, _T("colorcount"), m_colorCount   );
  setValue(doc, root, _T("cellsizeMM"), m_cellSizeMM   );
  setValue(doc, root, _T("imagefile" ), m_imageFileName);
  setValue(doc, root, _T("imagetime" ), m_fileTime     );
}

void GridParameters::getDataFromDoc(XMLDoc &doc) {
  XMLNodePtr root = doc.getRoot();
  getValue(doc, root, _T("cellsize"  ), m_cellSize     );
  getValue(doc, root, _T("cellcount" ), m_cellCount    );
  getValue(doc, root, _T("colorcount"), m_colorCount   );
  getValue(doc, root, _T("cellsizeMM"), m_cellSizeMM   );
  getValue(doc, root, _T("imagefile" ), m_imageFileName);
  getValue(doc, root, _T("imagetime" ), m_fileTime     );
}

void GridParameters::setFileTime() {
  const struct _stat64 st = STAT64(m_imageFileName);
  m_fileTime = st.st_mtime;
}

void GridParameters::checkFileTime() const {
  const struct _stat64 st = STAT64(m_imageFileName);
  if(m_fileTime != st.st_mtime) {
    throwException(_T("Timestamp mismatch on image file %s"), m_imageFileName.cstr());
  }
}
