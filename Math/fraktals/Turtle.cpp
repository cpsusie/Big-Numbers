#include "stdafx.h"
#include <Math/MathLib.h>
#include <Math/Transformation.h>
#include "Turtle.h"

void Turtle::move(double l) {
  double dx = cos(m_currentdir) * l;
  double dy = sin(m_currentdir) * l;
  moveTo(m_currentp.x + dx, m_currentp.y + dy);
}

void Turtle::moveTo(double x, double y) {
  Point2DP cp = m_tr.forwardTransform(m_currentp);
  m_currentp.x = x;
  m_currentp.y = y;
  Point2DP np = m_tr.forwardTransform(m_currentp);
  CClientDC dc(m_dlg);
  dc.MoveTo(cp);
  dc.LineTo(np);
}

void Turtle::moveTo(const Point2D &p) {
  moveTo(p.x,p.y);
}

void Turtle::jump(double l) {
  double dx = cos(m_currentdir) * l;
  double dy = sin(m_currentdir) * l;
  jumpTo(m_currentp.x + dx,m_currentp.y + dy);
}

void Turtle::jumpTo(double x, double y) {
  m_currentp.x = x;
  m_currentp.y = y;
}

void Turtle::jumpTo(const Point2D &p) {
  m_currentp = p;
}

Point2D Turtle::pos() { 
  return m_currentp;
}

void Turtle::turn(double dir) {
  m_currentdir += GRAD2RAD(dir);
}

void Turtle::turnTo(double dir) {
  m_currentdir = GRAD2RAD(dir);
}

double Turtle::dir(void) { 
  return RAD2GRAD(m_currentdir);
}

void Turtle::init(CDialog *dlg, double minx, double maxx,double miny, double maxy) {
  m_dlg = dlg;
  CRect rect = getClientRect(dlg);
  rect.top += 40; rect.bottom -= 40;
  rect.left += 40; rect.right -= 40;

  m_tr.setToRectangle(Rectangle2D::makeBottomUpRectangle(Rectangle2DR(rect)));
  m_tr.setFromRectangle(Rectangle2D(minx,miny,maxx-minx,maxy-miny));
  jumpTo(0,0);
  turnTo(0);
}

void Turtle::OnSize(CDialog *dlg) {
  CRect rect = getClientRect(dlg);
  rect.top += 40; rect.bottom -= 40;
  rect.left += 40; rect.right -= 40;
  m_tr.setToRectangle(Rectangle2DR(rect));
}

