/* io.cpp : input-output */
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

#include "stdafx.h"
#include "io.h"

char tmpfilenam[L_tmpnam];

FILE *efopen(char *file, const char *mode) {
  FILE *fp;
  if(fp = fopen(file, mode)) {
    return fp;
  }
  fprintf(DFILE, "couldn't open file %s mode %s\n",file,mode);
  exit(1);
  return NULL;
}

#define popen _popen

void pclose(FILE *f) {
  _pclose(f);
}

FILE *epopen(char *com, const char *mode) {
  FILE *fp;
  if(fp = popen(com, mode)) return fp;
  fprintf(stderr, "couldn't open stream %s mode %s\n",com,mode);
  exit(1);
  return 0;
}
