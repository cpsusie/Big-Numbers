#include "stdafx.h"
#include "EdgeMark.h"

static CBrush whiteBrush(WHITE);
static CPen   blackPen(PS_SOLID, 1, BLACK);

int EdgeMark::getCursorId() const {
  switch(m_type) {
  case RIGHTMARK      : return OCR_SIZEWE;
  case BOTTOMMARK     : return OCR_SIZENS;
  case RIGHTBOTTOMMARK: return OCR_SIZENWSE;
  default             : return OCR_SIZEALL;
  }
}

#define EDGEWINSIZE 26

CRect EdgeMark::createRect(const CPoint &p, int size) const {
  switch(m_type) {
  case RIGHTMARK      : return CRect(p.x        ,p.y-size/2 , p.x+size  ,p.y+size/2);
  case BOTTOMMARK     : return CRect(p.x-size/2 ,p.y        , p.x+size/2,p.y+size  );
  case RIGHTBOTTOMMARK: return CRect(p.x        ,p.y        , p.x+size  ,p.y+size  );
  default             : return CRect(0,0,0,0);
  }
}

CRect EdgeMark::createActiveRect(const CPoint &p) const {
  return createRect(p,EDGEWINSIZE);
}

CRect EdgeMark::createVisibleRect(const CPoint &p) const {
  return createRect(p,EDGEMARKSIZE);
}

void EdgeMark::setPosition(const CPoint &p) {
  m_activeRect  = createActiveRect(p);
  m_visibleRect = createVisibleRect(p);
  setVisible(true);
}

#ifdef _DEBUG
const TCHAR *typeToStr(EdgeMarkType type) {
  switch(type) {
#define CASETYPE(t) case t##MARK:return _T(#t)
  CASETYPE(RIGHT);
  CASETYPE(BOTTOM);
  CASETYPE(RIGHTBOTTOM);
  }
  return _T("?");
}

String EdgeMark::toString() const {
  if(!isVisible()) {
    return format(_T("%s:Invisible%18s"), typeToStr(getType()),_T(""));
  } else {
    return format(_T("%s:Visible  :(%3d,%3d,%3d,%3d)")
                 ,typeToStr(getType())
                 ,m_visibleRect.left , m_visibleRect.top
                 ,m_visibleRect.right, m_visibleRect.bottom
                 );
  }
}
#endif // _DEBUG

EdgeMarkArray::EdgeMarkArray() {
  add(EdgeMark(RIGHTMARK      ));
  add(EdgeMark(BOTTOMMARK     ));
  add(EdgeMark(RIGHTBOTTOMMARK));
  m_visibleCount = 0;
}

void EdgeMarkArray::setPosition(EdgeMarkType type, const CPoint &pos) {
  (*this)[type].setPosition(pos);
  m_visibleCount++;
}

void EdgeMarkArray::clear() {
  for(size_t i = 0; i < size(); i++) {
    (*this)[i].setVisible(false);
  }
  m_visibleCount = 0;
}

void EdgeMarkArray::paintAll(CDC &dc) {
  if(getVisibleCount() > 0) {
    CBrush whiteBrush(WHITE);
    CPen blackPen;
    blackPen.CreatePen(PS_SOLID, 1, BLACK);
    CBrush *pOldBrush = dc.SelectObject(&whiteBrush);
    CPen   *pOldPen   = dc.SelectObject(&blackPen  );
    for(size_t i = 0; i < size(); i++) {
      const EdgeMark &m = (*this)[i];
      if(m.isVisible()) {
        dc.Rectangle(m.getVisibleRect());
      }
    }
    dc.SelectObject(pOldBrush);
    dc.SelectObject(pOldPen  );
  }
}

const EdgeMark *EdgeMarkArray::findEdgeMark(const CPoint &point) const {
  for(int i = 0; i < size(); i++) {
    const EdgeMark &m = (*this)[i];
    if(m.isVisible() && m.getActiveRect().PtInRect(point)) {
      return &m;
    }
  }
  return NULL;
}

#ifdef _DEBUG
String EdgeMarkArray::toString() const {
  String result;
  for (size_t i = 0; i < size(); i++) {
    result += (*this)[i].toString();
  }
  result += format(_T(" VisibleCount:%d"), m_visibleCount);
  return result;
}
#endif // _DEBUG
