#include "stdafx.h"
#include <Math.h>
#include <Math/MathException.h>
#include <Math/Expression/Expression.h>
#include "TestExpression.h"

static Real eval(Expression &expr, Real x) {
  try {
    expr.setValue("x",x);
  } catch(Exception) {
  }
  return expr.evaluate();
}

static bool evalBool(Expression &expr, Real x) {
  try {
    expr.setValue("x",x);
  } catch(Exception) {
  }
  return expr.evaluateBool();
}

#define EPS 1e-8
static Real numericDfDx(Expression &expr, Real x) {
  Real y = eval(expr,x);
  Real x1 = (x==0)?EPS:(x*(1+EPS));
  Real y1 = eval(expr,x1);
  return (y-y1)/(x-x1);
}

static void testexpr3d(Expression &expr) {
  TCHAR line[100];
//  strcpy(line,"d1 = sqrt(sqr(x-2) + sqr(x-1) + sqr(x+1));\n"
//              "d2 = sqrt(sqr(x-3) + sqr(x-3));\n"
//              "exp(-d1) * sin(d2)");
  _tcscpy(line, _T("2+2"));
  expr.compile(line,false);

  if(!expr.isOk()) {
    expr.listErrors();
  } else {
    _tprintf(_T("%s\n"), expr.treeToString().cstr());
    for(;;) {
      try {
        _tprintf(_T("Indtast x,y,z:"));
        double x,y,z;
        _tscanf(_T("%le %le %le"),&x,&y,&z);
        expr.setValue("x",x);
        expr.setValue("y",y);
        expr.setValue("z",z);
        _tprintf(_T("%s = %lg\n"),line,expr.evaluate());
      } catch(Exception e) {
        _tprintf(_T("error:%s\n"),e.what());
      }
    }
  }
}

int main(int argc, char **argv) {
  testExpression();
//  return 0;
  for(;;) {
    _tprintf(_T("Enter expression:"));
    TCHAR line[100];
    GETS(line);
//    strcpy(line,"sum(x*v(i);i=0;i<dim(v);i=i+1)");
    if(_tcsicmp(line,_T("quit")) == 0) {
      break;
    }
    try {
      Expression expr1,expr2;
//      expr.setDebug(true);
      expr1.compile(line,true );
      expr2.compile(line,false);
      if(!expr1.isOk()) {
        expr1.listErrors();
      } else {
        _tprintf(_T("Parsertree:\n%s"), expr1.treeToString().cstr());
        switch(expr1.getReturnType()) {
        case EXPR_RETURN_REAL:
          { Real result1 = eval(expr1,0.55);
            Real result2 = eval(expr2,0.55);
            _tprintf(_T("result(compiled   ):%s\n"), toString(result1).cstr());
            _tprintf(_T("result(interpreter):%s\n"), toString(result2).cstr());
            break;
          }
        case EXPR_RETURN_BOOL:
          { bool result1 = evalBool(expr1,0.55);
            bool result2 = evalBool(expr2,0.55);
            _tprintf(_T("result(compiled   ):%s\n"), result1? _T("true"): _T("false"));
            _tprintf(_T("result(interpreter):%s\n"), result2? _T("true"): _T("false"));
            break;
          }
        }
      }
    } catch(Exception e) {
      _tprintf(_T("Exception:%s\n"), e.what());
    }
  }
  return 0;
}
