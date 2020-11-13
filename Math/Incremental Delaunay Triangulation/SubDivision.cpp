#include "stdafx.h"
#include <CompactHashSet.h>
#include "SubDivision.h"

QuadEdge::QuadEdge() {
  m_e[0].m_num = 0;
  m_e[1].m_num = 1;
  m_e[2].m_num = 2;
  m_e[3].m_num = 3;
  m_e[0].m_next = &(m_e[0]);
  m_e[1].m_next = &(m_e[3]);
  m_e[2].m_next = &(m_e[2]);
  m_e[3].m_next = &(m_e[1]);
}

static Edge *newEdge() {
  QuadEdge *ql = new QuadEdge; TRACE_NEW(ql);
  return ql->getEdge0();
}

void Edge::remove() {
  splice(Oprev());
  Sym()->splice(Sym()->Oprev());
  QuadEdge *qe = getQuadEdge();
  SAFEDELETE(qe);
}

Edge *Edge::connect(Edge *b) {
  Edge *e = newEdge();
  e->splice(Lnext());
  e->Sym()->splice(b);
  e->setEndPoints(Dest(), b->Org());
  return e;
}

void Edge::swap() {
  Edge *a = Oprev();
  Edge *b = Sym()->Oprev();
  splice(a);
  Sym()->splice(b);
  splice(a->Lnext());
  Sym()->splice(b->Lnext());
  setEndPoints(a->Dest(), b->Dest());
}

void Edge::splice(Edge *e) {
  Edge *alpha   = Onext()->Rot();
  Edge *beta    = e->Onext()->Rot();
  Edge *t1      = e->Onext();
  Edge *t2      = Onext();
  Edge *t3      = beta->Onext();
  Edge *t4      = alpha->Onext();
  m_next        = t1;
  e->m_next     = t2;
  alpha->m_next = t3;
  beta->m_next  = t4;
}

/*************** Geometric Predicates for Delaunay Diagrams *****************/
// Returns twice the area of the oriented triangle (a, b, c), i.e., the
// area is positive if the triangle is oriented counterclockwise.
static inline Real TriArea(const Point2d &a, const Point2d &b, const Point2d &c) {
  return (b.x() - a.x())*(c.y() - a.y()) - (b.y() - a.y())*(c.x() - a.x());
}

// Returns true, if the point d is inside the circle defined by the
// points a, b, c. See Guibas and Stolfi (1985) p.107.
static bool InCircle(const Point2d &a, const Point2d &b, const Point2d &c, const Point2d &d) {
  return (a.x()*a.x() + a.y()*a.y()) * TriArea(b, c, d)
       - (b.x()*b.x() + b.y()*b.y()) * TriArea(a, c, d)
       + (c.x()*c.x() + c.y()*c.y()) * TriArea(a, b, d)
       - (d.x()*d.x() + d.y()*d.y()) * TriArea(a, b, c) > 0;
}

// Returns true, if the points a, b, c are in a counterclockwise order
static bool ccw(const Point2d &a, const Point2d &b, const Point2d &c) {
  return TriArea(a, b, c) > 0;
}

bool Edge::rightOf(const Point2d &x) const {
  return ccw(x, Dest2d(), Org2d());
}

bool Edge::leftOf(const Point2d &x) const {
  return ccw(x, Org2d(), Dest2d());
}

#define EPS 1e-8

bool Edge::onEdge(const Point2d &x) const {
  const Real t1 = (x - Org2d()).norm();
  const Real t2 = (x - Dest2d()).norm();
  if(t1 < EPS || t2 < EPS) {
    return true;
  }
  const Real t3 = (Org2d() - Dest2d()).norm();
  if(t1 > t3 || t2 > t3) {
    return false;
  }
  const Line line(Org2d(), Dest2d());
  return (fabs(line.eval(x)) < EPS);
}

/************* An Incremental Algorithm for the Construction of *************/

/************* Topological Operations for Delaunay Diagrams *****************/
SubDivision::SubDivision(const Point2d &a, const Point2d &b, const Point2d &c) {
  Point2d *da = newPoint(a), *db = newPoint(b), *dc = newPoint(c);
  Edge    *ea = newEdge();
  ea->setEndPoints(da, db);
  Edge *eb = newEdge();
  ea->Sym()->splice(eb);
  eb->setEndPoints(db, dc);
  Edge *ec = newEdge();
  eb->Sym()->splice(ec);
  ec->setEndPoints(dc, da);
  ec->Sym()->splice(ea);
  m_startingEdge = ea;
}

SubDivision::~SubDivision() {
  clear();
}

void SubDivision::clear() {
  deleteAllEdges();
  m_pointArray.clear();
  m_startingEdge = NULL;
}

typedef CompactKeyType<const Edge*>  EdgeKey;
class EdgeSet : public CompactHashSet<EdgeKey> {
private:
  void addAll(Edge *e);
public:
  EdgeSet(Edge *e = NULL) {
    if(e) addAll(e);
  }
};

void EdgeSet::addAll(Edge *e) {
  if(!contains(e)) {
    add(e);
    addAll(e->Rnext());
    addAll(e->Lnext());
  }
}

typedef CompactKeyType<const QuadEdge*>  QuadEdgeKey;
typedef CompactHashSet<QuadEdgeKey>      QuadEdgeSet;

void SubDivision::getAllQuadEdges(CompactArray<QuadEdge*> &a) {
  const EdgeSet set(m_startingEdge);
  QuadEdgeSet   qset;
  for(auto it = set.getIterator(); it.hasNext();) {
    const Edge *e = it.next();
    qset.add(e->getQuadEdge());
  }
  a.clear();
  for(auto it = qset.getIterator(); it.hasNext();) {
    const QuadEdge *qe = it.next();
    a.add((QuadEdge*)qe);
  }
}

void SubDivision::deleteAllEdges() {
  CompactArray<QuadEdge*> a;
  getAllQuadEdges(a);
  const size_t n = a.size();
  for(size_t i = 0; i < n; i++) {
    QuadEdge *qe = a[i];
    SAFEDELETE(qe);
  }
}

Edge *SubDivision::locate(const Point2d &x) {
  if(m_startingEdge == NULL) {
    return NULL;
  }
  Edge *e = m_startingEdge;
  EdgeSet visited;
  for(;;) {
    if(visited.contains(e)) {
      return NULL;
    }
    visited.add(e);
    if((x == e->Org2d()) || (x == e->Dest2d())) {
      return e;
    } else if(e->rightOf(x)) {
      e = e->Sym();
    } else if(!e->Onext()->rightOf(x)) {
      e = e->Onext();
    } else if(!e->Dprev()->rightOf(x)) {
      e = e->Dprev();
    } else {
      return e;
    }
  }
}

void SubDivision::insertPoint(const Point2d &x) {
  Edge *e = locate(x);
  if((e == NULL) || (x == e->Org2d()) || (x == e->Dest2d())) { // point is already in
    return;
  } else if(e->onEdge(x)) {
    e = e->Oprev();
    e->Onext()->remove();
  }

// Connect the new point to the vertices of the containing
// triangle (or quadrilateral, if the new point fell on an
// existing edge.)
  Edge *base = newEdge();
  base->setEndPoints(e->Org(), newPoint(x));
  base->splice(e);
  m_startingEdge = base;
  do {
    base = e->connect(base->Sym());
    e = base->Oprev();
  } while(e->Lnext() != m_startingEdge);

  // Examine suspect edges to ensure that the Delaunay condition is satisfied.
  for(;;) {
    Edge *t = e->Oprev();
    if(e->rightOf(t->Dest2d()) && InCircle(e->Org2d(), t->Dest2d(), e->Dest2d(), x)) {
      e->swap();
      e = e->Oprev();
    } else if(e->Onext() != m_startingEdge) { // pop a suspect edge
      e = e->Onext()->Lprev();
    } else { // no more suspect edges
      return;
    }
  }
}

Point2d *SubDivision::newPoint(const Point2d &p) {
  m_pointArray.add(p);
  return &m_pointArray.last();
}

void SubDivision::paint(HDC hdc) const {
  const EdgeSet set(m_startingEdge);
  for(auto it = set.getIterator(); it.hasNext();) {
    const Edge *e = it.next();
    const Point2d &p1 = e->Org2d(), &p2 = e->Dest2d();
    POINT old;
    MoveToEx(hdc, (int)p1.x(),(int)p1.y(), &old);
    LineTo(  hdc, (int)p2.x(),(int)p2.y());
  }
}
