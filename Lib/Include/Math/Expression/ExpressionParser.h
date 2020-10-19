/* DO NOT EDIT THIS FILE - it is machine generated */
#pragma once

#include <LRparser.h>

#line 3 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
#include <Scanner.h>
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/ExpressionNode.h>


namespace Expr {


class ExpressionParser : public LRparser {
private:
  static const ParserTables *ExpressionTables;
public:
  ExpressionParser(ParserTree &tree, Scanner *scanner = nullptr) : m_tree(tree), LRparser(*ExpressionTables,scanner) {
  }
  void verror(const SourcePosition &pos, _In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr) override;
  static const ParserTables &getTables() {
    return *ExpressionTables;
  }
private:
  ParserTree &m_tree;
  ExpressionNode *m_leftSide, **m_stacktop, *m_userstack[256];
  ExpressionNode *getStackTop(int fromtop)        { return m_stacktop[-fromtop];           }
  void userStackInit()                   override { m_stacktop = m_userstack;              }
  void userStackShiftSymbol(UINT symbol) override { m_stacktop++;                          } // push 1 element (garbage) on userstack
  void userStackPopSymbols( UINT count ) override { m_stacktop      -= count;              } // pop count symbols from userstack
  void userStackShiftLeftSide()          override { *(++m_stacktop) = m_leftSide;          } // push($$) on userstack
  void defaultReduce(       UINT prod  ) override { m_leftSide      = *m_stacktop;         } // $$ = $1
  int  reduceAction(        UINT prod  ) override;
  ExpressionNode   *newNode(const SourcePosition &pos, ExpressionInputSymbol symbol, ...   );
};

}; // namespace Expr

#line 17 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
