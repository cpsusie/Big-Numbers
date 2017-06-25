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

