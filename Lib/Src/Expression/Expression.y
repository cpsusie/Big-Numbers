%{

#include <Math/Expression/ParserTree.h>
#include <Math/Expression/ExpressionNode.h>

#include "ExpressionLex.h"

$PUSHNAMESPACE$

class ExpressionParser : public LRparser {
private:
  static const ParserTables *ExpressionTables;
public:
  ExpressionParser(ParserTree &tree, ExpressionLex *lex = NULL) : m_tree(tree), LRparser(*ExpressionTables,lex) {}
  void verror(const SourcePosition &pos, _In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr);
  static const ParserTables &getTables() {
    return *ExpressionTables;
  }
private:
  ParserTree &m_tree;
  ExpressionNode *m_leftSide, **m_stacktop, *m_userstack[256];
  ExpressionNode *getStackTop(int fromtop)       { return m_stacktop[-fromtop];            }
  void userStackInit()                           { m_stacktop = m_userstack;               }
  void userStackShiftSymbol(unsigned int symbol) { m_stacktop++;                           } // push 1 element (garbage) on userstack
  void userStackPopSymbols( unsigned int count ) { m_stacktop      -= count;               } // pop count symbols from userstack
  void userStackShiftLeftSide()                  { *(++m_stacktop) = m_leftSide;           } // push($$) on userstack
  void defaultReduce(       unsigned int prod)   { m_leftSide      = *m_stacktop;          } // $$ = $1
  int  reduceAction(        unsigned int prod);
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
%left SUM ADDENT PLUS MINUS      /*  + - (lowest precedence)      */
%left PRODUCT PROD QUOT MOD      /*  * / %                        */
%left POW                        /*  ^                            */
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
#include "ExpressionParser.h"

%}
%%

start               : function                              { m_tree.setRoot(newNode(getPos(1), STMTLIST, $1, NULL));   }
                    ;

function            : final_expr                        
                    | assignStmtList final_expr             { $$ = newNode( getPos(1), SEMI   , $1, $2, NULL );         }
                    ;

final_expr          : expr
                    | boolExpr
                    ;

assignStmtList      : assignStmt
                    | assignStmtList assignStmt             { $$ = newNode( getPos(1), SEMI   , $1, $2, NULL );         }
                    ;

assignStmt          : assign SEMI                           { $$ = $1; }
                    ;

assign              : name ASSIGN expr                      { $$ = newNode( getPos(2), ASSIGN , $1, $3, NULL );         }
                    ;

expr                : expr PLUS  expr                       { $$ = newNode( getPos(2), PLUS   , $1, $3, NULL);          }
                    | expr MINUS expr                       { $$ = newNode( getPos(2), MINUS  , $1, $3, NULL);          }
                    | expr PROD  expr                       { $$ = newNode( getPos(2), PROD   , $1, $3, NULL);          }
                    | expr QUOT  expr                       { $$ = newNode( getPos(2), QUOT   , $1, $3, NULL);          }
                    | expr MOD   expr                       { $$ = newNode( getPos(2), MOD    , $1, $3, NULL);          }
                    | expr POW   expr                       { $$ = newNode( getPos(2), POW    , $1, $3, NULL);          }
                    | unary
          ;

unary               : number
                    | name
                    | function1 LPAR expr RPAR              { $$ = newNode( $1->getPos(),$1->getSymbol(), $3, NULL);    }
                    | function2 LPAR expr COMMA expr RPAR   { $$ = newNode( $1->getPos(),$1->getSymbol(), $3, $5, NULL);}
                    | LPAR expr RPAR                        { $$ = $2;                                                  }
                    | MINUS expr                            { $$ = newNode( getPos(1), MINUS, $2, NULL);                }
                    | PLUS  expr                            { $$ = $2;                                                  }
                    | conditionalExpr
                    | sumExpr
                    | productExpr
                    | polyExpr
                    | LB boolExpr RB                        { $$ = newNode( getPos(1), IIF
                                                                , $2
                                                                , newNode(getPos(1), NUMBER  , strtor("1",NULL))
                                                                , newNode(getPos(1), NUMBER  , strtor("0",NULL))
                                                                , NULL);                                      }
                    ;

conditionalExpr     : IIF LPAR boolExpr COMMA expr COMMA expr RPAR
                                                            { $$ = newNode( getPos(1), IIF, $3, $5, $7, NULL);          }
                    ;

sumExpr             : INDEXEDSUM     LPAR assign TO expr RPAR expr   %prec INDEXEDSUM
                                                            { $$ = newNode( getPos(1), INDEXEDSUM, $3, $5, $7, NULL);   }
                    ;

productExpr         : INDEXEDPRODUCT LPAR assign TO expr RPAR expr   %prec INDEXEDPRODUCT
                                                            { $$ = newNode( getPos(1), INDEXEDPRODUCT, $3, $5, $7,NULL);}
                    ;

polyExpr            : POLY LB exprList RB LPAR expr RPAR    { $$ = newNode( getPos(1), POLY, $3, $6, NULL);             }
                    ;

exprList            : exprList COMMA expr                   { $$ = newNode( getPos(2), COMMA, $1, $3, NULL);            }
                    | expr
                    ;
                    
function1           : ABS                                   { $$ = newNode( getPos(1), ABS     , NULL);                 }
                    | ACOS                                  { $$ = newNode( getPos(1), ACOS    , NULL);                 }
                    | ACOSH                                 { $$ = newNode( getPos(1), ACOSH   , NULL);                 }
                    | ACOT                                  { $$ = newNode( getPos(1), ACOT    , NULL);                 }
                    | ACSC                                  { $$ = newNode( getPos(1), ACSC    , NULL);                 }
                    | ASEC                                  { $$ = newNode( getPos(1), ASEC    , NULL);                 }
                    | ASIN                                  { $$ = newNode( getPos(1), ASIN    , NULL);                 }
                    | ASINH                                 { $$ = newNode( getPos(1), ASINH   , NULL);                 }
                    | ATAN                                  { $$ = newNode( getPos(1), ATAN    , NULL);                 }
                    | ATANH                                 { $$ = newNode( getPos(1), ATANH   , NULL);                 }
                    | CEIL                                  { $$ = newNode( getPos(1), CEIL    , NULL);                 }
                    | COS                                   { $$ = newNode( getPos(1), COS     , NULL);                 }
                    | COSH                                  { $$ = newNode( getPos(1), COSH    , NULL);                 }
                    | COT                                   { $$ = newNode( getPos(1), COT     , NULL);                 }
                    | CSC                                   { $$ = newNode( getPos(1), CSC     , NULL);                 }
                    | ERF                                   { $$ = newNode( getPos(1), ERF     , NULL);                 }
                    | EXP                                   { $$ = newNode( getPos(1), EXP     , NULL);                 }
                    | EXP10                                 { $$ = newNode( getPos(1), EXP10   , NULL);                 }
                    | EXP2                                  { $$ = newNode( getPos(1), EXP2    , NULL);                 }
                    | FAC                                   { $$ = newNode( getPos(1), FAC     , NULL);                 }
                    | FLOOR                                 { $$ = newNode( getPos(1), FLOOR   , NULL);                 }
                    | GAMMA                                 { $$ = newNode( getPos(1), GAMMA   , NULL);                 }
                    | GAUSS                                 { $$ = newNode( getPos(1), GAUSS   , NULL);                 }
                    | INVERF                                { $$ = newNode( getPos(1), INVERF  , NULL);                 }
                    | LN                                    { $$ = newNode( getPos(1), LN      , NULL);                 }
                    | LOG10                                 { $$ = newNode( getPos(1), LOG10   , NULL);                 }
                    | LOG2                                  { $$ = newNode( getPos(1), LOG2    , NULL);                 }
                    | NORM                                  { $$ = newNode( getPos(1), NORM    , NULL);                 }
                    | PROBIT                                { $$ = newNode( getPos(1), PROBIT  , NULL);                 }
                    | SEC                                   { $$ = newNode( getPos(1), SEC     , NULL);                 }
                    | SIGN                                  { $$ = newNode( getPos(1), SIGN    , NULL);                 }
                    | SIN                                   { $$ = newNode( getPos(1), SIN     , NULL);                 }
                    | SINH                                  { $$ = newNode( getPos(1), SINH    , NULL);                 }
                    | SQR                                   { $$ = newNode( getPos(1), SQR     , NULL);                 }
                    | SQRT                                  { $$ = newNode( getPos(1), SQRT    , NULL);                 }
                    | TAN                                   { $$ = newNode( getPos(1), TAN     , NULL);                 }
                    | TANH                                  { $$ = newNode( getPos(1), TANH    , NULL);                 }
                    ;

boolExpr            : expr EQ expr                          { $$ = newNode( getPos(2), EQ      , $1, $3, NULL);         }
                    | expr NE expr                          { $$ = newNode( getPos(2), NE      , $1, $3, NULL);         }
                    | expr LE expr                          { $$ = newNode( getPos(2), LE      , $1, $3, NULL);         }
                    | expr LT expr                          { $$ = newNode( getPos(2), LT      , $1, $3, NULL);         }
                    | expr GE expr                          { $$ = newNode( getPos(2), GE      , $1, $3, NULL);         }
                    | expr GT expr                          { $$ = newNode( getPos(2), GT      , $1, $3, NULL);         }
                    | boolExpr AND boolExpr                 { $$ = newNode( getPos(2), AND     , $1, $3, NULL);         }
                    | boolExpr OR  boolExpr                 { $$ = newNode( getPos(2), OR      , $1, $3, NULL);         }
                    | NOT boolExpr                          { $$ = newNode( getPos(2), NOT     , $2    , NULL);         }
                    | LPAR boolExpr RPAR                    { $$ = $2;                                                  }
                    ;

function2           : MAX                                   { $$ = newNode( getPos(1), MAX      , NULL);                }
                    | MIN                                   { $$ = newNode( getPos(1), MIN      , NULL);                }
                    | NORMRAND                              { $$ = newNode( getPos(1), NORMRAND , NULL);                }
                    | RAND                                  { $$ = newNode( getPos(1), RAND     , NULL);                }
                    | ROOT                                  { $$ = newNode( getPos(1), ROOT     , NULL);                }
                    | BINOMIAL                              { $$ = newNode( getPos(1), BINOMIAL , NULL);                }
                    | CHI2DENS                              { $$ = newNode( getPos(1), CHI2DENS , NULL);                }
                    | CHI2DIST                              { $$ = newNode( getPos(1), CHI2DIST , NULL);                }
                    | LINCGAMMA                             { $$ = newNode( getPos(1), LINCGAMMA, NULL);                }
                    | ATAN2                                 { $$ = newNode( getPos(1), ATAN2    , NULL);                }
                    | HYPOT                                 { $$ = newNode( getPos(1), HYPOT    , NULL);                }
                    ;

name                : NAME                                  { $$ = newNode( getPos(1), NAME    , getText());            }
                    ;

number              : NUMBER                                { $$ = newNode( getPos(1), NUMBER  , _tcstor(getText(),NULL)); }
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
