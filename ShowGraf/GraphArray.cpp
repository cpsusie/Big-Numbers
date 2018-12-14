#include "stdafx.h"
#include "GraphArray.h"

void GraphArray::paint(CCoordinateSystem &cs, CFont &buttonFont, const CRect &buttonPanelRect) const {
  Viewport2D &vp = cs.getViewport();
  findButtonPositions(*vp.getDC(),buttonFont,buttonPanelRect);
  m_error = EMPTYSTRING;
  for(size_t i = 0; i < size(); i++) {
    try {
      getItem(i).paint(cs, buttonFont,(int)i==m_selected);
    } catch(Exception e) {
      if(m_error.length() == 0) {
        m_error = e.what();
      }
    }
  }
  for(size_t i = 0; i < m_pointArray.size(); i++) {
    m_pointArray[i]->paint(cs);
  }
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

void GraphArray::setRollAvgSize(int rollAvgSize) {
  for(size_t i = 0; i < size(); i++) {
    getItem(i).getGraph().setRollAvgSize(rollAvgSize);
  }
}

void GraphArray::unselect() {
  m_selected = -1;
}

void GraphArray::select(intptr_t i) {
  unselect();
  m_selected = i;
}

bool GraphArray::OnLButtonDown(UINT nFlags, const CPoint &point, const RectangleTransformation &tr) {
  for(size_t i = 0; i < size(); i++) {
    if(getItem(i).getButtonRect().PtInRect(point)) {
      select(i);
      return true;
    }
  }
  double   minDistance = EMPTY_DISTANCE;
  intptr_t selected    = -1;
  for(size_t i = 0; i < size(); i++) {
    const GraphItem &item = getItem(i);
    if(!item.getGraph().isVisible()) {
      continue;
    }
    const double distance = item.getGraph().distance(point,tr);
    if((distance < 7) && (distance < minDistance)) {
      selected    = i;
      minDistance = distance;
    }
  }
  if(selected >= 0) {
    select(selected);
    return true;
  }
  return false;
}

int GraphArray::getMaxButtonWidth(CDC &dc, CFont &font) const {
  dc.SelectObject(font);
  int result = 0;
  for(size_t i = 0; i < size(); i++) {
    const CSize cs = dc.GetTextExtent(getItem(i).getDisplayName().cstr());
    if(cs.cx > result) {
      result = cs.cx;
    }
  }
  return min(150, result + 8);
}

int GraphArray::getButtonHeight(CDC &dc, CFont &font) const {
  dc.SelectObject(font);
  const CSize cs = dc.GetTextExtent(_T("T"), 1);
  return cs.cy + 8;
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

void GraphArray::remove(size_t index) {
  if(index == m_selected) {
    unselect();
  }
  GraphItem &item = getItem(index);
  BitSet pointIndexSet = findPointsBelongingToGraph(item.m_graph);
  SAFEDELETE(item.m_graph);
  __super::removeIndex(index);
  for(Iterator<size_t> it = pointIndexSet.getReverseIterator(); it.hasNext();) {
    removePoint(it.next());
  }
  calculateDataRange();

  if(index < size()) {
    select(index);
  } else if(size() > 0) {
    select(size()-1);
  }
}

void GraphArray::addPoint(MoveablePoint *p) {
  m_pointArray.add(p);
}

void GraphArray::addPointArray(const MoveablePointArray &pa) {
  m_pointArray.addAll(pa);
}

void GraphArray::removePoint(size_t index) {
  MoveablePoint *p = m_pointArray[index];
  m_pointArray.remove(index);
  delete p;
}

BitSet GraphArray::findPointsBelongingToGraph(const Graph *g) const {
  BitSet result(m_pointArray.size() + 1);
  const size_t n = m_pointArray.size();
  for(size_t i = 0; i < n; i++) {
    if(&m_pointArray[i]->getGraph() == g) {
      result.add(i);
    }
  }
  return result;
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
  for(size_t i = 0; i < size(); i++) {
    SAFEDELETE(getItem(i).m_graph);
  }
  __super::clear();
  for(size_t i = 0; i < m_pointArray.size(); i++) {
    delete m_pointArray[i];
  }
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
