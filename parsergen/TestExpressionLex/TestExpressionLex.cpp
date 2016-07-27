// TestExpressionLex.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Expression/ExpressionLex.h>
#include <Expression/ExpressionParser.h>

static void scan(char *s) {
  StringStream stream(s);
  ExpressionLex lex(&stream);

  int symbol;
  do {
    symbol = lex.getNextLexeme();
    SourcePosition pos = lex.getPos();
    SourcePosition ppos = lex.getPreviousPos();
    printf("ppos:(%d,%d). startPos:(%d,%d) : symbol:%s\n"
     ,ppos.getLineNumber(),ppos.getColumn()
     ,pos.getLineNumber(),pos.getColumn()
     ,ExpressionTables->getSymbolName(symbol));
    lex.markPrevious();
  } while(symbol != 0);
}
    
int main(int argc, char **argv) {
  scan("sin ( + * x = ;");
  return 0;
}
