// olm637.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "olm637lex.h"
#include "olm637parser.h"

static char *input = "afafdx";
//static char *input = "a f a f a f d x\n\n\n";
//static char *input = "a f a f a f a f a f a f a f a f d  x ";

static void test(char *input) {
  LexStringStream str(input);
  Olm637Lex    lex;
  lex.newStream(&str);
  Olm637Parser p(&lex);
  p.setDebug(true);
  p.parse();
}

int main(int argc, char* argv[])
{ 
  for(;;) {
    char line[100];
    printf("Indtast inputstreng:");
    gets(line);
    test(line);
  }

  return 0;
}
