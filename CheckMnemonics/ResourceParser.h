/* DO NOT EDIT THIS FILE - it is machine generated */
#pragma once

#include <LRparser.h>

#line 3 "C:\\Mytools2015\\CheckMnemonics\\Resource.y"
#include "ResourceSymbol.h"
#include "ResourceLex.h"
#include "ParserTree.h"

class ResourceParser : public LRparser {
private:
  static const AbstractParserTables *ResourceTables;
  static int    _ttoi(const TCHAR *s);
  static String stripQuotes(const String &s);
public:
  ResourceParser(ParserTree &tree, ResourceLex *lex = nullptr) : m_tree(tree), LRparser(*ResourceTables,lex) {
  }
  void  verror(const SourcePosition &pos, const TCHAR *format,va_list argptr) override;
  static const AbstractParserTables &getTables() {
    return *ResourceTables;
  }
private:
  ParserTree &m_tree;
  SyntaxNode *m_leftSide,**m_stacktop,*m_userstack[256];
  SyntaxNode *getStackTop(int fromtop)            { return m_stacktop[-fromtop];           }
  void userStackInit()                   override { m_stacktop = m_userstack;              }
  void userStackShiftSymbol(UINT symbol) override { m_stacktop++;                          } // push 1 element (garbage) on userstack
  void userStackPopSymbols( UINT count ) override { m_stacktop      -= count;              } // pop count symbols from userstack
  void userStackShiftLeftSide()          override { *(++m_stacktop) = m_leftSide;          } // push($$) on userstack
  void defaultReduce(       UINT prod  ) override { m_leftSide      = *m_stacktop;         } // $$ = $1
  int  reduceAction(        UINT prod  ) override;
  SyntaxNode       *newNode(const SourcePosition &pos, int symbol, ... );
};
#line 17 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
