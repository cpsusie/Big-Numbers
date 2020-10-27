%{

  /* this part goes to the h file */

#include "olm641symbol.h"

class Olm641Parser : public LRparser {
public:
  static const ParserTables *Olm641Tables;
public:
  Olm641Parser(Scanner *lex=nullptr) : LRparser(*Olm641Tables,lex) {
  }
private:
  int m_leftSide,*m_stacktop,m_userstack[256];
  void userStackInit()                   override { m_stacktop = m_userstack;              }
  void userStackShiftSymbol(UINT symbol) override { m_stacktop++;                          } // push 1 element (garbage) on userstack
  void userStackPopSymbols( UINT count ) override { m_stacktop      -= count;              } // pop count symbols from userstack
  void userStackShiftLeftSide()          override { *(++m_stacktop) = m_leftSide;          } // push($$) on userstack
  void defaultReduce(       UINT prod  ) override { m_leftSide      = *m_stacktop;         } // $$ = $1
  int  reduceAction(        UINT prod  ) override;
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

