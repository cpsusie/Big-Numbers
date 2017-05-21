#include "stdafx.h"
#include <FileTreeWalker.h>

class PrintFileName : public FileNameHandler {
  int m_level;
public:
  PrintFileName() {
    m_level = 0;
  }

  void handleFileName(const TCHAR *name, DirListEntry &info);
  void handleStartDir(const TCHAR *name);
  void handleEndDir(  const TCHAR *name);
};

void PrintFileName::handleFileName(const TCHAR *name, DirListEntry &info) {
  _tprintf(_T("%s\n"), name);
}

void PrintFileName::handleStartDir(const TCHAR *name) {
  _tprintf(_T("Begin dir:%s\n"), name);
}

void PrintFileName::handleEndDir(  const TCHAR *name) {
  _tprintf(_T("End dir:%s\n"), name);
}

static void usage() {
  _ftprintf(stderr,_T("TestFileTreeWalker [-r] files\n"));
  exit(-1);
}

int _tmain(int argc, TCHAR **argv) {
  bool   recurse = false;
  TCHAR *cp;
  for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
    for(cp++; *cp; cp++) {
      switch(*cp) {
      case 'r': recurse = true; continue;
      default : usage();
      }
      break;
    }
  }
  FileTreeWalker::traverseArgv(argv, PrintFileName(), recurse);

  return 0;
}
