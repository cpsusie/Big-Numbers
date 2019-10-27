#include "stdafx.h"
#include <FileNameSplitter.h>
#include <Math/FPU.h>
#include "TestSamples.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static Real relativeDiff(const Real &x, const Real &x0) {
  return (x0 == 0) ? fabs(x) : fabs((x-x0)/x0);
}

#define endl "\n"

using namespace Expr;

namespace TestExpression {

#include <UnitTestTraits.h>

  class LOG : public std::wostringstream {
  public:
    ~LOG() {
      OUTPUT(_T("%s"), str().c_str());
    }
    LOG &operator<<(const char *s) {
      USES_ACONVERSION;
      const wchar_t *wstr = A2W(s);
      __super::operator<<( wstr );
      return *this;
    }
  };

//#define TEST_DERIVATIVES

  static void vprint(const TCHAR *format, va_list argptr) {
    OUTPUT(_T("%s"),vformat(format,argptr).cstr());
  }

  String makeFileName(int testCase, const String &dirComp1, const String &dirComponent = EMPTYSTRING) {
    const String fileName = Expression::getDefaultListFileName();
    FileNameSplitter fs(fileName);
    DirComponents dc = fs.getDirComponents();
    dc.add(3,dirComp1);
    if(dirComponent.length() > 0) {
      dc.add(4,dirComponent);
    }
    fs.setDir(dc);
    fs.setFileName(format(_T("testCase%04d"), testCase));
    return fs.getFullPath();
  }

  void verifyExprOk(const Expression &e, bool expectedOk=true) {
    if(expectedOk) {
      if(!e.isOk()) {
        OUTPUT(_T("Expr.isOk=false. %s"), e.getErrors().toString().cstr());
        verify(false);
      }
    } else {
      verify(!e.isOk());
    }
  }

//#define GENERATE_LISTFILES

#ifdef GENERATE_LISTFILES
#define COND_MKFOPEN(...) MKFOPEN(__VA_ARGS__)
#define FPRINTF(...) _ftprintf(__VA_ARGS__)
#else
#define COND_MKFOPEN(...) NULL
#define FPRINTF(...)
#endif // GENERATE_LISTFILES

  FILE *openEvalListFile(       int testCase) { return COND_MKFOPEN(makeFileName(testCase, _T("eval")                     ), _T("w")); }
  FILE *openEvalReducedListFile(int testCase) { return COND_MKFOPEN(makeFileName(testCase, _T("eval"), _T("reduced"      )), _T("w")); }
  FILE *openEvalDumpFile(       int testCase) { return COND_MKFOPEN(makeFileName(testCase, _T("eval"), _T("dump"         )), _T("w")); }
  FILE *openEvalReducedDumpFile(int testCase) { return COND_MKFOPEN(makeFileName(testCase, _T("eval"), _T("reduced\\dump")), _T("w")); }

  FILE *openBoolListFile(       int testCase) { return COND_MKFOPEN(makeFileName(testCase, _T("bool")                     ), _T("w")); }
  FILE *openBoolReducedListFile(int testCase) { return COND_MKFOPEN(makeFileName(testCase, _T("bool"), _T("reduced"      )), _T("w")); }
  FILE *openBoolDumpFile(       int testCase) { return COND_MKFOPEN(makeFileName(testCase, _T("bool"), _T("dump"         )), _T("w")); }
  FILE *openBoolReducedDumpFile(int testCase) { return COND_MKFOPEN(makeFileName(testCase, _T("bool"), _T("reduced\\dump")), _T("w")); }

  FILE *openPowListFile(        int testCase) { return COND_MKFOPEN(makeFileName(testCase, _T("pow" )                     ), _T("w")); }
  FILE *openRootListFile(       int testCase) { return COND_MKFOPEN(makeFileName(testCase, _T("root")                     ), _T("w")); }

#define FCLOSE(f) { if(f) { fclose(f); f=NULL; } }

  typedef enum {
    CMP_EQUAL
   ,CMP_EQUALMINUS
   ,CMP_NOTEQUAL
  } CompareResult;

  class EqualExpressionSample {
  public:
    const String        m_e1,m_e2;
    const CompareResult m_expected;
    EqualExpressionSample(const String &e1, const String &e2, CompareResult expected)
      : m_e1(e1)
      , m_e2(e2)
      , m_expected(expected)
    {
    }
    void doTest() const;
  };

  void EqualExpressionSample::doTest() const {
    Expression exp1, exp2;
    exp1.compile(m_e1, false);
    verifyExprOk(exp1);
    exp2.compile(m_e1, false);
    verifyExprOk(exp2);
    switch(m_expected) {
    case CMP_EQUAL     :
      verify(exp1.equal(exp2));
      break;
    case CMP_EQUALMINUS:
      verify(exp1.equalMinus(exp2));
      break;
    case CMP_NOTEQUAL  :
      verify(!exp1.equal(     exp2));
      verify(!exp1.equalMinus(exp2));
      break;
    }
  }

	TEST_CLASS(TestExpression) {
    public:

    TEST_METHOD(SNodeEqual) {
      static const EqualExpressionSample testData[] = {
        EqualExpressionSample("x"        ,"x"                       ,CMP_EQUAL     )
       ,EqualExpressionSample("x"        ,"-x"                      ,CMP_EQUALMINUS)
       ,EqualExpressionSample("1"        ,"1"                       ,CMP_EQUAL     )
       ,EqualExpressionSample("1"        ,"-1"                      ,CMP_EQUALMINUS)
       ,EqualExpressionSample("abs(x)"   ,"abs(-x)"                 ,CMP_EQUAL     )
       ,EqualExpressionSample("cos(x)"   ,"cos(-x)"                 ,CMP_EQUAL     )
       ,EqualExpressionSample("cosh(x)"  ,"cosh(-x)"                ,CMP_EQUAL     )
       ,EqualExpressionSample("gauss(x)" ,"gauss(-x)"               ,CMP_EQUAL     )
       ,EqualExpressionSample("sec(x)"   ,"sec(-x)"                 ,CMP_EQUAL     )
       ,EqualExpressionSample("sqr(x)"   ,"sqr(-x)"                 ,CMP_EQUAL     )
       ,EqualExpressionSample("acsc(x)"  ,"acsc(-x)"                ,CMP_EQUALMINUS)
       ,EqualExpressionSample("asin(x)"  ,"asin(-x)"                ,CMP_EQUALMINUS)
       ,EqualExpressionSample("asinh(x)" ,"asinh(-x)"               ,CMP_EQUALMINUS)
       ,EqualExpressionSample("atan(x)"  ,"atan(-x)"                ,CMP_EQUALMINUS)
       ,EqualExpressionSample("atanh(x)" ,"atanh(-x)"               ,CMP_EQUALMINUS)
       ,EqualExpressionSample("cot(x)"   ,"cot(-x)"                 ,CMP_EQUALMINUS)
       ,EqualExpressionSample("csc(x)"   ,"csc(-x)"                 ,CMP_EQUALMINUS)
       ,EqualExpressionSample("erf(x)"   ,"erf(-x)"                 ,CMP_EQUALMINUS)
       ,EqualExpressionSample("inverf(x)","inverf(-x)"              ,CMP_EQUALMINUS)
       ,EqualExpressionSample("sign(x)"  ,"sign(-x)"                ,CMP_EQUALMINUS)
       ,EqualExpressionSample("sin(x)"   ,"sin(-x)"                 ,CMP_EQUALMINUS)
       ,EqualExpressionSample("sinh(x)"  ,"sinh(-x)"                ,CMP_EQUALMINUS)
       ,EqualExpressionSample("tan(x)"   ,"tan(-x)"                 ,CMP_EQUALMINUS)
       ,EqualExpressionSample("tanh(x)"  ,"tanh(-x)"                ,CMP_EQUALMINUS)

      };
      for(size_t i = 0; i < ARRAYSIZE(testData); i++) {
        const EqualExpressionSample &sample = testData[i];
      }
    };

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
          const String    expr = test.getExpr();
          if (i == 119) {
            int fisk = 1;
          }
#ifdef TRACE_MEMORY
          debugLog(_T("testcase %3d:<%-50s>\n"),i,expr.cstr());
#endif
          INFO(_T("Test[%d]:%s"), i, expr.cstr());

          FILE * listFile   = openEvalListFile(       i);
          FILE *RlistFile   = openEvalReducedListFile(i);
          FILE * dumpFile   = openEvalDumpFile(       i);
          FILE *RdumpFile   = openEvalReducedDumpFile(i);

          Expression compiledExpr(   test.getTrigonometricMode());
          Expression interpreterExpr(test.getTrigonometricMode());
          Expression reducedExpr(    test.getTrigonometricMode());
          compiledExpr.compile(expr, true,false, listFile);
          reducedExpr.compile( expr, true,true ,RlistFile);

          FPRINTF( dumpFile, _T("%s\n%s\n"), expr.cstr(),compiledExpr.treeToString().cstr());
          FPRINTF(RdumpFile, _T("%s\n%s\n"), expr.cstr(),reducedExpr.treeToString().cstr());

          FCLOSE(RdumpFile);
          FCLOSE( dumpFile);
          FCLOSE(RlistFile);
          FCLOSE( listFile);

//          debugLog(_T("Test %d %s\n%s\n"), i, expr.cstr(), compiledExpr.treeToString().cstr());

          interpreterExpr.compile(expr, false,false);
          if(!compiledExpr.isOk()) {
            OUTPUT(_T("Error in testcase[%d]<%s>"), i, expr.cstr());
            const StringArray &errors = compiledExpr.getErrors();
            for(size_t i = 0; i < errors.size(); i++) {
              OUTPUT(_T("%s"), errors[i].cstr());
            }
            verify(false);
          } else {
            verifyExprOk(reducedExpr    );
            verifyExprOk(interpreterExpr);
            verify(compiledExpr.getReturnType()    == test.getReturnType());
            verify(reducedExpr.getReturnType()     == test.getReturnType());
            verify(interpreterExpr.getReturnType() == test.getReturnType());
            for(Real x = -1.91; x <= 2; x += 0.12489) {
              compiledExpr.setValue(   _T("x"), x);
              reducedExpr.setValue(    _T("x"), x);
              interpreterExpr.setValue(_T("x"), x);
              switch(compiledExpr.getReturnType()) {
              case EXPR_RETURN_REAL:
                { const Real cppResult          = test.fr(x);
                  const Real compiledResult     = compiledExpr.evaluate();
                  const Real reducedResult      = reducedExpr.evaluate();
                  const Real interpreterResult  = interpreterExpr.evaluate();
                  const bool cppDefined         = !isnan(cppResult        );
                  const bool compiledDefined    = !isnan(compiledResult   );
                  const bool reducedDefined     = !isnan(reducedResult    );
                  const bool interpreterDefined = !isnan(interpreterResult);



#define RESULT(v) format(_T("%-10s defined:%-5s, v=%s.")                          \
                        ,_T(#v), boolToStr(v##Defined)                            \
                        , toString(v##Result,20,28,std::ios::scientific).cstr())


#define LOGERROR(v)                                                                                  \
{ LOG log;                                                                                           \
  log << _T("TestCase[" << i << "]:<") << expr << ">(x=" << toString(x) << ") failed."      << endl  \
      << RESULT(cpp                   )                                                     << endl  \
      << RESULT(v                     )                                                     << endl  \
      << "Diff(" << #v << "Result - cppResult:" << toString(v##Result - cppResult  ) << "." << endl; \
}

#define CHECKDEFANDVALUE(v)                                                               \
{ if((v##Defined != cppDefined) || (v##Defined && fabs(v##Result - cppResult) > 3e-13)) { \
    LOGERROR(v);                                                                          \
    verify(false);                                                                        \
  }                                                                                       \
}


                  CHECKDEFANDVALUE(compiled   )
                  CHECKDEFANDVALUE(reduced    )
                  CHECKDEFANDVALUE(interpreter)
                }
                break;
              case EXPR_RETURN_BOOL:
                { const bool cppResult         = test.fb(x);
                  const bool compiledResult    = compiledExpr.evaluateBool();
                  const bool reducedResult     = reducedExpr.evaluateBool();
                  const bool interpreterResult = interpreterExpr.evaluateBool();
                  if((compiledResult != cppResult) || (reducedResult != cppResult) || (interpreterResult != cppResult)) {
                    LOG log;
                    log << "TestCase[" << i << "]:<" << expr << ">(x=" << toString(x) << ") failed." << endl
                        << "Result(C++          ):" << toString(cppResult        ) << "." << endl
                        << "Result(Compiled     ):" << toString(compiledResult   ) << "." << endl
                        << "Result(Reduced      ):" << toString(reducedResult    ) << "." << endl
                        << "Result(Interpreter  ):" << toString(interpreterResult) << "." << endl;
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

    static inline bool isOddInt(Real n) {
      return isInt(n) && !isEven(getInt(n));
    }

    TEST_METHOD(ExpressionTestPowRoot) {
      FPU::init();
      int listFileCounter = 0;
      String str;
      try {
        for(Real p = -70; p <= 70; p += 0.5) {
          str = format(_T("(1+x)^%s"),toString(p).cstr());
          Expression compPowExpr, interpretPowExpr;

          FILE *powListFile = openPowListFile(listFileCounter);
          compPowExpr.compile(   str, true, false, powListFile);
          FCLOSE(powListFile);

          interpretPowExpr.compile(str, false);
          verifyExprOk(compPowExpr     );
          verifyExprOk(interpretPowExpr);

          str = format(_T("root(1+x,%s)"),toString(p).cstr());
          Expression compRootExpr, interpretRootExpr;
          FILE *rootListFile = openRootListFile(listFileCounter++);
          compRootExpr.compile(   str, true, false, rootListFile);
          FCLOSE(rootListFile);

          interpretRootExpr.compile(str, false);
          verifyExprOk(compRootExpr     ,p!=0);
          verifyExprOk(interpretRootExpr);

          const Real startx = (p == getInt(p)) ? -1.9 : -0.9, step = 0.125;
          for(Real x = startx; x <= 0.5; x += step) {
            compPowExpr.setValue(     _T("x"),x);
            interpretPowExpr.setValue(_T("x"),x);
            const Real yp1 = mypow((1+x),p);
            const Real yp2 = compPowExpr.evaluate();
            const Real yp3 = interpretPowExpr.evaluate();
            verify(relativeDiff(yp2,yp1) < 1e-13);
            verify(relativeDiff(yp3,yp1) < 1e-13);

            if((p == 0) || ((1+x < 0) && !isOddInt(p))) {
              continue;
            }
            compRootExpr.setValue(     _T("x"),x);
            interpretRootExpr.setValue(_T("x"),x);
            const Real yr1 = root(1+x,p);
            const Real yr2 = compRootExpr.evaluate();
            const Real yr3 = interpretRootExpr.evaluate();
            verify(relativeDiff(yr2,yr1) < 1e-13);
            verify(relativeDiff(yr3,yr1) < 1e-13);

          }
        }

        for(int num = -1; num <= 1; num+=2) {
          for(int den = 2; den <= 64; den++) {
            const Rational p(num,den);
            str = format(_T("(1+x)^(%s)"),toString(p).cstr());
            Expression compPowExpr, interpretPowExpr;
            FILE *listPowFile = openPowListFile(listFileCounter);
            compPowExpr.compile(   str, true, false, listPowFile);
            FCLOSE(listPowFile);
            interpretPowExpr.compile(str, false);
            verifyExprOk(compPowExpr     );
            verifyExprOk(interpretPowExpr);

            str = format(_T("root(1+x,%s)"),toString(p).cstr());
            Expression compRootExpr, interpretRootExpr;
            FILE *listRootFile = openRootListFile(listFileCounter++);
            compRootExpr.compile(   str, true, false, listRootFile);
            FCLOSE(listRootFile);
            interpretRootExpr.compile(str, false);
            verifyExprOk(compRootExpr     );
            verifyExprOk(interpretRootExpr);

            const Real startx = -0.9, step = 0.125;
            for(Real x = startx; x <= 0.5; x += step) {
              compPowExpr.setValue(     _T("x"),x);
              interpretPowExpr.setValue(_T("x"),x);
              const Real yp1 = mypow((1+x),getReal(p));
              const Real yp2 = compPowExpr.evaluate();
              const Real yp3 = interpretPowExpr.evaluate();
              verify(relativeDiff(yp2,yp1) < 1e-13);
              verify(relativeDiff(yp3,yp1) < 1e-13);

              compRootExpr.setValue(     _T("x"),x);
              interpretRootExpr.setValue(_T("x"),x);
              const Real yr1 = root(1+x,getReal(p));
              const Real yr2 = compRootExpr.evaluate();
              const Real yr3 = interpretRootExpr.evaluate();
              verify(relativeDiff(yr2,yr1) < 1e-13);
              verify(relativeDiff(yr3,yr1) < 1e-13);
            }
          }
        }

      } catch (Exception e) {
        OUTPUT(_T("Exception:%s"), e.what());
        OUTPUT(_T("str:[%s]"    ), str.cstr());
        verify(false);
      }
    }

    TEST_METHOD(ExpressionTestAndOrReduction) {
      FPU::init();
      const TCHAR *relOp[] = {
        _T("=="), _T("!="), _T("<"), _T("<="), _T(">"), _T(">=")
      };
      const TCHAR *relOpr[] = {
        _T("=="), _T("!="), _T(">"), _T(">="), _T("<"), _T("<=")
      };
      const TCHAR *boolOp[] = { _T("&&"), _T("||") };
      StringArray aAndbb,aAndbr,aAndrb,aAndrr;
      StringArray aOrbb,aOrbr,aOrrb,aOrrr;
      for(int rop1 = 0; rop1 < ARRAYSIZE(relOp); rop1++) {
        for(int rop2 = 0; rop2 < ARRAYSIZE(relOp); rop2++) {
          for(int c1 = 1; c1 <= 3; c1++) {
            for(int c2 = 1; c2 <= 3; c2++) {
              const String op1  = format(_T("x %s %d"), relOp[rop1], c1);
              const String op2  = format(_T("x %s %d"), relOp[rop2], c2);
              const String op1r = format(_T("%d %s x"), c1, relOpr[rop1]);
              const String op2r = format(_T("%d %s x"), c2, relOpr[rop2]);
              aAndbb.add(format(_T("%s && %s"), op1.cstr() , op2.cstr()));
              aAndbr.add(format(_T("%s && %s"), op1.cstr() , op2r.cstr()));
              aAndrb.add(format(_T("%s && %s"), op1r.cstr(), op2.cstr()));
              aAndrr.add(format(_T("%s && %s"), op1r.cstr(), op2r.cstr()));
              aOrbb.add( format(_T("%s || %s"), op1.cstr() , op2.cstr()));
              aOrbr.add( format(_T("%s || %s"), op1.cstr() , op2r.cstr()));
              aOrrb.add( format(_T("%s || %s"), op1r.cstr(), op2.cstr()));
              aOrrr.add( format(_T("%s || %s"), op1r.cstr(), op2r.cstr()));
            }
          }
        }
      }
      StringArray exprArray;
      exprArray.addAll(aAndbb); aAndbb.clear();
      exprArray.addAll(aAndbr); aAndbr.clear();
      exprArray.addAll(aAndrb); aAndrb.clear();
      exprArray.addAll(aAndrr); aAndrr.clear();
      exprArray.addAll(aOrbb ); aOrbb.clear();
      exprArray.addAll(aOrbr ); aOrbr.clear();
      exprArray.addAll(aOrrb ); aOrrb.clear();
      exprArray.addAll(aOrrr ); aOrrr.clear();

      for(int rop1 = 0; rop1 < ARRAYSIZE(relOp); rop1++) {
        for(int rop2 = 0; rop2 < ARRAYSIZE(relOp); rop2++) {
          const String op1  = format(_T("2*x %s x+1")  , relOp[rop1] );
          const String op2  = format(_T("2*x %s x+1")  , relOp[rop2] );
          const String op1r = format(_T("x+1 %s 2*x")  , relOpr[rop1]);
          const String op2r = format(_T("x+1 %s 2*x")  , relOpr[rop2]);
          aAndbb.add(format(_T("%s && %s"), op1.cstr() , op2.cstr()) );
          aAndbr.add(format(_T("%s && %s"), op1.cstr() , op2r.cstr()));
          aAndrb.add(format(_T("%s && %s"), op1r.cstr(), op2.cstr()) );
          aAndrr.add(format(_T("%s && %s"), op1r.cstr(), op2r.cstr()));
          aOrbb.add( format(_T("%s || %s"), op1.cstr() , op2.cstr()) );
          aOrbr.add( format(_T("%s || %s"), op1.cstr() , op2r.cstr()));
          aOrrb.add( format(_T("%s || %s"), op1r.cstr(), op2.cstr()) );
          aOrrr.add( format(_T("%s || %s"), op1r.cstr(), op2r.cstr()));
        }
      }
      exprArray.addAll(aAndbb); aAndbb.clear();
      exprArray.addAll(aAndbr); aAndbr.clear();
      exprArray.addAll(aAndrb); aAndrb.clear();
      exprArray.addAll(aAndrr); aAndrr.clear();
      exprArray.addAll(aOrbb ); aOrbb.clear();
      exprArray.addAll(aOrbr ); aOrbr.clear();
      exprArray.addAll(aOrrb ); aOrrb.clear();
      exprArray.addAll(aOrrr ); aOrrr.clear();

      try {
        const size_t n = exprArray.size();
        for(UINT i = 0; i < n; i++) {
          const String  expr = exprArray[i];
          if(i % 100 == 0) {
            INFO(_T("Test[%d]:%s"), i, expr.cstr());
          }
          FILE      * listFile = openBoolListFile(       i);
          FILE      *RlistFile = openBoolReducedListFile(i);
          FILE      * dumpFile = openBoolDumpFile(       i);
          FILE      *RdumpFile = openBoolReducedDumpFile(i);

          Expression compiledExpr, interpreterExpr, reducedExpr;
          compiledExpr.compile(expr, true,false, listFile);
          reducedExpr.compile( expr, true,true ,RlistFile);

          FPRINTF( dumpFile, _T("%s\n%s\n"), expr.cstr(),compiledExpr.treeToString().cstr());
          FPRINTF(RdumpFile, _T("%s\n%s\n"), expr.cstr(),reducedExpr.treeToString().cstr());
          FCLOSE(RdumpFile);
          FCLOSE( dumpFile);
          FCLOSE(RlistFile);
          FCLOSE( listFile);

//          debugLog(_T("Test %d %s\n%s\n"), i, expr.cstr(), compiledExpr.treeToString().cstr());

          interpreterExpr.compile(expr, false,false);
          if(!compiledExpr.isOk()) {
            OUTPUT(_T("Error in testcase[%d]<%s>"), i, expr.cstr());
            const StringArray &errors = compiledExpr.getErrors();
            for(size_t i = 0; i < errors.size(); i++) {
              OUTPUT(_T("%s"), errors[i].cstr());
            }
            verify(false);
          } else {
            verifyExprOk(reducedExpr    );
            verifyExprOk(interpreterExpr);
            verify(compiledExpr.getReturnType()    == EXPR_RETURN_BOOL);
            verify(reducedExpr.getReturnType()     == EXPR_RETURN_BOOL);
            verify(interpreterExpr.getReturnType() == EXPR_RETURN_BOOL);
            for(Real x = 0; x <= 4; x++) {
              compiledExpr.setValue(   _T("x"), x);
              reducedExpr.setValue(    _T("x"), x);
              interpreterExpr.setValue(_T("x"), x);
              const bool compiledResult    = compiledExpr.evaluateBool();
              const bool reducedResult     = reducedExpr.evaluateBool();
              const bool interpreterResult = interpreterExpr.evaluateBool();
              if((compiledResult != interpreterResult) || (reducedResult != interpreterResult)) {
                LOG log;
                log << "TestCase[" << i << "]:<" << expr << ">(x=" << toString(x) << ") failed." << endl
                    << "Result(Compiled     ):" << toString(compiledResult   ) << "." << endl
                    << "Result(Reduced      ):" << toString(reducedResult    ) << "." << endl
                    << "Result(Interpreter  ):" << toString(interpreterResult) << "." << endl;
                verify(false);
              }
            } // for(x..
          } // else
        } // for(i...
      } catch (Exception e) {
        OUTPUT(_T("Exception:%s"), e.what());
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
        for(size_t i = 0; i < n; i++) {
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
          Expression compiledDFDX    = compiledExpr.getDerived(   _T("x"),true);
          Expression interpreterDFDX = interpreterExpr.getDerived(_T("x"),true);
          verifyExprOk(compiledDFDX   );
          verifyExprOk(interpreterDFDX);
          for(Real x = -2; x <= 2; x += 0.31) {
            const Real y = test.fr(x);
            if(isnan(y)) continue;

            compiledDFDX.setValue(   _T("x"), x);
            interpreterDFDX.setValue(_T("x"), x);
            const Real cppResult          = test.numDFDX(x);
            const Real compiledResult     = compiledDFDX.evaluate();
            const Real interpreterResult  = interpreterDFDX.evaluate();
            const bool cppDefined         = !isnan(cppResult);
            const bool interpreterDefined = !isnan(interpreterResult);
            const bool compiledDefined    = !isnan(compiledResult);
            Real       relDiff            = 0;

#define USE_ABSERROR
#ifdef USE_ABSERROR
#define CALCERROR(x,x0) fabs((x)-(x0))
#else
#define CALCERROR(x,x0) relativeDiff(x,x0)
#endif

            if((compiledDefined != cppDefined) || (compiledDefined && ((relDiff=CALCERROR(compiledResult,cppResult)) > 5e-6))) {
              LOG log;
              log << "TestCase[") << i << "]:<") << expr << "'>(x=") << toString(x) << ") failed.") << endl
                  << "f'(x) = ")  << compiledDFDX.toString() << endl
                  << "Defined(C++         ):") << boolToStr(cppDefined        ) << "." << endl
                  << "Defined(Compiled    ):") << boolToStr(compiledDefined   ) << "." << endl
                  << "Defined(Interpreter ):") << boolToStr(interpreterDefined) << "." << endl
                  << "Result(C++          ):") << toString(cppResult          ) << "." << endl
                  << "Result(Compiled     ):") << toString(compiledResult     ) << "." << endl
                  << "Result(Interpreter  ):") << toString(interpreterResult  ) << "." << endl
                  << "Difference(comp-C++ ):") << toString(relDiff            ) << "." << endl;
              verify(false);
            }
            if((interpreterDefined != cppDefined) || (interpreterDefined && ((relDiff=CALCERROR(interpreterResult,cppResult)) > 5e-6))) {
              LOG log;
              log << "TestCase[") << i << "]:<") << expr << "'>(x=") << toString(x)  << ") failed.") << endl
                  << "Defined(C++         ):") << boolToStr(cppDefined        ) << "." << endl
                  << "Defined(Compiled    ):") << boolToStr(compiledDefined   ) << "." << endl
                  << "Defined(Interpreter ):") << boolToStr(interpreterDefined) << "." << endl
                  << "Result(C++          ):") << toString(cppResult          ) << "." << endl
                  << "Result(Interpreter  ):") << toString(interpreterResult  ) << "." << endl
                  << "Result(Compiled     ):") << toString(compiledResult     ) << "." << endl
                  << "Difference(intp-C++ ):") << toString(relDiff            ) << "." << endl;
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

  };
}
