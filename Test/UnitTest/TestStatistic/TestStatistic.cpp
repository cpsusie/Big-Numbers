#include "stdafx.h"
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

    TEST_METHOD(Statistic) {
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
  };
}
