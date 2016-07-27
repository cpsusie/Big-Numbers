%{

  /* this part goes to the h file */

#include "olm627symbol.h"

class Olm627Parser : public LRparser {
public:
  Olm627Parser(Scanner *lex=NULL) : LRparser(*Olm627Tables,lex) {}
private:
  int m_dollardollar,*m_stacktop,m_userstack[256];
  int reduceAction(unsigned int prod);
  void userStackInit()         { m_stacktop = m_userstack;                           }
  void userStackShiftSymbol(unsigned int symbol) { m_stacktop++;                     } // push 1 element (garbage) on userstack
  void userStackPopSymbols( unsigned int count)  { m_stacktop     -= count;          } // pop count symbols from userstack
  void userStackShiftDollarDollar()              { *(++m_stacktop) = m_dollardollar; } // push($$) on userstack
  void defaultReduce(       unsigned int p)      { m_dollardollar  = *m_stacktop;    } // $$ = $1
};

%}

%term a b c d f 

%{

  /* This part goes to the first part of the parser cpp file */

#include "stdafx.h"
#include "Olm627Parser.h"
%}
%%

  /* this grammar is LALR(1) but not SLR(1) */

start	: S
		;

S		: a A d
		| a f c
		| b A c
		| b f d
		;

A		: f
		;

%%

  /* This part goes to the last part of the parser cpp file */

