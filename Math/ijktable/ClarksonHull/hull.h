#pragma once

/* hull.h */

#include <stdio.h>
#include "stormacs.h"
#include "points.h"
#include "util.h"

extern site  p;                        /* the current site                                              */

extern Coord   infinity[10];           /* point at infinity for Delaunay triang                         */
extern Coord   mins[MAXDIM], maxs[MAXDIM];

extern int     rdim;                   /* region dimension: (max) number of sites specifying region     */
extern int     cdim;                   /* number of sites currently specifying region                   */
extern long    pnum;
extern double  Huge;

class SiteIterator {
public:
  virtual site next() = 0;
};

class SiteToIndexConverter {
public:
  virtual long getIndex(const site s) const = 0;
};

class StdSiteConverter : public SiteToIndexConverter {
  const u_int        m_dimension;
  const doublearray &m_points;
public:
  StdSiteConverter(u_int dimension, const doublearray &points)
    : m_dimension(dimension)
    , m_points(   points   )
  {
    Assert(points.size() % dimension == 0);
  }
  long getIndex(const site s) const {
    return (p - &m_points[0]) / m_dimension;
  }
};

extern  SiteIterator               *get_site;
extern  const SiteToIndexConverter *site_num;
extern  double                      mult_up;

typedef struct basis_s {
  struct basis_s  *next;               /* free list                                                     */
  int              ref_count;          /* storage management                                            */
  int              lscale;             /* the log base 2 of total scaling of vector                     */
  Coord            sqa, sqb;           /* sums of squared norms of a part and b part                    */
  Coord            vecs[1];            /* the actual vectors, extended by malloc'ing bigger             */
} basis_s;

STORAGE_GLOBALS(basis_s)

typedef struct neighbor {
  site             vert;               /* vertex of simplex                                             */
  struct simplex  *simp;               /* neighbor sharing all vertices but vert                        */
  basis_s         *basis;              /* derived vectors                                               */
} neighbor;

typedef struct simplex {
  struct simplex  *next;               /* free list                                                     */
  long             visit;              /* number of last site visiting this simplex                     */
/*      float Sb; */
  short            mark;
  basis_s         *normal;             /* normal vector pointing inward                                 */
  neighbor         peak;               /* if null, remaining vertices give facet                        */
  neighbor         neigh[1];           /* neighbors of simplex                                          */
} simplex;

STORAGE_GLOBALS(simplex)

class SimplexVisitor {
public:
  virtual void *visit(simplex *, void *) = 0;
};

class VisitSetMark : public SimplexVisitor {
  const short m_newMarkValue;
public:
  VisitSetMark(int markValue) : m_newMarkValue(markValue) {
  }
  void *visit(simplex *s, void *dummy) {;
    s->mark = m_newMarkValue;
    return NULL;
  }
};

class SimplexTester {
public:
  virtual bool test(simplex *, int, void *) = 0;
};

class SimplexTesterTrue : public SimplexTester {
public:
  bool test(simplex *s, int i, void *dummy) {
    return true;
  }
};

class VisitFacetTest : public SimplexVisitor {
public:
  void *visit(simplex *s, void *dummy) {
    return (!s->peak.vert) ? s : NULL;
  }
};

class CheckSimplex : public SimplexVisitor {
public:
  void *visit(simplex *s, void *dummy);
};

class TestIsHull : public SimplexTester {
public:
  bool test(simplex *s, int i, void *dummy) {
    return i > -1;
  }
};

extern SimplexTesterTrue visitAll;
extern VisitFacetTest    facetTest;
extern TestIsHull        testIsHull;

class PointPrinter {
public:
  virtual void out(point *, int, FILE *, int) = 0;
};

class NeighborPrinter {
public:
  virtual void  print(FILE*, neighbor*) = 0;
};

class StdSiteIterator : public SiteIterator {
  const u_int    m_dimension;
  const u_int    m_pointCount;
  doublearray   &m_points;
  mutable u_int  m_currentPoint;
public:
  StdSiteIterator(u_int dimension, doublearray &points, u_int pointCount)
    : m_dimension( dimension )
    , m_points(    points    )
    , m_pointCount(pointCount)
  {
    Assert(points.size() >= m_pointCount * m_dimension);
    m_currentPoint = 0;
  }
  site next() {
    return (m_currentPoint < m_pointCount)
         ? &m_points[m_currentPoint++ * m_dimension]
         : NULL;
  }
};

void  *visit_outside_ashape(simplex *root, SimplexVisitor &visitor);
void  *visit_triang_gen(    simplex *s,    SimplexVisitor &visitor, SimplexTester &tester);
void  *visit_triang(        simplex *s,    SimplexVisitor &visitor);
void  *visit_hull(          simplex *s,    SimplexVisitor &visitor);
void  *print_simplex_f(     simplex *s,    FILE    *F, NeighborPrinter *pf);
bool   sees(                site p    ,    simplex *s);
int    reduce(              basis_s **v,   point p, simplex *s, int k);
void   get_basis_sede(      simplex *s);
void   buildhull(           simplex *root);
void   check_triang(        simplex *root);
void   check_new_triangs(   simplex *s);
Coord  Vec_dot_pdim(        point x, point y);

class PrintFull : public NeighborPrinter {
public:
  void  print(FILE *f, neighbor *n);
};

extern PrintFull printFull;

class AlphaTest : public SimplexTester {
public:
  /* returns true if not an alpha-facet */
  bool test(simplex *s, int i, void *alphap);
};

extern AlphaTest alphaTest;

