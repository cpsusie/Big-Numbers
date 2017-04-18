#pragma once

#include "Graph.h"

class GraphItem {
private:
  Graph *m_graph;
  CRect  m_buttonRect;

  void paintButton(CDC &dc, bool selected);
public:
  GraphItem(Graph *g);
  
  inline Graph &getGraph() const {
    return *m_graph;
  }
  
  inline const String &getFullName() const {
    return m_graph->getParam().getName();
  }

  inline String getDisplayName() const {
    return m_graph->getParam().getDisplayName();
  }

  void paint(CCoordinateSystem &cs, CFont &buttonFont, bool selected); // width in characters
  
  inline const CRect &getButtonRect() const {
    return m_buttonRect;
  }
  
  inline void setButtonRect(const CRect &r) {
    m_buttonRect = r;
  }

  friend class GraphArray;
};

