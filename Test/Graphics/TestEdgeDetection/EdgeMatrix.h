#pragma once

#include <MFCUtil\PointSet.h>

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

class FillInfo {
private:
  CRect    m_r;
  PointSet m_filledSet, m_edgeSet, m_innerSet;
public:
  FillInfo(const CRect &r)
    : m_r(        r)
    , m_filledSet(r)
    , m_edgeSet(  r)
    , m_innerSet( r)
  {
  }
  inline void addFilled(const CPoint &p) {
    m_filledSet.add(p);
  }
  inline bool contains(const CPoint &p) const {
    return m_r.PtInRect(p) ? m_filledSet.contains(p) : false;
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
    m_filledSet |= m_edgeSet;
  }
  CSize getSize() const {
    return m_r.Size();
  }
  inline const CRect &getRect() const {
    return m_r;
  }
  void clear() {
    m_filledSet.clear(); m_edgeSet.clear(); m_innerSet.clear();
  }
  String toString() const;
};
