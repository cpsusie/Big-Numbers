#pragma once

#include "Real.h"

class DescriptiveStatistics {
private:
  Real findMoment(           const Array<Real> &data, int r);
  Real findMomentAboutMean(  const Array<Real> &data, int r);
  Real findMode(             const Array<Real> &data);
  Real findStandardDeviation(const Array<Real> &data);
  Real findStandardError(    const Array<Real> &data);
  Real findMedian(           const Array<Real> &data);
  Real findSkewness(         const Array<Real> &data);
  Real findKurtosis(         const Array<Real> &data);
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
  DescriptiveStatistics(const Array<Real> &data);
  String toString(int prec = 6);
};

