#include "stdafx.h"
#include "CppUnitTest.h"
#include <Math/FPU.h>
#include "TestSamples.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static Real relativeDiff(const Real &x, const Real &x0) {
  return (x0 == 0) ? fabs(x) : fabs((x-x0)/x0);
}

#define endl _T("\n")

using namespace Expr;

namespace TestExpression {

#include <UnitTestTraits.h>

  class LOG : public std::wostringstream {
  public:
    ~LOG() {
      OUTPUT(_T("%s"), str().c_str());
    }
  };

  static void vprint(const TCHAR *format, va_list argptr) {
    OUTPUT(_T("%s"),vformat(format,argptr).cstr());
  }

  FILE *openListFile(int testCase) {
    const String fileName = Expression::getDefaultListFileName();
    FileNameSplitter fs(fileName);
    fs.setFileName(format(_T("testCase%03d"), testCase));
    return MKFOPEN(fs.getFullPath(),_T("w"));
  }

	TEST_CLASS(TestExpression) {
    public:

    TEST_METHOD(ExpressionTestEvaluate) {
      ExpressionTest::startEvaluateTest(OUTPUT);
      FPU::init();
#ifdef TRACE_MEMORY
//redirectDebugLog();
#endif
      try {
        const CompactArray<ExpressionTest*> &testArray = ExpressionTest::getAllSamples();
        const size_t                         n         = testArray.size();
        for(UINT i = 0; i < n; i++) {
          ExpressionTest &test = *testArray[i];
          const String expr = test.getExpr();
#ifdef TRACE_MEMORY
          debugLog(_T("testcase %3d:<%-50s>\n"),i,expr.cstr());
#endif
          OUTPUT(_T("Test[%d]:%s"),i,expr.cstr());
          FILE *listFile = openListFile(i);
          Expression compiledExpr, interpreterExpr;
          compiledExpr.compile(expr, true, listFile);
          fclose(listFile);

//          debugLog(_T("Test %d %s\n%s\n"), i, expr.cstr(), compiledExpr.treeToString().cstr());

          interpreterExpr.compile(expr, false);
          if(!compiledExpr.isOk()) {
            OUTPUT(_T("Error in testcase[%d]<%s>"), i, expr.cstr());
            const StringArray &errors = compiledExpr.getErrors();
            for(size_t i = 0; i < errors.size(); i++) {
              OUTPUT(_T("%s"), errors[i].cstr());
            }
            verify(false);
          } else {
            verify(compiledExpr.getReturnType()    == test.getReturnType());
            verify(interpreterExpr.getReturnType() == test.getReturnType());
            for(Real x = -2; x <= 2; x += 0.5) {
              compiledExpr.setValue(   _T("x"), x);
              interpreterExpr.setValue(_T("x"), x);
              switch(compiledExpr.getReturnType()) {
              case EXPR_RETURN_REAL:
                { const Real cppResult          = test.fr(x);
                  const Real compiledResult     = compiledExpr.evaluate();
                  const Real interpreterResult  = interpreterExpr.evaluate();
                  const bool cppDefined         = !isNan(cppResult);
                  const bool compiledDefined    = !isNan(compiledResult);
                  const bool interpreterDefined = !isNan(interpreterResult);

                  if((compiledDefined != cppDefined) || (compiledDefined && fabs(compiledResult - cppResult) > 3e-15)) {
                    LOG log;
                    log << _T("TestCase[") << i << _T("]:<") << expr << _T(">(x=") << toString(x) << _T(") failed.") << endl
                        << _T("Result(C++          ):") << toString(cppResult                   ) << _T(".") << endl
                        << _T("Result(Compiled     ):") << toString(compiledResult              ) << _T(".") << endl
                        << _T("Result(Interpreter  ):") << toString(interpreterResult           ) << _T(".") << endl
                        << _T("Difference(comp-C++ ):") << toString(compiledResult - cppResult  ) << _T(".") << endl;
                    verify(false);
                  }
                  if((interpreterDefined != cppDefined) || (interpreterDefined && fabs(interpreterResult - cppResult) > 3e-15)) {
                    LOG log;
                    log << _T("TestCase[") << i << _T("]:<") << expr << _T(">(x=") << toString(x)  << _T(") failed.") << endl
                        << _T("Result(C++          ):") << toString(cppResult                    ) << _T(".") << endl
                        << _T("Result(Interpreter  ):") << toString(interpreterResult            ) << _T(".") << endl
                        << _T("Result(Compiled     ):") << toString(compiledResult               ) << _T(".") << endl
                        << _T("Difference(intp-C++ ):") << toString(interpreterResult - cppResult) << _T(".") << endl;
                    verify(false);
                  }
                }
                break;
              case EXPR_RETURN_BOOL:
                { const bool compiledResult    = compiledExpr.evaluateBool();
                  const bool interpreterResult = interpreterExpr.evaluateBool();
                  const bool cppResult         = test.fb(x);
                  if((compiledResult != cppResult) || (interpreterResult != cppResult)) {
                    LOG log;
                    log << _T("TestCase[") << i << _T("]:<") << expr << _T(">(x=") << toString(x) << _T(") failed.") << endl
                        << _T("Result(C++          ):") << toString(cppResult        ) << _T(".") << endl
                        << _T("Result(Compiled     ):") << toString(compiledResult   ) << _T(".") << endl
                        << _T("Result(Interpreter  ):") << toString(interpreterResult) << _T(".") << endl;
                    verify(false);
                  }
                }
                break;
              } // switch
            } // for(x..
            test.setEvaluated();
          } // else
        } // for(i...
      } catch (Exception e) {
        OUTPUT(_T("Exception:%s"), e.what());
        verify(false);
      }
    }

    TEST_METHOD(ExpressionTestPow) {
      FPU::init();
      String str;
      try {
        for(Real p = -70; p <= 70; p += 0.5) {
          str = format(_T("(1+x)^%s"),toString(p).cstr());
          Expression compiledExpr, interpreterExpr;
          compiledExpr.compile(   str, true);
          interpreterExpr.compile(str, false);
          verify(compiledExpr.isOk());
          verify(interpreterExpr.isOk());
          const Real startx = (p == getInt(p)) ? -1.9 : -0.9, step = 0.125;
          for(Real x = startx; x <= 0.5; x += step) {
            compiledExpr.setValue(   _T("x"),x);
            interpreterExpr.setValue(_T("x"),x);
            const Real y1 = mypow((1+x),p);
            const Real y2 = compiledExpr.evaluate();
            const Real y3 = interpreterExpr.evaluate();
            verify(relativeDiff(y2,y1) < 1e-13);
            verify(relativeDiff(y3,y1) < 1e-13);
          }
        }
      } catch (Exception e) {
        OUTPUT(_T("Exception:%s"), e.what());
        OUTPUT(_T("str:[%s]"    ), str.cstr());
        verify(false);
      }
    }

#ifdef TEST_DERIVATIVES
    TEST_METHOD(ExpressionTestDerive) {
      ExpressionTest::startDeriveTest(OUTPUT);
      FPU::init();
//      redirectDebugLog();
      try {
        const CompactArray<ExpressionTest*> &testArray = ExpressionTest::getAllSamples();
        const size_t                         n         = testArray.size();
        for(int i = 0; i < n; i++) {
          ExpressionTest &test = *testArray[i];
          if(!test.isDerivable()) {
            continue;
          }
          const String expr = test.getExpr();
          Expression compiledExpr, interpreterExpr;
          compiledExpr.compile(expr, true);

//          OUTPUT(_T("Test %d %s\n%s\n"), i, expr.cstr(), compiledExpr.treeToString().cstr());

          interpreterExpr.compile(expr, false);
          if(!compiledExpr.isOk()) {
            OUTPUT(_T("Error in testcase[%d]<%s>"), i, expr.cstr());
            const StringArray &errors = compiledExpr.getErrors();
            for(size_t i = 0; i < errors.size(); i++) {
              OUTPUT(_T("%s"), errors[i].cstr());
            }
            verify(false);
          }
          verify(compiledExpr.getReturnType()    == EXPR_RETURN_REAL);
          verify(interpreterExpr.getReturnType() == EXPR_RETURN_REAL);
          Expression compiledDFDX    = compiledExpr.getDerived(   _T("x"),false);
          Expression interpreterDFDX = interpreterExpr.getDerived(_T("x"),false);
          verify(compiledDFDX.isOk());
          verify(interpreterDFDX.isOk());
          for(Real x = -2; x <= 2; x += 0.31) {
            const Real y = test.fr(x);
            if(isNan(y)) continue;

            compiledDFDX.setValue(   _T("x"), x);
            interpreterDFDX.setValue(_T("x"), x);
            const Real cppResult          = test.numDFDX(x);
            const Real compiledResult     = compiledDFDX.evaluate();
            const Real interpreterResult  = interpreterDFDX.evaluate();
            const bool cppDefined         = !isNan(cppResult);
            const bool interpreterDefined = !isNan(interpreterResult);
            const bool compiledDefined    = !isNan(compiledResult);
            Real       relDiff            = 0;

#define USE_ABSERROR
#ifdef USE_ABSERROR
#define CALCERROR(x,x0) fabs((x)-(x0))
#else
#define CALCERROR(x,x0) relativeDiff(x,x0)
#endif

            if((compiledDefined != cppDefined) || (compiledDefined && ((relDiff=CALCERROR(compiledResult,cppResult)) > 5e-6))) {
              LOG log;
              log << _T("TestCase[") << i << _T("]:<") << expr << _T("'>(x=") << toString(x) << _T(") failed.") << endl
                  << _T("f'(x) = ")  << compiledDFDX.toString() << endl
                  << _T("Defined(C++         ):") << boolToStr(cppDefined        ) << _T(".") << endl
                  << _T("Defined(Compiled    ):") << boolToStr(compiledDefined   ) << _T(".") << endl
                  << _T("Defined(Interpreter ):") << boolToStr(interpreterDefined) << _T(".") << endl
                  << _T("Result(C++          ):") << toString(cppResult          ) << _T(".") << endl
                  << _T("Result(Compiled     ):") << toString(compiledResult     ) << _T(".") << endl
                  << _T("Result(Interpreter  ):") << toString(interpreterResult  ) << _T(".") << endl
                  << _T("Difference(comp-C++ ):") << toString(relDiff            ) << _T(".") << endl;
              verify(false);
            }
            if((interpreterDefined != cppDefined) || (interpreterDefined && ((relDiff=CALCERROR(interpreterResult,cppResult)) > 5e-6))) {
              LOG log;
              log << _T("TestCase[") << i << _T("]:<") << expr << _T("'>(x=") << toString(x)  << _T(") failed.") << endl
                  << _T("Defined(C++         ):") << boolToStr(cppDefined        ) << _T(".") << endl
                  << _T("Defined(Compiled    ):") << boolToStr(compiledDefined   ) << _T(".") << endl
                  << _T("Defined(Interpreter ):") << boolToStr(interpreterDefined) << _T(".") << endl
                  << _T("Result(C++          ):") << toString(cppResult          ) << _T(".") << endl
                  << _T("Result(Interpreter  ):") << toString(interpreterResult  ) << _T(".") << endl
                  << _T("Result(Compiled     ):") << toString(compiledResult     ) << _T(".") << endl
                  << _T("Difference(intp-C++ ):") << toString(relDiff            ) << _T(".") << endl;
              verify(false);
            }
          } // for(x..
          test.setDerived();
        } // for(i...
      } catch (Exception e) {
        OUTPUT(_T("Exception:%s"), e.what());
        verify(false);
      }
    }
#endif // TEST_DERIVATIVES

    TEST_METHOD(TestMachineCode) {
      try {
        generateTestSequence(vprint);
      } catch (Exception e) {
        OUTPUT(_T("Exception:%s"), e.what());
      }
    }
    TEST_METHOD(CallAssemblerCode) {
      callAssemblerCode();
    }

  };
}
