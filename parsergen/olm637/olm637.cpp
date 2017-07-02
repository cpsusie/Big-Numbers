#include "stdafx.h"
#include "Olm637lex.h"
#include "Olm637parser.h"

static String input = "afafdx";
//static char *input = "a f a f a f d x\n\n\n";
//static char *input = "a f a f a f a f a f a f a f a f d  x ";

static void test(const String input) {
  LexStringStream str(input);
  Olm637Lex    lex;
  lex.newStream(&str);
  Olm637Parser p(&lex);
  p.setDebug(true);
  p.parse();
}

int main(int argc, char* argv[]) {
  for(;;) {
    TCHAR line[100];
    _tprintf(_T("Indtast inputstreng:"));
    GETS(line);
    test(line);
  }

  return 0;
}
