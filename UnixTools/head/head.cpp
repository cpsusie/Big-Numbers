#include "stdafx.h"
#include <MyUtil.h>
#include <io.h>

static void usage() {
  _ftprintf(stderr, _T("Usage:head [linecount] [file]\n"));
  exit(-1);
}

int main(int argc, char **argv) {
  ULONG lineCount = 10;

  try {
    argv++;
    if((*argv) && (sscanf(*argv, "%lu", &lineCount) == 1))  {
      argv++;
    }
    FILE *f = *argv ? FOPEN(*argv, _T("r")) : stdin;
    if(isatty(f)) {
      usage();
    }
    TCHAR line[4096];
    for(UINT i = 0; i < lineCount; i++) {
      if(!FGETS(line,ARRAYSIZE(line), f)) {
        break;
      }
      _tprintf(_T("%s\n"),line);
    }
    if(f != stdin) {
      fclose(f);
    }
  } catch(Exception e) {
    _ftprintf(stderr, _T("head:%s\n"), e.what());
    return -1;
  }
  return 0;
}
