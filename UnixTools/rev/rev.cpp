#include "stdafx.h"
#include <MyUtil.h>
#include <Array.h>
#include <io.h>

static void usage() {
  fprintf( stderr, "Usage:rev [-f] [file]\n"
                   "          -f:Print lines of input in reverse order. Default is reverse each line.\n"
         );
  exit(-1);
}

int main(int argc, char **argv) {
  char *cp;
  TCHAR line[4096];
  bool reverseFile = false;

  for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
    for(cp++; *cp; cp++) {
      switch(*cp) {
      case 'f': reverseFile = true; continue;
      default : usage();
      }
    }
  }

  try {
    FILE *in = *argv ? FOPEN(*argv,"r") : stdin;
    if(isatty(in)) {
      usage();
    }
    setvbuf(in,nullptr,_IOFBF,0x10000);
    setvbuf(stdout,nullptr,_IOFBF,0x10000);

    if(reverseFile) {
      Array<String> lines;
      while(FGETS(line,ARRAYSIZE(line), in)) {
        lines.add(line);
      }
      for(intptr_t i = lines.size()-1; i >= 0; i--) {
        _tprintf(_T("%s\n"),lines[i].cstr());
      }
    } else {
      while(FGETS(line,ARRAYSIZE(line), in)) {
        _tprintf(_T("%s\n"), _tcsrev(line));
      }
    }
  } catch(Exception e) {
    _ftprintf(stderr,_T("rev:%s\n"),e.what());
    return -1;
  }
  return 0;
}

