#include "pch.h"
#include <Math/Statistic.h>

static double sum(const CompactDoubleArray &v) {
  double sum = 0;
  for(const double *vp = &v.first(), *ep = &v.last(); vp <= ep;) {
    sum += *(vp++);
  }
  return sum;
}

double chiSquareGoodnessOfFitTest(const CompactDoubleArray &observed, const CompactDoubleArray &expected, bool expectedAsFrequency) {
  const size_t n = observed.size();
  if(n != expected.size()) {
    throwInvalidArgumentException(__TFUNCTION__, _T("observed.size=%zu, expected.size=%zu"), n, expected.size());
  }
  if(n < 2) {
    throwInvalidArgumentException(__TFUNCTION__, _T("observed.size=%zu. must be >= 2"), n);
  }

  const double sumObs = sum(observed);
  CompactDoubleArray ex;
  if(expectedAsFrequency) {
    const double sumFreq = sum(expected);
    if(fabs(sumFreq - 1) > 1e-10) {
      throwInvalidArgumentException(__TFUNCTION__, _T("|sum(frequenceArray) - 1| > 1e-10 (=%le)."), sumFreq - 1);
    }
    for(size_t i = 0; i < n; i++) {
      ex.add(expected[i] * sumObs);
    }
  } else {
    const double sumEx = sum(expected);
    double err = fabs(sumObs - sumEx);
    if(sumObs != 0) err /= sumObs;
    if(err > 1e-10) {
      throwInvalidArgumentException(__TFUNCTION__, _T("Relative diference betwen observed (=%le) must be <= 1e-10"), err);
    }
    ex = expected;
  }
  double q = 0;
  for(size_t i = 0; i < n; i++) {
    const double &obsi = observed[i];
    const double &expi = ex[i];
    q += sqr(obsi - expi) / expi;
  }
  return 1.0 - getDouble(chiSquaredDistribution((double)n - 1, q));
}
