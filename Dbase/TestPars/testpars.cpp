#include "stdafx.h"
#include <FileNameSplitter.h>

static String getStmt() {
  String result = EMPTYSTRING;
  for(;;) {
    _tprintf(_T(">"));
    String line;
    readLine(stdin,line);
    String trimmet = line.trim();
    if(trimmet.length() > 0 && trimmet[trimmet.length()-1] == ';') {
      result += left(trimmet,trimmet.length()-1);
      return result;
    }
    result += line;
    result += _T("\n\r");
  }
}

void testParser(const String &fileName, const String &stmt) {
  ParserTree tree;
  SqlParser parser(tree);
  _tprintf(_T("now parsing <%s>\n"),fileName.cstr());
  tree.setFilename(fileName);
  parser.parse(1, stmt);
  _tprintf(_T("return from parser:%s\n"), tree.ok() ?_T("OK"):_T("ERRORS"));

  if(!tree.ok()) {
    tree.listErrors();
  } else if(fileName.length() > 0) {
    String dumpFileName = FileNameSplitter(fileName).setExtension(_T("dmp")).getFullPath();
    _tprintf(_T("Now dumping syntaxtree to <%s>\n"),dumpFileName.cstr());
    FILE *f = fopen(dumpFileName,_T("w"));
    if(f) {
      tree.dumpTree(f);
      fclose(f);
      f = FOPEN(dumpFileName,_T("a"));
      _ftprintf(f,_T("sprintTree:%s\n"),tree.m_root->toString().cstr());
      fclose(f);
    }
  } else {
    tree.dumpTree();
    _tprintf(_T("sprintTree:%s\n"),tree.m_root->toString().cstr());
  }
}

static void testParser(TCHAR **argv) {
  if(*argv == nullptr) {
    for(;;) {
      String stmt = getStmt();
      testParser( EMPTYSTRING,stmt);
    }
  } else {
    for(;*argv;argv++) {
      try {
        FILE *input = FOPEN(*argv,_T("r"));
        String stmt = readTextFile(input);
        fclose(input);
        testParser(*argv,stmt);
      } catch(Exception e) {
        _tprintf(_T("%s\n"),e.what());
      }
    }
  }
}

static void testScanner(const String &stmt) {
  _tprintf(_T("now scanning <%s>\n"),stmt.cstr());

  LexStringStream stream(stmt);
  SqlLex lex;
//  parser.setscanner(&lex);
  lex.newStream(&stream,1);
//  lex.setDebug(true);
  int s;
  while(s = lex.getNextLexeme()) {
    TCHAR *str = lex.getText();
    int l = 25 - (int)_tcslen(str);
    if(l < 0) {
      l = 0;
    }
    _tprintf(_T("<%s>%*.*s\tsym:%s\n"),str,l,l,EMPTYSTRING,SqlParser::getTables().getSymbolName(s));
  }
//  parser.listErrors();
}

static void testScanner(TCHAR **argv) {
  if(*argv == nullptr) {
    for(;;) {
      String stmt = getStmt();
      testScanner(stmt);
    }
  } else {
    for(;*argv;argv++) {
      try {
        FILE *input = FOPEN(*argv,_T("r"));
        String stmt = readTextFile(input);
        fclose(input);
        testScanner(stmt);
      } catch(Exception e) {
        _tprintf(_T("%s\n"),e.what());
      }
    }
  }
}

static void usage() {
  _ftprintf(stderr, _T("testpars [-l] [inputfiles...]\n"));
  exit(-1);
}

int _tmain(int argc, TCHAR **argv) {
  TCHAR *cp;
  bool testlex = false;

  argvExpand(argc,argv);

  for(argv++;*argv && *(cp = *argv) == '-'; argv++) {
    for(cp++;*cp;cp++) {
      switch(*cp) {
      case 'l':
        testlex = true; continue;
        default:usage();
      }
    }
  }

  try {
    if(testlex) {
      testScanner(argv);
    } else {
      testParser(argv);
    }
  } catch(Exception e) {
    _ftprintf(stderr,_T("Exception:%s\n"),e.what());
    return -1;
  }
  return 0;
}

