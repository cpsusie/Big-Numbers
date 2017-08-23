#pragma once

#include "NFA.h"
#include "AutomatePainter.h"

class NFAStatePoint {
private:
  CPoint          m_position;
  // accept,startState,etc...
  BYTE            m_attributes;
  static BYTE createAttributes(const NFAState *s);
public:
  const NFAState *m_state;
  short           m_subNFAIndex;
  short           m_gridX, m_gridY;
  NFAStatePoint(const NFAState *state, short subNFAIndex, int gridX, int gridY)
    : m_state(state)
    , m_subNFAIndex(subNFAIndex)
    , m_gridX(gridX), m_gridY(gridY)
    , m_position(-1,-1)
    , m_attributes(createAttributes(state))
  {
  }
  inline int getStateID() const {
    return m_state->getID();
  }
  inline void setPosition(const CPoint &p) {
    m_position = p;
  }
  inline void setPosition(int x, int y) {
    m_position = CPoint(x,y);
  }
  inline const CPoint &getPosition() const {
    return m_position;
  }
  inline BYTE getAttributes() const {
    return m_attributes;
  }
  inline void setNoPredecessor() {
    m_attributes &= ~ATTR_HASPREDECESSOR;
  }
  DEFINE_ATTRIBUTE_TRAITS
};

class FallingNFAState {
public:
  Point2DP m_p0;
  CPoint   m_pos;
  BYTE     m_attributes;
  int      m_id;
  FallingNFAState() {
  }
  FallingNFAState(const NFAStatePoint *state, int id);
};

typedef CompactArray<FallingNFAState> FallingNFAStateArray;

class NFAPointArray : public CompactArray<NFAStatePoint*> {
public:
  ~NFAPointArray() {
    deleteAll();
  }
  void add(NFAStatePoint *p) {
    TRACE_NEW(p);
    __super::add(p);
  }
  void deleteAll() {
    for(size_t i = 0; i < size(); i++) {
      SAFEDELETE((*this)[i]);
    }
    __super::clear();
  }
};

class NFAPainter : public AutomatePainter {
private:
  NFAPointArray m_nfaPoints;
  static NFAPointArray s_oldNFAPoints, s_newNFAPoints;

  void paintArray(     const NFAPointArray &pointArray);
  void paintState(     const NFAPointArray &pointArray, size_t i);
  void paintState(     HDC hdc, CPoint p, int id, BYTE attr);
  void unpaintState(   HDC hdc, CPoint p);
  void paintTransition(const NFAStatePoint *from, const NFAStatePoint *to);
  BitMatrix createGridMatrix(const CSize &gridSize) const;
  void setPositions(size_t subNFACount);
  static void checkAllocated();
  static void moveNewToOld();
  void animateFallingStates(HDC hdc, FallingNFAStateArray &states);
public:
  NFAPainter(const CSize &size) : AutomatePainter(size) {
    checkAllocated();
  }
  void calculateAllPositions();
  void paintNew(       HDC hdc);
  void animateOldToNew(HDC hdc);
  void shiftCurrentToNew();
};

