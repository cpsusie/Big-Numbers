#include "stdafx.h"
#include <FileTreeWalker.h>
#include "FindStringsLex.h"

class StringSearcher : public FileNameHandler {
private:
  bool m_showOldChars;
  bool m_showOldStrings;
  bool m_verbose;
public:
  StringSearcher(bool showOldStrings, bool showOldchars, bool verbose) 
    : m_showOldStrings(showOldStrings)
    , m_showOldChars(showOldchars)
    , m_verbose(verbose)
  {
  }
  void handleFileName(const TCHAR *name, DirListEntry &entry) {
    scan(name);
  }
  void scan(const String &name);
};

void StringSearcher::scan(const String &name) {
  if (m_verbose) {
    _ftprintf(stderr, _T("Scanning %-50s             \r"), name.cstr());
  }
  LexFileStream input(name);

  if(!input.ok()) {
    _tperror(name.cstr());
    return;
  }
  FindStringsLex lex;
  lex.newStream(&input);

  bool lastWasDirective = false;
  int sym;
  while (sym = lex.getNextLexeme()) {
    switch (sym) {
    case DIRECTIVE :
      lastWasDirective = true;
      break;
    case OLDSTRING :
      if(!lastWasDirective) {
        String tmp = lex.getText();
        _tprintf(_T("%-50s:%s\n"), name.cstr(), tmp.trim().cstr());
      }
      lastWasDirective = false;
      break;
    case OLDCHAR   :
      if(m_showOldStrings) {
        if(m_showOldChars) {
          if(!lastWasDirective) {
            String tmp = lex.getText();
            _tprintf(_T("%-50s:%s\n"), name.cstr(), tmp.trim().cstr());
          }
        }
      }
      lastWasDirective = false;
      break;
    case NEWSTRING :
    case NEWCHAR   :
      lastWasDirective = false;
      break;
    default:
      lastWasDirective = false;
      _ftprintf(stderr,_T("Unexpected symbol:%d\n"), sym);
    }
  }
}

static void usage() {
  _ftprintf(stderr, _T("Usage:findStrings file\n"));
  exit(-1);
}

int _tmain(int argc, TCHAR **argv) {
  bool recurse = false;
  bool showStrings = true;
  bool showChars   = true;
  bool verbose     = false;
  const TCHAR *cp;

  for (argv++; *argv && *(cp = *argv) == _T('-'); argv++) {
    for (cp++; *cp; cp++) {
      switch (*cp) {
      case 'r': recurse     = true ; continue;
      case 's': showStrings = false; continue;
      case 'c': showChars   = false; continue;
      case 'v': verbose     = true ; continue;
      default : usage();
      }
      break;
    }
  }

  try {
    StringSearcher ss(showStrings, showChars, verbose);
    FileTreeWalker::traverseArgv(argv,ss,recurse);
  } catch(Exception e) {
    _ftprintf(stderr, _T("FindStrings:%s\n"), e.what());
    return -1;
  }
  return 0;
}

