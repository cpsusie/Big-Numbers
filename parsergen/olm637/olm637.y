%{

  /* this part goes to the h file */

#include "Olm637symbol.h"

#define YYSTYPE int
class Olm637Parser : public LRparser {
public:
  Olm637Parser(Scanner *lex=NULL) : LRparser(*Olm637Tables,lex) {}
private:
  YYSTYPE m_leftSide,*m_stacktop,m_userstack[256];
  YYSTYPE getStackTop(int fromtop)               { return m_stacktop[-fromtop];            }
  void userStackInit()                           { m_stacktop = m_userstack;               }
  void userStackShiftSymbol(unsigned int symbol) { m_stacktop++;                           } // push 1 element (garbage) on userstack
  void userStackPopSymbols( unsigned int count ) { m_stacktop      -= count;               } // pop count symbols from userstack
  void userStackShiftLeftSide()                  { *(++m_stacktop) = m_leftSide;           } // push($$) on userstack
  void defaultReduce(       unsigned int prod)   { m_leftSide      = *m_stacktop;          } // $$ = $1
  int  reduceAction(        unsigned int prod);
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

