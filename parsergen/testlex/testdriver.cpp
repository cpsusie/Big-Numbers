#include "stdafx.h"
#include "clex.h"

static LexFileStream   input;
static LexStringStream strstream(" ++kukkku->++--!=?:+=*=/=\n54.5e-33f");
//static StringStream strstream("54.5e-33f");

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
    switch(token) {
#define casepr(name) case name:_tprintf(_T("line %2d %-10s:'%s'\n"),lex.getLineNumber(),_T(#name),lex.getText()); break
    casepr(_EOI_      );
    casepr(STRING     );
    casepr(ICON       );
    casepr(FCON       );
    casepr(TYPE       );
    casepr(STRUCT     );
    casepr(ENUM       );
    casepr(RETURN     );
    casepr(GOTO       );
    casepr(IF         );
    casepr(ELSE       );
    casepr(SWITCH     );
    casepr(CASE       );
    casepr(DEFAULT    );
    casepr(BREAK      );
    casepr(CONTINUE   );
    casepr(WHILE      );
    casepr(DO         );
    casepr(FOR        );
    casepr(LC         );
    casepr(RC         );
    casepr(SEMI       );
    casepr(ELLIPSIS   );
    casepr(COMMA      );
    casepr(EQUAL      );
    casepr(ASSIGNOP   );
    casepr(QUEST      );
    casepr(COLON      );
    casepr(OROR       );
    casepr(ANDAND     );
    casepr(OR         );
    casepr(XOR        );
    casepr(AND        );
    casepr(EQUOP      );
    casepr(RELOP      );
    casepr(SHIFTOP    );
    casepr(PLUS       );
    casepr(MINUS      );
    casepr(STAR       );
    casepr(DIVOP      );
    casepr(SIZEOF     );
    casepr(UNOP       );
    casepr(INCOP      );
    casepr(LB         );
    casepr(RB         );
    casepr(LP         );
    casepr(RP         );
    casepr(STRUCTOP   );
    casepr(TTYPE      );
    casepr(CLASS      );
    casepr(NAME       );
    casepr(DEFINE     );
    casepr(HASHMARK   );
    default:
      _tprintf(_T("unknown token:%d:'%s'\n"), token,lex.getText());
      break;
    }
  } while(token != _EOI_);
}

static void scanfile(char *fname) {
  CLex lex;
  input.open(fname);
  lex.newStream(&input);
  scan(lex);
}

void scanstring() {
  for(;;) {
    printf("Enter String:");
    TCHAR line[1024];
    GETS(line);
    LexStringStream strstream(line);
    CLex lex;
    lex.newStream(&strstream);
    scan(lex);
  }
}

static void usage() {
  fprintf(stderr,"Usage:testlex [file]\n");
  exit(-1);
}

int main(int argc, char **argv) {
  argv++;
  if(*argv) {
    scanfile(*argv);
  } else {
    scanstring();
  }
  return 0;
}
