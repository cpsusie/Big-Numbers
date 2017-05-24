#include "stdafx.h"
#include "DFAPainter.h"

#ifdef _DEBUG

// ----------------------------------------- DFATransition ----------------------------------------

DFATransition::DFATransition(int fromState, int successor, _TUCHAR ch) {
  m_fromState = fromState;
  m_successor = successor;
  m_ch        = ch;
  m_set       = NULL;
}

DFATransition::DFATransition(int fromState, int successor, const CharacterSet &set) {
  m_fromState = fromState;
  m_successor = successor;
  m_set       = new CharacterSet(set);
  m_ch        = 0;
}

DFATransition::DFATransition(const DFATransition &src) {
  copy(src);
}

DFATransition &DFATransition::operator=(const DFATransition &src) {
  cleanup();
  copy(src);
  return *this;
}

DFATransition::~DFATransition() {
  cleanup();
}

void DFATransition::copy(const DFATransition &src) {
  m_fromState = src.m_fromState;
  m_successor = src.m_successor;
  m_path      = src.m_path;
  if(src.m_set) {
    m_set = new CharacterSet(*src.m_set);
    m_ch  = 0;
  } else {
    m_set = NULL;
    m_ch  = src.m_ch;
  }
}

void DFATransition::cleanup() {
  if(m_set) delete m_set;
}

String DFATransition::toString() const {
  if(m_set) {
    return charBitSetToString(*m_set, NFAState::getFormater());
  } else {
    return NFAState::getFormater()->toString(m_ch);
  }
}

// ----------------------------------------- DFAStatePoint ----------------------------------------

DFAStatePoint::DFAStatePoint(const DFAState &state, int gridX, int gridY) 
  : m_stateID(state.m_id)
  , m_state(state)
  , m_gridX(gridX), m_gridY(gridY)
  , m_position(-1,-1)
  , m_loopTransitionIndex(-1)
  , m_attributes(createAttributes(state))
{
  DFATransitionMap map;
  state.getTransitionMap(map);
  for(Iterator<Entry<int, CharacterSet> > it = map.entrySet().getIterator(); it.hasNext();) {
    const Entry<int, CharacterSet> &e = it.next();
    const int           successor = e.getKey();
    const CharacterSet &set       = e.getValue();
    if(set.size() == 1) {
      m_transitions.add(DFATransition(state.m_id, successor, set.getFirst()));
    } else {
      m_transitions.add(DFATransition(state.m_id, successor, set));
    }
    if(successor == state.m_id) {
      m_loopTransitionIndex = (short)m_transitions.size()-1;
    } else {
      m_attributes |= ATTR_HASSUCCESSSORS;
    }
  }
}

BYTE DFAStatePoint::createAttributes(const DFAState &s) { // static
  return s.m_accept.m_acceptAttribute | (s.m_id?ATTR_HASPREDECESSOR:0);
}

bool DFAStatePoint::hasTransition(int to) const {
  for(size_t i = 0; i < m_transitions.size(); i++) {
    if(m_transitions[i].getSuccessor() == to) {
      return true;
    }
  }
  return false;
}

#define ISFREEDIR3(d1,d2,d3) (!m_dirOccupied.contains(d1) && !m_dirOccupied.contains(d2) && !m_dirOccupied.contains(d3))
#define ISFREEDIR2(d1,d2)    (!m_dirOccupied.contains(d1) && !m_dirOccupied.contains(d2))

DirectionPair DFAStatePoint::findBestLoopPosition() const {
  for(int i = 0,j=7, k=6; i < 8; k = j, j = i++) {
    if(ISFREEDIR3(i,j,k)) {
      return DirectionPair((ArrowDirection)i, (ArrowDirection)k);
    }
  }
  for(int i = 0, j = 7; i < 8; j = i++) {
    if(ISFREEDIR2(i,j)) {
      return DirectionPair((ArrowDirection)i, (ArrowDirection)j);
    }
  }
  for(int i = 0; i < 8; i++) {
    if(!m_dirOccupied.contains(i)) {
      return DirectionPair((ArrowDirection)i, (ArrowDirection)i);
    }
  }
  return DirectionPair(DIR_S, DIR_SE);
}

String DFAStatePoint::usedDirectionsToString() const {
  String result;
  const TCHAR *delim = NULL;
  for(int i = 0; i < 8; i++) {
    if(m_dirOccupied.contains(i)) {
      if(delim) result += delim; else delim = _T(",");
      result += directionToStr((ArrowDirection)i);
    }
  }
  return result;
}
    
void DFAStatePoint::paint(HDC hdc, bool marked) const {
  const CPoint p = m_position;
  bool ring = true;
  if(m_attributes & ATTR_ISACCEPTSTATE) {
    AutomatePainter::paintRing(  hdc, p, CIRCLE_RADIUS, AutomatePainter::getBlackPen(), AutomatePainter::getGreenBrush()     );
  } else if(m_attributes & ATTR_ISSTARTSTATE) {
    AutomatePainter::paintRing(  hdc, p, CIRCLE_RADIUS, AutomatePainter::getBlackPen(), AutomatePainter::getRedBrush()       );
  } else if(!(m_attributes & ATTR_HASPREDECESSOR)) {
    AutomatePainter::paintRing(  hdc, p, CIRCLE_RADIUS, AutomatePainter::getBlackPen(), AutomatePainter::getPinkBrush()      );
  } else if(!(m_attributes & ATTR_HASSUCCESSSORS)) {
    AutomatePainter::paintRing(  hdc, p, CIRCLE_RADIUS, AutomatePainter::getBlackPen(), AutomatePainter::getLightGreenBrush());
  } else {
    AutomatePainter::paintCircle(hdc, p, CIRCLE_RADIUS, AutomatePainter::getBlackPen(), NULL              );
    ring = false;
  }
  if(marked) {
    int markRadius = ring ? (CIRCLE_RADIUS-4) : CIRCLE_RADIUS;
    AutomatePainter::paintCircle(hdc, p, markRadius, NULL, AutomatePainter::getOrangeBrush());
  }

  const String text     = format(_T("%d"), getStateID());
  CFont       &font     = AutomatePainter::getFont();
  HGDIOBJ      oldgdi   = SelectObject(hdc, font);
  const CSize  textSize = getTextExtent(hdc, text);
  SelectObject(hdc, oldgdi);
  AutomatePainter::textOut(hdc, p - textSize / 2, font, text);
}

// ----------------------------------------- DFAPointArray ----------------------------------------

DFAPointArray::~DFAPointArray() {
  deleteAll();
}

void DFAPointArray::deleteAll() {
  for(size_t i = 0; i < size(); i++) {
    delete (*this)[i];
  }
  clear();
}

void DFAPointArray::findPredecessors() {
  for(size_t i = 0; i < size(); i++) {
    DFAStatePoint *state = (*this)[i];
    const Array<DFATransition> &trans = state->getTransitions();
    for(size_t j = 0; j < trans.size(); j++) {
      const int succ = trans[j].getSuccessor();
      if(succ != i) {
        (*this)[succ]->m_pred.add(state);
      }
    }
  }
}

// ----------------------------------------- DFAPainter ----------------------------------------

void DFAPainter::calculateAllPositions() {
  m_dfaPoints.deleteAll();
  const Array<DFAState> &stateArray = m_dfa.getStateArray();
  const size_t           stateCount = stateArray.size();

  if(stateCount == 0) {
    return;
  }

  for(size_t i = 0; i < stateCount; i++) {
    m_dfaPoints.add(new DFAStatePoint(stateArray[i], 0, 0));
  }
  BitSet            done(stateCount);
  CompactStack<int> pointIndexStack;;
  DFAStatePoint *point = m_dfaPoints[0];
  point->setGrid(0,0);
  pointIndexStack.push(0);
  done.add(0);

  CompactShortArray columnSize; // counts elements with same gridx. indexed by gridX
  columnSize.add(1);

  int maxGridX = 1, maxGridY = 1;
  while(!pointIndexStack.isEmpty()) {
    const int                   index       = pointIndexStack.pop();
    const DFAStatePoint        *sp          = m_dfaPoints[index];
    const Array<DFATransition> &transitions = sp->getTransitions();
    const size_t                transCount  = transitions.size();
    for(size_t i = 0; i < transitions.size(); i++) {
      const DFATransition &transition = transitions[i];
      const int successor = transition.getSuccessor();
      if(!done.contains(successor)) {
        DFAStatePoint *succPoint = m_dfaPoints[successor];
        const int newGridX = sp->m_gridX+1;
        if(newGridX == columnSize.size()) columnSize.add(0);
        const int newGridY = columnSize[newGridX]++;
        succPoint->setGrid(newGridX, newGridY);
        maxGridX = max(maxGridX, newGridX+1);
        maxGridY = max(maxGridY, newGridY+1);
        done.add(successor);
        pointIndexStack.push(successor);
      }
    }
  }
  assert(done.size() == stateCount);

  CSize gridSize(maxGridX, maxGridY);

  Array<CompactShortArray> columnIndices;
  for(int c = 0; c < gridSize.cx; c++) {
    columnIndices.add(CompactShortArray());
  }
  for(size_t i = 0; i < m_dfaPoints.size(); i++) {
    columnIndices[m_dfaPoints[i]->m_gridX].add((short)i);
  }
  for(int c = 0; c < gridSize.cx; c++) {
    const int n = columnSize[c];
    const CompactShortArray &statesInColumn = columnIndices[c];
    assert(n == statesInColumn.size());
    for(int i = 0; i < n; i++) {
      DFAStatePoint *sp = m_dfaPoints[statesInColumn[i]];
      sp->setPosition(Point2DP((float)m_size.cx * (c + 0.5) / gridSize.cx 
                              ,(float)m_size.cy * (i + 0.5) / n));
    }
  }
  m_grid.markStateCircles(m_dfaPoints);
}

BitMatrix DFAPainter::createGridMatrix(const CSize gridSize) const {
  const MatrixDimension dim(gridSize.cy, gridSize.cx);
  BitMatrix result(dim);
  for(size_t i = 0; i < m_dfaPoints.size(); i++) {
    const DFAStatePoint *dp = m_dfaPoints[i];
    result.set(dp->m_gridY, dp->m_gridX, true);
  }
  return result;
}

void DFAPainter::paint(const DFAPointArray &pointArray, HDC hdc) {
  paintStates(             pointArray, hdc);
  paintOutgoingTransitions(pointArray, hdc);
  paintLoopTransitions(    pointArray, hdc);
}

void DFAPainter::paintStates(const DFAPointArray &pointArray, HDC hdc) {
  for(size_t i = 0; i < pointArray.size(); i++) {
    paintState(pointArray, i, hdc);
  }
}

void DFAPainter::paintState(const DFAPointArray &pointArray, size_t index, HDC hdc) {
  DFAStatePoint *sp = pointArray[index];
  sp->paint(getDC(hdc));
  if(index == 0) {
    markStartState(hdc, sp->getPosition());
  }
}

void DFAPainter::paintOutgoingTransitions(const DFAPointArray &pointArray, HDC hdc) {
  for(size_t i = 0; i < pointArray.size(); i++) {
    const DFAStatePoint *sp = pointArray[i];
    const Array<DFATransition> &transitions = sp->getTransitions();
    for(size_t t = 0; t < transitions.size(); t++) {
      const DFATransition &transition = transitions[t];
      if(!transition.isLoopTransition()) {
        paintOutgoingTransition(pointArray, i, transition, hdc);
      }
    }
  }
}

void DFAPainter::paintOutgoingTransition(const DFAPointArray &pointArray, size_t index, const DFATransition &transition, HDC hdc) {
  DFAStatePoint *from = pointArray[index];
  DFAStatePoint *to   = pointArray[transition.getSuccessor()];

  const Point2DP cf = from->getPosition();
  const Point2DP ct = to->getPosition();

  const DFAState &sf = from->m_state;
  const DFAState &st = to->m_state;

  if(&sf == &st) { // transition to the same state.
    throwException(_T("paintOutgoingTransition cannot paint loop transitions"));
  } 
  const Point2DP v = (ct - cf).normalize(); 
  Point2DP pFrom, pTo;

  pFrom = cf + v * CIRCLE_RADIUS;
  pTo   = ct - v * CIRCLE_RADIUS;

  const ArrowDirection arrowDir = getVectorDirection(v);
  const DirectionPair dp(arrowDir, getOppositeDirection(arrowDir));

  from->m_dirOccupied.add(dp.m_startDir);
  to->m_dirOccupied.add(  dp.m_endDir);
  for(;;) {
    try {
      transition.setPath(m_grid.findShortestFreePath(pFrom, pTo));
      paintPathArrow(transition.getPath(), hdc);
      m_grid.markPathAsOccupied(transition.getPath());
      break;
    } catch(Exception e) {
      decimateEdgeMarkings(pointArray);
    }
  }

  const String text     = transition.toString();
  CFont       &font     = getFont();
  const CSize  textSize = getTextSize(font, text);
  const CPoint textPos  = findBestTextPosition(pFrom, dp.m_startDir, textSize);
  textOut(textPos, font, text, hdc);
}

void DFAPainter::decimateEdgeMarkings(const DFAPointArray &pointArray) {
  for(size_t s = 0; s < pointArray.size(); s++) {
    const Array<DFATransition> &transitions = pointArray[s]->getTransitions();
    for(size_t t = 0; t < transitions.size(); t++) {
      m_grid.unmarkPathAsOccupied(transitions[t].getPath());
    }
  }
  m_grid.incrEdgeMarkStep();
  for(size_t s = 0; s < pointArray.size(); s++) {
    const Array<DFATransition> &transitions = pointArray[s]->getTransitions();
    for(size_t t = 0; t < transitions.size(); t++) {
      m_grid.markPathAsOccupied(transitions[t].getPath());
    }
  }
}

void DFAPainter::paintLoopTransitions(const DFAPointArray &pointArray, HDC hdc) {
  for(size_t i = 0; i < pointArray.size(); i++) {
    const DFAStatePoint *sp = pointArray[i];
    if(sp->hasLoopTransition()) {
      paintLoopTransition(sp);
    }
  }
}

void DFAPainter::paintLoopTransition(const DFAStatePoint *sp, HDC hdc) {
  const int            stateID     = sp->m_state.m_id;
  const Point2DP       center      = sp->getPosition();
  const DFATransition &trans       = sp->getLoopTransition();
  const DirectionPair  dirPair     = sp->findBestLoopPosition();
  paintLoopArrow(sp->getPosition(), CIRCLE_RADIUS, dirPair, trans.toString(), hdc);
}

void DFAPainter::paintNew(HDC hdc, int currentState) {
  setToWhite();
  try {
    if(s_newPoints.size() < s_lastPaintedSize) {
      s_edgeMarkStep = 2;
    }
    m_grid.setEdgeMarkStep(s_edgeMarkStep);
    paint(s_newPoints);
    if(currentState >= 0) {
      markCurrentState(currentState);
    }
    s_edgeMarkStep    = m_grid.getEdgeMarkStep();
    s_lastPaintedSize = s_newPoints.size();

    flushImage(hdc);
  } catch(Exception e) {
    flushImage(hdc);
    throw;
  }
}

DFAPointArray DFAPainter::s_newPoints;
int           DFAPainter::s_edgeMarkStep    = 2;
size_t        DFAPainter::s_lastPaintedSize = 0;

void DFAPainter::shiftCurrentToNew() {
  s_newPoints.deleteAll();
  s_newPoints = m_dfaPoints;
  m_dfaPoints.clear();
}

void DFAPainter::markCurrentState(int state, HDC hdc) {
  hdc = getDC(hdc);
  if((state >= 0) && (state < (int)s_newPoints.size())) {
    const DFAStatePoint *dp = s_newPoints[state];
    dp->paint(hdc, true);
  }
}

bool DFAPainter::markState(HDC hdc, int state, bool marked) {
  if((state >= 0) && (state < (int)s_newPoints.size())) {
    s_newPoints[state]->paint(hdc, marked);
    return true;
  } else {
    return false;
  }
}

#endif
