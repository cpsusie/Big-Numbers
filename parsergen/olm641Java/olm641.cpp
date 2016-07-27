// olm641.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "olm641lex.h"
#include "olm641parser.h"

static char *input = "afafdx";
//static char *input = "a f a f a f d x\n\n\n";
//static char *input = "a f a f a f a f a f a f a f a f d  x ";

static void test(char *input) {
  StringStream str(input);
  Olm641Lex    lex;
  lex.newStream(&str);
  Olm641Parser p(&lex);
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
