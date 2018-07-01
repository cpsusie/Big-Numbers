#pragma once

#include <Math/Point2D.h>
#include <MFCUtil/WinTools.h>

typedef Point2D Point2d;
#define norm() length()

class Line : public Line2D {
public:
  inline Line(const Point2d &p1, const Point2d &p2) : Line2D(p1, p2) {
  }
  Real eval(const Point2d &x) const {
    return distanceFromLineSegment(*this,x);
  }
};

class Edge {
  friend class QuadEdge;
private:
  int      m_num;
  Edge    *m_next;
  Point2d *m_data;

  // Return the dual of the current edge, directed from its right to its left.
  inline const Edge *Rot() const {
    return (m_num < 3) ? this + 1 : this - 3;
  }
  inline Edge *Rot() {
    return (m_num < 3) ? this + 1 : this - 3;
  }
  // Return the dual of the current edge, directed from its left to its right.
  inline const Edge *invRot() const {
    return (m_num > 0) ? this - 1 : this + 3;
  }
  inline Edge *invRot() {
    return (m_num > 0) ? this - 1 : this + 3;
  }
public:
  inline Edge() {
    m_data = NULL;
  }
  void remove();

  // Return the edge from the destination to the origin of the current edge.
  inline const Edge *Sym() const {
    return (m_num < 2) ? this + 2 : this - 2;
  }
  inline Edge *Sym() {
    return (m_num < 2) ? this + 2 : this - 2;
  }

  // Return the next ccw edge around (from) the origin of the current edge.
  inline Edge *Onext() const {
    return m_next;
  }

  // Return the next cw edge around (from) the origin of the current edge.
  inline Edge *Oprev() const {
    return Rot()->Onext()->Rot();
  }

  // Return the next ccw edge around (into) the destination of the current edge.
  inline Edge *Dnext() const {
    return Sym()->Onext()->Sym();
  }

  // Return the next cw edge around (into) the destination of the current edge.
  inline Edge *Dprev() const {
    return invRot()->Onext()->invRot();
  }

  // Return the ccw edge around the left face following the current edge.
  inline Edge *Lnext() const {
    return invRot()->Onext()->Rot();
  }

  // Return the ccw edge around the left face before the current edge.
  inline Edge *Lprev() const {
    return Onext()->Sym();
  }

  // Return the edge around the right face ccw following the current edge.
  inline Edge *Rnext() const {
    return Rot()->Onext()->invRot();
  }

  // Return the edge around the right face ccw before the current edge.
  inline Edge *Rprev() const {
    return Sym()->Onext();
  }

  inline QuadEdge *getQuadEdge() const {
    return (QuadEdge*)(this - m_num);
  }

  /************** Access to data pointers *************************************/
  inline Point2d *Org() const {
    return m_data;
  }

  inline Point2d *Dest() const {
    return Sym()->m_data;
  }

  inline const Point2d &Org2d() const {
    return *m_data;
  }

  inline const Point2d &Dest2d() const {
    return (m_num < 2) ? *((this + 2)->m_data) : *((this - 2)->m_data);
  }

  inline void setEndPoints(Point2d *or, Point2d *de) {
    m_data = or;
    Sym()->m_data = de;
  }

  // This operator affects the two edge rings around the origins of this and e,
  // and, independently, the two edge rings around the left faces of this and e.
  // In each case, (i) if the two rings are distinct, splice will combine
  // them into one; (ii) if the two are the same ring, splice will break it
  // into two separate pieces.
  // Thus, splice can be used both to attach the two edges together, and
  // to break them apart. See Guibas and Stolfi (1985) p.96 for more details
  // and illustrations.
  void splice(Edge *e);

  // Add a new edge e connecting the destination of this to the
  // origin of b, in such a way that all three have the same
  // left face after the connection is complete.
  // Additionally, the data pointers of the new edge are set.
  Edge *connect(Edge *b);

  // Essentially turns edge counterclockwise inside its enclosing
  // quadrilateral. The data pointers are modified accordingly.
  void swap();

  // A predicate that determines if the point x is on the *this.
  // The point is considered on if it is in the EPS-neighborhood
  // of the edge.
  bool onEdge( const Point2d &x) const;
  bool rightOf(const Point2d &x) const;
  bool leftOf( const Point2d &x) const;
};

class QuadEdge {
private:
  Edge m_e[4];
public:
  QuadEdge();
  inline Edge *getEdge0() {
    return m_e;
  }
};

class SubDivision {
private:
  Edge                   *m_startingEdge;
  Array<Point2d>          m_pointArray;

  Point2d *newPoint(const Point2d &p);
  // Returns an edge e, s.t. either x is on e, or e is an edge of
  // a triangle containing x. The search starts from startingEdge
  // and proceeds in the general direction of x. Based on the
  // pseudocode in Guibas and Stolfi (1985) p.121.
  Edge *locate(const Point2d &x);
  void getAllQuadEdges(CompactArray<QuadEdge*> &a);
  void deleteAllEdges();
public:
  // Initialize a subdivision to the triangle defined by the points a, b, c.
  SubDivision(const Point2d &p1, const Point2d &p2, const Point2d &p3);
  // Inserts a new point into a subdivision representing a Delaunay
  // triangulation, and fixes the affected edges so that the result
  // is still a Delaunay triangulation. This is based on the
  // pseudocode from Guibas and Stolfi (1985) p.120, with slight
  // modifications and a bug fix.
  virtual ~SubDivision();
  void clear();
  void insertPoint(const Point2d &x);
  void paint(HDC hdc) const;
};
