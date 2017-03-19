#include "stdafx.h"
#include <Console.h>

static String getStmt() {
  String result;
  for(;;) {
    _tprintf(_T(">"));
    TCHAR line[1024];
    GETS(line);
    String trimmet = trim(line);
    int l = (int)trimmet.length();
    if(l > 0 && trimmet[l-1] == ';') {
      result += substr(trimmet,0,l-1);
      break;
    }
    result += line;
    result += '\n';
  }
  return result;
}

static void testParser() {
  _tprintf(_T("\n"));
  String stmt = getStmt();
  ParserTree t;
  t.parse(_T("stdin"),1,stmt.cstr());
  if(t.ok()) {
    t.dumpTree();
  } else {
    t.listErrors();
  }
}

static void scan(const String &stmt) {
  _tprintf(_T("now scanning <%s>\n"),stmt.cstr());
//  for(const char *cp = stmt; *cp; cp++)
//    _tprintf(_T("%02x "),*cp);
//  _tprintf(_T("\n"));

  LexStringStream stream(stmt);
  SqlLex lex;
  lex.newStream(&stream,1);
  int s;
  while(s = lex.getNextLexeme()) {
    TCHAR *str = lex.getText();
    int l = 25 - (int)_tcslen(str);
    if(l < 0) l = 0;
    _tprintf(_T("<%s>%*.*s\tsym:%s\n"),str,l,l,EMPTYSTRING,SqlTables->getSymbolName(s));
  }
}

static void testScanner() {
  _tprintf(_T("\n"));
  scan(getStmt());
}

int main(int argc, char **argv) {
  for(;;) {
    _tprintf(_T("(S)can,(P)arse,(Q)uit:"));
    switch(Console::getKey()) {
    case 's':
    case 'S':
      testScanner();
      break;
    case 'p':
    case 'P':
      testParser();
      break;
    case 'q':
    case 'Q':
      exit(0);
    }
  }
  return 0;
}
