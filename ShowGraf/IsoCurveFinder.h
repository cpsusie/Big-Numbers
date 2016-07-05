#pragma once

#include <Stack.h>
#include <CompactArray.h>
#include <CompactHashSet.h>
#include <CompactHashMap.h>
#include <HashMap.h>
#include <Math/Rectangle2D.h>

typedef enum {
   RC_SW // left bottom  corner
  ,RC_NW // left top     corner
  ,RC_SE // right bottom corner
  ,RC_NE // right top    corner
} RectCorner;

    // the RC_NW corner of rectangle (i, j), corresponds with location
    // (m_start.x+(i-0.5)*m_cellSize, m_start.y+(j-0.5)*m_cellSize)

class IsoCurveTest {   // test the function for a signed value
public:
  Point2D m_point;
  bool    m_positive;
  bool    m_ok;
};

class Point2DKey {
public:
  int i, j;

  inline Point2DKey() {}

  inline Point2DKey(int _i, int _j) : i(_i), j(_j) {
  }
  inline unsigned long hashCode() const {
    return i*2347 + j;
  }
  inline bool operator==(const Point2DKey &key) const {
    return i==key.i && j==key.j;
  }
  inline String toString() const {
    return format(_T("(% 5d,% 5d)"), i, j);
  }
};

class RectEdgeHashKey {
private:
  Point2DKey m_key1, m_key2;

  inline void swapKeys() {
    const Point2DKey tmp(m_key1); m_key1 = m_key2; m_key2 = tmp;
  }
  inline void checkAndSwap() {
    if((m_key1.i>m_key2.i) || (m_key1.i==m_key2.i && m_key1.j>m_key2.j)) {
      swapKeys();
    }
  }

public:
  inline RectEdgeHashKey() {}
  inline RectEdgeHashKey(const Point2DKey &key1, const Point2DKey &key2) : m_key1(key1), m_key2(key2) {
    checkAndSwap();
  }
  unsigned long hashCode() const {
    return m_key1.hashCode() + m_key2.hashCode();
  }
  inline bool operator==(const RectEdgeHashKey &e) const {
    return (m_key1 == e.m_key1) && (m_key2 == e.m_key2);
  }
};

class HashedRectCorner { // corner of a rectangle
public:
  Point2DKey  m_key;
  Point2D     m_point;
  bool        m_positive;

  HashedRectCorner(const Point2DKey &k, const Point2D &p) : m_key(k), m_point(p) {
  }
  String toString() const {
    return format(_T("HashedRectCorner:(K:%s, P:%s, %c)"), m_key.toString().cstr(), m_point.toString(6).cstr(), m_positive?'+':'-');
  }
};

class LineSegment { // parameter to receiveLineSegment
public:
  unsigned int m_i1,m_i2;         // indices into pointArray
  inline LineSegment() : m_i1(0), m_i2(0) {
  }
  inline LineSegment(unsigned int i1, unsigned int i2) : m_i1(i1), m_i2(i2) {
  }

  String toString() const {
    return format(_T("Line(%d,%d)"), m_i1,m_i2);
  }
};

class StackedRectangle {
private:
  inline void initCornerArray() {
    memset(m_corners, 0, sizeof(m_corners));
  }
public:
  Point2DKey              m_key;
  const HashedRectCorner *m_corners[4];

  inline StackedRectangle(const Point2DKey &key) : m_key(key) {
    initCornerArray();
  }

  inline StackedRectangle(int i, int j) : m_key(i,j) {
    initCornerArray();
  }
  
/*
  inline bool contains(const Point2D &p) const {
    return (m_corners[RC_SW]->m_point <= p) && (p <= m_corners[RC_NE]->m_point);
  }
*/
  void validate() const;
  inline Point2D getSize() const {
    return m_corners[RC_NE]->m_point - m_corners[RC_SW]->m_point;
  }
  inline Point2D getCenter() const {
    return (m_corners[RC_NE]->m_point + m_corners[RC_SW]->m_point)/2;
  }
  bool hasSignChange() const;
  String toString() const;
};

class IsoCurveEvaluator {
public:
  virtual double evaluate(const Point2D &p) = 0;
  virtual void receiveLineSegment(const LineSegment &line) = 0;
};

class IsoCurveFinderStatistics {
public:
  double       m_threadTime;
  unsigned int m_lineCount;
  unsigned int m_pointCount;
  unsigned int m_rectCount;
  unsigned int m_cornerCount;
  unsigned int m_edgeCount;
  unsigned int m_cornerHits;
  unsigned int m_edgeHits;
  unsigned int m_zeroHits;
  unsigned int m_evalCount;
  unsigned int m_doTriangleCalls;
  unsigned int m_nonProductiveCalls;
  String       m_hashStat;

  IsoCurveFinderStatistics();
  void clear();
  String toString() const;
};

class IsoCurveFinder {
private:
  IsoCurveEvaluator                    &m_eval;            // Implicit surface function
  double                                m_cellSize;
  Rectangle2D                           m_boundingBox;     // bounding box
  Stack<StackedRectangle>               m_rectangleStack;  // Active cubes
  Point2DArray                          m_pointArray;
  CompactHashSet<Point2DKey>            m_centerSet;       // done so far
  HashMap<Point2DKey, HashedRectCorner> m_cornerMap;       // Corners of cubes
  CompactHashMap<RectEdgeHashKey, int>  m_edgeMap;         // Edge -> point
  IsoCurveFinderStatistics              m_statistics;

  Point2D      findStartPoint(const Point2D &start);
  IsoCurveTest findStartPoint(bool positive, const Point2D &p);
  bool         putInitialCube(const Point2D &centerPoint, const Point2DKey &key);
  void         doTriangle(const StackedRectangle &rectangle, RectCorner c1, RectCorner c2, RectCorner c3);
  bool         addCenter(const Point2DKey &key);
  void         testEdge(int i, int j, const StackedRectangle &oldRectangle, int edge, RectCorner c1, RectCorner c2);
  void         resetTables();

  inline double evaluate(const Point2D &p) {
    m_statistics.m_evalCount++;
    return m_eval.evaluate(p);
  }
  void         pushRectangle(const StackedRectangle &rect);
  int          getPointId(const HashedRectCorner &c1, const HashedRectCorner &c2);
  const HashedRectCorner *getCorner(int i, int j);
  Point2D      getCornerPoint(int i, int j) const;
  Point2D      converge(const Point2D &p1, const Point2D &p2, bool p1Positive, int itCount = 0);
  void         saveStatistics(double startTime);

  inline bool boundingBoxContains(const StackedRectangle &r) const {
    return m_boundingBox.contains(r.m_corners[RC_NW]->m_point) && m_boundingBox.contains(r.m_corners[RC_SE]->m_point);
  }

  inline bool boundingBoxContains(const Point2D &p) const {
    return m_boundingBox.contains(p);
  }

public:
  IsoCurveFinder(IsoCurveEvaluator &eval);
  ~IsoCurveFinder();
  void findCurve(double             cellSize
                ,const Rectangle2D &boundingBox
                 );

  inline const IsoCurveFinderStatistics &getStatistics() const {
    return m_statistics;
  }
  inline const Point2DArray &getPointArray() const {
    return m_pointArray;
  }
};

