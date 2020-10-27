%{

  /* this part goes to the h file */

#include "Olm637symbol.h"

#define YYSTYPE int
class Olm637Parser : public LRparser {
private:
  static const ParserTables *Olm637Tables;
public:
  Olm637Parser(Scanner *lex=nullptr) : LRparser(*Olm637Tables,lex) {
  }
private:
  YYSTYPE m_leftSide,*m_stacktop,m_userstack[256];
  YYSTYPE getStackTop(int fromtop)                { return m_stacktop[-fromtop];           }
  void userStackInit()                   override { m_stacktop = m_userstack;              }
  void userStackShiftSymbol(UINT symbol) override { m_stacktop++;                          } // push 1 element (garbage) on userstack
  void userStackPopSymbols( UINT count ) override { m_stacktop      -= count;              } // pop count symbols from userstack
  void userStackShiftLeftSide()          override { *(++m_stacktop) = m_leftSide;          } // push($$) on userstack
  void defaultReduce(       UINT prod  ) override { m_leftSide      = *m_stacktop;         } // $$ = $1
  int  reduceAction(        UINT prod  ) override;
};

%}

%term a b c d f 

%{

  /* This part goes to the first part of the parser cpp file */

#include "stdafx.h"
#include "Olm637parser.h"
%}
%%

start	: S
		;

S		: a A d { $$ = $2; }
		| a B c
		| b A c
		| b B d
		;

A		: f
		;

B		: f
		;

%%

  /* This part goes to the last part of the parser cpp file */

