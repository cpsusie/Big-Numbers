#include "stdafx.h"
#include "Maze.h"

Maze::Maze(const CRect &rect, int doorWidth) {
  m_rect      = rect;
  m_doorWidth = doorWidth;
}

void Maze::createPens() {
  m_wallPen.CreatePen(PS_SOLID|PS_GEOMETRIC|PS_ENDCAP_SQUARE, 1, BLACK);
  m_erasePen.CreatePen(PS_SOLID|PS_GEOMETRIC|PS_ENDCAP_SQUARE, 1, WHITE);
}

void Maze::fillRectangle(CDC &dc, const CRect &r, COLORREF color) {
  dc.FillSolidRect(r.left,r.top,r.right,r.bottom,color);
}

