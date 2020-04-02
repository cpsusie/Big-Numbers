
/* Kenneth Clarkson's hull routines merged in a single file: 
 * by R. Wenger, November, 2001 
 * clarkson_convex_hull: interface routine for simple retrieval 
 * of convex hull simplices (and their vertices)
 * by R. Wenger, November, 2001
 */

/*
 * Ken Clarkson wrote this. Copyright (c) 1995 by AT&T..
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


#include "stdafx.h"
#include "io.h"
#include "ClarksonHull.h"

/* clarkson_convex_hull */
/* by Rephael Wenger, November, 2001 */

ClarksonConvexHull::ClarksonConvexHull(u_int        dimension
                                      ,doublearray &point_coord
                                      ,u_int        num_points
                                      )
  : m_cch_dimension(  dimension)
  , m_cch_point_coord(point_coord)
  , m_cch_num_points( num_points)
{
    m_cch_convex_hull_dimension = 0;
    m_cch_num_simplices         = 0;
    m_cch_current_simplex_vert  = 0;
}

ClarksonConvexHull::~ClarksonConvexHull() {
  free_hull_storage();
}

void ClarksonConvexHull::free_hull_storage() {
  free_basis_s_storage();
  free_simplex_storage();
  free_Tree_storage();
  free_fg_storage();
}

class SimplexCounter : public SimplexVisitor {
private:
  u_int m_count;
public:
  inline SimplexCounter() : m_count(0) {
  }
  void *visit(simplex *, void *) {
    m_count++;
    return NULL;
  }
  inline u_int getCount() const {
    return m_count;
  }
};

// point_coord = list of point coordinates
// num_points = number of points
// convex_hull_dimension = dimension of convex hull (including interior)
// simplex_vert = list of simplex vertices
// num_simplices = number of simplices
void ClarksonConvexHull::clarkson_convex_hull(u_int &convex_hull_dimension, fixedarray<u_int> &simplex_vert, u_int &num_simplices) {
  struct simplex *root;
  u_int num_simplex_vert;            // total number of simplex vertices

  DFILE = stderr;

  convex_hull_dimension = 0;
  simplex_vert.setSize(0);
  num_simplices         = 0;

  if((m_cch_dimension < 1) || (m_cch_num_points == 0)) {
    return;
  }

  StdSiteIterator        it(     m_cch_dimension, m_cch_point_coord, m_cch_num_points);
  root = build_convex_hull(it, *this, m_cch_dimension, 0);

  // initialize simplex data
  m_cch_convex_hull_dimension = cdim;
  m_cch_num_simplices         = 0;

  // count number of simplices
  SimplexCounter counter;
  visit_hull(root, counter);
  m_cch_num_simplices = counter.getCount();

  // allocate memory
  num_simplex_vert = m_cch_num_simplices * m_cch_convex_hull_dimension;

  fixedarray<u_int> cch_simplex_vert(num_simplex_vert);
  m_result = &cch_simplex_vert;

  // store simplex vertices
  m_cch_current_simplex_vert = 0;
  visit_hull(root, *this);

  convex_hull_dimension = m_cch_convex_hull_dimension;
  simplex_vert          = cch_simplex_vert;
  num_simplices         = m_cch_num_simplices;

  free_hull_storage();
}

// return site number of p
long ClarksonConvexHull::getIndex(const site p) const {
  return (p - &m_cch_point_coord[0]) / m_cch_dimension;
}

// s = simplex
// ignore = ignore this field
void *ClarksonConvexHull::visit(simplex *s, void *ignore) {
  for(u_int j = 0; j < m_cch_convex_hull_dimension; j++) {
    (*m_result)[m_cch_current_simplex_vert++] = getIndex(s->neigh[j].vert);
  }
  return NULL;
}

void free_simplex_vertices(u_int * simplex_vert) {
  if(simplex_vert != NULL) {
    free(simplex_vert);
  }
}

void vols(fg *f, Tree *t, basis_s* n, int depth) {
  static simplex *s;
  static neighbor *sn;
  int tdim = cdim;
  basis_s *nn = 0;
  int signum;
  point nnv;
  double sqq;

  if(!t) {
    return;
  }

  if(!s) {
    NEWL(simplex, s); sn = s->neigh;
  }
  cdim = depth;
  s->normal = n;
  if(depth > 1 && sees(t->key, s)) {
    signum = -1;
  }
  else {
    signum = 1;
  }
  cdim = tdim;

  if(t->fgs->dist == 0) {
    sn[depth - 1].vert = t->key;
    NULLIFY(basis_s, sn[depth - 1].basis);
    cdim = depth;
    get_basis_sede(s);
    cdim = tdim;
    reduce(&nn, infinity, s, depth);
    nnv = nn->vecs;
    if(t->key == infinity || f->dist == Huge || NEARZERO(nnv[rdim - 1])) {
      t->fgs->dist = Huge;
    }
    else {
      t->fgs->dist = Vec_dot_pdim(nnv, nnv) / 4 / nnv[rdim - 1] / nnv[rdim - 1];
    }
    if(!t->fgs->facets) {
      t->fgs->vol = 1;
    }
    else {
      vols(t->fgs, t->fgs->facets, nn, depth + 1);
    }
  }

  Assert(f->dist != Huge || t->fgs->dist == Huge);
  if(t->fgs->dist == Huge || t->fgs->vol == Huge) {
    f->vol = Huge;
  }
  else {
    sqq = t->fgs->dist - f->dist;
    if(NEARZERO(sqq)) {
      f->vol = 0;
    }
    else {
      f->vol += signum * sqrt(sqq) *t->fgs->vol / (cdim - depth + 1);
    }
  }
  vols(f, t->left, n, depth);
  vols(f, t->right, n, depth);
}

void find_volumes(fg *faces_gr, FILE *F) {
  if(!faces_gr) {
    return;
  }
  vols(faces_gr, faces_gr->facets, 0, 1);
  print_fg(faces_gr, F);
}

/* outfuncs: given a list of points, output in a given format */
void vlist_out(point *v, int vdim, FILE *Fin, int amble) {
  static FILE *F;
  int j;

  if(Fin) {
    F=Fin; 
    if(!v) {
      return;
    }
  }
  for(j = 0; j < vdim; j++) {
    fprintf(F, "%d ", site_num->getIndex(v[j]));
  }
  fprintf(F,"\n");
}

void off_out(point *v, int vdim, FILE *Fin, int amble) {
  static FILE *F, *G;
  static FILE *OFFFILE;
  static char offfilenam[L_tmpnam];
  char comst[100], buf[200];
  int j,i;

  if(Fin) {F=Fin;}

  if(pdim!=3) { warning(-10, off apparently for 3d points only); return;}

  if(amble == 0) {
    for(i = 0; i < vdim; i++) {
      if(v[i] == infinity) {
        return;
      }
    }
    fprintf(OFFFILE, "%d ", vdim);
    for(j = 0; j < vdim; j++) {
      fprintf(OFFFILE, "%d ", site_num->getIndex(v[j]));
    }
    fprintf(OFFFILE, "\n");
  } else if(amble == -1) {
    OFFFILE = efopen(tmpnam(offfilenam), "w");
  } else {
    fclose(OFFFILE);
    fprintf(F, "	OFF\n");
        
    sprintf(comst, "wc %s", tmpfilenam);
    G = epopen(comst, "r");
    fscanf(G, "%d", &i);
    fprintf(F, " %d", i);
    pclose(G);
       
    sprintf(comst, "wc %s", offfilenam);
    G = epopen(comst, "r");
    fscanf(G, "%d", &i);
    fprintf(F, " %d", i);
    pclose(G);
        
    fprintf (F, " 0\n");
        
    G = efopen(tmpfilenam, "r");
    while(fgets(buf, sizeof(buf), G)) {
      fprintf(F, "%s", buf);
    }
    fclose(G);
    G = efopen(offfilenam, "r");
    while(fgets(buf, sizeof(buf), G)) {
      fprintf(F, "%s", buf);
    }
    fclose(G);
  }
}

/* should fix scaling */
void mp_out(point *v, int vdim, FILE *Fin, int amble) {
  static int figno=1;
  static FILE *F;

  if(Fin) {
    F=Fin;
  }

  if(pdim!=2) { 
    warning(-10, mp for planar points only);
    return;
  }
  if(amble==0) {
    int i;
    if(!v) {
      return;
    }
    for(i = 0; i < vdim; i++) if(v[i] == infinity) {
      point t = v[i];
      v[i] = v[vdim - 1];
      v[vdim - 1] = t;
      vdim--;
      break;
    }
    fprintf(F, "draw ");
    for(i = 0; i < vdim; i++) {
      fprintf(F, (i + 1 < vdim) ? "(%Gu,%Gu)--" : "(%Gu,%Gu);\n", v[i][0] / mult_up, v[i][1] / mult_up);
    }
  } else if(amble==-1) {
    if(figno == 1) {
      fprintf(F, "u=1pt;\n");
    }
    fprintf(F , "beginfig(%d);\n",figno++);
  } else if(amble==1) {
    fprintf(F , "endfig;\n");
  }
}

void ps_out(point *v, int vdim, FILE *Fin, int amble) {
  static FILE *F;
  static double scaler;

  if(Fin) {
    F=Fin;
  }

  if(pdim!=2) { 
    warning(-10, ps for planar points only);
    return;
  }

  if(amble==0) {
    if(!v) {
      return;
    }
    for(int i = 0; i < vdim; i++) if(v[i] == infinity) {
      point t = v[i];
      v[i] = v[vdim - 1];
      v[vdim - 1] = t;
      vdim--;
      break;
    }
    fprintf(F, "newpath %G %G moveto\n", v[0][0]*scaler,v[0][1]*scaler);
    for(int i = 1; i < vdim; i++) {
      fprintf(F, "%G %G lineto\n", v[i][0] * scaler, v[i][1] * scaler);
    }
    fprintf(F, "stroke\n");
  } else if(amble==-1) {
    double len[2], maxlen;
    fprintf(F, "%%!PS\n");
    len[0] = maxs[0]-mins[0];
    len[1] = maxs[1]-mins[1];
    maxlen = (len[0]>len[1]) ? len[0] : len[1];
    scaler = 216/maxlen;
        
    fprintf(F,"%%%%BoundingBox: %G %G %G %G \n"
             ,mins[0]*scaler
             ,mins[1]*scaler
             ,maxs[0]*scaler
             ,maxs[1]*scaler
            );
    fprintf(F, "%%%%Creator: hull program\n");
    fprintf(F, "%%%%Pages: 1\n");
    fprintf(F, "%%%%EndProlog\n");
    fprintf(F, "%%%%Page: 1 1\n");
    fprintf(F, " 0.5 setlinewidth [] 0 setdash\n");
    fprintf(F, " 1 setlinecap 1 setlinejoin 10 setmiterlimit\n");
  } else if(amble==1) {
    fprintf(F , "showpage\n %%%%EOF\n");
  }
}

void cpr_out(point *v, int vdim, FILE *Fin, int amble) {
  static FILE *F;

  if(Fin) {
    F=Fin;
    if(!v) {
      return;
    }
  }

  if(pdim!=3) {
    warning(-10, cpr for 3d points only);
    return;
  }
        
  for(int i = 0; i < vdim; i++) {
    if(v[i] == infinity) {
      return;
    }
  }

  fprintf(F, "t %G %G %G %G %G %G %G %G %G 3 128\n"
           , v[0][0]/mult_up,v[0][1]/mult_up,v[0][2]/mult_up
           , v[1][0]/mult_up,v[1][1]/mult_up,v[1][2]/mult_up
           , v[2][0]/mult_up,v[2][1]/mult_up,v[2][2]/mult_up
  );
}

void *afacets_print(simplex *s, void *p) {
  static out_func *out_func_here;
  point v[MAXDIM];

  if(p) {
    out_func_here = (out_func*)p;
    if(!s) {
      return NULL;
    }
  }

  for(int j = 0; j < cdim; j++) { /* check for ashape consistency */
    int k;
    for(k = 0; k < cdim; k++) {
      if(s->neigh[j].simp->neigh[k].simp == s) {
        break;
      }
    }
    if(alphaTest.test(s, j, 0) != alphaTest.test(s->neigh[j].simp, k, 0)) {
      DEB(-10, alpha - shape not consistent)
        DEBTR(-10)
        print_simplex_f(s, DFILE, &printFull);
      print_simplex_f(s->neigh[j].simp, DFILE, &printFull);
      fflush(DFILE);
      exit(1);
    }
  }
  for(int j = 0; j < cdim; j++) {
    int vnum = 0;
    if(alphaTest.test(s, j, 0)) {
      continue;
    }
    for(int k = 0; k < cdim; k++) {
      if(k == j) {
        continue;
      }
      v[vnum++] = s->neigh[k].vert;
    }
    out_func_here(v, cdim - 1, 0, 0);
  }
  return NULL;
}

/* to print facets, alpha facets, ridges */
class FacetsPrint : public SimplexVisitor {
public:
  void *visit(simplex *s, void *p);
};

/* vist_funcs for different kinds of output: facets, alpha shapes, etc. */
void *FacetsPrint::visit(simplex *s, void *p) {
  static out_func *out_func_here;
  point v[MAXDIM];
  if(p) {
    out_func_here = (out_func*)p;
    if(!s) {
      return NULL;
    }
  }
  for(int j = 0; j < cdim; j++) {
    v[j] = s->neigh[j].vert;
  }
  out_func_here(v, cdim, 0, 0);
  return NULL;
}

void *ridges_print(simplex *s, void *p) {
  static out_func *out_func_here;
  point v[MAXDIM];

  if(p) {
    out_func_here = (out_func*)p;
    if(!s) {
      return NULL;
    }
  }

  for(int j = 0; j < cdim; j++) {
    int vnum = 0;
    for(int k = 0; k < cdim; k++) {
      if(k == j) {
        continue;
      }
      v[vnum++] = (s->neigh[k].vert);
    }
    out_func_here(v, cdim - 1, 0, 0);
  }
  return NULL;
}
