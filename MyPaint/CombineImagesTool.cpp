#include "stdafx.h"
#include "DrawTool.h"

CombineImagesTool::CombineImagesTool(PixRectContainer *container) : DrawTool(container) {
  m_copy = m_old = nullptr;
}

CombineImagesTool::~CombineImagesTool() {
  if(m_copy != nullptr) {
    invertDragRect();
    makeFinalImage();
    releaseCopy();
  }
}

void CombineImagesTool::releaseCopy() {
  SAFEDELETE(m_copy);
  SAFEDELETE(m_old );
}

void CombineImagesTool::invertDragRect() {
  getImage()->rectangle(CRect(m_p0,m_p1),0,true);
  repaint();
}

void CombineImagesTool::OnLButtonDown(UINT nFlags, const CPoint &point) {
  if(m_copy == nullptr) {
    m_p0 = m_p1 = point;
    invertDragRect();
  } else if(m_rect.PtInRect(point)) {
    m_lastPoint = point;
    invertDragRect(); // remove dragrect
  } else {
    invertDragRect(); // remove dragrect
    makeFinalImage();
    releaseCopy();
    m_p0 = m_p1 = point;
    invertDragRect();
  }
}

void CombineImagesTool::OnMouseMove(UINT nFlags, const CPoint &point) {
  if(nFlags & MK_LBUTTON) {
    if(m_copy == nullptr) {
      invertDragRect(); // remove dragrect
      m_p1 = point;
      invertDragRect(); // draw dragRect
    } else {
      m_container->saveDocState();
      getImage()->rop(m_rect.TopLeft(),m_rect.Size(),SRCCOPY, m_old, ORIGIN); // restore old picture
      m_rect += point - m_lastPoint;
      m_p0 += point - m_lastPoint;
      m_p1 += point - m_lastPoint;
      m_lastPoint = point;
      m_old->rop(ORIGIN,m_rect.Size(),SRCCOPY, getImage(),m_rect.TopLeft());
      getImage()->rop(m_rect.TopLeft(),m_rect.Size(),SRCINVERT, m_copy, ORIGIN);
    }
    repaint();
  }
}

void CombineImagesTool::OnLButtonUp(UINT nFlags, const CPoint &point) {
  if(m_copy == nullptr) {
    invertDragRect();   // remove dragrect
    m_rect = CRect(min(m_p0.x,m_p1.x),min(m_p0.y,m_p1.y),max(m_p0.x,m_p1.x),max(m_p0.y,m_p1.y)); // define rect
    if(m_rect.Width() > 0 && m_rect.Height() > 0) {                                              // if valid rectangle
      m_copy = theApp.fetchPixRect(m_rect.Size());                                                       //   define copy
      m_old  = theApp.fetchPixRect(m_rect.Size());                                                       //   define old
      m_copy->rop(ORIGIN,m_rect.Size(),NOTSRCCOPY, getImage(),m_rect.TopLeft());               //   take a copy
      m_old->fillRect(0,0,m_rect.Width(),m_rect.Height(), WHITE);                                 //   take a copy

      invertDragRect(); // draw dragrect
    }
  } else {
    invertDragRect(); // draw dragrect
  }
}

int CombineImagesTool::getCursorId() const {
  return m_copy == nullptr ? IDC_CURSORHAIRCROSS : IDC_CURSORMOVE4WAYS;
}

void CombineImagesTool::makeFinalImage() {
  const int width  = m_rect.Width();
  const int height = m_rect.Height();

  PixRect *m       = m_copy->clone();
  m->rop(ORIGIN,m_rect.Size(),NOTSRCCOPY,m_copy,ORIGIN);

  PixRect *result  = m->clone(true);

  PixelAccessor *mpa = m->getPixelAccessor();
  PixelAccessor *opa = m_old->getPixelAccessor();
  PixelAccessor *rpa = result->getPixelAccessor();
  CPoint p;
  for(p.y = 0; p.y < height; p.y++) {
    for(p.x = 0; p.x < width; p.x++) {
      if((ARGB_TORGB(mpa->getPixel(p) ^ opa->getPixel(p))) != 0) {
        rpa->setPixel(p,ARGB_SETALPHA(BLACK,0));
      }
    }
  }
  result->releasePixelAccessor();
  m_old->releasePixelAccessor();
  m->releasePixelAccessor();

  SAFEDELETE(m);
  getImage()->rop(m_rect.TopLeft(),m_rect.Size(),SRCCOPY,result,ORIGIN);
  SAFEDELETE(result);
}
