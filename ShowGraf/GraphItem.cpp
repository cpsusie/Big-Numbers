#include "stdafx.h"
#include "GraphItem.h"

GraphItem::GraphItem(Graph *g) {
  m_graph = g;
  m_graph->setVisible(true);
}

COLORREF getComplementColor(COLORREF color) {
  return RGB(255-GetRValue(color),255-GetGValue(color),255-GetBValue(color));
}

void GraphItem::paintButton(CDC &dc, bool selected) {
  const CString buttonText = m_graph->getParam().getPartialName().cstr();
  dc.FillSolidRect(&m_buttonRect,m_graph->getParam().m_color);
  CRect tr = m_buttonRect;
  tr.left += 5; tr.top += 4; tr.right -= 3; tr.bottom -= 3;
  COLORREF color = m_graph->getParam().m_color;
  dc.SetBkColor(color);
//  CSize cs  = dc.GetTextExtent(buttonText.cstr());
  COLORREF textColor = getComplementColor(color);
  dc.SetTextColor(textColor);
  dc.DrawText(buttonText, &tr, DT_END_ELLIPSIS);
//  dc.TextOut(m_buttonRect.left + (m_buttonRect.Width()-cs.cx)/2,m_buttonRect.top+3,buttonText.cstr());
  DrawEdge(dc.m_hDC,&m_buttonRect,EDGE_RAISED,BF_RECT);
  if(selected) {
    CRect fr = m_buttonRect;
    fr.top +=3; fr.left += 3;
    fr.bottom -= 3; fr.right -= 3;
    DrawFocusRect(dc.m_hDC,&fr);
  }
}

void GraphItem::paint(CCoordinateSystem &cs, CFont &buttonFont, bool selected) {
  Viewport2D &vp = cs.getViewport();
  if(m_graph->isVisible()) {
    vp.setClipping(true);
    m_graph->paint(cs);
    vp.setClipping(false);
  }
  CDC &dc = *vp.getDC();
  dc.SelectObject(buttonFont);
  paintButton(dc, selected);

}

