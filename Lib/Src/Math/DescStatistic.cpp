#include "pch.h"
#include <limits>
#include <HashMap.h>
#include <Math/Statistic.h>

DescriptiveStatistics::DescriptiveStatistics(const CompactRealArray &data) {
  m_average           = 0;
  m_standardError     = 0;
  m_median            = 0;
  m_mode              = 0;
  m_standardDeviation = 0;
  m_variance          = 0;
  m_kurtosis          = 0;
  m_skewness          = 0;
  m_range             = 0;
  m_minimum           = 0;
  m_maximum           = 0;
  m_sum               = 0;
  m_count             = (int)data.size();

  if(m_count == 0)
    return;

  m_minimum = m_maximum = data[0];
  for(int i = 0; i < m_count; i++) {
    const Real &x = data[i];
    m_sum  += x;
    if(x < m_minimum) m_minimum = x;
    if(x > m_maximum) m_maximum = x;
  }
  m_mode              = findMode(data);
  m_range             = m_maximum - m_minimum;
  m_average           = m_sum/m_count;
  m_median            = findMedian(data);
  m_standardDeviation = findStandardDeviation(data);
  m_standardError     = findStandardError(data);
  m_variance          = sqr(m_standardDeviation);
  m_kurtosis          = findKurtosis(data);
  m_skewness          = findSkewness(data);
}

Real DescriptiveStatistics::findMomentAboutMean(const CompactRealArray &data, int r) {
  Real sum = 0;
  for(int i = 0; i < m_count; i++) {
    sum += mypow(data[i] - m_average, r);
  }
  return sum / m_count;
}

Real DescriptiveStatistics::findMoment(const CompactRealArray &data, int r) {
  Real sum = 0;
  for(int i = 0; i < m_count; i++) {
    sum += mypow(data[i],r);
  }
  return sum / m_count;
}

Real DescriptiveStatistics::findStandardDeviation(const CompactRealArray &data) {
  if(m_count < 2) {
    return 0;
  }
  return sqrt(findMomentAboutMean(data,2) * m_count / (m_count-1));
}

Real DescriptiveStatistics::findStandardError(const CompactRealArray &data) {
  if(m_count < 2) {
    return 0;
  }
  return m_standardDeviation / sqrt(m_count);
}

Real DescriptiveStatistics::findSkewness(const CompactRealArray &data) {
  if(m_count < 3 || m_standardDeviation == 0) {
    return std::numeric_limits<Real>::quiet_NaN();
  }

  Real factor = sqr((double)m_count) / ((m_count-1) * (m_count-2));
  Real m3     = findMomentAboutMean(data,3) / mypow(m_standardDeviation,3);
  return factor * m3;
}

Real DescriptiveStatistics::findKurtosis(const CompactRealArray &data) {
  if(m_count < 4 || m_standardDeviation == 0) {
    return std::numeric_limits<Real>::quiet_NaN();
  }

  Real factor = sqr((double)m_count) * (m_count+1) / ((m_count-1) * (m_count-2) * (m_count-3));
  Real m4     = findMomentAboutMean(data,4) / mypow(m_standardDeviation,4);
  Real k      = 3 * sqr((double)m_count-1) / ((m_count-2) * (m_count-3));
  return factor * m4 - k;
}

static int realCmp(const Real &d1, const Real &d2) {
  return sign(d1-d2);
}

Real DescriptiveStatistics::findMedian(const CompactRealArray &data) {
  CompactRealArray copy(data);
  copy.sort(realCmp);
  return copy[copy.size()/2];
}

// find the element that occurs most often in the dataset
Real DescriptiveStatistics::findMode(const CompactRealArray &data) {
  CompactRealArray copy(data);
  copy.sort(realCmp);
  int currentMax = 0;
  Real last   = data[0];
  Real result = last;
  int count = 1;
  for(int i = 1; i < m_count; i++) {
    const Real &x = copy[i];
    if(x == last) {
      count++;
    } else {
      if(count > currentMax) {
        currentMax = count;
        result     = last;
      }
      count = 1;
      last  = x;
    }
  }
  if(count > currentMax) {
    result = last;
  }
  return result;
}

String DescriptiveStatistics::toString(int prec) {
  const String formatStr =
  "Average          :%.*lf\n"
  "StandardError    :%.*lf\n"
  "Median           :%.*lf\n"
  "Mode             :%.*lf\n"
  "StandardDeviation:%.*lf\n"
  "Variance         :%.*lf\n"
  "Kurtosis         :%.*lf\n"
  "Skewness         :%.*lf\n"
  "Range            :%.*lf\n"
  "Minimum          :%.*lf\n"
  "Maximum          :%.*lf\n"
  "Sum              :%.*lf\n"
  "Count            :%ld\n";

  return format(formatStr.cstr()
               ,prec, (double)m_average
               ,prec, (double)m_standardError
               ,prec, (double)m_median
               ,prec, (double)m_mode
               ,prec, (double)m_standardDeviation
               ,prec, (double)m_variance
               ,prec, (double)m_kurtosis
               ,prec, (double)m_skewness
               ,prec, (double)m_range
               ,prec, (double)m_minimum
               ,prec, (double)m_maximum
               ,prec, (double)m_sum
               ,m_count
              );
}
