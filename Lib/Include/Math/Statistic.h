#pragma once

#include "Matrix.h"
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

// Chisquare Independency test. return p-value for null-hypothesis: ross in m are indenpent (of row-number)
// p close to 1 is good for the null-hypothesis, close to 0 is bad. (close to 0 usually < 0.05 (significance level)
// Assume v.size == expected.size, and both are >= 2. and all elements of expected > 0
double chiSquareIndependencyTest(const Matrix &m);

// Chisquare Goodness of Fit test. return p-value for null-hypothesis observed == expected.
// p close to 1 is good for the null-hypothesis, close to 0 is bad. (close to 0 usually < 0.05 (significance level)
// Assume observed.size == expected.size, observed.size >= 2.
// If expectedAsFrequency = true, expected is interpreted as frequencies, and sum(expected) == 1
// In this case the expected values are calculated as ex[i] = exepected[i] * sum(observed)
// else expected are used as given, and sum(expected) must equal sum(observed)
double chiSquareGoodnessOfFitTest(const CompactDoubleArray &observed, const CompactDoubleArray &expected, bool expectedAsFrequency=true);
