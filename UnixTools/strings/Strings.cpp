#include "stdafx.h"
#include "ByteStringExtractor.h"
#include "CStringExtractor.h"

static void usage() {
  fprintf(stderr,"Usage:strings [-dhrcv] [-lminimumLen] files\n"
                 "      -d:Interpret danish characters as part of valid strings\n"
                 "      -h:Show name of the files that contain the strings\n"
                 "      -r:Recurse subdirectories\n"
                 "      -c:Extract C-like strings enclosed by \"..\"\n"
                 "      -v:Verbose\n"
                 );
  exit(-1);
}

int main(int argc, char **argv) {
  char *cp;
  bool danish    = false;
  bool showNames = false;
  bool recurse   = false;
  bool verbose   = false;
  bool Cstrings  = false;

  UINT minLength = 4;

  for(argv++; *argv && *(cp = *argv) == '-'; argv++ ) {
    for(cp++;*cp;cp++) {
      switch(*cp) {
      case 'l':
        if(sscanf(cp+1,"%u", &minLength ) != 1) {
          usage();
        }
        if(minLength < 1) {
          _ftprintf(stderr, _T("Minimum length must be > 0 (=%lu)\n"), minLength);
          exit(-1);
        }
        break;
      case 'd':
        danish    = true;
        continue;
      case 'h':
        showNames = true;
        continue;
      case 'r':
        recurse   = true;
        continue;
      case 'c':
        Cstrings  = true;
        continue;
      case 'v':
        verbose   = true;
        continue;
      default:
        usage();
      }
      break;
    }
  }

  if(!*argv) {
    usage();
  }

  setvbuf(stdout, NULL, _IOFBF, 0x10000);
  if(Cstrings) {
    FileTreeWalker::traverseArgv(argv, CStringExtractor(minLength, showNames, verbose), recurse);
  } else {
    FileTreeWalker::traverseArgv(argv, ByteStringExtractor(minLength, showNames, verbose, danish), recurse);
  }
  return 0;
}
