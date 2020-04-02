#pragma once

#include "stormacs.h"
#include "points.h"
#include "hull.h"

typedef struct fg_node fg;
typedef struct tree_node Tree;
struct tree_node {
  Tree            *left;
  Tree            *right;
  site             key;
  int              size;               /* maintained to be the number of nodes rooted here              */
  fg              *fgs;
  Tree            *next;               /* freelist                                                      */
};

STORAGE_GLOBALS(Tree)

typedef struct fg_node {
  Tree            *facets;
  double           dist, vol;          /* of Voronoi face dual to this                                  */
  fg              *next;               /* freelist                                                      */
  short            mark;
  int              ref_count;
} fg_node;

STORAGE_GLOBALS(fg)

fg  *build_fg(     simplex *root);
void print_fg(                    fg *faces_gr, FILE *F);
void print_fg_alt(                fg *faces_gr, FILE *F, int fd);
void print_hist_fg(simplex *root, fg *faces_gr, FILE *F);
