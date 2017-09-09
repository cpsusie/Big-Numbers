#include "stdafx.h"
#include <Random.h>
#include "IsoCurveFinder.h"

#define RES  10

typedef enum {
   L   // left edge    i--
  ,T   // top edge     j++
  ,R   // right edge   i++
  ,B   // bottom edge  j--
} Edge;

#define HASHSIZE 100001
#define BIT(i, bit) (((i)>>(bit))&1)
#define FLIP(i,bit) ((i)^(1<<(bit))) // flip the given bit of i

IsoCurveFinder::IsoCurveFinder(IsoCurveEvaluator &curveEvaluator)
: m_eval(curveEvaluator)
, m_cornerMap(HASHSIZE)
{
}

IsoCurveFinder::~IsoCurveFinder() {
}

void IsoCurveFinder::findCurve(double             cellSize
                              ,const Rectangle2D &boundingBox
                              ) {

  const double startTime = getThreadTime();


  randomize();
  m_cellSize    = cellSize;
  m_boundingBox = boundingBox;

  m_statistics.clear();
  resetTables();

  Point2D    centerPoint;
  Point2DKey startKey;
  int initialRectanglesCounter = 0;
  for(centerPoint.x = boundingBox.getMinX(), startKey.i = 0; centerPoint.x <= boundingBox.getMaxX(); centerPoint.x += cellSize, startKey.i++) {
    for(centerPoint.y = boundingBox.getMinY(), startKey.j = 0; centerPoint.y <= boundingBox.getMaxY(); centerPoint.y += cellSize, startKey.j++) {
      if(m_centerSet.contains(startKey)) {
        continue;
      }
      if(!putInitialCube(centerPoint, startKey)) {
        continue;
      }
      initialRectanglesCounter++;
      while(!m_rectangleStack.isEmpty()) { // process active rectangles until empty stack
        StackedRectangle rectangle = m_rectangleStack.pop();
        doTriangle(rectangle, RC_SW, RC_NW, RC_SE);
        doTriangle(rectangle, RC_NE, RC_SE, RC_NW);

    // test four edge directions, maybe add to stack:
        testEdge(rectangle.m_key.i - 1, rectangle.m_key.j    , rectangle, L, RC_SW, RC_NW);
        testEdge(rectangle.m_key.i + 1, rectangle.m_key.j    , rectangle, R, RC_NE, RC_SE);
        testEdge(rectangle.m_key.i    , rectangle.m_key.j - 1, rectangle, B, RC_SE, RC_SW);
        testEdge(rectangle.m_key.i    , rectangle.m_key.j + 1, rectangle, T, RC_NW, RC_NE);
      }
    }
  }

  if(initialRectanglesCounter == 0) {
    throwException(_T("Can't find start rectangle"));
  }
  saveStatistics(startTime);
}

void IsoCurveFinder::resetTables() {
  m_pointArray.clear();
  m_edgeMap.clear();
  m_centerSet.clear();
  m_cornerMap.clear();
}

bool IsoCurveFinder::putInitialCube(const Point2D &centerPoint, const Point2DKey &key) {
  if(!boundingBoxContains(centerPoint)) {
    return false;
  }

  StackedRectangle rectangle(key);
  for(int c = 0; c < ARRAYSIZE(rectangle.m_corners); c++) {
    rectangle.m_corners[c] = getCorner(key.i + BIT(c, 1), key.j + BIT(c, 0));
  }

  try {
    if(boundingBoxContains(rectangle) && rectangle.hasSignChange()) { // check rectangle out of bounds,
      pushRectangle(rectangle);                                       // Found a starting point. push initial rectangle on stack
      addCenter(rectangle.m_key);                                     // add its center
      return true;
    }
  } catch(...) {
  }
  return false;
}

void IsoCurveFinder::pushRectangle(const StackedRectangle &rect) {
  m_rectangleStack.push(rect);
  m_statistics.m_rectCount++;
#ifdef DUMP_CUBES
  debugLog(_T("pushRectangle():%s"), rect.toString().cstr());
#endif
}

void IsoCurveFinder::doTriangle(const StackedRectangle &rectangle, RectCorner c1, RectCorner c2, RectCorner c3) {
  m_statistics.m_doTriangleCalls++;

  const HashedRectCorner &a = *rectangle.m_corners[c1];
  const HashedRectCorner &b = *rectangle.m_corners[c2];
  const HashedRectCorner &c = *rectangle.m_corners[c3];
  int index = 0;

  if(a.m_positive) index += 1;
  if(b.m_positive) index += 2;
  if(c.m_positive) index += 4;
  // index is now 3-bit number representing one of the 8 possible cases

  // 6 productive cases (000 and 111 do not give any lineSegments), evaluate positive right for lineSegment P1 -> P2
  switch(index) {
  case  0:                                                     //---
  case  7:                                                     //+++
    m_statistics.m_nonProductiveCalls++;
    return;
  case  1: // +a -b -c
    m_eval.receiveLineSegment(LineSegment(getPointId(a, b), getPointId(a, c)));
    break;
  case  2: // -a +b -c
    m_eval.receiveLineSegment(LineSegment(getPointId(b, c), getPointId(a, b)));
    break;
  case  3: // +a +b -c
    m_eval.receiveLineSegment(LineSegment(getPointId(b, c), getPointId(a, c)));
    break;
  case  4: // -a -b +c
    m_eval.receiveLineSegment(LineSegment(getPointId(a, c), getPointId(b, c)));
    break;
  case  5: // +a -b +c
    m_eval.receiveLineSegment(LineSegment(getPointId(a, b), getPointId(b, c)));
    break;
  case  6: // -a +b +c
    m_eval.receiveLineSegment(LineSegment(getPointId(a, c), getPointId(a, b)));
    break;
  }
  if(m_statistics.m_lineCount++ > 500000) {
    throwException(_T("%s:Too many linesegments. Try a bigger cellsize"), __TFUNCTION__);
  }
}

int IsoCurveFinder::getPointId(const HashedRectCorner &c1, const HashedRectCorner &c2) {
#ifdef VALIDATE_OPPOSITESIGN
  if(c1.m_positive == c2.m_positive) {
    throwException(_T("getPointId:corners have same sign. c1:%s, c2:%s")
                  ,c1.toString().cstr(), c2.toString().cstr());
  }
#endif
  RectEdgeHashKey edgeKey(c1.m_key,c2.m_key);
  int *p = m_edgeMap.get(edgeKey);
  if(p != NULL) {
    m_statistics.m_edgeHits++;
    return *p; // already computed
  }
  const Point2D v = converge(c1.m_point, c2.m_point, c1.m_positive);
  int id = (int)m_pointArray.size();
  m_pointArray.add(v);
  m_edgeMap.put(edgeKey,id);
  return id;
}

Point2D IsoCurveFinder::converge(const Point2D &p1, const Point2D &p2, bool p1Positive, int itCount) {
  Point2D pos, neg;
  if(p1Positive) {
    pos = p1;
    neg = p2;
  } else {
    pos = p2;
    neg = p1;
  }
  double v;
  for(int i = itCount = max(itCount, RES);;) {
    const Point2D result = (pos + neg) / 2;
    if(i-- == 0) {
      return result;
    }
    if((v = evaluate(result)) > 0.0) {
      pos = result;
    } else if(v < 0) {
      neg = result;
    } else {
      m_statistics.m_zeroHits++;
      return result;
    }
  }
}


void IsoCurveFinder::testEdge(int i, int j, const StackedRectangle &oldRectangle, int edge, RectCorner c1, RectCorner c2) {
  static const int flipBits[] = {1, 0,  1, 0}; // indexed by edge. Flip this bit to get the opposite edge

  const bool c1Positive = oldRectangle.m_corners[c1]->m_positive;
  const bool c2Positive = oldRectangle.m_corners[c2]->m_positive;

  // test if no curve crossing or already visited:
  if(c1Positive == c2Positive) {
    return;
  }

  StackedRectangle newRectangle(i, j);
  if(!addCenter(newRectangle.m_key)) {
    return;
  }

  const int bit = flipBits[edge];
  newRectangle.m_corners[FLIP(c1, bit)] = oldRectangle.m_corners[c1];
  newRectangle.m_corners[FLIP(c2, bit)] = oldRectangle.m_corners[c2];
  for(int n = 0; n < ARRAYSIZE(newRectangle.m_corners); n++) {
    if(newRectangle.m_corners[n] == NULL) {
      newRectangle.m_corners[n] = getCorner(i + BIT(n, 1), j + BIT(n, 0));
    }
  }

  if(boundingBoxContains(newRectangle)) { // check rectangle out of bounds,
    pushRectangle(newRectangle);
  }
}

bool IsoCurveFinder::addCenter(const Point2DKey &key) {
  if(m_centerSet.contains(key)) {
    return false;
  } else {
    m_centerSet.add(key);
    return true;
  }
}

const HashedRectCorner *IsoCurveFinder::getCorner(int i, int j) {
  const Point2DKey key(i, j);
  const HashedRectCorner *result = m_cornerMap.get(key);
  if(result != NULL) {
    m_statistics.m_cornerHits++;
    return result;
  } else {
    HashedRectCorner corner(key, getCornerPoint(i,j));
    corner.m_positive = evaluate(corner.m_point) > 0;
    m_cornerMap.put(key, corner);
    if(m_statistics.m_cornerCount++ > 1000000) {
      throwException(_T("%s:Too many corners. Try a bigger cellsize"), __TFUNCTION__);
    }
    return m_cornerMap.get(key);
  }
}

Point2D IsoCurveFinder::getCornerPoint(int i, int j)  const {
  return Point2D(m_boundingBox.m_x + (i-0.5) * m_cellSize, m_boundingBox.m_y + (j-0.5) * m_cellSize);
}

void StackedRectangle::validate() const {
  const HashedRectCorner &nw = *m_corners[RC_NW];
  const HashedRectCorner &sw = *m_corners[RC_SW];
  const HashedRectCorner &ne = *m_corners[RC_NE];
  const HashedRectCorner &se = *m_corners[RC_SE];
  assert(nw.m_point.x == sw.m_point.x);
  assert(ne.m_point.x == se.m_point.x);
  assert(nw.m_point.y == ne.m_point.y);
  assert(sw.m_point.y == se.m_point.y);
  assert(nw.m_point.y >  sw.m_point.y);
  assert(ne.m_point.y >  se.m_point.y);
  assert(se.m_point.x >  sw.m_point.x);
  assert(ne.m_point.x >  nw.m_point.x);
  assert(nw.m_key.i == sw.m_key.i);
  assert(ne.m_key.i == se.m_key.i);
  assert(nw.m_key.j == ne.m_key.j);
  assert(sw.m_key.j == se.m_key.j);
  assert(nw.m_key.j >  sw.m_key.j);
  assert(ne.m_key.j >  se.m_key.j);
  assert(se.m_key.i >  sw.m_key.i);
  assert(ne.m_key.i >  nw.m_key.i);
}

bool StackedRectangle::hasSignChange() const {
  bool c0p = m_corners[0]->m_positive;
  for(int i = 1; i < ARRAYSIZE(m_corners); i++) {
    if(m_corners[i]->m_positive != c0p) {
      return true;
    }
  }
  return false;
}

String StackedRectangle::toString() const {
  String result = format(_T("Rect:Key:%s. Size:%s\n"), m_key.toString().cstr(), getSize().toString(5).cstr());
  for(int i = 0; i < ARRAYSIZE(m_corners); i++) {
    result += format(_T("  %s:%s\n"), ::toString((RectCorner)i).cstr(), m_corners[i]->toString().cstr());
  }
  return result;
}

static String intArrayToString(const CompactIntArray &a) {
  if(a.size() == 0) {
    return EMPTYSTRING;
  } else {
    String result = format(_T("%d:%s"), 0, format1000(a[0]).cstr());
    for(size_t i = 1; i < a.size(); i++) {
      result += format(_T(", %d:%s"), i, format1000(a[i]).cstr());
    }
    return result;
  }
}

void IsoCurveFinder::saveStatistics(double startTime) {
  m_statistics.m_threadTime  = getThreadTime() - startTime;
  m_statistics.m_pointCount  = (int)m_pointArray.size();
  m_statistics.m_cornerCount = (int)m_cornerMap.size();
  m_statistics.m_edgeCount   = (int)m_edgeMap.size();
  m_statistics.m_hashStat    = _T("  CornerMap:") + intArrayToString(m_cornerMap.getLength()) + _T("\n")
                             + _T("  EdgeMap  :") + intArrayToString(m_edgeMap.getLength())   + _T("\n")
                             + _T("  DoneSet  :") + intArrayToString(m_centerSet.getLength()) + _T("\n");
}

IsoCurveFinderStatistics::IsoCurveFinderStatistics() {
  clear();
}

void IsoCurveFinderStatistics::clear() {
  m_threadTime         = 0;
  m_lineCount          = 0;
  m_pointCount         = 0;
  m_rectCount          = 0;
  m_cornerCount        = 0;
  m_edgeCount          = 0;
  m_cornerHits         = 0;
  m_edgeHits           = 0;
  m_zeroHits           = 0;
  m_evalCount          = 0;
  m_doTriangleCalls    = 0;
  m_nonProductiveCalls = 0;
  m_hashStat           = EMPTYSTRING;
}

String IsoCurveFinderStatistics::toString() const {
  return format(_T("IsoCurveFinder statistics\n"
                   "Time                : %.2lf sec.\n"
                   "#Lines              : %s\n"
                   "#Points             : %s\n"
                   "#Rectangles         : %s\n"
                   "#Corners            : %s\n"
                   "#Edges              : %s\n"
                   "#Corner hits        : %s\n"
                   "#Edge hits          : %s\n"
                   "#ZeroHits           : %s\n"
                   "#Functionevaluations: %s\n"
                   "#doTriangle calls   : %s\n"
                   "#Nonproductive calls: %s\n"
                   "HashStatistics:\n"
                   "%s"
                  )
              ,m_threadTime / 1000000
              ,format1000(m_lineCount         ).cstr()
              ,format1000(m_pointCount        ).cstr()
              ,format1000(m_rectCount         ).cstr()
              ,format1000(m_cornerCount       ).cstr()
              ,format1000(m_edgeCount         ).cstr()
              ,format1000(m_cornerHits        ).cstr()
              ,format1000(m_edgeHits          ).cstr()
              ,format1000(m_zeroHits          ).cstr()
              ,format1000(m_evalCount         ).cstr()
              ,format1000(m_doTriangleCalls   ).cstr()
              ,format1000(m_nonProductiveCalls).cstr()
              ,m_hashStat.cstr()
              );
}
