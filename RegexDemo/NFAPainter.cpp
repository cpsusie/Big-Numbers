#include "stdafx.h"
#include <CompactStack.h>
#include <Math/sigmoidIterator.h>
#include "NFAPainter.h"

#if defined(TESTGRIDMATRIX)

#include <DebugLog.h>

static void dumpGridMatrix(const String &label, const BitMatrix &m) {
  debugLog(_T("%s:\n%s\n"), label.cstr(), m.toString().cstr());
}
#define DUMPGRIDMATRIX(label, m) dumpGridMatrix(label, m)

#else // TESTGRIDMATRIX

#define DUMPGRIDMATRIX(label, m)

#endif // TESTGRIDMATRIX

// ----------------------------------------- NFAStatePoint ----------------------------------------

BYTE NFAStatePoint::createAttributes(const NFAState *s) { // static
  return s->m_accept.m_acceptAttribute
      | (s->isStartState()                             ? ATTR_ISSTARTSTATE   : ATTR_HASPREDECESSOR)
      | (((s->m_next != nullptr) || (s->m_next2 != nullptr)) ? ATTR_HASSUCCESSSORS : 0);
}

static int compareById(NFAState * const &s1, NFAState * const &s2) { // static
  return s1->getID() - s2->getID();
}

static int compareById(NFAStatePoint * const &s1, NFAStatePoint * const &s2) { // static
  return s1->getStateID() - s2->getStateID();
}

// ----------------------------------------- NFAPainter ----------------------------------------

void NFAPainter::paintNew(HDC hdc) {
  if(s_newNFAPoints.size() == 0) {
    setToWhite();
  } else {
    paintArray(s_newNFAPoints);
  }
  flushImage(hdc);
}

void NFAPainter::paintArray(const NFAPointArray &pointArray) {
  setToWhite();
  for(size_t i = 0; i < pointArray.size(); i++) {
    paintState(pointArray, i);
  }
}

class SubNFAInfo {
public:
  int m_maxGridX;
  int m_topGridY;
  int m_minGridY, m_maxGridY;
  int m_nodeCount;
  SubNFAInfo() : m_maxGridX(0), m_minGridY(0), m_maxGridY(0), m_nodeCount(0) {
  }
  inline int getGridSpan() const {
    return m_maxGridY - m_minGridY + 1;
  }
};

BitMatrix NFAPainter::createGridMatrix(const CSize &gridSize) const {
  const MatrixDimension dim(gridSize.cy, gridSize.cx);
  BitMatrix result(dim);
  for(size_t i = 0; i < m_nfaPoints.size(); i++) {
    const NFAStatePoint *sp = m_nfaPoints[i];
    result.set(sp->m_gridY, sp->m_gridX, true);
  }
  return result;
}

void NFAPainter::calculateAllPositions() {
  const CompactArray<NFAState*> &allStates = NFAState::getAllAllocated();
  const size_t stateCount = allStates.size();
  m_nfaPoints.clear();

  if(stateCount == 0) {
    return;
  }

  BitSet hasPredecessor(stateCount);
  for(size_t i = 0; i < stateCount; i++) {
    const NFAState *state = allStates[i];
    if(state->m_next ) hasPredecessor.add(state->m_next->getID());
    if(state->m_next2) hasPredecessor.add(state->m_next2->getID());
  }
  BitSet leftStateSet = hasPredecessor;
  leftStateSet.invert();


  CompactArray<NFAState*> leftMostStates;
  for(auto it = leftStateSet.getIterator(); it.hasNext();) {
    leftMostStates.add(allStates[it.next()]);
  }

  leftMostStates.sort(compareById);

  const size_t rowCount = leftMostStates.size();

  BitSet               done(stateCount);
  CompactStack<size_t> pointIndexStack;;

  for(size_t r = 0; r < leftMostStates.size(); r++) {
    NFAStatePoint *sp = new NFAStatePoint(leftMostStates[r], (short)r, 0, 0);
    sp->setNoPredecessor();
    done.add(sp->getStateID());
    pointIndexStack.push(m_nfaPoints.size());
    m_nfaPoints.add(sp);
  }

  while(!pointIndexStack.isEmpty()) {
    const NFAStatePoint *sp = m_nfaPoints[pointIndexStack.pop()];
    int succesorCount = 0;

    NFAStatePoint *sp1 = nullptr, *sp2 = nullptr;
    if(sp->m_state->m_next) {
      const int id = sp->m_state->m_next->getID();
      if(!done.contains(id)) {
        sp1 = new NFAStatePoint(sp->m_state->m_next, sp->m_subNFAIndex, sp->m_gridX+1, sp->m_gridY);
        succesorCount++;
      }
    }
    if(sp->m_state->m_next2) {
      const int id = sp->m_state->m_next2->getID();
      if(!done.contains(id)) {
        sp2 = new NFAStatePoint(sp->m_state->m_next2, sp->m_subNFAIndex, sp->m_gridX+1, sp->m_gridY);
        succesorCount++;
      }
    }
    switch(succesorCount) {
    case 0: break;
    case 1:
      { NFAStatePoint *ssp = sp1 ? sp1 : sp2;
        done.add(ssp->getStateID());
        pointIndexStack.push(m_nfaPoints.size());
        m_nfaPoints.add(ssp);
      }
      break;

    case 2:
      { const size_t arraySize = m_nfaPoints.size();
        sp1->m_gridY--;
        done.add(sp1->getStateID());
        pointIndexStack.push(arraySize);

        sp2->m_gridY++;
        done.add(sp2->getStateID());
        pointIndexStack.push(arraySize+1);

        m_nfaPoints.add(sp1);
        m_nfaPoints.add(sp2);
      }
      break;
    }
  }

  if(done.size() != stateCount || m_nfaPoints.size() != stateCount) {
    throwException(_T("Missing states in paintAll. stateCount=%d, done=%s, nfaPoints().size=%d"), stateCount, done.toString().cstr(), m_nfaPoints.size());
  }

  setPositions(leftMostStates.size());

  m_nfaPoints.sort(compareById);
}

void NFAPainter::setPositions(size_t subNFACount) {
  CompactArray<SubNFAInfo> subNFAInfoArray;
  for(size_t i = 0; i < subNFACount; i++) {
    subNFAInfoArray.add(SubNFAInfo());
  }
  for(size_t i = 0; i < m_nfaPoints.size(); i++) {
    const NFAStatePoint *sp = m_nfaPoints[i];
    SubNFAInfo &info = subNFAInfoArray[sp->m_subNFAIndex];
    info.m_nodeCount++;
    if(sp->m_gridX > info.m_maxGridX) {
      info.m_maxGridX = sp->m_gridX;
    }
    if(sp->m_gridY < info.m_minGridY) {
      info.m_minGridY = sp->m_gridY;
    }
    if(sp->m_gridY > info.m_maxGridY) {
      info.m_maxGridY = sp->m_gridY;
    }
  }
  for(size_t i = 0; i < m_nfaPoints.size(); i++) {      // first adjust gridY indices to go from 0..maxGridY
    NFAStatePoint *sp = m_nfaPoints[i];
    sp->m_gridY -= subNFAInfoArray[sp->m_subNFAIndex].m_minGridY;
  }

  for(size_t i = 0; i < subNFAInfoArray.size(); i++) {  // and for infoArray too
    SubNFAInfo &info = subNFAInfoArray[i];
    info.m_maxGridY -= info.m_minGridY;
    info.m_minGridY = 0;
  }

  CSize gridSize(0,0);
  int totalNodeCount  = 0;
  for(size_t i = 0; i < subNFAInfoArray.size(); i++) {
    SubNFAInfo &info = subNFAInfoArray[i];
    info.m_topGridY = gridSize.cy;
    gridSize.cx     = max(gridSize.cx, info.m_maxGridX);
    gridSize.cy    += info.getGridSpan();
    totalNodeCount += info.m_nodeCount;
  }

  for(size_t i = 0; i < m_nfaPoints.size(); i++) {      // then adjust gridY to the correct row, so we can ignore info.m_topGridY
    NFAStatePoint *sp = m_nfaPoints[i];
    sp->m_gridY += subNFAInfoArray[sp->m_subNFAIndex].m_topGridY;
  }
  subNFAInfoArray.clear();

  assert(totalNodeCount == m_nfaPoints.size());
  gridSize.cx++;

  if(gridSize.cx > 5) { // adjust positions

    DUMPGRIDMATRIX(_T("Before adjust position"), createGridMatrix(gridSize));

    Array<CompactShortArray> rowIndices;
    for(int r = 0; r < gridSize.cy; r++) {
      rowIndices.add(CompactShortArray());
    }
    for(size_t i = 0; i < m_nfaPoints.size(); i++) {
      rowIndices[m_nfaPoints[i]->m_gridY].add((short)i);
    }
    int rowsAdded = 0;
    for(size_t r = 0; r < rowIndices.size(); r++) {
      CompactShortArray &row = rowIndices[r];
      const bool splitRow = ((int)row.size() > 2*gridSize.cx / 3);
      for(size_t k = 0; k < row.size(); k++) {
        NFAStatePoint *sp = m_nfaPoints[row[k]];
        if(splitRow) {
          sp->m_gridY += rowsAdded + ((int)k%2);
        } else {
          sp->m_gridY += rowsAdded;
          sp->m_gridX = (short)(k * gridSize.cx / row.size() + (sp->m_gridY%2));
        }
        if(sp->m_gridX >= gridSize.cx) sp->m_gridX = (short)gridSize.cx-1;
      }
      if(splitRow) {
        rowsAdded++;
      }
    }
    gridSize.cy += rowsAdded;

    DUMPGRIDMATRIX(_T("After adjust position"), createGridMatrix(gridSize));

  }

  const BitMatrix gm = createGridMatrix(gridSize);
  for(size_t i = 0; i < m_nfaPoints.size(); i++) {
    NFAStatePoint *sp   = m_nfaPoints[i];
    const BYTE     attr = sp->getAttributes();
    if(attr) {
      if(!sp->hasSuccessor()) {
        short    &gridx    = sp->m_gridX;
        int       gridy    = sp->m_gridY;
        const int maxGridX = gridSize.cx-1;
        if(gridx < maxGridX) {
          for(; (gridx < maxGridX) && !gm.get(gridy,gridx+1); gridx++);
        }
      } else if(!sp->hasPredecessor()) {
        short &gridx = sp->m_gridX;
        int    gridy = sp->m_gridY;
        if(gridx > 0) {
          for(; (gridx > 0) && !gm.get(gridy,gridx-1); gridx--);
        }
      }
    }
  }

  DUMPGRIDMATRIX(_T("Final Grid"), createGridMatrix(gridSize));

  for(size_t i = 0; i < m_nfaPoints.size(); i++) {
    NFAStatePoint *sp = m_nfaPoints[i];
    sp->setPosition((CPoint)Point2D((float)m_size.cx * (sp->m_gridX + 0.5) / gridSize.cx
                                   ,(float)m_size.cy * (sp->m_gridY + 0.5) / gridSize.cy));
  }
}

void NFAPainter::paintState(const NFAPointArray &pointArray, size_t i) {
  const NFAStatePoint *sp = pointArray[i];
  paintState(m_dc, sp->getPosition(), sp->getStateID(), sp->getAttributes());
  if(sp->m_state->m_next) {
    paintTransition(sp, pointArray[sp->m_state->m_next->getID()]);
  }
  if(sp->m_state->m_next2) {
    paintTransition(sp, pointArray[sp->m_state->m_next2->getID()]);
  }
}

void NFAPainter::paintState(HDC hdc, CPoint p, int id, BYTE attr) {
  if(attr & ATTR_ISACCEPTSTATE) {
    paintRing(  p, CIRCLE_RADIUS, getBlackPen(), getGreenBrush()      , hdc);
  } else if(attr & ATTR_ISSTARTSTATE) {
    paintRing(  p, CIRCLE_RADIUS, getBlackPen(), getRedBrush()        , hdc);
    markStartState(hdc, p);
  } else if(!(attr & ATTR_HASPREDECESSOR)) {
    paintRing(  p, CIRCLE_RADIUS, getBlackPen(), getPinkBrush()       , hdc);
  } else if(!(attr & ATTR_HASSUCCESSSORS)) {
    paintRing(  p, CIRCLE_RADIUS, getBlackPen(), getLightGreenBrush() , hdc);
  } else {
    paintCircle(p, CIRCLE_RADIUS, getBlackPen(), nullptr                 , hdc);
  }
  const String text     = format(_T("%d"), id);
  CFont       &font     = getFont();
  const CSize  textSize = getTextSize(font, text);
  textOut(p - textSize / 2, font, text, hdc);
}

void NFAPainter::unpaintState(HDC hdc, CPoint p) {
  paintCircle(p, CIRCLE_RADIUS, getWhitePen(), getWhiteBrush(), hdc);
}

void NFAPainter::paintTransition(const NFAStatePoint *from, const NFAStatePoint *to) {
  const Point2D cf = from->getPosition();
  const Point2D ct = to->getPosition();

  const NFAState &sf = *from->m_state;
  const NFAState &st = *to->m_state;

  Point2D v;
  if(cf == ct) { // transition to the same state. No possible with NFA
    v = Point2D(1,-1).normalize();
    const Point2D pFrom = cf + v  * CIRCLE_RADIUS;
    const Point2D v2(-v.x(),v.y());
    const Point2D pTo   = cf + v2 * CIRCLE_RADIUS;
    paintBezierArrow((CPoint)pFrom, (CPoint)pTo);
  } else {
    v = (ct - cf).normalize();
    Point2D pFrom, pTo;

    if((st.getID() < sf.getID()) && (st.m_next == &sf || st.m_next2 == &sf)) {
      Point2D v2 = -v;
      v  = rotate(v, GRAD2RAD(-45));
      v2 = rotate(v2,GRAD2RAD( 45));
      pFrom = cf + v  * CIRCLE_RADIUS;
      pTo   = ct + v2 * CIRCLE_RADIUS;

      paintBezierArrow((CPoint)pFrom, (CPoint)pTo);
    } else {
      pFrom = cf + v * CIRCLE_RADIUS;
      pTo   = ct - v * CIRCLE_RADIUS;
      paintLineArrow((CPoint)pFrom, (CPoint)pTo);
    }
  }
  Point2D tv(v.y(), -v.x());
  Point2D textPos = cf + (v * (CIRCLE_RADIUS+2) + tv * 17);
  String text;
  CFont *font;
  switch(sf.getEdge()) {
  case EDGE_EPSILON :
    text = format(_T("%c"),101);
    font = &getFont(true, v);
    break;
  case EDGE_CHCLASS :
    text = charBitSetToString(sf.getCharacterSet(), NFAState::getFormater());
    font = &getFont(false, v);
    break;
  default           :
    text = NFAState::getFormater()->toString(sf.getEdge());
    font = &getFont(false, v);
    break;
  }
  textOut((CPoint)textPos, *font, text, m_dc);
}

void NFAPainter::checkAllocated() {
  const int allocated = NFAState::getAllocated();
  if(allocated == 0) {
    s_oldNFAPoints.deleteAll();
    s_newNFAPoints.deleteAll();
  }
}

void NFAPainter::shiftCurrentToNew() {
  moveNewToOld();
  s_newNFAPoints = m_nfaPoints;
  m_nfaPoints.clear();
}

void NFAPainter::moveNewToOld() { // static
  s_oldNFAPoints.deleteAll();
  s_oldNFAPoints = s_newNFAPoints;
  s_newNFAPoints.clear();
}

NFAPointArray NFAPainter::s_oldNFAPoints;
NFAPointArray NFAPainter::s_newNFAPoints;

// -------------------------------------------- Animation ------------------------------------------------------

#define COUNT_CHANGED       0x01
#define POSITIONS_CHANGED   0x02
#define TRANSITIONS_CHANGED 0x04
#define ATTRIBUTES_CHANGED  0x08

class AnimationPoint {
public:
  NFAStatePoint *m_oldP,*m_newP;  // m_oldP can be nullptr
  Point2D        m_from, m_to;    // if m_oldP is nullptr, then just *m_newP
  AnimationPoint() : m_oldP(nullptr), m_newP(nullptr), m_from(0,0), m_to(0,0) {
  }
  AnimationPoint(NFAStatePoint *oldP, NFAStatePoint *newP)
    : m_oldP(oldP)
    , m_newP(newP)
    , m_from(oldP?oldP->getPosition():newP->getPosition())
    , m_to(newP->getPosition())
  {
  }
  CPoint interpolate(double t) const {
    const double t1 = 1.0 - t;
    return (CPoint)(m_from*t1 + m_to*t);
  }
  int getUpdateFlags() const;
};

int AnimationPoint::getUpdateFlags() const {
  if(m_oldP == nullptr) {
    return COUNT_CHANGED;
  }
  int result = 0;
  if(m_from != m_to) {
    result |= POSITIONS_CHANGED;
  }
  const NFAState *p1 = m_oldP->m_state;
  const NFAState *p2 = m_newP->m_state;
  if((p1->m_next != p2->m_next) || (p1->m_next2 != p2->m_next2)) {
    result |= TRANSITIONS_CHANGED;
  }
  if(m_oldP->getAttributes() != m_newP->getAttributes()) {
    result |= ATTRIBUTES_CHANGED;
  }
  return result;
}

void NFAPainter::animateOldToNew(HDC hdc) {
  const size_t pointCount    = s_newNFAPoints.size();
  const size_t oldPointCount = s_oldNFAPoints.size();

  if(pointCount == 0) {
    paintNew(hdc);
    return;
  }

#define MAXSIZE 400
  double animationSteps = (double)MAXSIZE / (pointCount + 5);
  double paintTimePerImage = 1.86 + pointCount * 0.014; // msec

#define TOTALANIMATIONTIME 500.0

  // The relation: TOTALANIMATIONTIME = animationSteps * (paintTimePerImage+sleeptime)
  // => sleepTime = TOTALANIMATIONTIME / animationSteps - paintTimePerImage
  // => animationSteps = TOTALANIMATIONTIME / (sleepTime + paintTimePerImage)

  double sleepTime = TOTALANIMATIONTIME / animationSteps - paintTimePerImage;
  if(sleepTime < 0) {
    sleepTime = 1;
    animationSteps = TOTALANIMATIONTIME / (sleepTime + paintTimePerImage);
    if(animationSteps < 10) {
      animationSteps = 10;
    }
  }

  BitSet usedPointSet(10);
  bool anyMissing = false;
  if(pointCount < oldPointCount) {
    usedPointSet.setCapacity(oldPointCount);
    anyMissing = true;
  }

  int updateFlags = 0;
  CompactArray<AnimationPoint> pointArray;
  for(size_t i = 0; i < pointCount; i++) {
    NFAStatePoint  *newP     = s_newNFAPoints[i];
    const NFAState *newState = newP->m_state;
    NFAStatePoint  *oldP     = nullptr;
    for(size_t j = 0; j < s_oldNFAPoints.size(); j++) { // find the corresponding StatePoint in old pointArray, if any
      NFAStatePoint *old = s_oldNFAPoints[j];
      if(old->m_state == newState) {
        oldP = old;
        if(anyMissing) {
          usedPointSet.add(old->getStateID());
        }
        break;
      }
    }
    pointArray.add(AnimationPoint(oldP, newP));
    updateFlags |= pointArray.last().getUpdateFlags();
  }

  if(updateFlags == 0) {
    return;
  }
  if(anyMissing) {
    BitSet removedPointSet = usedPointSet.invert();
    FallingNFAStateArray fallArray;
    for(auto it = removedPointSet.getIterator(); it.hasNext();) {
      const int id = (int)it.next();
      fallArray.add(FallingNFAState(s_oldNFAPoints[id], id));
    }
    animateFallingStates(hdc, fallArray);
  }
  if(updateFlags & POSITIONS_CHANGED) {
    for(SigmoidIterator it(0,1, (UINT)animationSteps); it.hasNext();) {
      const double t = it.next();
      for(size_t i = 0; i < pointArray.size(); i++) {
        AnimationPoint &ap = pointArray[i];
        ap.m_newP->setPosition(ap.interpolate(t));
      }
      paintNew(hdc);
      Sleep((DWORD)sleepTime);
    }
  } else {
    paintNew(hdc);
  }
}

FallingNFAState::FallingNFAState(const NFAStatePoint *state, int id) {
  m_p0         = state->getPosition();
  m_attributes = state->getAttributes();
  m_id         = id;
}

#define SAVEBACKGROUND()     BitBlt(saveDC, 0,0,m_size.cx, m_size.cy, hdc, 0,0,SRCCOPY)
#define RESTOREBACKGROUND(p) BitBlt(hdc   , p.x-CIRCLE_RADIUS,p.y-CIRCLE_RADIUS,CIRCLE_SIZE,CIRCLE_SIZE, saveDC,p.x-CIRCLE_RADIUS,p.y-CIRCLE_RADIUS,SRCCOPY)

void NFAPainter::animateFallingStates(HDC hdc, FallingNFAStateArray &states) {
  HDC     saveDC  = CreateCompatibleDC(nullptr);
  HBITMAP saveBM  = CreateBitmap(m_size.cx, m_size.cy, 1, 32, nullptr);
  HRGN    clipRgn = CreateRectRgn(0,0,m_size.cx, m_size.cy);
  HGDIOBJ oldGDI  = SelectObject(saveDC, saveBM);
  SelectClipRgn(hdc, clipRgn);

  for(size_t i = 0; i < states.size(); i++) {
    unpaintState(hdc, (CPoint)states[i].m_p0);
  }
  SAVEBACKGROUND();
  const int maxY = m_size.cy + CIRCLE_RADIUS;
  const Point2D v0(-7,-10);
  const Point2D g(0,9.82);
  for(double t = 0;; t += 0.05) {
    bool anyInside = false;
    for(size_t i = 0; i < states.size(); i++) {
      FallingNFAState &s = states[i];
      s.m_pos = (CPoint)(((g * (0.5 * t)) + v0) * t + s.m_p0);
      paintState(hdc, s.m_pos, s.m_id, s.m_attributes);
      if(s.m_pos.y <= maxY) anyInside = true;
    }
    Sleep(1);
    for(size_t i = 0; i < states.size(); i++) {
      const FallingNFAState &s = states[i];
      RESTOREBACKGROUND(s.m_pos);
    }
    if(!anyInside) break;
  }

  SelectObject(saveDC, oldGDI);
  DeleteObject(saveBM);
  DeleteDC(saveDC);

  SelectClipRgn(hdc, nullptr);
  DeleteObject(clipRgn);
}

