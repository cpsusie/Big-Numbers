#include "stdafx.h"
#include "CppUnitTest.h"
#include <Math.h>
#include <Math/MathLib.h>
#include <Math/Simplex.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#ifdef verify
#undef verify
#endif
#define verify(expr) Assert::IsTrue(expr, _T(#expr))

namespace TestSimplex {		

  void OUTPUT(const TCHAR *format, ...) {
    va_list argptr;
    va_start(argptr, format);
    const String msg = vformat(format, argptr);
    va_end(argptr);
    Logger::WriteMessage(msg.cstr());
  }

  class Tracer : public SimplexTracer {
  public:
    void handleData(const SimplexTraceElement &data);
  };

  void Tracer::handleData(const SimplexTraceElement &data) {
    OUTPUT(_T("%s"), data.m_msg.cstr());
    //  data.m_Tableau.print();
    //  pause();
  }

  static Tracer tracer;

	TEST_CLASS(TestSimplex)	{
    public:

    TEST_METHOD(SimplexTest1) {
      const int xCount = 5;
      const int constraintCount = 2;

      TableauConstraint  con1(" 5 -4 13 -2 1 = 20");
      TableauConstraint  con2(" 1 -1  5 -1 1 =  8");
      TableauCostFactors costFactors("1  6 -7 1 5");

      Tableau tab(xCount, constraintCount, &tracer, 0 /*TRACE_ALL*/);
      tab.setConstraint(1, con1);
      tab.setConstraint(2, con2);
      tab.setCostFactors(costFactors);

      tab.twoPhaseSimplex();
      SimplexSolution solution = tab.getSolution();

      //  printf("Solution:\n%s",solution.toString().cstr());

      const CompactArray<BasisVariable> &bv = solution.getVariables();
      verify(bv.size() == 2);
      verifyEqualsInt(2, bv[0].m_index);
      verifyAlmostEquals((double)4 / 7, bv[0].m_value, 1e-14);
      verifyEqualsInt(3, bv[1].m_index);
      verifyAlmostEquals((double)12 / 7, bv[1].m_value, 1e-14);
    }

    TEST_METHOD(SimplexTest2) {
      const int xCount = 26;
      const int constraintCount = 17;

      Array<TableauConstraint> constraints;

      constraints.add(TableauConstraint("2   0 437   2   1  15  20   3 147   0   4   0  95   1   5   3   1 -1  0  0  0  0  0  0  0  0  = 0"));
      constraints.add(TableauConstraint("2   1  52   2   0   1   1   0  17   0   1   0  60   1   1   0   0  0 -1  0  0  0  0  0  0  0  = 0"));
      constraints.add(TableauConstraint("2   2 290   1   1   0   1   5  27   0   5   0  60  11  43   0   0  0  0 -1  0  0  0  0  0  0  = 0"));
      constraints.add(TableauConstraint("2   1  97   2   2   0   1   0  22   0   2   0  50  51 170   0   0  0  0  0 -1  0  0  0  0  0  = 0"));
      constraints.add(TableauConstraint("1   1 500   1   1   0   0   0   7   1  49   0  70  10   6   0   1  0  0  0  0 -1  0  0  0  0  = 0"));
      constraints.add(TableauConstraint("2   1 247   1   2   3   1   0  19   0   2   0  70  51  43   0   1  0  0  0  0  0 -1  0  0  0  = 0"));
      constraints.add(TableauConstraint("2   1  34   1   2   0   0   0  14   1   1   0  40   1   1   0   0  0  0  0  0  0  0 -1  0  0  = 0"));
      constraints.add(TableauConstraint("2   0  84   1   0   2   1   1  15   0   7   0  90   1   1   1   0  0  0  0  0  0  0  0 -1  0  = 0"));
      constraints.add(TableauConstraint("1   1  82   1   2   4   0   4  20   0   5   1  95   1   5   0   0  0  0  0  0  0  0  0  0 -1  = 0"));
      constraints.add(TableauConstraint("0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  1 -1  0  0  0  0  0  0  0  > 1"));
      constraints.add(TableauConstraint("0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  0  1 -1  0  0  0  0  0  0  > 1"));
      constraints.add(TableauConstraint("0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  0  0  1 -1  0  0  0  0  0  > 1"));
      constraints.add(TableauConstraint("0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  0  0  0  1 -1  0  0  0  0  > 1"));
      constraints.add(TableauConstraint("0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  0  0  0  0  1 -1  0  0  0  > 1"));
      constraints.add(TableauConstraint("0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  0  0  0  0  0  1 -1  0  0  > 1"));
      constraints.add(TableauConstraint("0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  0  0  0  0  0  0  1 -1  0  > 1"));
      constraints.add(TableauConstraint("0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  0  0  0  0  0  0  0  1 -1  > 1"));

      verify(constraintCount == constraints.size());

      TableauCostFactors costFactors("1   2   1   2   3   4   2   1   2   1   3   1   2   4   2   3   1 10 10 10 10 10 10 10 10 10");
      verify(xCount == costFactors.size());

      Tableau tab(xCount, constraintCount, &tracer, 0 /*TRACE_ALL*/);
      for (size_t i = 0; i < constraints.size(); i++) {
        const TableauConstraint &con = constraints[i];
        verify(xCount == con.getXCount());
        tab.setConstraint(i + 1, con);
      }
      tab.setCostFactors(costFactors);

      tab.twoPhaseSimplex();
      SimplexSolution solution = tab.getSolution();

      const double expectedTotalCost = 3827.477272741715;
      //  printf("Totalcost:%s. diff:%s\n", ::toString(solution.getTotalCost(),20).cstr(), toString(fabs(solution.getTotalCost() - expectedTotalCost)).cstr());
      verify(fabs(solution.getTotalCost() - expectedTotalCost) < 2e-8);

      //  printf("TotalCost:%20.15lg\n", getDouble(solution.getTotalCost()));
      //  printf("Solution:\n%s",tab.getSolution().toString().cstr());
    }

  };
}
