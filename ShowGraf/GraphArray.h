#pragma once

#include "GraphItem.h"

typedef enum {
  NOSELECTION
 ,GRAPHSELECTED
 ,POINTSELECTED
} SelectedType;

class SelectedData {
private:
  SelectedType m_type;
  size_t       m_index;
public:
  inline SelectedData() {
    unselect();
  }
  inline void select(SelectedType type, size_t index) {
    m_type  = type;
    m_index = index;
  }
  inline void unselect() {
    m_type = NOSELECTION;
    m_index = -1;
  }
  inline SelectedType getType() const {
    return m_type;
  }
  inline size_t getIndex() const {
    return m_index;
  }
  inline bool hasSelection() const {
    return getType() != NOSELECTION;
  }
};

class GraphArray : public Array<GraphItem> {
private:
  SelectedData           m_selectedData;
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
  void removeGraphItem(size_t index);
public:
  GraphArray() {
  }

  inline void unselect() {
    m_selectedData.unselect();
  }
  inline void select(SelectedType type, size_t index) {
    m_selectedData.select(type, index);
  }
  void add(Graph *g);
  void addPoint(MoveablePoint *p);
  // if removeOldOfSameType, old points, belonging to same graph and with same type will be removed
  // before the new points are added
  void addPointArray(const MoveablePointArray &pa, bool removeOldOfSameType);
  void removeCurrentSelection();
  void clear();
  void refresh();
  bool needRefresh() const;

  const SelectedData &getCurrentSelection() const {
    return m_selectedData;
  }
  inline bool hasSelection() const {
    return getCurrentSelection().hasSelection();
  }
  const GraphItem *getSelectedGraphItem() const {
    return (getCurrentSelection().getType() == GRAPHSELECTED) 
         ? &(*this)[getCurrentSelection().getIndex()]
         : NULL;
  }
  MoveablePoint *getSelectedPoint() const {
    return (getCurrentSelection().getType() == POINTSELECTED)
         ? m_pointArray[getCurrentSelection().getIndex()]
         : NULL;
  }

  int  getMaxButtonWidth(CDC &dc, CFont &font) const;
  int  getButtonHeight(  CDC &dc, CFont &font) const;

  const DataRange &getDataRange() const {
    return m_dataRange;
  }

  double getSmallestPositiveX() const;
  double getSmallestPositiveY() const;

  bool OnLButtonDown(UINT nFlags, const CPoint &point);
  void paintItems(       CDC &dc, CFont &buttonFont, const CRect &buttonPanelRect) const;
  void paintPointArray(  CDC &dc) const;
  void unpaintPointArray(CDC &dc);
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
