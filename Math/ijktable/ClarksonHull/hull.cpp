/*
 *  Includes Clarkson's modification of:
 *
 * An implementation of top-down splaying with sizes
 * D. Sleator <sleator@cs.cmu.edu>, January 1994.
 */

#include "stdafx.h"
#include "points.h"
#include "io.h"
#include "hull.h"
#include "fg.h"

/* hull.c */

site                        p;
SiteIterator               *get_site;
const SiteToIndexConverter *site_num;
double                      mult_up = 1.0;

Coord        infinity[10] = { 57.2,0,0,0,0 }; /* point at infinity for vd; value not used */
Coord        mins[MAXDIM] = { DBL_MAX,DBL_MAX,DBL_MAX,DBL_MAX,DBL_MAX,DBL_MAX,DBL_MAX,DBL_MAX };
Coord        maxs[MAXDIM] = { -DBL_MAX,-DBL_MAX,-DBL_MAX,-DBL_MAX,-DBL_MAX,-DBL_MAX,-DBL_MAX,-DBL_MAX };

int          rdim;        /* region dimension: (max) number of sites specifying region */
int          cdim;        /* number of sites currently specifying region               */
long         pnum;
static int   site_size;   /* size of malloc needed for a site                          */
static int   point_size;  /* size of malloc needed for a point                         */


static void  get_normal(        simplex *s);
static void  set_ch_root(       simplex *d);
static void  print_site(site p, FILE    *F);

static bool     out_of_flat(     simplex *root, point p);
static simplex *extend_simplices(simplex *s);
static simplex *search(          simplex *root);
static simplex *make_facets(     simplex *seen);
static void     connect(         simplex *s);

static void  print_neighbor_snum(FILE *F, neighbor *n);
static void *print_facet(        FILE *F, simplex  *s, NeighborPrinter &p);
static void  print_basis(        FILE *F, basis_s  *b);
static void  print_triang(   simplex *root, FILE *F,   NeighborPrinter &p);

static neighbor *op_simp(    simplex *a, simplex *b);
static neighbor *op_vert(    simplex *a, site     b);

SimplexTesterTrue visitAll;
TestIsHull        testIsHull;
VisitFacetTest    facetTest;
PrintFull         printFull;
CheckSimplex      checkSimplex;
STORAGE(simplex)

#define push(x) *(st+tms++) = x;
#define pop(x)  x = *(st + --tms);

point get_another_site() {
  /* static int scount =0; */
  /* if(!(++scount%1000)) {fprintf(DFILE,"site %d...", scount);} */
  /* check_triang(); */
  point pnext = get_site->next();
  if(!pnext) {
    return NULL;
  }
  pnum = site_num->getIndex(pnext) + 2;
  return pnext;
}

void buildhull(simplex *root) {
  while(cdim < rdim) {
    p = get_another_site();
    if(!p) return;
    if(out_of_flat(root,p)) {
      extend_simplices(root);
    } else {
      connect(make_facets(search(root)));
    }
  }

  while(p = get_another_site()) {
    connect(make_facets(search(root)));
  }
}

/*
 * starting at s, visit simplices t such that test(s,i,0) is true,
 * and t is the i'th neighbor of s;
 * apply visit function to all visited simplices;
 * when visit returns non NULL, exit and return its value
 */
void *visit_triang_gen(simplex *s, SimplexVisitor &visitor, SimplexTester &tester) {
  neighbor *sn;
  void     *v;
  simplex  *t;
  int       i;
  long      tms = 0;
  static long      vnum = -1;
  static long      ss = 2000;
  static simplex **st;

  vnum--;
  if(!st) { 
    /* bug fix: 10-17-2006 by R. Wenger */
    st = MALLOC(simplex*,(ss+MAXDIM+1));
    Assert(st != 0);
  }

  if(s) {
    push(s);
  }

  while(tms) {
    if(tms > ss) {
      DEBEXP(-1, tms);
      /* bug fix: 10-17-2006 by R. Wenger */
      st = REALLOC(st, simplex*,(ss += ss) + MAXDIM + 1);
      Assert(st != 0);
    }
    pop(t);
    if(!t || t->visit == vnum) {
      continue;
    }
    t->visit = vnum;
    if(v = visitor.visit(t,0)) {
      return v;
    }
    for(i = -1, sn = t->neigh - 1; i < cdim; i++, sn++) {
      if((sn->simp->visit != vnum) && sn->simp && tester.test(t, i, 0)) {
        push(sn->simp);
      }
    }
  }
  return NULL;
}

/* visit the whole triangulation */
void *visit_triang(simplex *root, SimplexVisitor &visitor) {
  return visit_triang_gen(root, visitor, visitAll);
}

/* visit all simplices with facets of the current hull */
void *visit_hull(simplex *root, SimplexVisitor &visit) {
  return visit_triang_gen((simplex*)visit_triang(root, facetTest), visit, testIsHull);
}


#define lookup(a,b,what,whatt) {                                        \
  int i;                                                                \
  neighbor *x;                                                          \
  for(i=0, x = a->neigh; (x->what != b) && (i<cdim) ; i++, x++);        \
  if(i<cdim) {                                                          \
    return x;                                                           \
  } else {                                                              \
    throwException("%s:adjacency failure,op_%s", __FUNCTION__, #what);  \
    return 0;                                                           \
  }                                                                     \
}

/* the neighbor entry of a containing b */
static neighbor *op_simp(simplex *a, simplex *b) {
  lookup(a, b, simp, simplex);
}

/* the neighbor entry of a containing b */
static neighbor *op_vert(simplex *a, site b) {
  lookup(a, b, vert, site);
}

/* make neighbor connections between newly created simplices incident to p */
static void connect(simplex *s) {
  site xf,xb,xfi;
  simplex *sb, *sf, *seen;
  int i;
  neighbor *sn;

  if(!s) return;
  Assert(!s->peak.vert && s->peak.simp->peak.vert==p && !op_vert(s,p)->simp->peak.vert);
  if(s->visit == pnum) {
    return;
  }
  s->visit = pnum;
  seen = s->peak.simp;
  xfi = op_simp(seen,s)->vert;
  for(i=0, sn = s->neigh; i<cdim; i++,sn++) {
    xb = sn->vert;
    if (p == xb) {
      continue;
    }
    sb = seen;
    sf = sn->simp;
    xf = xfi;
    if(!sf->peak.vert) {   /* are we done already? */
      sf = op_vert(seen,xb)->simp;
      if(sf->peak.vert) {
        continue;
      }
    } else {
      do {
        xb = xf;
        xf = op_simp(sf,sb)->vert;
        sb = sf;
        sf = op_vert(sb,xb)->simp;
      } while(sf->peak.vert);
    }

    sn->simp = sf;
    op_vert(sf,xf)->simp = s;
    connect(sf);
  }
}

/*
 * visit simplices s with sees(p,s), and make a facet for every neighbor
 * of s not seen by p
 */
static simplex *make_facets(simplex *seen) {
  simplex        *n;
  static simplex *ns;
  neighbor       *bn;
  int             i;

  if(!seen) return NULL;
  DEBS(-1) Assert(sees(p,seen) && !seen->peak.vert); EDEBS
  seen->peak.vert = p;

  for(i=0,bn = seen->neigh; i<cdim; i++,bn++) {
    n = bn->simp;
    if(pnum != n->visit) {
      n->visit = pnum;
      if(sees(p, n)) {
        make_facets(n);
      }
    } 
    if(n->peak.vert) {
      continue;
    }
    copy_simp(ns,seen);
    ns->visit = 0;
    ns->peak.vert = 0;
    ns->normal = 0;
    ns->peak.simp = seen;
/*              ns->Sb -= ns->neigh[i].basis->sqb; */
    NULLIFY(basis_s,ns->neigh[i].basis);
    ns->neigh[i].vert = p;
    bn->simp = op_simp(n,seen)->simp = ns;
  }
  return ns;
}

/*
 * p lies outside flat containing previous sites;
 * make p a vertex of every current simplex, and create some new simplices
 */
static simplex *extend_simplices(simplex *s) {
  int       i;
  int       ocdim = cdim-1;
  simplex  *ns;
  neighbor *nsn;

  if(s->visit == pnum) return s->peak.vert ? s->neigh[ocdim].simp : s;
  s->visit = pnum;
  s->neigh[ocdim].vert = p;
  NULLIFY(basis_s,s->normal);
  NULLIFY(basis_s,s->neigh[0].basis);
  if(!s->peak.vert) {
    s->neigh[ocdim].simp = extend_simplices(s->peak.simp);
    return s;
  } else {
    copy_simp(ns,s);
    s->neigh[ocdim].simp = ns;
    ns->peak.vert = NULL;
    ns->peak.simp = s;
    ns->neigh[ocdim] = s->peak;
    inc_ref(basis_s,s->peak.basis);
    for(i = 0, nsn = ns->neigh; i < cdim; i++, nsn++) {
      nsn->simp = extend_simplices(nsn->simp);
    }
  }
  return ns;
}

/* return a simplex s that corresponds to a facet of the
 * current hull, and sees(p, s)
 */
static simplex *search(simplex *root) {
  simplex         *s;
  static simplex **st;
  static long      ss = MAXDIM;
  neighbor        *sn;
  int              i;
  long             tms = 0;

  if(!st) {
    st = MALLOC(simplex*,ss+MAXDIM+1);
  }
  push(root->peak.simp);
  root->visit = pnum;
  if(!sees(p, root)) {
    for(i = 0, sn = root->neigh; i < cdim; i++, sn++) {
      push(sn->simp);
    }
  }
  while(tms) {
    if(tms>ss) {
      /* bug fix: 10-17-2006 by R. Wenger */
      st = REALLOC(st, simplex*, (ss+=ss)+MAXDIM+1);
      Assert(st != 0);
    }

    pop(s);
    if(s->visit == pnum) {
      continue;
    }
    s->visit = pnum;
    if(!sees(p, s)) {
      continue;
    }
    if (!s->peak.vert) {
      return s;
    }
    for (i = 0, sn = s->neigh; i < cdim; i++, sn++) {
      push(sn->simp);
    }
  }
  return NULL;
}

/* ch.c */
short    check_overshoot_f=0;
simplex *ch_root;

#define SMALL (100*FLT_EPSILON)*(100*FLT_EPSILON)
#define SWAP(X,a,b) {X t; t = a; a = b; b = t;}
#define DMAX 
double Huge;

#define check_overshoot(x) {                                                     \
  if(CHECK_OVERSHOOT && check_overshoot_f && ((x)>9e15)) {                       \
    warning(-20, overshot exact arithmetic);                                     \
  }                                                                              \
}

#define DELIFT 0
int basis_vec_size;

#define lookupshort(a,b,whatb,c,whatc) {                                         \
  int i;                                                                         \
  neighbor *x;                                                                   \
  c = NULL;                                                                      \
  for(i = -1, x = a->neigh-1; (x->whatb != b) && (i<cdim); i++, x++);            \
  if(i<cdim) c = x->whatc;                                                       \
}
        
Coord Vec_dot(point x, point y) {
  int i;
  Coord sum = 0;
  for(i = 0; i < rdim; i++) {
    sum += x[i] * y[i];
  }
  return sum;
}

Coord Vec_dot_pdim(point x, point y) {
  int i;
  Coord sum = 0;
  for(i = 0; i < pdim; i++) {
    sum += x[i] * y[i];
  }
/* check_overshoot(sum); */
  return sum;
}

Coord Norm2(point x) {
  int i;
  Coord sum = 0;
  for(i = 0; i < rdim; i++) {
    sum += x[i] * x[i];
  }
  return sum;
}

static void Ax_plus_y(Coord a, point x, point y) {
  int i;
  for(i = 0; i < rdim; i++) {
    *y++ += a * *x++;
  }
}

void Ax_plus_y_test(Coord a, point x, point y) {
  int i;
  for(i = 0; i <rdim; i++) {
    check_overshoot(*y + a * *x);
    *y++ += a * *x++;
  }
}

static void Vec_scale(int n, Coord a, Coord *x) {
  register Coord *xx = x, *xend = xx + n;
  while(xx != xend) {
    *xx++ *= a;
  }
}

void Vec_scale_test(int n, Coord a, Coord *x) {
  register Coord *xx = x, *xend = xx + n;
  check_overshoot(a);
  while(xx!=xend) {
    *xx *= a;
    check_overshoot(*xx);
    xx++;
  }
}

int   exact_bits;
float b_err_min, b_err_min_sq;

static short vd;
static basis_s  tt_basis = {0,1,-1,0,0,0},
                *tt_basisp = &tt_basis,
                *infinity_basis;


STORAGE(basis_s)

/*
 * get_s        returns next site each call;
 *              hull construction stops when NULL returned;
 * site_numm    returns number of site when given site;
 * dim          dimension of point set;
 * vdd          if(vdd) then return Delaunay triangulation
 */
simplex *build_convex_hull(SiteIterator &it, const SiteToIndexConverter &site_numm, short dim, short vdd) {
  simplex *s, *root;

  if(!Huge) {
    Huge = DBL_MAX * DBL_MAX;
  }

  cdim     = 0;
  get_site = &it;
  site_num = &site_numm;
  pdim     = dim;
  vd       = vdd;

  exact_bits   = DBL_MANT_DIG /**log(FLT_RADIX)/log(2)*/;
  b_err_min    = DBL_EPSILON*MAXDIM*(1<<MAXDIM)*MAXDIM*3.01;
  b_err_min_sq = b_err_min * b_err_min;

  Assert(get_site!=NULL); Assert(site_num!=NULL);

  rdim = vd ? pdim+1 : pdim;
  if(rdim > MAXDIM) {
    panic("dimension bound MAXDIM exceeded; rdim=%d; pdim=%d\n", rdim, pdim);
    /* fprintf(DFILE, "rdim=%d; pdim=%d\n", rdim, pdim); fflush(DFILE);*/
  }

  point_size     = site_size = sizeof(Coord)*pdim;
  basis_vec_size = sizeof(Coord)*rdim;
  basis_s_size   = sizeof(basis_s) + (2*rdim-1)*sizeof(Coord   );
  simplex_size   = sizeof(simplex) + (  rdim-1)*sizeof(neighbor);
  Tree_size      = sizeof(Tree);
  fg_size        = sizeof(fg);

  root = NULL;
  if(vd) {
    p = infinity;
    NEWLRC(basis_s, infinity_basis);
    infinity_basis->vecs[2*rdim-1] = infinity_basis->vecs[rdim-1] = 1;
    infinity_basis->sqa            = infinity_basis->sqb          = 1;
  } else if(!(p = get_site->next())) {
    return 0;
  }

  NEWL(simplex,root);

  ch_root = root;
  copy_simp(s,root);
  root->peak.vert = p;
  root->peak.simp = s;
  s->peak.simp = root;

  buildhull(root);
  return root;
}

typedef Coord site_struct;

       
static void print_site(site p, FILE *F) {
  print_point(F, pdim,p);
  fprintf(F, "\n");
}

#define VA(x) ((x)->vecs+rdim)
#define VB(x) ((x)->vecs)

/* tables for runtime stats */
int A[100]={0}, B[100] ={0}, C[100] = {0}, D[100] ={0};
int tot =0, totinf=0, bigt=0; 

#define two_to(x) ( ((x)<20) ? 1<<(x) : ldexp(1,(x)) )

/* amount by which to scale up vector, for reduce_inner */
double sc(basis_s *v,simplex *s, int k, int j) {
  double          labound;
  static int      lscale;
  static double   max_scale,
                  ldetbound,
                  Sb;

  if(j<10) {
    labound = logb(v->sqa)/2;
    max_scale = exact_bits - labound - 0.66*(k-2)-1  -DELIFT;
    if(max_scale<1) {
      warning(-10, overshot exact arithmetic);
      max_scale = 1;
    }

    if(j==0) {
      int     i;
      neighbor *sni;
      basis_s *snib;

      ldetbound = DELIFT;

      Sb = 0;
      for(i=k-1,sni=s->neigh+k-1;i>0;i--,sni--) {
        snib = sni->basis;
        Sb += snib->sqb;
        ldetbound += logb(snib->sqb)/2 + 1;
        ldetbound -= snib->lscale;
      }
    }
  }
  if(ldetbound - v->lscale + logb(v->sqb)/2 + 1 < 0) {
    DEBS(-2)
      DEBTR(-2) DEBEXP(-2, ldetbound)
      print_simplex_f(s, DFILE, &printFull);
      print_basis(DFILE,v);
    EDEBS                   
    return 0;                                       
  } else {
    lscale = LOGB(2*Sb/(v->sqb + v->sqa*b_err_min))/2;      
    if(lscale > max_scale) {
      lscale = (int)max_scale;
    } else if(lscale<0) {
      lscale = 0;
    }
    v->lscale += lscale;
    return two_to(lscale);
  }
}

double lower_terms(basis_s* v) {
  point vp = v->vecs;
  int i,j,h,hh=0;
  int facs[6] = {2,3,5,7,11,13};
  double out = 1;

DEBTR(-10) print_basis(DFILE, v); printf("\n");
DEBTR(0)

  for(j = 0; j < 6; j++) {
    do {
      for(i = 0; i < 2 * rdim && facs[j] * floor(vp[i] / facs[j]) == vp[i]; i++);
      if(h = (i==2*rdim)) {
        hh=1;
        out *= facs[j];
        for(i = 0; i < 2 * rdim; i++) {
          vp[i] /= facs[j];
        }
      }
    } while(h);
  }
  /* if(hh) {DEBTR(-10)  print_basis(DFILE, v);} */
  return out;
}

double lower_terms_point(point vp) {
  int i,j,h,hh=0;
  int facs[6] = {2,3,5,7,11,13};
  double out = 1;

  for(j = 0; j < 6; j++) {
    do {
      for(i = 0; i < 2 * rdim && facs[j] * floor(vp[i] / facs[j]) == vp[i]; i++);
      if(h = (i == 2 * rdim)) {
        hh = 1;
        out *= facs[j];
        for(i = 0; i < 2 * rdim; i++) {
          vp[i] /= facs[j];
        }
      }
    } while(h);
  }
  return out;
}

int reduce_inner(basis_s *v, simplex *s, int k) {
  point   va = VA(v);
  point   vb = VB(v);
  int     i, j;
  double  dd;
  double  ldetbound = 0;
  double  Sb        = 0;
  double  scale;
  basis_s *snibv;
  neighbor *sni;
  static int failcount;

/*      lower_terms(v); */
  v->sqa = v->sqb = Norm2(vb);
  if(k<=1) {
    memcpy(vb,va,basis_vec_size);
    return 1;
  }
/*      if(vd) {
          snibv = s->neigh[1].basis;
          scale = floor(sqrt(snibv->sqa/v->sqa));
          if(scale > 1) Vec_scale(rdim,scale,va);
          dd = Vec_dot(VA(snibv),va)/snibv->sqa;
          dd = -floor(0.5+dd);
          Ax_plus_y( dd, VA(snibv), va);
  }
*/              
  for(j = 0; j < 250; j++) {
    memcpy(vb, va, basis_vec_size);
    for(i = k - 1, sni = s->neigh + k - 1; i > 0; i--, sni--) {
      snibv = sni->basis;
      dd = -Vec_dot(VB(snibv), vb) / snibv->sqb;
      Ax_plus_y(dd, VA(snibv), vb);
    }
    v->sqb = Norm2(vb);
    v->sqa = Norm2(va);

    if(2 * v->sqb >= v->sqa) {
      B[j]++;
      return 1;
    }

    Vec_scale_test(rdim, scale = sc(v, s, k, j), va);

    for(i = k - 1, sni = s->neigh + k - 1; i > 0; i--, sni--) {
      snibv = sni->basis;
      dd = -Vec_dot(VB(snibv), va) / snibv->sqb;
      dd = floor(dd + 0.5);
      Ax_plus_y_test(dd, VA(snibv), va);
    }
  }
  if(failcount++<10) {
    DEB(-8, reduce_inner failed on:)
    DEBTR(-8) print_basis(DFILE, v); 
    print_simplex_f(s, DFILE, &printFull);
  }
  return 0;
}

#define trans(z,p,q) {int i; for(i=0;i<pdim;i++) z[i+rdim] = z[i] = p[i] - q[i];}
#define lift(z,s) {if(vd) z[2*rdim-1] =z[rdim-1]= ldexp(Vec_dot_pdim(z,z), -DELIFT);}
                                /*not scaling lift to 2^-DELIFT */



int reduce(basis_s **v, point p, simplex *s, int k) {
  point   z;
  point   tt = s->neigh[0].vert;

  if(!*v) {
    NEWLRC(basis_s, (*v))
  } else {
    (*v)->lscale = 0;
  }
  z = VB(*v);
  if(vd) {
    if(p == infinity) {
      memcpy(*v, infinity_basis, basis_s_size);
    } else {
      trans(z, p, tt); lift(z, s);
    }
  } else {
    trans(z,p,tt);
  }
  return reduce_inner(*v,s,k);
}

void get_basis_sede(simplex *s) {
  int       k=1;
  neighbor *sn = s->neigh+1, *sn0 = s->neigh;

  if(vd && sn0->vert == infinity && cdim >1) {
    SWAP(neighbor, *sn0, *sn );
    NULLIFY(basis_s,sn0->basis);
    sn0->basis = tt_basisp;
    tt_basisp->ref_count++;
  } else {
    if(!sn0->basis) {
      sn0->basis = tt_basisp;
      tt_basisp->ref_count++;
    } else {
      while(k < cdim && sn->basis) {
        k++;
        sn++;
      }
    }
  }
  while(k < cdim) {
    NULLIFY(basis_s, sn->basis);
    reduce(&sn->basis, sn->vert, s, k);
    k++; sn++;
  }
}

static bool out_of_flat(simplex *root, point p) {
  static neighbor p_neigh={0,0,0};

  if(!p_neigh.basis) {
    p_neigh.basis = MALLOC(basis_s,1);
  }

  p_neigh.vert = p;
  cdim++;
  root->neigh[cdim-1].vert = root->peak.vert;
  NULLIFY(basis_s,root->neigh[cdim-1].basis);
  get_basis_sede(root);
  if(vd && root->neigh[0].vert == infinity) {
    return true;
  }
  reduce(&p_neigh.basis,p,root,cdim);
  if(p_neigh.basis->sqa != 0) {
    return true;
  }
  cdim--;
  return false;
}

double cosangle_sq(basis_s* v,basis_s* w)  {
  double dd;
  point  vv=v->vecs, wv=w->vecs;
  dd = Vec_dot(vv,wv);
  return dd*dd/Norm2(vv)/Norm2(wv);
}

bool check_perps(simplex *s) {
  static basis_s *b = NULL;
  point   z,y;
  point   tt;
  double  dd;
  
  for(int i = 1; i < cdim; i++) {
    if(NEARZERO(s->neigh[i].basis->sqb)) {
      return false;
    }
  }
  if(!b) {
    /* bug fix: 10-17-2006 by R. Wenger */
    b = MALLOC(basis_s, 1);
    Assert(b != 0);
  }
  b->lscale = 0;
  z = VB(b);
  tt = s->neigh[0].vert;
  for(int i = 1; i < cdim; i++) {
    y = s->neigh[i].vert;
    if(vd && y == infinity) {
      memcpy(b, infinity_basis, basis_s_size);
    } else {
      trans(z, y, tt); lift(z, s);
    }
    if(s->normal && cosangle_sq(b, s->normal) > b_err_min_sq) {
      DEBS(0)
        DEB(0, bad normal) DEBEXP(0, i) DEBEXP(0, dd)
        print_simplex_f(s, DFILE, &printFull);
      EDEBS
      return false;
    }
    for(int j = i + 1; j < cdim; j++) {
      if(cosangle_sq(b, s->neigh[j].basis) > b_err_min_sq) {
        DEBS(0)
          DEB(0, bad basis)DEBEXP(0, i) DEBEXP(0, j) DEBEXP(0, dd)
          DEBTR(-8) print_basis(DFILE, b);
        print_simplex_f(s, DFILE, &printFull);
        EDEBS
        return false;
      }
    }
  }
  return true;
}

static void get_normal_sede(simplex *s) {
  neighbor *rn;

  get_basis_sede(s);
  if(rdim==3 && cdim==3) {
    point   c,
            a = VB(s->neigh[1].basis),
            b = VB(s->neigh[2].basis);
    NEWLRC(basis_s,s->normal);
    c = VB(s->normal);
    c[0] = a[1]*b[2] - a[2]*b[1];
    c[1] = a[2]*b[0] - a[0]*b[2];
    c[2] = a[0]*b[1] - a[1]*b[0];
    s->normal->sqb = Norm2(c);
    rn = ch_root->neigh + cdim - 1;
    for(int i = cdim + 1; i; i--, rn--) {
      int j;
      for(j = 0; j < cdim && rn->vert != s->neigh[j].vert; j++);
      if(j < cdim) {
        continue;
      }
      if(rn->vert == infinity) {
        if(c[2] > -b_err_min) {
          continue;
        }
      } else if(!sees(rn->vert, s)) {
        continue;
      }
      c[0] = -c[0]; c[1] = -c[1]; c[2] = -c[2];
      break;
    }
    DEBS(-1) if(!check_perps(s)) exit(1); EDEBS
    return;
  }       
                
  rn = ch_root->neigh + cdim - 1;
  for(int i = cdim + 1; i; i--, rn--) {
    int j;
    for(j = 0; j < cdim && rn->vert != s->neigh[j].vert; j++);
    if(j < cdim) {
      continue;
    }
    reduce(&s->normal, rn->vert, s, cdim);
    if(s->normal->sqb != 0) {
      break;
    }
  }

  DEBS(-1) if(!check_perps(s)) {
    DEBTR(-1) exit(1);
  }
  EDEBS
}

void get_normal(simplex *s) {
  get_normal_sede(s);
}

bool sees(site p, simplex *s) {
  static basis_s *b = NULL;
  point   tt,zz;
  double  dd,dds;

  if(!b) {
    /* bug fix: 10-17-2006 by R. Wenger */
    b = MALLOC(basis_s, 1);
    Assert(b != 0);
  }
  b->lscale = 0;
  zz = VB(b);
  if(cdim == 0) {
    return false;
  }
  if(!s->normal) {
    get_normal_sede(s);
    for(int i = 0; i < cdim; i++) {
      NULLIFY(basis_s, s->neigh[i].basis);
    }
  }
  tt = s->neigh[0].vert;
  if(vd) {
    if(p == infinity) {
      memcpy(b, infinity_basis, basis_s_size);
    } else {
      trans(zz, p, tt);
      lift(zz, s);
    }
  } else {
    trans(zz,p,tt);
  }
  int i;
  for(i = 0; i < 3; i++) {
    dd = Vec_dot(zz, s->normal->vecs);
    if(dd == 0.0) {
      DEBS(-7) DEB(-6, degeneracy:);
      DEBEXP(-6, site_num->getIndex(p));
      print_site(p, DFILE);
      print_simplex_f(s, DFILE, &printFull);
      EDEBS
      return false;
    }
    dds = dd * dd / s->normal->sqb / Norm2(zz);
    if(dds > b_err_min_sq) {
      return dd < 0;
    }
    get_basis_sede(s);
    reduce_inner(b, s, cdim);
  }
  DEBS(-7)
    if(i==3) {
      DEB(-6, looped too much in sees);
      DEBEXP(-6,dd)
      DEBEXP(-6,dds)
      DEBEXP(-6,site_num->getIndex(p));
      print_simplex_f(s, DFILE, &printFull);
      exit(1);
    }
  EDEBS
  return false;
}

double radsq(simplex *s) {
  neighbor *sn = s->neigh;

  /* square of ratio of circumcircle radius to max edge length for Delaunay tetrahedra */
  for(int i = 0; i < cdim; i++, sn++) {
    if(sn->vert == infinity) {
      return Huge;
    }
  }

  if(!s->normal) {
    get_normal_sede(s);
  }

  /* compute circumradius */
  point n = s->normal->vecs;

  if(NEARZERO(n[rdim - 1])) {
    return Huge;
  }
  return Vec_dot_pdim(n, n) / 4 / n[rdim - 1] / n[rdim - 1];
}

void *show_marks(simplex *s, void *dum) {
  printf("%d",s->mark);
  return NULL;
}

#define swap_points(a,b) { point t; t=a; a=b; b=t;}


AlphaTest alphaTest;

/* returns true if not an alpha-facet */
bool AlphaTest::test(simplex *s, int i, void *alphap) {
  simplex *si;
  double  rs,rsi,rsfi;
  neighbor *scn, *sin;
  int k, nsees, ssees;
  static double alpha;

  if(alphap) {
    alpha=*(double*)alphap;
    if(!s) {
      return true;
    }
  }
  if(i == -1) {
    return false;
  }

  si  = s->neigh[i].simp;
  scn = s->neigh+cdim-1;
  sin = s->neigh+i;
  nsees = 0;

  for(k = 0; k < cdim; k++) {
    if(s->neigh[k].vert == infinity && k != i) {
      return true;
    }
  }
  rs = radsq(s);
  rsi = radsq(si);

  if(rs < alpha &&  rsi < alpha) {
    return true;
  }

  swap_points(scn->vert,sin->vert);
  NULLIFY(basis_s, s->neigh[i].basis);
  cdim--;
  get_basis_sede(s);
  reduce(&s->normal,infinity,s,cdim);
  rsfi = radsq(s);

  for(k = 0; k < cdim; k++) {
    if(si->neigh[k].simp == s) {
      break;
    }
  }

  ssees = sees(scn->vert,s);
  if(!ssees) {
    nsees = sees(si->neigh[k].vert, s);
  }
  swap_points(scn->vert,sin->vert);
  cdim++;
  NULLIFY(basis_s, s->normal);
  NULLIFY(basis_s, s->neigh[i].basis);

  if(ssees) return alpha<rs;
  if(nsees) return alpha<rsi;
  Assert(rsfi<=rs+FLT_EPSILON && rsfi<=rsi+FLT_EPSILON);
  return alpha<=rsfi;
}

class ConvertFacetv : public SimplexVisitor {
public:
  void *visit(simplex *s, void *dummy);
};

void *ConvertFacetv::visit(simplex *s, void *dum) {
  for(int i = 0; i < cdim; i++) {
    if(s->neigh[i].vert == infinity) {
      return s;
    }
  }
  return NULL;
}

void *visit_outside_ashape(simplex *root, SimplexVisitor &visit) {
  ConvertFacetv conv;
  return visit_triang_gen((simplex*)visit_hull(root, conv), visit, alphaTest);
}

/* set root to s, for purposes of getting normals etc. */
static void set_ch_root(simplex *s) {
  ch_root = s;
}

static void print_neighbor_snum(FILE *F, neighbor *n){
  Assert(site_num!=NULL);
  if(n->vert) {
    fprintf(F, "%d ", site_num->getIndex(n->vert));
  } else {
    fprintf(F, "NULL vert ");
  }
  fflush(stdout);
}

static void print_basis(FILE *F, basis_s *b) {
  if(!b) {
    fprintf(F, "NULL basis ");
    fflush(stdout);
    return;
  }
  if(b->lscale<0) {
    fprintf(F, "\nbasis computed");
    return;
  }
  fprintf(F, "\n%p  %d \n b=",(void*)b,b->lscale);
  print_point(F, rdim, b->vecs);
  fprintf(F, "\n a= ");
  print_point_int(F, rdim, b->vecs+rdim); fprintf(F, "   ");
  fflush(F);
}

void print_simplex_num(FILE *F, simplex *s) {
  fprintf(F, "simplex ");
  if(!s) fprintf(F, "NULL ");
  else fprintf(F, "%p  ", (void*)s);
}

void PrintFull::print(FILE *F, neighbor *n) {
  if(!n) {
    fprintf(F, "null neighbor\n");
    return;
  }

  print_simplex_num(F, n->simp);
  print_neighbor_snum(F, n);fprintf(F, ":  ");fflush(F);
  if(n->vert) {
/* if(n->basis && n->basis->lscale <0) fprintf(F, "trans ");*/
/* else */
    print_point(F, pdim,n->vert);
    fflush(F);
  }
  print_basis(F, n->basis);
  fflush(F);
  fprintf(F, "\n");
}

static void *print_facet(FILE *F, simplex *s, NeighborPrinter &pf) {
  int i;
  neighbor *sn = s->neigh;

/* fprintf(F, "%d ", s->mark);*/
  for(i = 0; i < cdim; i++, sn++) {
    pf.print(F, sn);
  }
  fprintf(F, "\n");
  fflush(F);
  return NULL;
}

void *print_simplex_f(simplex *s, FILE *F, NeighborPrinter *pf) {
  static NeighborPrinter *pnf = NULL;
  if(pf) {
    pnf = pf;
    if(!s) {
      return NULL;
    }
  }

  print_simplex_num(F, s);
  fprintf(F, "\n");
  if(!s) {
    return NULL;
  }
  fprintf(F, "normal =");
  print_basis(F, s->normal);
  fprintf(F, "\n");
  fprintf(F, "peak =");
  pnf->print(F, &(s->peak));
  fprintf(F, "facet =\n");
  fflush(F);
  return print_facet(F, s, *pnf);
}

class SimplexPrinter : public SimplexVisitor {
private:
  FILE *m_f;
public:
  SimplexPrinter(FILE *f) : m_f(f) {
  }
  void *visit(simplex *s, void *dummy) {
    return print_simplex_f(s, m_f, 0);
  }
};

static void print_triang(simplex *root, FILE *F, NeighborPrinter &p) {
  print_simplex_f(0,0,&p);
  SimplexPrinter sp(F);
  visit_triang(root, sp);
}

void *p_peak_test(simplex *s) {
  return (s->peak.vert==p) ? (void*)s : (void*)NULL;
}

void *CheckSimplex::visit(simplex *s, void *dummy) {
  int i,j,k,l;
  neighbor *sn, *snn, *sn2;
  simplex *sns;
  site vn;

  for(i = -1, sn = s->neigh - 1; i < cdim; i++, sn++) {
    sns = sn->simp;
    if(!sns) {
      fprintf(DFILE, "check_triang; bad simplex\n");
      print_simplex_f(s, DFILE, &printFull);
      fprintf(DFILE, "site_num(p)=%d\n", site_num->getIndex(p));
      return s;
    }
    if(!s->peak.vert && sns->peak.vert && i != -1) {
      fprintf(DFILE, "huh?\n");
      print_simplex_f(s, DFILE, &printFull);
      print_simplex_f(sns, DFILE, &printFull);
      exit(1);
    }
    for(j = -1, snn = sns->neigh - 1; j < cdim && snn->simp != s; j++, snn++);
    if(j == cdim) {
      fprintf(DFILE, "adjacency failure:\n");
      DEBEXP(-1, site_num->getIndex(p))
        print_simplex_f(sns, DFILE, &printFull);
      print_simplex_f(s, DFILE, &printFull);
      exit(1);
    }
    for(k = -1, snn = sns->neigh - 1; k < cdim; k++, snn++) {
      vn = snn->vert;
      if(k != j) {
        for(l = -1, sn2 = s->neigh - 1; l < cdim && sn2->vert != vn; l++, sn2++);
        if(l == cdim) {
          fprintf(DFILE, "cdim=%d\n", cdim);
          fprintf(DFILE, "error: neighboring simplices with incompatible vertices:\n");
          print_simplex_f(sns, DFILE, &printFull);
          print_simplex_f(s, DFILE, &printFull);
          exit(1);
        }
      }
    }
  }
  return NULL;
}

class PNeight : public SimplexTester {
public:
  bool test(simplex *s, int i, void *dummy) {
    return s->neigh[i].vert != p;
  }
};

void check_triang(simplex *root) {
  visit_triang(root, checkSimplex);
}

void check_new_triangs(simplex *s) {
  PNeight test;
  visit_triang_gen(s, checkSimplex, test);
}

