#pragma once

#include <CompactStack.h>
#include <MatrixTemplate.h>
#include <TinyBitSet.h>
#include <BitMatrix.h>
#include <Math/Point2D.h>
#include <Math/Transformation.h>
#include "AutomatePainter.h"
#include "DFA.h"

class DFATransition {
private:
  int                    m_fromState, m_successor;
  const CharacterSet    *m_set;
  _TUCHAR                m_ch;
  mutable TransitionPath m_path;
  void cleanup();
  void copy(const DFATransition &src);
public:
  DFATransition(const DFATransition &src);
  DFATransition(int fromState, int successor, _TUCHAR ch);
  DFATransition(int fromState, int successor, const CharacterSet &set);
  DFATransition &operator=(const DFATransition &src);
 ~DFATransition();
  int getSuccessor() const {
    return m_successor;
  }
  inline bool isLoopTransition() const {
    return m_fromState == m_successor;
  }
  inline void setPath(TransitionPath &path) const {
    m_path = path;
  }
  inline const TransitionPath &getPath() const {
    return m_path;
  }
  String toString() const;
};

class DFAStatePoint {
private:
  const int                    m_stateID;
  CPoint                       m_position;
  Array<DFATransition>         m_transitions;
  // accept,startState,etc...
  BYTE                         m_attributes;
  static BYTE createAttributes(const DFAState &s);
public:
  const DFAState              &m_state;
  short                        m_gridX, m_gridY;
  short                        m_loopTransitionIndex;
  BitSet8                      m_dirOccupied;
  CompactArray<DFAStatePoint*> m_pred;

  DFAStatePoint(const DFAState &state, int gridX, int gridY);
  bool hasTransition(int to) const;

  inline int getStateID() const {
    return m_stateID;
  }
  inline const Array<DFATransition> &getTransitions() const {
    return m_transitions;
  }
  inline void setGrid(short x, short y) {
    m_gridX = x; m_gridY = y;
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
  inline bool hasLoopTransition() const {
    return m_loopTransitionIndex >= 0;
  }
  const DFATransition &getLoopTransition() const {
    return m_transitions[m_loopTransitionIndex];
  }
  void paint(HDC hdc, bool marked = false) const;
  DirectionPair findBestLoopPosition() const;
  String usedDirectionsToString() const;
  DEFINE_ATTRIBUTE_TRAITS
};

class DFAPointArray : public CompactArray<DFAStatePoint*> {
public:
  ~DFAPointArray() override {
    deleteAll();
  }
  void add(DFAStatePoint *p) {
    TRACE_NEW(p);
    __super::add(p);
  }
  void deleteAll() {
    for(size_t i = 0; i < size(); i++) {
      SAFEDELETE((*this)[i]);
    }
    __super::clear();
  }
  void findPredecessors();
};

#pragma pack(push,1)
class GridElement {
private:
  union {
    struct {
      UINT m_distance : 30;
      UINT m_included : 1;
      UINT m_occupied : 1;
    } d;
    UINT m_data;
  };
public:
  inline GridElement(UINT n = 0) {
    m_data = n;
  }
  void reset() {
    d.m_distance = 0;
    d.m_included = FALSE;
  }
  inline void setOccupied(bool occupied) {
    d.m_occupied = occupied;
  }
  inline bool isOccupied() const {
    return d.m_occupied;
  }
  inline bool isIncluded() const {
    return d.m_included;
  }
  inline UINT getDistance() const {
    return d.m_distance;
  }
  void setDistance(UINT dist) {
    d.m_distance = dist;
    d.m_included = TRUE;
  }
};
#pragma pack(pop)

String toString(const GridElement &ge);

class TransitionGrid {
private:
  const CSize                 m_winSize;
  RectangleTransformation     m_tr;
  MatrixTemplate<GridElement> m_grid;
  UINT                        m_distMatrix[3][3];
  int                         m_horzDist, m_vertDist, m_diagDist;
  int                         m_edgeMarkStep;

  // convert window point to grid point
  inline CPoint winToGrid(int x, int y) const {
    return (Point2DP)m_tr.forwardTransform(x, y);
  }
  // convert window point to grid point
  inline CPoint winToGrid(const CPoint &p) const {
    return (Point2DP)m_tr.forwardTransform(p.x, p.y);
  }
  inline CPoint gridToWin(const CPoint &p) const {
    return (Point2DP)m_tr.backwardTransform((Point2DP)p);
  }
  // x,y in window space
  void markCircle(int x, int y);

  // center in window space
  inline void markCircle(const CPoint &center) {
    markCircle(center.x, center.y);
  }
  // gp is in grid space
  inline GridElement &getGridElement(const CPoint &gp) {
    return m_grid(gp.y, gp.x);
  }
  // gp is in grid space
  inline const GridElement &getGridElement(const CPoint &gp) const {
    return m_grid(gp.y, gp.x);
  }
  // x,y is in window space
  inline GridElement &getGridElement(int x, int y) {
    return getGridElement(winToGrid(x, y));
  }
  inline bool isInsideGrid(const CPoint &gp) const {
    return (UINT)gp.x < (UINT)m_grid.getColumnCount()
        && (UINT)gp.y < (UINT)m_grid.getRowCount();
  }
  UINT getMindistance(const CPoint &gp) const;
  CPoint findNeighbourWithMinDistance(const CPoint &gp) const;
  // occupied is not touched
  void initGrid();
public:
  TransitionGrid(const CSize winSize);
  void markStateCircles(const DFAPointArray &pointArray);
  TransitionPath findShortestFreePath(const CPoint &from, const CPoint &to);
  void markPathAsOccupied(  const TransitionPath &path);
  void unmarkPathAsOccupied(const TransitionPath &path);
  void incrEdgeMarkStep() {
    m_edgeMarkStep++;
  }
  void setEdgeMarkStep(int step) {
    m_edgeMarkStep = max(2, step);
  };
  int getEdgeMarkStep() const {
    return m_edgeMarkStep;
  }
  String toString() const;
  String distancesToString() const;
};

class DFAPainter : public AutomatePainter {
private:
  const DFA             &m_dfa;
  DFAPointArray          m_dfaPoints;
  static DFAPointArray   s_newPoints;
  static int             s_edgeMarkStep;
  static size_t          s_lastPaintedSize;
  // if < 0, no blinking. else index of blinking state
  static int             s_blinkingState;
  static CWnd           *s_wnd;
  TransitionGrid         m_grid;

  void paint(                   const DFAPointArray &pointArray                                                , HDC hdc = NULL);
  void paintStates(             const DFAPointArray &pointArray                                                , HDC hdc = NULL);
  void paintState(              const DFAPointArray &pointArray, size_t index                                  , HDC hdc = NULL);
  void paintOutgoingTransitions(const DFAPointArray &pointArray                                                , HDC hdc = NULL);
  void paintOutgoingTransition( const DFAPointArray &pointArray, size_t index , const DFATransition &transition, HDC hdc = NULL);
  void paintLoopTransitions(    const DFAPointArray &pointArray                                                , HDC hdc = NULL);
  void paintLoopTransition(     const DFAStatePoint *sp                                                        , HDC hdc = NULL);
  void markCurrentState(        int state                                                                      , HDC hdc = NULL);
  void decimateEdgeMarkings(    const DFAPointArray &pointArray);
  BitMatrix createGridMatrix(const CSize gridSize) const;
public:
  DFAPainter(const DFA &dfa, const CSize &size) : AutomatePainter(size), m_dfa(dfa), m_grid(size) {
  }
  void calculateAllPositions();
  void paintNew(  HDC hdc, int currentState);
  // for marking lastAcceptState as blinking
  static bool markState( HDC hdc, int state, bool marked);
  void shiftCurrentToNew();
#if defined(_DEBUG)
  static void startBlinking(CWnd *wnd, int state);
  static void stopBlinking();
  static void setBlinkersVisible(bool visible);
#endif
};
