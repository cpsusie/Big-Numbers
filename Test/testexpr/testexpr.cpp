#include "stdafx.h"
#include <Math/Expression/Expression.h>

static const TCHAR *jacktool = 
_T("pi2 = 2 * pi;\n"
   "br0 = 8/(9*(1.1+cos(t*pi2)));\n"
   "br1 = 8/(9*(1.1+cos((t+1/6)*pi2)));\n"
   "br2 = 8/(9*(1.1+cos((t+2/6)*pi2)));\n"
   "br3 = 8/(9*(1.1+cos((t+3/6)*pi2)));\n"
   "br4 = 8/(9*(1.1+cos((t+4/6)*pi2)));\n"
   "br5 = 8/(9*(1.1+cos((t+5/6)*pi2)));\n"
   "\n"
   "sqrx=sqr(x);\n"
   "sqry=sqr(y);\n"
   "sqrz=sqr(z);\n"
   "\n"
   "( (sqrx/9+4*(sqry+sqrz))^-4\n"
   "+ (sqry/9+4*(sqrx+sqrz))^-4\n"
   "+ (sqrz/9+4*(sqry+sqrx))^-4\n"
   "+ ((4/3*x-4)^2+br0*(sqry+sqrz))^-4\n"
   "+ ((4/3*x+4)^2+br1*(sqry+sqrz))^-4\n"
   "+ ((4/3*z-4)^2+br2*(sqrx+sqry))^-4\n"
   "+ ((4/3*z+4)^2+br3*(sqrx+sqry))^-4\n"
   "+ ((4/3*y-4)^2+br4*(sqrx+sqrz))^-4\n"
   "+ ((4/3*y+4)^2+br5*(sqrx+sqrz))^-4)\n"
   "^-0.25\n"
   "-1");


static const TCHAR *samples[] = {
   _T("poly[1,poly[2,-1,3](x),3,4](x)")
  ,_T("cos(x+1)")
  ,_T("if(x+1 == x*2,x+1,x-1)")
  ,_T("if(x+1 <> x*2,x+1,x-1)")
  ,_T("if(x+1 <  x*2,x+1,x-1)")
  ,_T("if(x+1 <= x*2,x+1,x-1)")
  ,_T("if(x+1 >  x*2,x+1,x-1)")
  ,_T("if(x+1 >= x*2,x+1,x-1)")
  ,_T("if(not(x+1 >= x*2),x+1,x-1)")
  ,_T("if(x+1 >= x*2 and x > 0 ,x+1,x-1)")
  ,_T("if(x+1 >= x*2 and x < 0 ,x+1,x-1)")
  ,_T("if(x+1 >= x*2 or  x > 0 ,x+1,x-1)")
  ,_T("if(x+1 >= x*2 or  x < 0 ,x+1,x-1)")
  ,_T("if(not(x+1 >= x*2 and x > 0),x+1,x-1)")
  ,_T("if(not(x+1 >= x*2 and x < 0),x+1,x-1)")
  ,_T("if(not(x+1 >= x*2 or  x > 0),x+1,x-1)")
  ,_T("if(not(x+1 >= x*2 or  x < 0),x+1,x-1)")
  ,_T("if(not(x+1 >= x*2 or  x < 0 or x == 0),x+1,x-1)")
  ,_T("if(not(x+1 >= x*2 and x < 0 or x == 0),x+1,x-1)")
  ,_T("if(not((x < 0 or x == 0) and x+1 >= x*2),x+1,x-1)")
  ,jacktool
};

static void listSamples() {
  for(int i = 0; i < ARRAYSIZE(samples); i++) {
    _tprintf(_T("%2d:%s\n"), i, samples[i]);
  }
}

static void OUTPUT(const TCHAR *format,...) {
  va_list argptr;
  va_start(argptr,format);
  _vtprintf(format,argptr);
  va_end(argptr);
}

static void test(const String &expr, bool runCode) {
  Expression compiledExpr, interpreterExpr;
  try {
    compiledExpr.compile(expr, true, stdout);

    interpreterExpr.compile(expr, false);
    if(!compiledExpr.isOk()) {
      const StringArray &errors = compiledExpr.getErrors();
      for(size_t i = 0; i < errors.size(); i++) {
        OUTPUT(_T("%s"), errors[i].cstr());
      }
    } else {
      if(!runCode) {
        OUTPUT(_T("No test is done\n"));
        return;
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
            }
          }
          break;
        case EXPR_RETURN_BOOL:
          { const bool compiledResult    = compiledExpr.evaluateBool();
            const bool interpreterResult = interpreterExpr.evaluateBool();
            if(compiledResult != interpreterResult) {
              OUTPUT(_T("Result(Compiled     ):%s.\n"),toString(compiledResult   ).cstr());
              OUTPUT(_T("Result(Interpreter  ):%s.\n"),toString(interpreterResult).cstr());
            }
          }
          break;
        } // switch
      } // for(x..
    } // else
  } catch (Exception e) {
    OUTPUT(_T("Exception:%s\n"), e.what());
  }
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
      { int n;
        if((_stscanf(str.cstr()+1,_T("%d"), &n) == 1) && (n >= 0) && (n < ARRAYSIZE(samples))) {
          expr = samples[n];
        } else {
          _tprintf(_T("%d:Illegal index (valid range:[0..%zu]"), n, ARRAYSIZE(samples)-1);
          continue;
        }
      }
      break;
    default:
      expr = str;
      break;
    }
    test(expr, runCode);
  }

  return 0;
}
