#include "stdafx.h"
#include <CompactHashMap.h>
#include <Math/Point2D.h>
#include "DFARegex.h"
#include "AutomatePainter.h"

AutomatePainter::AutomatePainter(const CSize &size) : m_size(size) {
  m_dc.CreateCompatibleDC(NULL);
  m_bm.CreateBitmap(m_size.cx, size.cy, 1, 32, NULL);
  m_dc.SelectObject(&m_bm);

  initStaticGDIObjects();
}

AutomatePainter::~AutomatePainter() {
}

void AutomatePainter::flushImage(HDC hdc) {
  BitBlt(hdc, 0, 0, m_size.cx, m_size.cy, m_dc.m_hDC, 0,0, SRCCOPY);
}

void AutomatePainter::setToWhite(HDC hdc) {
  if(hdc == NULL) {
    m_dc.FillSolidRect(0,0,m_size.cx, m_size.cy, RGB(255,255,255));
  } else {
    HGDIOBJ oldBrush = SelectObject(hdc, s_whiteBrush);
    HGDIOBJ oldPen   = SelectObject(hdc, s_whitePen  );
    Rectangle(hdc, 0,0, m_size.cx, m_size.cy);
    SelectObject(hdc, oldPen  );
    SelectObject(hdc, oldBrush);
  }
}

void AutomatePainter::paintCircle(const CPoint &p, int radius, CPen *pen, CBrush *brush, HDC hdc) {
  paintCircle(getDC(hdc), p, radius, pen, brush);
}

void AutomatePainter::paintRing(const CPoint &p, int radius, CPen *pen, CBrush *brush, HDC hdc) {
  paintRing(getDC(hdc), p, radius, pen, brush);
}

void AutomatePainter::textOut(const CPoint &p, CFont &font, const String &text, HDC hdc) {
  textOut(getDC(hdc), p, font, text);
}

void AutomatePainter::paintRing(HDC hdc, const CPoint &p, int radius, CPen *pen, CBrush *brush) { // static
  HGDIOBJ oldPen   = pen   ? SelectObject(hdc, *pen)   : NULL;
  HGDIOBJ oldBrush = brush ? SelectObject(hdc, *brush) : NULL;

  Ellipse(hdc, p.x - radius, p.y - radius, p.x + radius, p.y + radius);

  SelectObject(hdc, s_whitePen);
  SelectObject(hdc, s_whiteBrush);
  radius -= 4;

  Ellipse(hdc, p.x - radius, p.y - radius, p.x + radius, p.y + radius);

  if(pen)   SelectObject(hdc, oldPen  );
  if(brush) SelectObject(hdc, oldBrush);
}

void AutomatePainter::paintCircle(HDC hdc, const CPoint &p, int radius, CPen *pen, CBrush *brush) { // static
  HGDIOBJ oldPen   = pen   ? SelectObject(hdc, *pen)   : NULL;
  HGDIOBJ oldBrush = brush ? SelectObject(hdc, *brush) : NULL;

  Ellipse(hdc, p.x - radius, p.y - radius, p.x + radius, p.y + radius);

  if(pen)   SelectObject(hdc, oldPen  );
  if(brush) SelectObject(hdc, oldBrush);
}

void AutomatePainter::markStartState(HDC hdc, const CPoint &pos) {
  const Point2D  d    = pos;
  Point2D v(1,1);
  v.normalize();
  const Point2D to   = d - v * CIRCLE_RADIUS;
  const Point2D from = d - v * ((float)CIRCLE_RADIUS * 2);
  paintLineArrow(from, to, hdc);
}

void AutomatePainter::textOut(HDC hdc, const CPoint &p, CFont &font, const String &text) { // static
  textOutTransparentBackground(hdc, p, text, font, RGB(0,0,0));
}

ArrowDirection AutomatePainter::getVectorDirection(const Point2D &vector) { // static
  double theta = RAD2GRAD(atan2(vector.y, vector.x));
  if(theta >= 0) {
    if(theta <= 22.5) {
      return DIR_E;
    } else if(theta <= 67.5 ) {
      return DIR_SE;
    } else if(theta <= 112.5) {
      return DIR_S;
    } else if(theta <= 157.5) {
      return DIR_SW;
    } else {
      return DIR_W;
    }
  } else { // theta < 0
    if(theta >= -22.5) {
      return DIR_E;
    } else if(theta >= -67.5 ) {
      return DIR_NE;
    } else if(theta >= -112.5) {
      return DIR_N;
    } else if(theta >= -157.5) {
      return DIR_NW;
    } else {
      return DIR_W;
    }
  }
}

DirectionPair AutomatePainter::paintLineArrow(const CPoint &from, const CPoint &to, HDC hdc) {
  hdc = getDC(hdc);

  HGDIOBJ oldPen = SelectObject(hdc, s_blackPen);
  MoveToEx(hdc, from.x, from.y, NULL);
  LineTo(hdc, to.x, to.y);
  paintArrowEnd(from, to, hdc);
  SelectObject(hdc, oldPen);
  const  ArrowDirection startDir = getVectorDirection(Point2D(to-from));
  return DirectionPair(startDir, getOppositeDirection(startDir));
}

DirectionPair AutomatePainter::paintBezierArrow(const CPoint &from, const CPoint &to, HDC hdc) {
  CPoint bp[4];
  const Point2D v = Point2D(to - from).normalize();
  const Point2D m = (from + to)/2;
  Point2D vr90(v.y, -v.x);
  vr90 *= 15;

  bp[0] = from;
  bp[1] = Point2D(m + vr90);
  bp[2] = bp[1];
  bp[3] = to;

  hdc = getDC(hdc);

  HGDIOBJ oldPen = SelectObject(hdc, s_blackPen);
  PolyBezier(hdc, bp, 4);
  paintArrowEnd(bp[2], to, hdc);
  SelectObject(hdc, oldPen);
  return DirectionPair(getVectorDirection(Point2D(bp[1])-Point2D(from)), getVectorDirection(Point2D(bp[2])-Point2D(to)));
}

void AutomatePainter::paintPathArrow(const TransitionPath &path, HDC hdc) {
  if(path.size() < 2) return;
  hdc = getDC(hdc);
  CPen redPen;
  HGDIOBJ oldPen = SelectObject(hdc, s_blackPen);
  const CPoint &p0 = path[0];
  MoveToEx(hdc, p0.x, p0.y, NULL);
  for(size_t i = 1; i < path.size(); i++) {
    const CPoint p1 = path[i];
    LineTo(hdc, p1.x, p1.y);
  }
  paintArrowEnd(path[1], path[0], hdc);
  SelectObject(hdc, oldPen);
}

const TCHAR *directionToStr(ArrowDirection dir) {
  static const TCHAR *dirNames[] = {
    _T("N") ,_T("NE") ,_T("E") ,_T("SE") ,_T("S") ,_T("SW") ,_T("W") ,_T("NW")
  };
  return dirNames[dir];
}

const Point2D AutomatePainter::s_dirVector[8] = {
  Point2D( 0,-1)             // DIR_N
 ,Point2D( 1,-1).normalize() // DIR_NE
 ,Point2D( 1, 0)             // DIR_E
 ,Point2D( 1, 1).normalize() // DIR_SE
 ,Point2D( 0, 1)             // DIR_S
 ,Point2D(-1, 1).normalize() // DIR_SW
 ,Point2D(-1, 0)             // DIR_W
 ,Point2D(-1,-1).normalize() // DIR_NW
};

const ArrowDirection AutomatePainter::s_oppositeDir[8] = {
  DIR_S
 ,DIR_SW
 ,DIR_W
 ,DIR_NW
 ,DIR_N
 ,DIR_NE
 ,DIR_E
 ,DIR_SE
};

void AutomatePainter::paintLoopArrow(const CPoint &from, int radius, DirectionPair dirPair, const String &text, HDC hdc) {
  hdc = getDC(hdc);

  const Point2D center = from;
  Point2D vs,ve;
  if(dirPair.m_startDir == dirPair.m_endDir) {
    vs = (getDirectionVector(dirPair.m_startDir) * radius).rotate(10);
    ve = vs.rotate(-20);
  } else {
    vs = getDirectionVector(dirPair.m_startDir) * radius;
    ve = getDirectionVector(dirPair.m_endDir  ) * radius;
  }

  const Point2D pFrom = center + vs;
  const Point2D pTo   = center + ve;
  CPoint bp[4];

#define LOOPSIZE 5

  bp[0] = pFrom;
  bp[1] = center + vs * LOOPSIZE;
  bp[2] = center + ve * LOOPSIZE;
  bp[3] = pTo;

  HGDIOBJ oldPen = SelectObject(hdc, s_blackPen);
  PolyBezier(hdc, bp, 4);
  paintArrowEnd(bp[2], pTo, hdc);
  SelectObject(hdc, oldPen);

  CFont &font = getFont();
  const CSize textSize = getTextSize(font, text);

  const Point2D vm = (vs + ve)/2;

  const Point2D origPos = center + vm * 2;
  Point2D textPos;
  switch(dirPair.m_startDir) {
  case DIR_N :
    textPos = origPos;
    textPos.x -= textSize.cx / 2;
    textPos.x = minMax((int)textPos.x, 0, (int)(m_size.cx - textSize.cx));
    textPos.y -= textSize.cy;
    if(textPos.y < 0) {
      if(textPos.x < m_size.cx/2) { // Put text to the right
        textPos = center + vm * (LOOPSIZE/2) + getDirectionVector(DIR_E) * (LOOPSIZE/2);
        textPos.x += 2;
      } else {                      // Put text to the left
        textPos = center + vm * (LOOPSIZE/2) + getDirectionVector(DIR_W) * textSize.cx;
        textPos.x -= 2;
      }
      textPos.y -= textSize.cy/2;
    }
    break;
  case DIR_E :
    textPos = origPos;
    textPos.y -= textSize.cy/2;
    if(textPos.x > m_size.cx - textSize.cx) {
      if(textPos.y < m_size.cy/2) { // Put text below
        textPos = center + vm * (LOOPSIZE/2) + getDirectionVector(DIR_S) * textSize.cy;
        textPos.y += 2;
      } else {                      // Put text above
        textPos = center + vm * (LOOPSIZE/2) + getDirectionVector(DIR_N) * 2*textSize.cy;
        textPos.y -= 2;
      }
      textPos.x -= textSize.cx/2;
    }
    break;
  case DIR_NE:

  case DIR_SE:
  case DIR_S :
    textPos = origPos;
    textPos.x -= textSize.cx / 2;
    if(textPos.y > m_size.cy - textSize.cy) {
      if(textPos.x < m_size.cx/2) { // Put text to the right
        textPos = center + vm * (LOOPSIZE/2) + getDirectionVector(DIR_E) * (LOOPSIZE/2);
        textPos.x += 2;
      } else {                      // Put text to the left
        textPos = center + vm * (LOOPSIZE/2) + getDirectionVector(DIR_W) * textSize.cx;
        textPos.x -= 2;
      }
      textPos.y -= textSize.cy/2;
    }
    break;

  case DIR_SW:

  case DIR_W :
    textPos = origPos;
    textPos.x -= textSize.cx  + 2;
    textPos.y -= textSize.cy/2;
    if(textPos.x < 0) {
      if(textPos.y < m_size.cy/2) { // Put text below
        textPos = center + vm * (LOOPSIZE/2) + getDirectionVector(DIR_S) * textSize.cy;
        textPos.y += 2;
      } else {                      // Put text above
        textPos = center + vm * (LOOPSIZE/2) + getDirectionVector(DIR_N) * 2*textSize.cy;
        textPos.y -= 2;
      }
      textPos.x -= textSize.cx/2;
    }
    break;

  case DIR_NW:
    textPos = origPos;
    break;

  default     :
    throwInvalidArgumentException(__TFUNCTION__, _T("direction="), dirPair.m_startDir);
  }

  textPos.x = minMax((int)textPos.x, 1, (int)(m_size.cx - textSize.cx - 1));
  textPos.y = minMax((int)textPos.y, 1, (int)(m_size.cy - textSize.cy - 1));

  textOutTransparentBackground(hdc, textPos, text, font, RGB(0,0,0));
}

CPoint AutomatePainter::findBestTextPosition(const CPoint pFrom, ArrowDirection direction, const CSize &textSize) const {
  switch(direction) {
  case DIR_N :
  case DIR_E :
    return CPoint(pFrom.x+2, pFrom.y - textSize.cy - 2);

  case DIR_NE:
    { const Point2D &v = getDirectionVector(direction);
      Point2D textPos  = Point2D(pFrom) + v * textSize.cy / -v.y;
      textPos.x += 5;
      return textPos;
    }

  case DIR_SE:
    { const Point2D &v       = getDirectionVector(direction);
      Point2D        textPos = Point2D(pFrom) + v * textSize.cy / v.y;
      textPos.x += 5;
      textPos.y -= textSize.cy;
      return textPos;
    }
  case DIR_S :
    return CPoint(pFrom.x+2, pFrom.y + 2);

  case DIR_SW:
    return CPoint(pFrom.x+5, pFrom.y + 2);

  case DIR_W :
    return CPoint(pFrom.x-textSize.cx - 2, pFrom.y - textSize.cy - 2);

  case DIR_NW:
    { const Point2D &v       = getDirectionVector(direction);
      Point2D        textPos = Point2D(pFrom) + v * textSize.cy / -v.y;
      textPos.x -= textSize.cx;
      return textPos;
    }
  default     :
    throwInvalidArgumentException(__TFUNCTION__, _T("direction="), direction);
    return pFrom;
  }
}

CSize AutomatePainter::getTextSize(CFont &font, const String &text) {
  CFont      *oldFont = m_dc.SelectObject(&font);
  const CSize result  = getTextExtent(m_dc,text);
  m_dc.SelectObject(oldFont);
  return result;
}

void AutomatePainter::paintArrowEnd(const CPoint &from, const CPoint &to, HDC hdc) {
  if(hdc == NULL) hdc = m_dc;

  Point2D v = from - to;
  v.normalize();
  Point2D vleft  = v.rotate(GRAD2RAD( 26)) * 6;
  Point2D vright = v.rotate(GRAD2RAD(-26)) * 6;
  CPoint vertices[3];
  vertices[0] = to;
  vertices[1] = to + vleft;
  vertices[2] = to + vright;
  HGDIOBJ oldBrush = SelectObject(hdc, s_blackBrush);
  Polygon(hdc, vertices, 3);
  SelectObject(hdc, oldBrush);
}

CFont &AutomatePainter::getFont() { // static
  static CFont &defaultFont = getFont(false,Point2D(1,0));
  return defaultFont;
}

CPen   AutomatePainter::s_whitePen;
CPen   AutomatePainter::s_blackPen;
CBrush AutomatePainter::s_blackBrush;
CBrush AutomatePainter::s_whiteBrush;
CBrush AutomatePainter::s_redBrush;
CBrush AutomatePainter::s_orangeBrush;
CBrush AutomatePainter::s_pinkBrush;
CBrush AutomatePainter::s_greenBrush;
CBrush AutomatePainter::s_lightGreenBrush;

void AutomatePainter::initStaticGDIObjects() { // static
  static bool initDone = false;
  if(!initDone) {
    s_blackPen.CreatePen(PS_SOLID, 1,  RGB(  0,  0,  0));
    s_whitePen.CreatePen(PS_SOLID, 1,  RGB(255,255,255));
    s_blackBrush.CreateSolidBrush(     RGB(  0,  0,  0));
    s_whiteBrush.CreateSolidBrush(     RGB(255,255,255));
    s_redBrush.CreateSolidBrush(       RGB(160,  0,  0));
    s_orangeBrush.CreateSolidBrush(    RGB(240,105, 23));
    s_pinkBrush.CreateSolidBrush(      RGB(237,101,121));
    s_greenBrush.CreateSolidBrush(     RGB(  0,127,  0));
    s_lightGreenBrush.CreateSolidBrush(RGB(  0,255,  0));
    initDone = true;
  }
}

FontCache  AutomatePainter::s_fontCache;

FontCache::~FontCache() {
  for(Iterator<Entry<FontKey, CFont*> > it = getEntryIterator(); it.hasNext();) {
    Entry<FontKey, CFont*> &e = it.next();
    e.getValue()->DeleteObject();
    SAFEDELETE(e.getValue());
  }
  clear();
}

typedef struct {
  const TCHAR *m_fontName;
  int m_height, m_width;
} FontInfo;

static const FontInfo fontInfo[2] = {
  _T("Courier"), 8 , 6
 ,_T("Symbol") , 16,10
};

CFont &AutomatePainter::getFont(bool symbol, const Point2D &vector) { // static
  int degree = (int)RAD2GRAD(atan2(vector.y,vector.x));
  const FontKey key(symbol, degree);
  CFont **fontpp = s_fontCache.get(key), *fontp;
  if(fontpp) {
    fontp = *fontpp;
  } else {
    fontp = new CFont(); TRACE_NEW(fontp);
    const FontInfo &fi = fontInfo[symbol?1:0];
    fontp->CreateFont(fi.m_height, fi.m_width, degree*10, degree*10, 400, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS
                     ,CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY
                     ,DEFAULT_PITCH | FF_MODERN
                     ,fi.m_fontName
                     );
    s_fontCache.put(key, fontp);
  }
  return *fontp;
}
