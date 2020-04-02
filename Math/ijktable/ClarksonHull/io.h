#pragma once

#include "points.h"

FILE *efopen(char *name, const char *mode);
FILE *epopen(char *com , const char *mode);
void  pclose(FILE *f);

extern char tmpfilenam[L_tmpnam];

void panic(const char *fmt, ...);
