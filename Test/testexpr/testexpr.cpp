#include "stdafx.h"
#include <Math/Expression/Expression.h>

using namespace Expr;

static void listSamples() {
  const UINT count = ExpressionSamples::getCount();
  for(UINT i = 0; i < count; i++) {
    _tprintf(_T("%3u: %-50s"), i, ExpressionSamples::getSample(i));
    if((i % 2 == 1) || (i == count - 1)) {
      _tprintf(_T("\n"));
    }
  }
}

static void OUTPUT(const TCHAR *format,...) {
  va_list argptr;
  va_start(argptr,format);
  _vtprintf(format,argptr);
  va_end(argptr);
}

static bool test(const String &expr, bool runCode) {
  Expression compiledExpr, interpreterExpr;
  bool ok = true;
  try {
    compiledExpr.compile(expr, true, stdout);
    interpreterExpr.compile(expr, false);
    if(!compiledExpr.isOk()) {
      const StringArray &errors = compiledExpr.getErrors();
      for(size_t i = 0; i < errors.size(); i++) {
        OUTPUT(_T("%s"), errors[i].cstr());
      }
      ok = false;
    } else {
      if(!runCode) {
        OUTPUT(_T("No test is done\n"));
        return true;
      }
      for(Real x = -2; x <= 2; x += 0.5) {
        compiledExpr.setValue(   _T("x"), x);
        interpreterExpr.setValue(_T("x"), x);
        switch(compiledExpr.getReturnType()) {
        case EXPR_RETURN_REAL:
          { const Real compiledResult     = compiledExpr.evaluate();
            const Real interpreterResult  = interpreterExpr.evaluate();
            const bool compiledDefined    = !isNan(compiledResult);
            const bool interpreterDefined = !isNan(interpreterResult);
            if((compiledDefined != interpreterDefined) || (compiledDefined && fabs(compiledResult - interpreterResult) > 3e-15)) {
              OUTPUT(_T("Result(Compiled       ):%s.\n"), toString(compiledResult              ).cstr());
              OUTPUT(_T("Result(Interpreter    ):%s.\n"), toString(interpreterResult           ).cstr());
              OUTPUT(_T("Difference(comp-interp):%s.\n"), toString(compiledResult - interpreterResult).cstr());
              ok = false;
            }
          }
          break;
        case EXPR_RETURN_BOOL:
          { const bool compiledResult    = compiledExpr.evaluateBool();
            const bool interpreterResult = interpreterExpr.evaluateBool();
            if(compiledResult != interpreterResult) {
              OUTPUT(_T("Result(Compiled     ):%s.\n"),toString(compiledResult   ).cstr());
              OUTPUT(_T("Result(Interpreter  ):%s.\n"),toString(interpreterResult).cstr());
              ok = false;
            }
          }
          break;
        } // switch
      } // for(x..
    } // else
  } catch (Exception e) {
    OUTPUT(_T("Exception:%s\n"), e.what());
    ok = false;
  }
  return ok;
}

int main() {
  bool runCode = true;
  for(;;) {
    const String str = inputString(_T("\nEnter expression:"));
    String expr;
    if(str.isEmpty()) continue;
    switch(str[0]) {
    case '?':
      listSamples();
      continue;
    case '-':
      runCode = false;
      _tprintf(_T("no run mode\n"));
      continue;
    case '+':
      runCode = true;
      _tprintf(_T("run mode\n"));
      continue;
    case ':':
      { const UINT count = ExpressionSamples::getCount();
        UINT       n;
        if((_stscanf(str.cstr()+1,_T("%u"), &n) == 1) && (n < count)) {
          expr = ExpressionSamples::getSample(n);
        } else {
          _tprintf(_T("%u:Illegal index (valid range:[0..%u]"), n, count-1);
          continue;
        }
      }
      break;
    case '*':
      { const UINT count = ExpressionSamples::getCount();
        for(UINT i = 0; i < count; i++) {
          _tprintf(_T("sample[%u]:"),i);
          const TCHAR *expr = ExpressionSamples::getSample(i);
          if(!test(expr,runCode)) break;
        }
        continue;
      }
    default:
      expr = str;
      break;
    }
    test(expr, runCode);
  }

  return 0;
}
