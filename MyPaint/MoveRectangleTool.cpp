#include "stdafx.h"
#include <assert.h>
#include "DrawTool.h"

MoveRectangleTool::MoveRectangleTool(PixRectContainer *container) : DrawTool(container) {
  m_copy = m_old = NULL;
  m_dragRectVisible = false;
}

MoveRectangleTool::~MoveRectangleTool() {
  if(m_copy != NULL) {
    invertDragRect();
    releaseCopy();
  }
}

void MoveRectangleTool::releaseCopy() {
  if(m_copy != NULL) {
    delete m_copy;
    delete m_old;
  }
  m_copy = m_old = NULL;
}

void MoveRectangleTool::invertDragRect() {
  static const CSize size(1, 1);
  if(m_dragRectVisible) {
    getImage()->drawDragRect(&m_lastDragRect, CSize(0,0), &m_lastDragRect, size);
  } else {
    m_lastDragRect = CRect(m_p0, m_p1);
    getImage()->drawDragRect(&m_lastDragRect, size, NULL, size);
  }

//  getImage()->rectangle(CRect(m_p0,m_p1),0,true);

  m_dragRectVisible = !m_dragRectVisible;
  repaint();
}

void MoveRectangleTool::OnLButtonDown(UINT nFlags, const CPoint &point) {
  if(m_copy == NULL) {
    m_p0 = m_p1 = point;
    invertDragRect();
    assert(m_dragRectVisible);
  } else if(m_rect.PtInRect(point)) {
    assert(m_dragRectVisible);
    m_lastPoint = point;
    invertDragRect(); // remove dragrect
  } else {
    assert(m_dragRectVisible);
    invertDragRect(); // remove dragrect
    releaseCopy();
    repaint();
    m_p0 = m_p1 = point;
    invertDragRect(); // draw dragRect
    assert(m_dragRectVisible);
  }
}

void MoveRectangleTool::OnMouseMove(UINT nFlags, const CPoint &point) {
  if(nFlags & MK_LBUTTON) {
    if(m_copy == NULL) {
      assert(m_dragRectVisible);
      invertDragRect(); // remove dragrect
      m_p1 = point;
      invertDragRect(); // draw dragRect
      assert(m_dragRectVisible);
    } else {
      m_container->saveDocState();
      getImage()->rop(m_rect.TopLeft(), m_rect.Size(), SRCCOPY, m_old, ORIGIN); // restore old picture
      m_rect += point - m_lastPoint;
      m_p0 += point - m_lastPoint;
      m_p1 += point - m_lastPoint;
      m_lastPoint = point;
      m_old->rop(ORIGIN,m_rect.Size(), SRCCOPY, getImage(), m_rect.TopLeft());
      getImage()->rop(m_rect.TopLeft(), m_rect.Size(), SRCCOPY, m_copy, ORIGIN);
      repaint();
    }
  }
}

void MoveRectangleTool::OnLButtonUp(UINT nFlags, const CPoint &point) {
  if(m_copy == NULL) {
    assert(m_dragRectVisible);
    invertDragRect();   // remove dragrect
    m_rect = CRect(min(m_p0.x, m_p1.x), min(m_p0.y, m_p1.y), max(m_p0.x, m_p1.x), max(m_p0.y, m_p1.y)); // define rect
    if(m_rect.Width() > 0 && m_rect.Height() > 0) {                                                     // if valid rectangle
      m_copy = theApp.fetchPixRect(m_rect.Size());                                                              //   define copy
      m_old  = theApp.fetchPixRect(m_rect.Size());                                                              //   define old
      m_copy->rop(ORIGIN, m_rect.Size(), SRCCOPY, getImage(), m_rect.TopLeft());                        //   take a copy
      m_old->rop( ORIGIN, m_rect.Size(), SRCCOPY, getImage(), m_rect.TopLeft());                        //   take a copy

      invertDragRect(); // draw dragrect
      assert(m_dragRectVisible);
    }
  } else {
    invertDragRect(); // draw dragrect
    assert(m_dragRectVisible);
  }
}

void MoveRectangleTool::cut() {

}

void MoveRectangleTool::copy() {
  if(m_copy == NULL) {
    DrawTool::copy();
  } else {
    HBITMAP bm = *m_copy;
    putClipboard(AfxGetMainWnd()->m_hWnd, bm);
    DeleteObject(bm);
  }
}

int MoveRectangleTool::getCursorId() const {
  return m_copy == NULL ? IDC_CURSORHAIRCROSS : IDC_CURSORMOVE4WAYS;
}
