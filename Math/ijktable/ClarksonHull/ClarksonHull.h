/*
 * Ken Clarkson wrote this.  Copyright (c) 1995 by AT&T..
 * Permission to use, copy, modify, and distribute this software for any
 * purpose without fee is hereby granted, provided that this entire notice
 * is included in all copies of any software which is or includes a copy
 * or modification of this software and in all copies of the supporting
 * documentation for such software.
 * THIS SOFTWARE IS BEING PROVIDED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY.  IN PARTICULAR, NEITHER THE AUTHORS NOR AT&T MAKE ANY
 * REPRESENTATION OR WARRANTY OF ANY KIND CONCERNING THE MERCHANTABILITY
 * OF THIS SOFTWARE OR ITS FITNESS FOR ANY PARTICULAR PURPOSE.
 */

#pragma once

#include "fg.h"

#define CHLIB_ROOT "c:/mytools2015/math/ijktable/ClarksonHull/"
#define CHLIB_NAME "ClarksonHull.lib"

#define CHLIB_VERSION CHLIB_ROOT _PLATFORM_ _CONFIGURATION_

#pragma comment(lib, CHLIB_VERSION CHLIB_NAME)

class ClarksonConvexHull : private SimplexVisitor, private SiteToIndexConverter {
private:
  uint               m_cch_dimension;
  fixedarray<Coord> &m_cch_point_coord;
  uint               m_cch_num_points;

  uint               m_cch_convex_hull_dimension;
  uint               m_cch_num_simplices;
  uint               m_cch_current_simplex_vert;
  fixedarray<uint > *m_result;

  void *visit(  simplex *s, void *ignore);
  long  getIndex(const site p) const;
  void  free_hull_storage();
public:
  ClarksonConvexHull(uint  dimension, doublearray &point_coord, uint  num_points);
  ~ClarksonConvexHull();
  void clarkson_convex_hull(uint  &convex_hull_dimension, fixedarray<uint > &simplex_vert, uint  &num_simplices);
};

typedef short zerovolf(simplex *);

simplex *build_convex_hull(SiteIterator &it, const SiteToIndexConverter &site_numm, short dim, short vdd);

/* for debugging */
bool check_perps(simplex *s);
void find_volumes(fg *faces_gr, FILE *F);

/* functions for different formats */
typedef void out_func(point *, int, FILE*, int);
out_func vlist_out, ps_out, cpr_out, mp_out, off_out;

class AFacetsPrint : public SimplexVisitor {
public:
  void *visit(simplex *s, void *p);
};

class RidgesPrint : public SimplexVisitor {
public:
  void *visit(simplex *s, void *p);
};
