#pragma once

#include "Real.h"

class DescriptiveStatistics {
private:
  Real findMoment(           const CompactRealArray &data, int r);
  Real findMomentAboutMean(  const CompactRealArray &data, int r);
  Real findMode(             const CompactRealArray &data);
  Real findStandardDeviation(const CompactRealArray &data);
  Real findStandardError(    const CompactRealArray &data);
  Real findMedian(           const CompactRealArray &data);
  Real findSkewness(         const CompactRealArray &data);
  Real findKurtosis(         const CompactRealArray &data);
public:
  Real m_average;
  Real m_standardError;
  Real m_median;
  Real m_mode;
  Real m_standardDeviation;
  Real m_variance;
  Real m_kurtosis;
  Real m_skewness;
  Real m_range;
  Real m_minimum;
  Real m_maximum;
  Real m_sum;
  int  m_count;
  DescriptiveStatistics(const CompactRealArray &data);
  String toString(int prec = 6);
};

// Chisquare Goodness of Fit test. return p-value for null-hypothesis v == expected.
// p close to 1 is good for the3 null-hypothesis, close to 0 is bad. (close to 0 usualy < 0.05 (significance level)
// Assume v.size == expected.size, and both are >= 2. and all elements of expected > 0
template<class ValueArray> static Real chiSquareGoodnessOfFitTest(const ValueArray &v, const ValueArray &expected) {
  if (v.size() != expected.size()) {
    throwInvalidArgumentException(__TFUNCTION__, _T("v.size=%zu, expected.size=%zu")
      , v.size(), expected.size());
  }
  if (v.size() < 2) {
    throwInvalidArgumentException(__TFUNCTION__, _T("v.size=%zu. must be >= 2")
      , v.size());
  }
  Real q = 0;
  const size_t n = v.size();
  for (size_t i = 0; i < n; i++) {
    q += sqr(v[i] - expected[i]) / expected[i];
  }
  return 1.0 - chiSquaredDistribution((Real)n - 1, q);
}
