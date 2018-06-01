#include "stdafx.h"
#include <Math/Matrix.h>
#include <Math/MathException.h>
#include <Math/Expression/Expression.h>

using namespace Expr;

String getExpression() {
  String result;
  _tprintf(_T("Enter expression (terminate with empty line):\n"));
  for(;;) {
    String line = inputString(_T(">"));
    line.trim();
    if(line.length() == 0) {
      break;
    }
    result += line;
  }
  return result;
}

class ZeroFunction : public VectorFunction {
private:
  Expression &m_e;
  Real       *m_x;
public:
  Vector operator()(const Vector &x);
  ZeroFunction(Expression &e);
  double y(double x);
};

Vector ZeroFunction::operator()(const Vector &x) {
  Vector result(x);
  if(m_x) *m_x = x[0];
  result[0] = m_e.evaluate();
  return result;
}

ZeroFunction::ZeroFunction(Expression &e) : m_e(e) {
  const ExpressionVariable *var = m_e.getVariable(_T("x"));
  if(var == NULL) {
    throwException(_T("Expression doesn't depend on x"));
  }
  m_x = &e.getValueRef(*var);
}

double ZeroFunction::y(double x) {
  if(m_x) *m_x = x;
  return getDouble(m_e.evaluate());
}

static void usage() {
  fprintf(stderr,"findzero:Usage:findzero [-eexpression] [-ffile][-xX0] [-y]\n");
  exit(-1);
}

int _tmain(int argc, TCHAR **argv) {
  String expr;
  TCHAR *cp;
  double x0;
  bool   fixedx0 = false;
  bool   listy   = false;

  try {
    for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
      for(cp++; *cp; cp++) {
        switch(*cp) {
        case 'e':
          if(cp[0] == '\0') {
            usage();
          }
          expr = cp+1;
          break;
        case 'f':
          { if(cp[1] == '\0') {
              usage();
            }
            expr = readTextFile(*argv);
            break;
          }
        case 'x':
          if(_stscanf(cp+1, _T("%le"), &x0) != 1) {
            usage();
          }
          fixedx0 = true;
          break;
        case 'y':
          listy = true;
          continue;
        default:
          usage();
        }
        break;
      }
    }
    if(expr.length() == 0) {
      expr = getExpression();
    }
    Expression e;
    e.compile(expr,true);
    if(!e.isOk()) {
      e.listErrors();
      exit(-1);
    }

    if(fixedx0) {
      ZeroFunction zf(e);
      Vector X(1);
      X[0] = x0;
      Vector result = newton(zf, X);
      _tprintf(_T("root:%-.16lg\n"), result[0]);
      if(listy) {
        _tprintf(_T("f(x):%-.16lg\n"), zf.y(getDouble(result[0])));
      }
    } else {
      bool rootFound = false;
      for(x0 = 0; x0 < 100; x0 = x0 ? x0 * -1.1 : 1e-3) {
        ZeroFunction zf(e);
        Vector X(1);
        X[0] = x0;
        try {
          Vector result = newton(zf, X);
          _tprintf(_T("root:%18.12le\n"), result[0]);
          if(listy) {
            _tprintf(_T("f(x):%18.12le\n"), zf.y(getDouble(result[0])));
          }
          rootFound = true;
          break;
        } catch(MathException) {
        }
      }
      if(!rootFound) {
        _tprintf(_T("Cannot find zero\n"));
      }
    }
  } catch(Exception e) {
    _ftprintf(stderr,_T("%s\n"), e.what());
    return -1;
  }
  return 0;
}
