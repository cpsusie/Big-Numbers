#include "stdafx.h"
#include <MyUtil.h>
#include <io.h>

static void usage() {
  fprintf(stderr,"Usage:nl [file]\n");
  exit(-1);
}

int main(int argc, char **argv ) {
  TCHAR line[1024];

  try {
    argv++;
    FILE *f = *argv ? FOPEN(*argv, _T("r")) : stdin;
    if(isatty(f)) {
      usage();
    }
    int n = 1;
    while(FGETS(line,ARRAYSIZE(line),f)) {
      _tprintf(_T("%6d %s\n"),n++,line);
    }
    if(f != stdin) {
      fclose(f);
    }
  } catch(Exception e) {
    _ftprintf(stderr, _T("%s\n"), e.what());
    exit(-1);
  }
  return 0;
}

