%{

  /* this part goes to the h file */

#include "Olm627symbol.h"

using namespace LRParsing;

class Olm627Parser : public LRparser {
private:
  static const AbstractParserTables *Olm627Tables;
public:
  Olm627Parser(Scanner *lex=nullptr) : LRparser(*Olm627Tables,lex) {
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

