#pragma once

#include "Graph.h"

class GraphItem {
private:
  Graph *m_graph;
  CRect  m_buttonRect;

  void paintButton(CDC &dc, bool selected);
public:
  GraphItem(Graph *g);
  
  Graph &getGraph() const {
    return *m_graph;
  }
  
  const String &getFullName() const {
    return m_graph->getParam().getFullName();
  }

  String getPartialName() const {
    return m_graph->getParam().getPartialName();
  }

  void paint(Viewport2D &vp, CFont &buttonFont, bool selected); // width in characters
  
  const CRect &getButtonRect() const {
    return m_buttonRect;
  }
  
  void setButtonRect(const CRect &r) {
    m_buttonRect = r;
  }

  friend class GraphArray;
};

