/* DO NOT EDIT THIS FILE - it is machine generated */
#pragma once

#include <LRparser.h>

#line 5 "C:\\Mytools2015\\ParserGen\\Olm627\\Olm627.y"
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

#line 17 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
