#pragma once

#include "GraphItem.h"

class GraphArray : public Array<GraphItem> {
private:
  intptr_t       m_selected;
  mutable String m_error;
  DataRange      m_dataRange;

  void findButtonPositions(CDC &dc, CFont &font, const CRect &buttonPanelRect) const;
  
  const GraphItem &getItem(size_t index) const {
    return (*this)[index];
  }

  GraphItem &getItem(size_t index) {
    return (*this)[index];
  }

public:
  GraphArray() {
    m_selected = -1;
  }
  
  void unselect();
  void select(intptr_t i);
  void add(Graph *g);
  void remove(size_t index);
  void clear();
  
  intptr_t  getCurrentSelection() const {
    return m_selected;
  }
  const GraphItem *getSelectedItem() const {
    return (m_selected >= 0) ? &(*this)[m_selected] : NULL;
  }
  
  int  getMaxButtonWidth(CDC &dc, CFont &font) const;
  int  getButtonHeight(  CDC &dc, CFont &font) const;

  const DataRange &getDataRange() const {
    return m_dataRange;
  }

  double getSmallestPositiveX() const;
  double getSmallestPositiveY() const;

  bool OnLButtonDown(UINT nFlags, const CPoint &point, const RectangleTransformation &tr);
  void paint(CCoordinateSystem &cs, CFont &buttonFont, const CRect &buttonPanelRect) const;
  void setStyle(GraphStyle style);
  void setRollAvgSize(int size);
  void setTrigoMode(TrigonometricMode mode);
  
  bool ok() const { 
    return m_error.length() == 0;
  }
  
  const String &getErrorMsg() const {
    return m_error;
  }
};
