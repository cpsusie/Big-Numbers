#include "stdafx.h"
#include "dummyparser.h"

int main() {
  for(;;) {
    TCHAR line[256];
    printf("Enter expr:");
    GETS(line);
    if(_tcslen(line) == 0) break;
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
