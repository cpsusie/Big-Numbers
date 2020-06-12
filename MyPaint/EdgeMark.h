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
  inline EdgeMark() {
  }
  inline EdgeMark(EdgeMarkType type) : m_type(type), m_visible(false) {
  }
  int getCursorId() const;

  inline EdgeMarkType getType() const {
    return m_type;
  }
  void setPosition(const CPoint &p);
  inline void setVisible(bool visible) {
    m_visible = visible;
  }
  inline bool isVisible() const {
    return m_visible;
  }
  inline const CRect &getActiveRect() const {
    return m_activeRect;
  }
  inline const CRect &getVisibleRect() const {
    return m_visibleRect;
  }
#if defined(_DEBUG)
  String toString() const;
#endif
};

class EdgeMarkArray : private CompactArray<EdgeMark> {
private:
  int m_visibleCount;
public:
  EdgeMarkArray();
  void setAllInvisible();
  void setPosition(EdgeMarkType type, const CPoint &pos);
  inline int getVisibleCount() const {
    return m_visibleCount;
  }
  void paintAll(CDC &dc);
  const EdgeMark *findEdgeMark(const CPoint &point) const;
#if defined(_DEBUG)
  String toString() const;
#endif
};
