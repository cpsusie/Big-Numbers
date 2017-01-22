#include "stdafx.h"

static void usage() {
  _ftprintf(stderr,_T("Usage:newline count\n"));
  exit(-1);
}

int main(int argc, char **argv) {
  int count = 1;

  argv++;
  if(*argv != NULL) {
    if(sscanf(*argv,"%u",&count) != 1) {
      usage();
    }
  }

  for(int i = 0; i < count; i++) {
    _tprintf(_T("\n"));
  }

  return 0;
}
