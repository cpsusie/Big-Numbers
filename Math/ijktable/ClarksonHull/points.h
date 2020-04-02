#pragma once

/* points.h       */

typedef double Coord;
typedef Coord *point;
extern int     pdim;   /* point dimension */

typedef point site;

#define MAXDIM 20

void  print_point(FILE *F, int dim, point p);
void  print_point_int(FILE *F, int dim, point p);
Coord maxdist(int dim, point p1, point p2);

#define NEARZERO(d)     ((d) < FLT_EPSILON && (d) > -FLT_EPSILON)
