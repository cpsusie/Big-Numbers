#include "pch.h"
#include <Math.h>
#include <float.h>
#include <MFCUtil/PixRect.h>

// ------------------------------------ GlyphPolygon ------------------------------

Point2DArray GlyphPolygon::getAllPoints() const {
  Point2DArray result;
  result.add(m_start);
  for(size_t i = 0; i < m_polygonCurveArray.size(); i++) {
    result.addAll(m_polygonCurveArray[i].getAllPoints());
  }
  return result;
}

Rectangle2D GlyphPolygon::getBoundingBox() const {
  return getAllPoints().getBoundingBox();
}

// ------------------------------------ GlyphData ------------------------------

GlyphData::GlyphData(PixRectDevice &device, HDC hdc, _TUCHAR ch, const MAT2 &m) : m_glyphCurveData(hdc, ch, m) {
  m_pixRect = nullptr;
  m_ch      = ch;
  DWORD buffersize = GetGlyphOutline(hdc,ch, GGO_BITMAP,&m_metrics,0,nullptr,&m);
  if(buffersize == GDI_ERROR) {
    return;
  }

  DWORD *buffer = new DWORD[buffersize/sizeof(DWORD)]; TRACE_NEW(buffer);
  GetGlyphOutline(hdc,ch, GGO_BITMAP,&m_metrics,buffersize,buffer,&m);
  const int width  = m_metrics.gmBlackBoxX;
  const int height = m_metrics.gmBlackBoxY;
  m_pixRect = new PixRect(device, PIXRECT_PLAINSURFACE, width,height); TRACE_NEW(m_pixRect);
  m_pixRect->fillRect(0,0,width,height,BLACK);

  PixelAccessor *pa = m_pixRect->getPixelAccessor();
  for(int y = 0; y < height; y++) {
    int startIndex = y * ((width+31)/32);
    char *rowBits = (char*)&buffer[startIndex];
    for(int x = 0; x < width; rowBits++) {
      for(BYTE mask = 0x80; mask && x < width; x++, mask >>= 1) {
        if(*rowBits&mask) {
          pa->setPixel(x,y,D3D_WHITE);
        }
      }
    }
  }
  m_pixRect->releasePixelAccessor();
  SAFEDELETEARRAY(buffer);
}

GlyphData::~GlyphData() {
  SAFEDELETE(m_pixRect);
}

// ------------------------------------ GlyphCurveData ------------------------------

GlyphCurveData::GlyphCurveData(HDC hdc, _TUCHAR ch, const MAT2 &m) {
  GLYPHMETRICS m_metrics;
  DWORD buffersize = GetGlyphOutline(hdc,ch, GGO_BEZIER,&m_metrics,0,nullptr,&m);
  if(buffersize == GDI_ERROR) {
    return;
  }

  char *buffer = new char[buffersize]; TRACE_NEW(buffer);
  GetGlyphOutline(hdc,ch, GGO_BEZIER,&m_metrics,buffersize,buffer,&m);

  for(int index = 0; index < (int)buffersize;) {
    TTPOLYGONHEADER *header = (TTPOLYGONHEADER*)&buffer[index];
    GlyphPolygon     polygon(header->pfxStart);
    TTPOLYCURVE     *c;
    int              h;
    for(h = sizeof(TTPOLYGONHEADER); h < (int)header->cb; h += sizeof(TTPOLYCURVE)+(c->cpfx-1)*sizeof(POINTFX)) {
      c = (TTPOLYCURVE*)&buffer[index+h];
      PolygonCurve2D curve(c->wType);
      for(int i = 0; i < c->cpfx; i++) {
        curve.addPoint(Point2D(c->apfx[i]));
      }
      polygon.addCurve(curve);
    }
    addPolygon(polygon);
    index += h;
  }
  SAFEDELETEARRAY(buffer);
}

Point2DArray GlyphCurveData::getAllPoints() const {
  Point2DArray result;
  for(size_t i = 0; i < m_glyphPolygonArray.size(); i++) {
    result.addAll(m_glyphPolygonArray[i].getAllPoints());
  }
  return result;
}

Rectangle2D GlyphCurveData::getBoundingBox() const {
  return getAllPoints().getBoundingBox();
}

// ------------------------------------ PixRectFont ------------------------------

PixRectFont::PixRectFont(PixRectDevice &device) : m_device(device) {
  m_font.CreateFont( 10, 8, 0, 0, 400, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
                     CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                     DEFAULT_PITCH | FF_MODERN,
                     _T("Times new roman")
                   );
  initGlyphData(0);
}

PixRectFont::PixRectFont(PixRectDevice &device, const LOGFONT &logfont, float orientation) : m_device(device) {
  m_font.CreateFontIndirect(&logfont);
  initGlyphData(orientation);
}

PixRectFont::~PixRectFont() {
  m_font.DeleteObject();
  for(size_t i = 0; i < m_glyphData.size(); i++) {
    GlyphData *gd = m_glyphData[i];
    SAFEDELETE(gd);
  }
  m_glyphData.clear();
}

void PixRectFont::initGlyphData(float orientation) {
  HDC screenDC = getScreenDC();
  SelectObject(screenDC,m_font.m_hObject);
  GetTextMetrics(screenDC,&m_textMetrics);
  MAT2 m = orientation == 0 ? getMAT2Id() : getMAT2Rotation(orientation);

  for(int i = 0; i < 256; i++) {
    GlyphData *gd = new GlyphData(m_device, screenDC,i, m); TRACE_NEW(gd);
    m_glyphData.add(gd);
  }
  DeleteDC(screenDC);
}

void PixRect::text(const CPoint &p, const String &text, const PixRectFont &font, D3DCOLOR color, bool invert) {
  int x = p.x;
  int y = p.y;
  PixRect *csrc = new PixRect(m_device,PIXRECT_PLAINSURFACE, font.getTextMetrics().tmMaxCharWidth,font.getTextMetrics().tmHeight); TRACE_NEW(csrc);
  csrc->fillRect(0,0,csrc->getWidth(),csrc->getHeight(),color);
  for(int i = 0; text[i] != '\0'; i++) {
    _TUCHAR ch = text[i];
    const GlyphData *gd = font.getGlyphData(ch);
    if(gd->m_pixRect != nullptr) {
      mask(x+gd->m_metrics.gmptGlyphOrigin.x,y-gd->m_metrics.gmptGlyphOrigin.y,gd->m_pixRect->getWidth(),gd->m_pixRect->getHeight()
          ,SRCCOPY,csrc,0,0,gd->m_pixRect);
      x += gd->m_metrics.gmCellIncX;
      y += gd->m_metrics.gmCellIncY;
    }
  }
  SAFEDELETE(csrc);
}

void applyToGlyphPolygon(const GlyphPolygon &polygon, CurveOperator &op) {
  const Point2D *pp = &polygon.getStart();
  op.beginCurve();
  op.apply(*pp);
  const Array<PolygonCurve2D> &a = polygon.getCurveArray();
  const size_t curveCount = a.size();
  for(size_t i = 0; i < curveCount; i++) {
    const PolygonCurve2D &curve = a[i];
    switch(curve.getType()) {
    case TT_PRIM_LINE   :
      { const Point2DArray &pa = curve.getAllPoints();
        for(size_t j = 0; j < pa.size(); j++) {
          const Point2D &np = pa[j];
          op.apply(np);
          pp = &np;
        }
      }
      break;
    case TT_PRIM_QSPLINE:
      { String str = curve.toString();
        int f = 1;
      }
      break;
    case TT_PRIM_CSPLINE:
      { const Point2DArray &pa = curve.getAllPoints();
        for(size_t j = 0; j < pa.size(); j+=3) {
          const Point2D &end = pa[j+2];
          applyToBezier(*pp,pa[j],pa[j+1],end, op,false);
          pp = &end;
        }
      }
      break;
    }
  }
  op.apply(polygon.getStart());
  op.endCurve();
}

void applyToGlyph(const GlyphCurveData &glyphCurve, CurveOperator &op) {
  const Array<GlyphPolygon> &pa = glyphCurve.getPolygonArray();
  for(size_t i = 0; i < pa.size(); i++) {
    applyToGlyphPolygon(pa[i], op);
  }
}

void applyToText(const String &text, const PixRectFont &font, TextOperator &op) {
  Point2D chPos(0,0);
  const size_t len = text.length();
  for(size_t i = 0; i < len; i++) {
    const _TUCHAR    ch = text[i];
    const GlyphData *gd = font.getGlyphData(ch);
    op.beginGlyph(chPos);
    if(gd->m_pixRect != nullptr) {
      applyToGlyph(gd->m_glyphCurveData,op);
      chPos.x += gd->m_metrics.gmCellIncX;
      chPos.y += gd->m_metrics.gmCellIncY;
    }
    op.endGlyph();
  }
}

void PixRect::drawGlyph(const CPoint &p, const GlyphCurveData &glyph, D3DCOLOR color, bool invert) {
  applyToGlyph(glyph,PixRectTextMaker(this,p,color,invert));
}

void PixRect::drawText(const CPoint &p, const String &text, const PixRectFont &font, D3DCOLOR color, bool invert) {
  applyToText(text,font,PixRectTextMaker(this,p,color,invert));
}
