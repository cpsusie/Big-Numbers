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
  friend void Splice(Edge *a, Edge *b);
private:
  int      num;
  Edge    *next;
  Point2d *data;
public:

  inline Edge() {
    data = NULL;
  }

  // Return the dual of the current edge, directed from its right to its left.
  inline const Edge *Rot() const {
    return (num < 3) ? this + 1 : this - 3;
  }
  inline Edge *Rot() {
    return (num < 3) ? this + 1 : this - 3;
  }

  // Return the dual of the current edge, directed from its left to its right.
  inline const Edge *invRot() const {
    return (num > 0) ? this - 1 : this + 3;
  }
  inline Edge *invRot() {
    return (num > 0) ? this - 1 : this + 3;
  }

  // Return the edge from the destination to the origin of the current edge.
  inline const Edge *Sym() const {
    return (num < 2) ? this + 2 : this - 2;
  }
  inline Edge *Sym() {
    return (num < 2) ? this + 2 : this - 2;
  }

  // Return the next ccw edge around (from) the origin of the current edge.
  inline Edge *Onext() const {
    return next;
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

  /************** Access to data pointers *************************************/
  inline Point2d *Org() const {
    return data;
  }

  inline Point2d *Dest() const {
    return Sym()->data;
  }

  inline const Point2d &Org2d() const {
    return *data;
  }

  inline const Point2d &Dest2d() const {
    return (num < 2) ? *((this + 2)->data) : *((this - 2)->data);
  }

  inline void EndPoints(Point2d *or, Point2d *de) {
    data = or;
    Sym()->data = de;
  }

  inline QuadEdge *Qedge() {
    return (QuadEdge*)(this - num);
  }
};

class QuadEdge {
private:
  Edge e[4];
public:
  QuadEdge();
  inline Edge *getEdge0() {
    return e;
  }
};

class Subdivision {
private:
  Edge *m_startingEdge;
  // Returns an edge e, s.t. either x is on e, or e is an edge of
  // a triangle containing x. The search starts from startingEdge
  // and proceeds in the general direction of x. Based on the
  // pseudocode in Guibas and Stolfi (1985) p.121.
  Edge *Locate(const Point2d &x);
public:
  Subdivision(const Point2d &p1, const Point2d &p2, const Point2d &p3);
  // Inserts a new point into a subdivision representing a Delaunay
  // triangulation, and fixes the affected edges so that the result
  // is still a Delaunay triangulation. This is based on the
  // pseudocode from Guibas and Stolfi (1985) p.120, with slight
  // modifications and a bug fix.
  void InsertSite(const Point2d &x);
  void paint(HDC hdc) const;
};
