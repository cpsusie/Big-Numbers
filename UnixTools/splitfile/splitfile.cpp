#include "stdafx.h"
#include <FileNameSplitter.h>

static void usage() {
  fprintf(stderr
        ,"Usage:splitfile [-v] [-lmaxLineCount] inputfile\n"
         "                -v:Verbose:Print filenames while splitting\n"
         "                -lmaxLineCount: Specify max number of lines in each outputfile. Default is 40000\n"
         );
  exit(-1);
}

int main(int argc, char **argv) {
  char *cp;
  UINT maxLines = 40000;
  bool verbose = false;

  for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
    for(cp++; *cp; cp++) {
      switch(*cp) {
      case 'v':
        verbose = true;
        continue;
      case 'l':
        if(sscanf(cp+1,"%u",&maxLines) != 1) {
          usage();
        }
        if(maxLines == 0) {
          fprintf(stderr,"Max linecount must be > 0\n");
          exit(-1);
        }
        break;

      default :
        usage();
      }
      break;
    }
  }

  if(!*argv) {
    usage();
  }
  char *inputFileName = *(argv++);

  FileNameSplitter info(inputFileName);
  String fileName = info.getFileName();

  try {
    FILE *input = FOPEN(inputFileName,"r");
    TCHAR line[10000];
    FILE *output = nullptr;
    UINT fileCount = 0;
    UINT lineCount = 0;
    while(FGETS(line, ARRAYSIZE(line), input) != nullptr) {
      lineCount++;
      if(output == nullptr || lineCount >= maxLines) {
        if(output != nullptr) {
          fclose(output);
          output = nullptr;
        }
        String outputFileName = info.setFileName(format(_T("%s%03d"),fileName.cstr(), ++fileCount)).getFullPath();
        if(verbose) {
          _ftprintf(stderr, _T("Writing to %s   \r"), outputFileName.cstr());
        }
        output = FOPEN(outputFileName,_T("w"));
        lineCount = 0;
      }
      _ftprintf(output,_T("%s\n"), line);
    }
    fclose(output);
  } catch(Exception e) {
    _ftprintf(stderr,_T("splitfile:%s\n"),e.what());
    return -1;
  }
  return 0;
}
