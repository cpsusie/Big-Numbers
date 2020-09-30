#include "stdafx.h"
#include <CompactArray.h>
#include <Math/Matrix.h>
#include <Math/Statistic.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestStatistic {		

#include <UnitTestTraits.h>

static double testData[] = {
 2    , 2    , 2.5  , 1    , 1.75
,2.25 , 2    , 2    , 2.25 , 2.75
,1.25 , 1.5  , 0.5  , 0.75 , 1.25
,2.5  , 2.5  , 2    , 2    , 2.5
,1    , 1.75 , 2.25 , 2    , 2
,2.25 , 2.75 , 1.25 , 2    , 2
,2.5  , 1    , 1.75 , 2.25 , 2
,2    , 2.25 , 2.75 , 1.25 , 1.5
,0.5
};

#define checkValue(field,expected) verify(fabs((field - expected) / expected) <= 1e-10)

  TEST_CLASS(TestStatistic) {
  public:

    TEST_METHOD(DescriptiveStatistic) {
      CompactRealArray data;

      for(int i = 0; i < ARRAYSIZE(testData); i++) {
        data.add(testData[i]);
      }
      DescriptiveStatistics result(data);

    //  printf("%s\n",result.toString().cstr());

      checkValue(result.m_average          , 1.85975609756     );
      checkValue(result.m_standardError    , 9.40488247647e-002);
      checkValue(result.m_median           , 2                 );
      checkValue(result.m_mode             , 2                 );
      checkValue(result.m_standardDeviation, 6.02206309353e-001);
      checkValue(result.m_variance         , 3.62652439024e-001);
      checkValue(result.m_kurtosis         ,-2.68517439126e-001);
      checkValue(result.m_skewness         ,-6.60308638129e-001);
      checkValue(result.m_range            , 2.25              );
      checkValue(result.m_minimum          , 0.5               );
      checkValue(result.m_maximum          , 2.75              );
      checkValue(result.m_sum              ,76.25              );
      checkValue(result.m_count            ,41                 );
    }

    TEST_METHOD(TestGodnessOfFit) {
      const double obsData[]  = { 50, 30, 32, 67, 78, 34, 1, 9, 3 };
      const double freqData[] = { 0.16, 0.10, 0.12, 0.22, 0.24, 0.11, 0.01, 0.03, 0.01 };

      CompactDoubleArray obs, freq;
      obs.insert(0, obsData, ARRAYSIZE(obsData));
      freq.insert(0, freqData, ARRAYSIZE(freqData));
      const double pValue = chiSquareGoodnessOfFitTest(obs, freq);
      verify(fabs(pValue - 0.96950428) < 1e-7);
    }

    TEST_METHOD(TestIndependency) {
      Matrix m(3, 4);
      m(0, 0) = 2; m(0, 1) = 4; m(0, 2) = 6; m(0, 3) = 8;
      m(1, 0) = 1; m(1, 1) = 3; m(1, 2) = 5; m(1, 3) = 7;
      m(2, 0) = 3; m(2, 1) = 5; m(2, 2) = 5; m(2, 3) = 6;
      const double pValue = chiSquareIndependencyTest(m);
      verify(fabs(pValue - 0.96148437) < 1e-7);
    }

  };
}
