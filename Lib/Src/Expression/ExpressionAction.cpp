/* DO NOT EDIT THIS FILE - it is machine generated */

#line 57 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
#include "pch.h"
#include <Math/Real.h>
#include "Math/Expression/ExpressionParser.h"

#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"

namespace Expr {

#pragma warning(disable : 4060)

int ExpressionParser::reduceAction(unsigned int prod) {
  switch(prod) {
  case 0: /* start -> function */
#line 64 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_tree.setRoot(newNode(getPos(1), STMTLIST, getStackTop(0), NULL));   }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 2: /* function -> assignStmtList final_expr */
#line 68 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), SEMI   , getStackTop(1), getStackTop(0), NULL );         }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 6: /* assignStmtList -> assignStmtList assignStmt */
#line 76 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), SEMI   , getStackTop(1), getStackTop(0), NULL );         }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 7: /* assignStmt -> assign SEMI */
#line 79 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = getStackTop(1); }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 8: /* assign -> name ASSIGN expr */
#line 82 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(2), ASSIGN , getStackTop(2), getStackTop(0), NULL );         }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 9: /* expr -> expr PLUS expr */
#line 85 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(2), PLUS   , getStackTop(2), getStackTop(0), NULL);          }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 10: /* expr -> expr MINUS expr */
#line 86 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(2), MINUS  , getStackTop(2), getStackTop(0), NULL);          }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 11: /* expr -> expr PROD expr */
#line 87 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(2), PROD   , getStackTop(2), getStackTop(0), NULL);          }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 12: /* expr -> expr QUOT expr */
#line 88 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(2), QUOT   , getStackTop(2), getStackTop(0), NULL);          }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 13: /* expr -> expr MOD expr */
#line 89 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(2), MOD    , getStackTop(2), getStackTop(0), NULL);          }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 14: /* expr -> expr POW expr */
#line 90 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(2), POW    , getStackTop(2), getStackTop(0), NULL);          }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 18: /* unary -> function1 LPAR expr RPAR */
#line 96 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getStackTop(3)->getPos(),getStackTop(3)->getSymbol(), getStackTop(1), NULL);    }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 19: /* unary -> function2 LPAR expr COMMA expr RPAR */
#line 97 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getStackTop(5)->getPos(),getStackTop(5)->getSymbol(), getStackTop(3), getStackTop(1), NULL);}
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 20: /* unary -> LPAR expr RPAR */
#line 98 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = getStackTop(1);                                                  }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 21: /* unary -> MINUS expr */
#line 99 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), UNARYMINUS, getStackTop(0), NULL);           }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 22: /* unary -> PLUS expr */
#line 100 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = getStackTop(0);                                                  }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 27: /* unary -> LB boolExpr RB */
#line 105 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), IIF
        , getStackTop(1), newNode(getPos(1), NUMBER  , strtor("1",NULL))
        , newNode(getPos(1), NUMBER  , strtor("0",NULL))
        , NULL);                                      }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 28: /* conditionalExpr -> IIF LPAR boolExpr COMMA expr COMMA expr RPAR */
#line 113 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), IIF, getStackTop(5), getStackTop(3), getStackTop(1), NULL);          }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 29: /* sumExpr -> INDEXEDSUM LPAR assign TO expr RPAR expr */
#line 117 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), INDEXEDSUM, getStackTop(4), getStackTop(2), getStackTop(0), NULL);   }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 30: /* productExpr -> INDEXEDPRODUCT LPAR assign TO expr RPAR expr */
#line 121 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), INDEXEDPRODUCT, getStackTop(4), getStackTop(2), getStackTop(0),NULL);}
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 31: /* polyExpr -> POLY LB exprList RB LPAR expr RPAR */
#line 124 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), POLY, getStackTop(4), getStackTop(1), NULL);             }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 32: /* exprList -> exprList COMMA expr */
#line 127 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(2), COMMA, getStackTop(2), getStackTop(0), NULL);            }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 34: /* function1 -> ABS */
#line 131 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), ABS     , NULL);                 }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 35: /* function1 -> ACOS */
#line 132 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), ACOS    , NULL);                 }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 36: /* function1 -> ACOSH */
#line 133 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), ACOSH   , NULL);                 }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 37: /* function1 -> ACOT */
#line 134 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), ACOT    , NULL);                 }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 38: /* function1 -> ACSC */
#line 135 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), ACSC    , NULL);                 }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 39: /* function1 -> ASEC */
#line 136 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), ASEC    , NULL);                 }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 40: /* function1 -> ASIN */
#line 137 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), ASIN    , NULL);                 }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 41: /* function1 -> ASINH */
#line 138 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), ASINH   , NULL);                 }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 42: /* function1 -> ATAN */
#line 139 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), ATAN    , NULL);                 }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 43: /* function1 -> ATANH */
#line 140 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), ATANH   , NULL);                 }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 44: /* function1 -> CEIL */
#line 141 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), CEIL    , NULL);                 }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 45: /* function1 -> COS */
#line 142 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), COS     , NULL);                 }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 46: /* function1 -> COSH */
#line 143 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), COSH    , NULL);                 }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 47: /* function1 -> COT */
#line 144 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), COT     , NULL);                 }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 48: /* function1 -> CSC */
#line 145 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), CSC     , NULL);                 }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 49: /* function1 -> ERF */
#line 146 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), ERF     , NULL);                 }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 50: /* function1 -> EXP */
#line 147 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), EXP     , NULL);                 }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 51: /* function1 -> EXP10 */
#line 148 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), EXP10   , NULL);                 }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 52: /* function1 -> EXP2 */
#line 149 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), EXP2    , NULL);                 }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 53: /* function1 -> FAC */
#line 150 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), FAC     , NULL);                 }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 54: /* function1 -> FLOOR */
#line 151 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), FLOOR   , NULL);                 }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 55: /* function1 -> GAMMA */
#line 152 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), GAMMA   , NULL);                 }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 56: /* function1 -> GAUSS */
#line 153 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), GAUSS   , NULL);                 }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 57: /* function1 -> INVERF */
#line 154 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), INVERF  , NULL);                 }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 58: /* function1 -> LN */
#line 155 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), LN      , NULL);                 }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 59: /* function1 -> LOG10 */
#line 156 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), LOG10   , NULL);                 }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 60: /* function1 -> LOG2 */
#line 157 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), LOG2    , NULL);                 }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 61: /* function1 -> NORM */
#line 158 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), NORM    , NULL);                 }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 62: /* function1 -> PROBIT */
#line 159 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), PROBIT  , NULL);                 }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 63: /* function1 -> SEC */
#line 160 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), SEC     , NULL);                 }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 64: /* function1 -> SIGN */
#line 161 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), SIGN    , NULL);                 }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 65: /* function1 -> SIN */
#line 162 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), SIN     , NULL);                 }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 66: /* function1 -> SINH */
#line 163 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), SINH    , NULL);                 }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 67: /* function1 -> SQR */
#line 164 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), SQR     , NULL);                 }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 68: /* function1 -> SQRT */
#line 165 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), SQRT    , NULL);                 }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 69: /* function1 -> TAN */
#line 166 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), TAN     , NULL);                 }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 70: /* function1 -> TANH */
#line 167 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), TANH    , NULL);                 }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 71: /* boolExpr -> expr EQ expr */
#line 170 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(2), EQ      , getStackTop(2), getStackTop(0), NULL);         }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 72: /* boolExpr -> expr NE expr */
#line 171 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(2), NE      , getStackTop(2), getStackTop(0), NULL);         }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 73: /* boolExpr -> expr LE expr */
#line 172 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(2), LE      , getStackTop(2), getStackTop(0), NULL);         }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 74: /* boolExpr -> expr LT expr */
#line 173 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(2), LT      , getStackTop(2), getStackTop(0), NULL);         }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 75: /* boolExpr -> expr GE expr */
#line 174 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(2), GE      , getStackTop(2), getStackTop(0), NULL);         }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 76: /* boolExpr -> expr GT expr */
#line 175 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(2), GT      , getStackTop(2), getStackTop(0), NULL);         }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 77: /* boolExpr -> boolExpr AND boolExpr */
#line 176 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(2), AND     , getStackTop(2), getStackTop(0), NULL);         }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 78: /* boolExpr -> boolExpr OR boolExpr */
#line 177 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(2), OR      , getStackTop(2), getStackTop(0), NULL);         }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 79: /* boolExpr -> NOT boolExpr */
#line 178 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(2), NOT     , getStackTop(0), NULL);         }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 80: /* boolExpr -> LPAR boolExpr RPAR */
#line 179 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = getStackTop(1);                                                  }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 81: /* function2 -> MAX */
#line 182 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), MAX      , NULL);                }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 82: /* function2 -> MIN */
#line 183 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), MIN      , NULL);                }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 83: /* function2 -> NORMRAND */
#line 184 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), NORMRAND , NULL);                }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 84: /* function2 -> RAND */
#line 185 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), RAND     , NULL);                }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 85: /* function2 -> ROOT */
#line 186 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), ROOT     , NULL);                }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 86: /* function2 -> BINOMIAL */
#line 187 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), BINOMIAL , NULL);                }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 87: /* function2 -> CHI2DENS */
#line 188 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), CHI2DENS , NULL);                }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 88: /* function2 -> CHI2DIST */
#line 189 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), CHI2DIST , NULL);                }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 89: /* function2 -> LINCGAMMA */
#line 190 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), LINCGAMMA, NULL);                }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 90: /* function2 -> ATAN2 */
#line 191 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), ATAN2    , NULL);                }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 91: /* function2 -> HYPOT */
#line 192 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), HYPOT    , NULL);                }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 92: /* name -> NAME */
#line 195 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), NAME    , getText());            }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 93: /* number -> NUMBER */
#line 198 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
    { m_leftSide = newNode( getPos(1), NUMBER  , _tcstor(getText(),NULL)); }
#line 34 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  }
  return 0;
}

#line 203 "C:\\Mytools2015\\Lib\\Src\\Expression\\Expression.y"
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
#line 39 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"

}; // namespace Expr
