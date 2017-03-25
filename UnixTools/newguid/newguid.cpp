#include "stdafx.h"

static void usage() {
  fprintf(stderr,"usage:newguid [count]\n");
  exit(-1);
}

int main(int argc,  char **argv) {

  int n = 1;

  argv++;

  if(*argv && (sscanf(*argv,"%d",&n) == 1) && (n > 0)) {
    ;
  } else {
    n = 1;
  }

  TCHAR tmp[100];
  for(int i = 0; i < n; i++) {
    _tprintf(_T("%s\n"), newGUID(tmp));
  }
  
  return 0;
}
