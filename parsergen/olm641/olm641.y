%{

  /* this part goes to the h file */

#include "olm641symbol.h"

class Olm641Parser : public LRparser {
public:
  static const ParserTables *Olm641Tables;
public:
  Olm641Parser(Scanner *lex=NULL) : LRparser(*Olm641Tables,lex) {}
private:
  int m_dollardollar,*m_stacktop,m_userstack[256];
  int reduceAction(unsigned int prod);
  void userStackInit()                           { m_stacktop = m_userstack;         }
  void userStackShiftSymbol(unsigned int symbol) { m_stacktop++;                     } // push 1 element (garbage) on userstack
  void userStackPopSymbols(unsigned int count)   { m_stacktop -= count;              } // pop count symbols from userstack
  void userStackShiftDollarDollar()              { *(++m_stacktop) = m_dollardollar; } // push($$) on userstack
  void defaultReduce(unsigned int p)             { m_dollardollar  = *m_stacktop;    } // $$ = $1
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

S		: a b A E  { debug(_T("reduce by a b A E pos of A is (%d,%d)\n"),getPos(3).getLineNumber(),getPos(3).getColumn()); }
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

E		:         { _tprintf(_T("reduce by E -> eps yypos(0):(%d,%d)\n"), getPos(0).getLineNumber(),getPos(0).getColumn()); }
		| z
		;

%%

  /* This part goes to the last part of the parser cpp file */

