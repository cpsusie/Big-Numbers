#pragma once

typedef enum {
  S
 ,E
 ,N
 ,W
 ,NODIR
} Direction;

extern const TCHAR *directionName[5];

typedef BYTE EdgeAttribute;
#define ATTR_INSIDE 0x1
#define ATTR_DIRTY  0x2

class EdgeMatrix {
private:
  union {
    ULONG m_allAttr;
    BYTE  m_attrRow[3];
  };
public:
  EdgeMatrix();

  inline bool isDirty(UINT i, UINT j) {
    return ((m_attrRow[i] >> (2*j)) & 3) == ATTR_DIRTY;
  }
  inline void setOutside(UINT i, UINT j) {
    m_attrRow[i] &= ~(3 << (2*j));
  }
  inline void setRowOutside(UINT i) {
    m_attrRow[i] = 0;
  }
  inline void setInside(UINT i, UINT j) {
    m_attrRow[i] ^= (3 << (2*j));
  }
  Direction     findStepDirection(     Direction dir) const;
  Direction     findStepDirectionFirst(Direction dir) const;
  void          adjustAttributes(      Direction dir);
  EdgeAttribute getLeftAttr(           Direction dir) const;
  String toString() const;
  static const CPoint dirStep[4], leftStep[4];
  static const Direction leftTurn[5], rightTurn[5], oppositeDir[5];
};

inline Direction turnLeft(Direction dir) {
  return EdgeMatrix::leftTurn[dir];
}

inline Direction turnRight( Direction dir) {
  return EdgeMatrix::rightTurn[dir];
}

inline Direction turnAround(Direction dir) {
  return EdgeMatrix::oppositeDir[dir];
}

class PointSet : public BitSet {
private:
  CRect m_rect;
  int   m_width;
  static inline UINT getPixelCount(const CRect &r) {
    return r.Width() * r.Height();
  }
  inline UINT getIndex(const CPoint &p) const {
    return (p.y-m_rect.top) * m_width + (p.x-m_rect.left);
  }
  inline CPoint getPoint(size_t index) const {
    return CPoint((int)(index % m_width + m_rect.left), (int)(index / m_width + m_rect.top));
  }
public:
  PointSet(const CRect &r) 
    : BitSet(getPixelCount(r))
    , m_rect(r)
    , m_width(r.Width())
  {
  }
  inline void add(const CPoint &p) {            // assume p is inside CRect
    BitSet::add(getIndex(p));
  }
  inline bool contains(const CPoint &p) const { // assume p is inside CRect
    return BitSet::contains(getIndex(p));
  }
  inline const CRect &getRect() const {
    return m_rect;
  }
  inline CPoint next(Iterator<size_t> &it) const {
    return getPoint(it.next());
  }
  String toString() const;
};

class FillInfo {
private:
  PointSet m_filledSet, m_edgeSet, m_innerSet;
public:
  FillInfo(const CRect &r) 
    : m_filledSet(r)
    , m_edgeSet( r)
    , m_innerSet(r)
  {
  }
  inline void addFilled(const CPoint &p) {
    m_filledSet.add(p);
  }
  inline bool contains(const CPoint &p) const {
    return m_filledSet.getRect().PtInRect(p) ? m_filledSet.contains(p) : false;
  }
  void setEdgeAndInnerSet(const PointSet &edgeSet, const PointSet &innerSet) {
    m_edgeSet = edgeSet; m_innerSet = innerSet;
  }
  const PointSet &getEdgeSet() const {
    return m_edgeSet;
  }
  const PointSet &getInnerSet() const {
    return m_innerSet;
  }
  void addEdgeSetToFilledSet() {
    m_filledSet += m_edgeSet;
  }
  CSize getSize() const {
    return m_filledSet.getRect().Size();
  }
  inline const CRect &getRect() const {
    return m_filledSet.getRect();
  }
  void clear() {
    m_filledSet.clear(); m_edgeSet.clear(); m_innerSet.clear();
  }
  String toString() const;
};
