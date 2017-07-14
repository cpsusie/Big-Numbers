#pragma once

typedef enum {
  RIGHTMARK
 ,BOTTOMMARK
 ,RIGHTBOTTOMMARK
} EdgeMarkType;

#define EDGEMARKSIZE 6

class EdgeMark {
private:
  EdgeMarkType m_type;
  CRect        m_activeRect;
  CRect        m_visibleRect;
  bool         m_visible;

  CRect createRect(const CPoint &p, int size) const;
  CRect createActiveRect( const CPoint &p) const;
  CRect createVisibleRect(const CPoint &p) const;
public:
  EdgeMark(EdgeMarkType type) : m_type(type), m_visible(false) {
  }
  int getCursorId() const;

  EdgeMarkType getType() const {
    return m_type;
  }
  void setPosition(const CPoint &p);
  void setVisible(bool visible) {
    m_visible = visible;
  }
  bool isVisible()  const {
    return m_visible;
  }
  const CRect &getActiveRect() const {
    return m_activeRect;
  }
  const CRect &getVisibleRect() const {
    return m_visibleRect;
  }
#ifdef _DEBUG
  String toString() const;
#endif
};

class EdgeMarkArray : private Array<EdgeMark> {
private:
  int m_visibleCount;
public:
  EdgeMarkArray();
  void setPosition(EdgeMarkType type, const CPoint &pos);
  void clear();
  inline int getVisibleCount() const {
    return m_visibleCount;
  }
  void paintAll(CDC &dc);
  const EdgeMark *findEdgeMark(const CPoint &point) const;
#ifdef _DEBUG
  String toString() const;
#endif
};
