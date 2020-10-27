// tee.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <MyUtil.h>
#include <Array.h>
#include <signal.h>
#include <fcntl.h>

static void usage() {
  fprintf(stderr,"Usage: tee [-a] file...\n"
                 "         -a:Append input to file(s)\n"
         );
  exit(-1);
}

int main(int argc, char **argv) {
  char *cp;
  bool append           = false;
  bool ignoreInterrupts = false;
  Array<FILE*> output;

  try {
    for(argv++; *argv && *(cp = *argv) == '-'; argv++ ) {
      for(cp++; *cp; cp++) {
        switch(*cp) {
        case 'a':
          append = true;
          continue;
        default :
          usage();
        }
        break;
      }
    }

    if(*argv == nullptr) {
      usage();
    }

    for(;*argv; argv++) {
      output.add(FOPEN(*argv, append?"ab":"wb"));
    }

    setFileMode(stdin ,_O_BINARY);
    setFileMode(stdout,_O_BINARY);
    TCHAR buffer[4096];
    size_t n;
    while((n = fread(buffer,1, ARRAYSIZE(buffer), stdin)) > 0) {
      fwrite(buffer, 1, n, stdout);
      for(size_t i = 0; i < output.size(); i++) {
        fwrite(buffer, 1, n, output[i]);
      }
    }

    for(size_t i = 0; i < output.size(); i++) {
      fclose(output[i]);
    }
    output.clear();
  } catch(Exception e) {
    _ftprintf(stderr,_T("tee:%s\n"), e.what());
    return -1;
  }
  return 0;
}
