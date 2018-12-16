#pragma once

#include "GraphItem.h"

class GraphArray : public Array<GraphItem> {
private:
  intptr_t               m_selected;
  mutable String         m_error;
  DataRange              m_dataRange;
  MoveablePointArray     m_pointArray;

  void findButtonPositions(CDC &dc, CFont &font, const CRect &buttonPanelRect) const;
  void calculateDataRange();

  const GraphItem &getItem(size_t index) const {
    return (*this)[index];
  }

  GraphItem &getItem(size_t index) {
    return (*this)[index];
  }
  void removePoint(size_t index);
  BitSet findPointsBelongingToGraph(const Graph *g) const;
public:
  GraphArray() {
    m_selected = -1;
  }

  void unselect();
  void select(intptr_t i);
  void add(Graph *g);
  void addPoint(MoveablePoint *p);
  void addPointArray(const MoveablePointArray &pa);
  void remove(size_t index);
  void clear();
  void refresh();
  bool needRefresh() const;

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
  void paintItems(     CCoordinateSystem &cs, CFont &buttonFont, const CRect &buttonPanelRect) const;
  void paintPointArray(CCoordinateSystem &cs, CFont &font) const;
  void setStyle(GraphStyle style);
  void setRollAvgSize(UINT size);
  void setTrigoMode(TrigonometricMode mode);

  bool ok() const {
    return m_error.length() == 0;
  }

  const String &getErrorMsg() const {
    return m_error;
  }
};
