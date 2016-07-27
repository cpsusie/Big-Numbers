// cpp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "cppsymbol.h"
#include "cppparser.h"

static LexFileStream   input;
static LexStringStream strstream(" ++kukkku->++--!=?:+=*=/=\n54.5e-33f");
//static stringstream strstream("54.5e-33f");

/*
bool nextfile(scanner &lex) {
  return true;
  static int count = 0;
  input.close();
  if(count++ > 1)
    return true;
  lex.newStream(&strstream);
  return false;
}
*/


static void scan(Scanner &lex) {
  int token;
  do {
    token = lex.getNextLexeme();
    printf("%2d %-20s : %s \n",lex.getLineNumber(),CppTables->getSymbolName(token),lex.getText());
  } while(token != 0);
}

static void scanFile(char *fname) {
  CppLex lex;
  input.open(fname);
  lex.newStream(&input);
  scan(lex);
}

void scanString() {
  for(;;) {
    printf("Enter String:");
    char line[1024];
    gets(line);
    LexStringStream strstream(line);
    CppLex lex;
    lex.newStream(&strstream);
    scan(lex);
  }
}

static void usage() {
  fprintf(stderr,"Usage:testlex [file]\n");
  exit(-1);
}

int main(int argc, char** argv) {
  argv++;
  if(*argv)
    scanFile(*argv);
  else
    scanString();
  getchar();
  return 0;
}


