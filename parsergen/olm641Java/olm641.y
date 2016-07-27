%{

  /* this part goes to the h file */

#pragma once

#include <LRparser.h>
#include "olm641symbol.h"

const extern PAtables *Olm641PAtables;

class Olm641Parser : public LRparser {
public:
  Olm641Parser(Scanner *lex=NULL) : LRparser(*Olm641PAtables,lex) {}
private:
  int m_dollardollar,*m_stacktop,m_userstack[256];
  int reduceAction(unsigned int prod);
  void initUserStack()                  { m_stacktop      = m_userstack;    }
  void shiftSymbol(unsigned int symbol) { m_stacktop++;                     } // push 1 element (garbage) on userstack
  void popSymbols(unsigned int count)   { m_stacktop     -= count;          } // pop count symbols from userstack
  void shiftDollarDollar()              { *(++m_stacktop) = m_dollardollar; } // push($$) on userstack
  void defaultReduce(unsigned int p)    { m_dollardollar  = *m_stacktop;    } // $$ = $1
};

%}

%term a b d f z x

%{

  /* This part goes to the first part of the parser cpp file */

#include "stdafx.h"
#include "olm641parser.h"
%}
%%

start	: S
		;

S		: a b A E  { debug("reduce by a b A E pos of A is (%d,%d)\n",yypos(3).m_lineno,yypos(3).m_col); }
		| a b B x
		| a D
		| A z
		| B x
		;

A		: d
		;

B		: d
		;

D		: f A
		| f B x
		| f a D
		;

E		:         { printf("reduce by E -> eps yypos(0):(%d,%d)\n",yypos(0).m_lineno,yypos(0).m_col); }
		| z
		;

%%

  /* This part goes to the last part of the parser cpp file */

