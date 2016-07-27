// dummy.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "dummyparser.h"
#include <String.h>
#include <stdio.h>

main() {
  for(;;) {
    char line[256];
    printf("Enter expr:");
    gets(line);
    if(strlen(line) == 0) break;
    LexStringStream stream(line);
    DummyLex lex;
    lex.newStream(&stream);
    DummyParser p(&lex);
//	p.PAsetcascadecount(0);
	p.setDebug(true);
    p.parse();
    printf("\n");
  }
  return 0;
}
