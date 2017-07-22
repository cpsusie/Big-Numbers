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
//  lex.setDebug(true);
  bool lastWasDirective = false;
  InputToken sym;
  while (sym = (InputToken)lex.getNextLexeme()) {
    switch (sym) {
    case DIRECTIVE :
      lastWasDirective = true;
      break;
    case OLDSTRING :
      if(!lastWasDirective) {
        String         tmp = lex.getText();
        SourcePosition pos = lex.getStartPos(); pos.incrColumn();
        _tprintf(_T("%s%s: %s\n"), name.cstr(), pos.toString().cstr(), tmp.trim().cstr());
      }
      lastWasDirective = false;
      break;
    case OLDCHAR   :
      if(m_showOldStrings) {
        if(m_showOldChars) {
          if(!lastWasDirective) {
            String tmp = lex.getText();
            _tprintf(_T("%s%s: %s\n"), name.cstr(), lex.getPos().toString().cstr(), tmp.trim().cstr());
          }
        }
      }
      lastWasDirective = false;
      break;
    case NEWSTRING :
    case NEWCHAR   :
      lastWasDirective = false;
      break;
    case OTHER     :
      lastWasDirective = false;
      break;
    default:
      lastWasDirective = false;
      _ftprintf(stderr,_T("Unexpected symbol:%d\n"), sym);
    }
  }
}

static void usage() {
  _ftprintf(stderr, _T("Usage:FindStrings [-rsvc] file\n"
                       "     :-r :Recursive search in subdirectories\n"
                       "     :-s :Show old ascii string literals\n"
                       "     :-c :Show old ascii char literals\n"
                       "     :-v :Verbose. Print filenames to stderr, while scanning\n")
           );
  exit(-1);
}

int _tmain(int argc, const TCHAR **argv) {
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

