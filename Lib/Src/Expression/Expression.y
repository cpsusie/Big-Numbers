%{

#include <Scanner.h>
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/ExpressionNode.h>

$PUSHNAMESPACE$

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

$POPNAMESPACE$

%}

%term NUMBER BOOLCONST NAME
%left IIF STMTLIST ASSIGN SEMI COMMA TO INDEXEDSUM INDEXEDPRODUCT
%left OR
%left AND
%term NOT
%term EQ GE GT LE LT NE
%left SUM ADDENT PLUS MINUS UNARYMINUS     /*  + - (lowest precedence)      */
%left PRODUCT PROD QUOT MOD                /*  * / %                        */
%left POW                                  /*  ^                            */
%term ACOS ACOSH ACOT ACSC ASEC ASIN ASINH ATAN ATAN2 ATANH COS COSH COT CSC SEC SIN SINH TAN TANH
%term ABS CEIL ERF EXP EXP10 EXP2 FAC FLOOR HYPOT GAMMA GAUSS INVERF LN LOG10 LOG2
%term NORM PROBIT CHI2DIST CHI2DENS LINCGAMMA
%term ROOT SIGN SQR SQRT
%term BINOMIAL MAX MIN NORMRAND POLY RAND 
%left LPAR RPAR LB RB            /*  ( ) [ ] (highest precedence) */

/* Adding new builtin functions, that require call-instruction, must be
   added to array in getBuiltInFunctionSelector() in ExpressionNode.cpp too
*/
%{

#include "pch.h"
#include <Math/Real.h>
#include <Math/Expression/ExpressionParser.h>

%}
%%

start               : function                              { m_tree.setRoot(newNode(getPos(1), STMTLIST, $1, nullptr));   }
                    ;

function            : final_expr                        
                    | assignStmtList final_expr             { $$ = newNode( getPos(1), SEMI   , $1, $2, nullptr );         }
                    ;

final_expr          : expr
                    | boolExpr
                    ;

assignStmtList      : assignStmt
                    | assignStmtList assignStmt             { $$ = newNode( getPos(1), SEMI   , $1, $2, nullptr );         }
                    ;

assignStmt          : assign SEMI                           { $$ = $1; }
                    ;

assign              : name ASSIGN expr                      { $$ = newNode( getPos(2), ASSIGN , $1, $3, nullptr );         }
                    ;

expr                : expr PLUS  expr                       { $$ = newNode( getPos(2), PLUS   , $1, $3, nullptr);          }
                    | expr MINUS expr                       { $$ = newNode( getPos(2), MINUS  , $1, $3, nullptr);          }
                    | expr PROD  expr                       { $$ = newNode( getPos(2), PROD   , $1, $3, nullptr);          }
                    | expr QUOT  expr                       { $$ = newNode( getPos(2), QUOT   , $1, $3, nullptr);          }
                    | expr MOD   expr                       { $$ = newNode( getPos(2), MOD    , $1, $3, nullptr);          }
                    | expr POW   expr                       { $$ = newNode( getPos(2), POW    , $1, $3, nullptr);          }
                    | unary
          ;

unary               : number
                    | name
                    | function1 LPAR expr RPAR              { $$ = newNode( $1->getPos(),$1->getSymbol(), $3, nullptr);    }
                    | function2 LPAR expr COMMA expr RPAR   { $$ = newNode( $1->getPos(),$1->getSymbol(), $3, $5, nullptr);}
                    | LPAR expr RPAR                        { $$ = $2;                                                     }
                    | MINUS expr                            { $$ = newNode( getPos(1), UNARYMINUS, $2, nullptr);           }
                    | PLUS  expr                            { $$ = $2;                                                     }
                    | conditionalExpr
                    | sumExpr
                    | productExpr
                    | polyExpr
                    | LB boolExpr RB                        { $$ = newNode( getPos(1), IIF
                                                                , $2
                                                                , newNode(getPos(1), NUMBER  , strtor("1",nullptr))
                                                                , newNode(getPos(1), NUMBER  , strtor("0",nullptr))
                                                                , nullptr);                                                }
                    ;

conditionalExpr     : IIF LPAR boolExpr COMMA expr COMMA expr RPAR
                                                            { $$ = newNode( getPos(1), IIF, $3, $5, $7, nullptr);          }
                    ;

sumExpr             : INDEXEDSUM     LPAR assign TO expr RPAR expr   %prec INDEXEDSUM
                                                            { $$ = newNode( getPos(1), INDEXEDSUM, $3, $5, $7, nullptr);   }
                    ;

productExpr         : INDEXEDPRODUCT LPAR assign TO expr RPAR expr   %prec INDEXEDPRODUCT
                                                            { $$ = newNode( getPos(1), INDEXEDPRODUCT, $3, $5, $7,nullptr);}
                    ;

polyExpr            : POLY LB exprList RB LPAR expr RPAR    { $$ = newNode( getPos(1), POLY, $3, $6, nullptr);             }
                    ;

exprList            : exprList COMMA expr                   { $$ = newNode( getPos(2), COMMA, $1, $3, nullptr);            }
                    | expr
                    ;
                    
function1           : ABS                                   { $$ = newNode( getPos(1), ABS     , nullptr);                 }
                    | ACOS                                  { $$ = newNode( getPos(1), ACOS    , nullptr);                 }
                    | ACOSH                                 { $$ = newNode( getPos(1), ACOSH   , nullptr);                 }
                    | ACOT                                  { $$ = newNode( getPos(1), ACOT    , nullptr);                 }
                    | ACSC                                  { $$ = newNode( getPos(1), ACSC    , nullptr);                 }
                    | ASEC                                  { $$ = newNode( getPos(1), ASEC    , nullptr);                 }
                    | ASIN                                  { $$ = newNode( getPos(1), ASIN    , nullptr);                 }
                    | ASINH                                 { $$ = newNode( getPos(1), ASINH   , nullptr);                 }
                    | ATAN                                  { $$ = newNode( getPos(1), ATAN    , nullptr);                 }
                    | ATANH                                 { $$ = newNode( getPos(1), ATANH   , nullptr);                 }
                    | CEIL                                  { $$ = newNode( getPos(1), CEIL    , nullptr);                 }
                    | COS                                   { $$ = newNode( getPos(1), COS     , nullptr);                 }
                    | COSH                                  { $$ = newNode( getPos(1), COSH    , nullptr);                 }
                    | COT                                   { $$ = newNode( getPos(1), COT     , nullptr);                 }
                    | CSC                                   { $$ = newNode( getPos(1), CSC     , nullptr);                 }
                    | ERF                                   { $$ = newNode( getPos(1), ERF     , nullptr);                 }
                    | EXP                                   { $$ = newNode( getPos(1), EXP     , nullptr);                 }
                    | EXP10                                 { $$ = newNode( getPos(1), EXP10   , nullptr);                 }
                    | EXP2                                  { $$ = newNode( getPos(1), EXP2    , nullptr);                 }
                    | FAC                                   { $$ = newNode( getPos(1), FAC     , nullptr);                 }
                    | FLOOR                                 { $$ = newNode( getPos(1), FLOOR   , nullptr);                 }
                    | GAMMA                                 { $$ = newNode( getPos(1), GAMMA   , nullptr);                 }
                    | GAUSS                                 { $$ = newNode( getPos(1), GAUSS   , nullptr);                 }
                    | INVERF                                { $$ = newNode( getPos(1), INVERF  , nullptr);                 }
                    | LN                                    { $$ = newNode( getPos(1), LN      , nullptr);                 }
                    | LOG10                                 { $$ = newNode( getPos(1), LOG10   , nullptr);                 }
                    | LOG2                                  { $$ = newNode( getPos(1), LOG2    , nullptr);                 }
                    | NORM                                  { $$ = newNode( getPos(1), NORM    , nullptr);                 }
                    | PROBIT                                { $$ = newNode( getPos(1), PROBIT  , nullptr);                 }
                    | SEC                                   { $$ = newNode( getPos(1), SEC     , nullptr);                 }
                    | SIGN                                  { $$ = newNode( getPos(1), SIGN    , nullptr);                 }
                    | SIN                                   { $$ = newNode( getPos(1), SIN     , nullptr);                 }
                    | SINH                                  { $$ = newNode( getPos(1), SINH    , nullptr);                 }
                    | SQR                                   { $$ = newNode( getPos(1), SQR     , nullptr);                 }
                    | SQRT                                  { $$ = newNode( getPos(1), SQRT    , nullptr);                 }
                    | TAN                                   { $$ = newNode( getPos(1), TAN     , nullptr);                 }
                    | TANH                                  { $$ = newNode( getPos(1), TANH    , nullptr);                 }
                    ;

boolExpr            : expr EQ expr                          { $$ = newNode( getPos(2), EQ      , $1, $3, nullptr);         }
                    | expr NE expr                          { $$ = newNode( getPos(2), NE      , $1, $3, nullptr);         }
                    | expr LE expr                          { $$ = newNode( getPos(2), LE      , $1, $3, nullptr);         }
                    | expr LT expr                          { $$ = newNode( getPos(2), LT      , $1, $3, nullptr);         }
                    | expr GE expr                          { $$ = newNode( getPos(2), GE      , $1, $3, nullptr);         }
                    | expr GT expr                          { $$ = newNode( getPos(2), GT      , $1, $3, nullptr);         }
                    | boolExpr AND boolExpr                 { $$ = newNode( getPos(2), AND     , $1, $3, nullptr);         }
                    | boolExpr OR  boolExpr                 { $$ = newNode( getPos(2), OR      , $1, $3, nullptr);         }
                    | NOT boolExpr                          { $$ = newNode( getPos(2), NOT     , $2    , nullptr);         }
                    | LPAR boolExpr RPAR                    { $$ = $2;                                                     }
                    ;

function2           : MAX                                   { $$ = newNode( getPos(1), MAX      , nullptr);                }
                    | MIN                                   { $$ = newNode( getPos(1), MIN      , nullptr);                }
                    | NORMRAND                              { $$ = newNode( getPos(1), NORMRAND , nullptr);                }
                    | RAND                                  { $$ = newNode( getPos(1), RAND     , nullptr);                }
                    | ROOT                                  { $$ = newNode( getPos(1), ROOT     , nullptr);                }
                    | BINOMIAL                              { $$ = newNode( getPos(1), BINOMIAL , nullptr);                }
                    | CHI2DENS                              { $$ = newNode( getPos(1), CHI2DENS , nullptr);                }
                    | CHI2DIST                              { $$ = newNode( getPos(1), CHI2DIST , nullptr);                }
                    | LINCGAMMA                             { $$ = newNode( getPos(1), LINCGAMMA, nullptr);                }
                    | ATAN2                                 { $$ = newNode( getPos(1), ATAN2    , nullptr);                }
                    | HYPOT                                 { $$ = newNode( getPos(1), HYPOT    , nullptr);                }
                    ;

name                : NAME                                  { $$ = newNode( getPos(1), NAME     , getText());              }
                    ;

number              : NUMBER                                { $$ = newNode( getPos(1), NUMBER  , _tcstor(getText(),nullptr)); }
                    ;

%%

ExpressionNode *ExpressionParser::newNode(const SourcePosition &pos, ExpressionInputSymbol symbol, ...) {
  va_list argptr;
  va_start(argptr, symbol);
  ExpressionNode *p = m_tree.vFetchNode(pos, symbol, argptr);
  va_end(argptr);
  return p;
}

void ExpressionParser::verror(const SourcePosition &pos, _In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr) {
  m_tree.vAddError(&pos, format, argptr);
}
