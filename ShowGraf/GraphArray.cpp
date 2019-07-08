#include "stdafx.h"
#include "GraphArray.h"

void GraphArray::paintItems(CDC &dc, CFont &buttonFont, const CRect &buttonPanelRect) const {
  findButtonPositions(dc,buttonFont,buttonPanelRect);
  m_error = EMPTYSTRING;
  const SelectedData &sel                = getCurrentSelection();
  const size_t        selectedGraphIndex = (sel.getType() == GRAPHSELECTED) ? sel.getIndex() : -1;
  for(size_t i = 0; i < size(); i++) {
    try {
      getItem(i).paint(dc, buttonFont,i == selectedGraphIndex);
    } catch(Exception e) {
      if(m_error.length() == 0) {
        m_error = e.what();
      }
    }
  }
}

void GraphArray::paintPointArray(CDC &dc) const {
  m_pointArray.paint(dc);
}

void GraphArray::unpaintPointArray(CDC &dc) {
  m_pointArray.unpaint(dc);
}

void GraphArray::setTrigoMode(TrigonometricMode mode) {
  for(size_t i = 0; i < size(); i++) {
    Graph &g = (*this)[i].getGraph();
    if(g.getType() == FUNCTIONGRAPH) {
      ((FunctionGraph&)g).setTrigoMode(mode);
    }
  }
}

void GraphArray::setStyle(GraphStyle style) {
  for(size_t i = 0; i < size(); i++) {
    getItem(i).getGraph().setStyle(style);
  }
}

void GraphArray::setRollAvgSize(UINT rollAvgSize) {
  for(size_t i = 0; i < size(); i++) {
    getItem(i).getGraph().setRollAvgSize(rollAvgSize);
  }
}

bool GraphArray::OnLButtonDown(UINT nFlags, const CPoint &point) {
  for(size_t i = 0; i < size(); i++) {
    if(getItem(i).getButtonRect().PtInRect(point)) {
      select(GRAPHSELECTED, i);
      return true;
    }
  }
  for(size_t i = 0; i < m_pointArray.size(); i++) {
    const MoveablePoint *p = m_pointArray[i];
    if(p->getTextRect().PtInRect(point)) {
      select(POINTSELECTED, i);
      return true;
    }
  }
  intptr_t bestIndex   = -1;
  double   minDistance = EMPTY_DISTANCE;
  for(size_t i = 0; i < size(); i++) {
    const GraphItem &item = getItem(i);
    if(!item.getGraph().isVisible()) {
      continue;
    }
    const double distance = item.getGraph().distance(point);
    if((distance < 7) && (distance < minDistance)) {
      bestIndex   = i;
      minDistance = distance;
    }
  }
  if(bestIndex >= 0) {
    select(GRAPHSELECTED, bestIndex);
    return true;
  }
  unselect();
  return false;
}

int GraphArray::getMaxButtonWidth(CDC &dc, CFont &font) const {
  CFont *oldFont = dc.SelectObject(&font);
  int result = 0;
  for(size_t i = 0; i < size(); i++) {
    const CSize cs = dc.GetTextExtent(getItem(i).getDisplayName().cstr());
    if(cs.cx > result) {
      result = cs.cx;
    }
  }
  dc.SelectObject(oldFont);
  return min(150, result + 8);
}

int GraphArray::getButtonHeight(CDC &dc, CFont &font) const {
  CFont *oldFont = dc.SelectObject(&font);
  const CSize cs = dc.GetTextExtent(_T("T"), 1);
  return cs.cy + 8;
  dc.SelectObject(oldFont);
}

void GraphArray::findButtonPositions(CDC &dc, CFont &font, const CRect &buttonPanelRect) const {
  int buttonWidth  = getMaxButtonWidth(dc,font);
  int buttonHeight = getButtonHeight(dc,font);
  for(size_t i = 0; i < size(); i++) {
    const GraphItem &gi = getItem(i);
    CRect buttonRect;
    buttonRect.left   = buttonPanelRect.left + (buttonPanelRect.Width() - buttonWidth)/2;
    buttonRect.right  = buttonRect.left + buttonWidth;
    buttonRect.top    = buttonPanelRect.top + 10 + (buttonHeight+3) * ((int)i + 1);
    buttonRect.bottom = buttonRect.top + buttonHeight;
    gi.setButtonRect(buttonRect);
  }
}

void GraphArray::add(Graph *g) {
  if(size() == 0) {
    m_dataRange = g->getDataRange();
  } else {
    m_dataRange += g->getDataRange();
  }
  __super::add(GraphItem(g));
}

void GraphArray::removeCurrentSelection() {
  const SelectedData &sel = getCurrentSelection();
  switch(sel.getType()) {
  case GRAPHSELECTED:
    removeGraphItem(sel.getIndex());
    break;
  case POINTSELECTED:
    removePoint(sel.getIndex());
    break;
  case NOSELECTION:
    return;
  }
  unselect();
}

void GraphArray::removeGraphItem(size_t index) {
  GraphItem &item = getItem(index);
  const BitSet pointIndexSet = m_pointArray.findPointsBelongingToGraph(item.m_graph);
  SAFEDELETE(item.m_graph);
  __super::removeIndex(index);
  m_pointArray.removePointSet(pointIndexSet);
  calculateDataRange();
}

void GraphArray::addPoint(MoveablePoint *p) {
  m_pointArray.add(p);
}

void GraphArray::removePoint(size_t index) {
  m_pointArray.removePoint(index);
}

void GraphArray::addPointArray(const MoveablePointArray &pa, bool removeOldOfSameType) {
  m_pointArray.addAll(pa, removeOldOfSameType);
}

void GraphArray::calculateDataRange() {
  if(size() > 0) {
    m_dataRange = getItem(0).m_graph->getDataRange();
    for(size_t i = 1; i < size(); i++) {
      m_dataRange += getItem(i).m_graph->getDataRange();
    }
  }
}

void GraphArray::clear() {
  unselect();
  for(size_t i = 0; i < size(); i++) {
    SAFEDELETE(getItem(i).m_graph);
  }
  __super::clear();
  m_pointArray.clear();
}

void GraphArray::refresh() {
  for(size_t i = 0; i < size(); i++) {
    GraphItem &item = getItem(i);
    if(item.getGraph().needRefresh()) {
      item.getGraph().refreshData();
    }
  }
  calculateDataRange();
}

bool GraphArray::needRefresh() const {
  for(size_t i = 0; i < size(); i++) {
    if(getItem(i).m_graph->needRefresh()) {
      return true;
    }
  }
  return false;
}

double GraphArray::getSmallestPositiveX() const {
  if(isEmpty()) {
    return 0;
  }
  double result = getItem(0).getGraph().getSmallestPositiveX();
  if(result < 0) {
    result = 0;
  }
  for(size_t i = 1; i < size(); i++) {
    result = Graph::getMinPositive(getItem(i).getGraph().getSmallestPositiveX(), result);
  }
  return result;
}

double GraphArray::getSmallestPositiveY() const {
  if(isEmpty()) {
    return 0;
  }
  double result = getItem(0).getGraph().getSmallestPositiveY();
  if(result < 0) {
    result = 0;
  }
  for(size_t i = 1; i < size(); i++) {
    result = Graph::getMinPositive(getItem(i).getGraph().getSmallestPositiveY(), result);
  }
  return result;
}
