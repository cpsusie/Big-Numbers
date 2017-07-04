#include "stdafx.h"
#include <ClipBoard.h>
#include "DrawTool.h"

InsertImageTool::InsertImageTool(PixRectContainer *container, PixRect *image) : DrawTool(container) {
  m_image = image;
  m_old = m_image->clone();
  m_upperLeft = CPoint(0,0);
  m_container->saveDocState();
  paintImage();
  invertImageFrame();
  repaint();
}

InsertImageTool::~InsertImageTool() {
  delete m_old;
  delete m_image;
}

void InsertImageTool::paintImage() {
  m_old->rop(ORIGIN,m_image->getSize(),SRCCOPY,getPixRect(),m_upperLeft);
  getPixRect()->rop(m_upperLeft,m_image->getSize(),SRCCOPY,m_image,ORIGIN);
}

void InsertImageTool::restoreOld() {
  getPixRect()->rop(m_upperLeft,m_image->getSize(),SRCCOPY,m_old,ORIGIN);
}

void InsertImageTool::invertImageFrame() {
  getPixRect()->rectangle(getImageRect(),0,true);
}

CRect InsertImageTool::getImageRect() const {
  return CRect(m_upperLeft,m_image->getSize());
}

void InsertImageTool::OnLButtonDown(UINT nFlags, const CPoint &point) {
  if(getImageRect().PtInRect(point)) {
    m_lastPoint = point;
    m_grabbedImage = true;
    invertImageFrame();
    repaint();
  } else {
    invertImageFrame();
    repaint();
    m_container->restoreOldTool();
  }
}

void InsertImageTool::OnMouseMove(UINT nFlags, const CPoint &point) {
  if(nFlags & MK_LBUTTON && m_grabbedImage) {
    CPoint dp = point - m_lastPoint;
    restoreOld();
    m_upperLeft += dp;
    paintImage();
    repaint();
    m_lastPoint = point;
  }
}

void InsertImageTool::OnLButtonUp(UINT nFlags, const CPoint &point) {
  invertImageFrame();
  repaint();
}

void InsertImageTool::OnDelete() {
  invertImageFrame();
  restoreOld();
  repaint();
  m_container->restoreOldTool();
}

int InsertImageTool::getCursorId() const {
  return IDC_CURSORMOVE4WAYS;
}
