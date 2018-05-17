#include "stdafx.h"
#include <Math/Expression/Expression.h>

using namespace Expr;

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
    _T("1")
   ,_T("1+2")
   ,_T("x")
   ,_T("x+1")
   ,_T("x-1")
   ,_T("x*2")
   ,_T("x/2")
   ,_T("(x+1)%2")
   ,_T("x^-2")
   ,_T("-x")
   ,_T("(x+1)+2")
   ,_T("(x+1)-2")
   ,_T("(x+1)*2")
   ,_T("(x+1)/2")
   ,_T("(x+1)^2")
   ,_T("x+1+x+2")
   ,_T("(x+1)-(x+2)")
   ,_T("(x+1)*(x+2)")
   ,_T("(x+1)/(x+2)")
   ,_T("(x+1)^(x+2)")
   ,_T("(x+1)*(x-1)/(x^2-1)")
   ,_T("x/(x*x)*x")
   ,_T("root(x,3)^3")
   ,_T("root(x*x,2)")
   ,_T("sin(x+1)")
   ,_T("cos(x+1)")
   ,_T("tan(x+1)")
   ,_T("cot(x+1)")
   ,_T("csc(x+1)")
   ,_T("sec(x+1)")
   ,_T("asin(x/3)")
   ,_T("acos(x/3)")
   ,_T("atan(x+1)")
   ,_T("atan2(x+7,5)")
   ,_T("acot(x+1)")
   ,_T("acsc(x+4)")
   ,_T("asec(x+4)")
   ,_T("exp(x+1)")
   ,_T("exp2(x+1)")
   ,_T("exp10(x+1)")
   ,_T("ln(x+1)")
   ,_T("log10(x+1)")
   ,_T("log2(x+1)")
   ,_T("fac(x+1)")
   ,_T("sqrt(x+1)")
   ,_T("sqr(x+1)")
   ,_T("hypot(x+7, x-5)")
   ,_T("sinh(x+1)")
   ,_T("cosh(x+1)")
   ,_T("tanh(x+1)")
   ,_T("asinh(x+1)")
   ,_T("acosh(x+2)")
   ,_T("atanh((x-0.5)/2)")
   ,_T("binomial(x+7,5)")
   ,_T("gauss(x*2)")
   ,_T("gamma(x+2)")
   ,_T("erf(x/2.4)")
   ,_T("inverf(x/3)")
   ,_T("norm(x/2.4)")
   ,_T("probit((x+0.2)/5)")
   ,_T("abs(x+1)")
   ,_T("floor(x+1)")
   ,_T("ceil(x+1)")
   ,_T("sign(x+1)")
   ,_T("cos(x)*-(x)")
   ,_T("max(x,x+1)")
   ,_T("max(x,x-1)")
   ,_T("min(x,x+1)")
   ,_T("min(x,x-1)")
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
   ,_T("x+x")
   ,_T("x+2*x")
   ,_T("3*x-4*x")
   ,_T("x/sqrt(x)")
   ,_T("sqrt(sqrt(x)) / x / sqrt(x)")
   ,_T("sum(i=0 to 10) x^i/fac(i)")
   ,_T("product(i=1 to 10) (x+i+0.25)/(x+i+1)")
   ,_T("root(x+1,root(2,x))")
   ,_T("a = x^2; a^2")
   ,_T("[x > 1]")
   ,_T("x > 1")
   ,_T("x < 1")
   ,_T("x != 1")
   ,_T("x*x < 1")
   ,_T("y=1; x+y < 1")
   ,_T("1 < x || x > 2")
   ,_T("if(x==1,sum(i=1 to 10) sin(x)^2+cos(i*2+0.25)/e^(x+i+1)"
                  "       ,sum(i=1 to 10) (x+i+0.55)/(x+i+1)"
                  "  )")
   ,_T("x^0.5")
   ,_T("x^-0.5")
   ,_T("root(x,-2)")
   ,_T("if(x==1 or x+1 < 1,x,x-1)")
   ,jacktool
};

static void listSamples() {
  for(size_t i = 0; i < ARRAYSIZE(samples); i++) {
    _tprintf(_T("%3zu: %-50s"), i, samples[i]);
    if((i % 2 == 1) || (i == ARRAYSIZE(samples) - 1)) {
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
      { int n;
        if((_stscanf(str.cstr()+1,_T("%d"), &n) == 1) && (n >= 0) && (n < ARRAYSIZE(samples))) {
          expr = samples[n];
        } else {
          _tprintf(_T("%d:Illegal index (valid range:[0..%zu]"), n, ARRAYSIZE(samples)-1);
          continue;
        }
      }
      break;
    case '*':
      { for(size_t i = 0; i < ARRAYSIZE(samples); i++) {
          _tprintf(_T("sample[%zu]:"),i);
          if(!test(samples[i],runCode)) break;
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
