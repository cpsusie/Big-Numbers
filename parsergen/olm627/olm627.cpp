// olm627.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "olm627lex.h"
#include "olm627parser.h"

static TCHAR *input = _T("afafdx");
//static char *input = "a f a f a f d x\n\n\n";
//static char *input = "a f a f a f a f a f a f a f a f d  x ";

static void test(TCHAR *input) {
  LexStringStream str(input);
  Olm627Lex    lex;
  lex.newStream(&str);
  Olm627Parser p(&lex);
  p.setDebug(true);
  p.parse();
}

int main(int argc, char* argv[])
{ 
  for(;;) {
    TCHAR line[100];
    _tprintf(_T("Indtast inputstreng:"));
    _fgetts(line, ARRAYSIZE(line), stdin);
    test(line);
  }

  return 0;
}
