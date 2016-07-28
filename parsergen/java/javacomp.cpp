#include "stdafx.h"
#include <MyUtil.h>
#include <FileTreeWalker.h>
#include "Java5Lex.h"
#include "Java5symbol.h"
#include "Java5parser.h"

static void parse(const String &fileName) {
  LexFileStream input(fileName);
  Java5Lex lex;
  lex.newStream(&input);
  Java5Parser parser(fileName.cstr(),&lex);
  parser.parse();
}

static void scan(const String &name) {
  LexFileStream input(name);

  if(!input.ok()) {
    _tperror(name.cstr());
    return;
  }
  Java5Lex lex;
  lex.newStream(&input);

  while(lex.getNextLexeme());
}

class TestJavaParser : public FileNameHandler {
private:
  bool m_testLex;
public:
  TestJavaParser(bool testLex) {
    m_testLex = testLex;
  }
  void test(const String &fileName);
  void handleFileName(const TCHAR *name, DirListEntry &info);
};

void TestJavaParser::test(const String &fileName) {
  if(m_testLex) {
    _tprintf(_T("scanning <%s>\n"), fileName.cstr());
     scan(fileName);
  } else {
    _tprintf(_T("parsing <%s>\n"), fileName.cstr());
    parse(fileName);
  }
}

void TestJavaParser::handleFileName(const TCHAR *name, DirListEntry &info) {
  test(name);
}

static void usage() {
  fprintf(stderr,"Usage:javacomp [-l] [-rdir] [-m] files...\n");
  exit(-1);
}

int main(int argc, char **argv) {
  char *cp;
  bool testLex  = false;
  bool recurse  = false;
  String dir;

  
  for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
    for(cp++;*cp;cp++) {
      switch(*cp) {
      case 'l': testLex = true; continue;
      case 'r': recurse = true; dir = cp+1; break;
      case 'm': Java5Lex::findBestHashMapSize();
                exit(0);
      default : usage();
      }
      break;
    }
  }
        
  TestJavaParser test(testLex);
  if(recurse) {
    TCHAR *argv[] = { _T("*.java"), NULL };
    FileTreeWalker::walkFileTree(dir,test,argv);
  } else {
    argvExpand(argc, argv);
    for(;*argv; argv++) {
      test.test(*argv);
    }
  }

  return 0;
}
