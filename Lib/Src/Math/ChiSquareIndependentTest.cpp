#include "pch.h"
#include <Math/Statistic.h>

// Chisquare Independency test. return p-value for null-hypothesis: ross in m are indenpent (of row-number)
// p close to 1 is good for the3 null-hypothesis, close to 0 is bad. (close to 0 usually < 0.05 (significance level)
// Assume v.size == expected.size, and both are >= 2. and all elements of expected > 0
double chiSquareIndependencyTest(const Matrix &m) {
  const MatrixDimension &dim = m.getDimension();
  const size_t           rows = dim.rowCount;
  const size_t           cols = dim.columnCount;
  if ((rows < 2) || (cols < 2)) {
    throwInvalidArgumentException(__TFUNCTION__, _T("m:%s"), m.getDimensionString().cstr());
  }
  CompactDoubleArray rowSum(rows), colSum(cols);
  double totalSum = 0;
  for(size_t r = 0; r < rows; r++) {
    double sum = 0;
    for(size_t c = 0; c < cols; c++) {
      sum += getDouble(m(r, c));
    }
    rowSum.add(sum);
    totalSum += sum;
  }
  for(size_t c = 0; c < cols; c++) {
    double sum = 0;
    for (size_t r = 0; r < rows; r++) {
      sum += getDouble(m(r, c));
    }
    colSum.add(sum);
  }
  double q = 0;
  for(size_t r = 0; r < rows; r++) {
    for(size_t c = 0; c < cols; c++) {
      const double expected = (rowSum[r] * colSum[c]) / totalSum;
      q += sqr(getDouble(m(r, c)) - expected) / expected;
    }
  }
  return 1.0 - getDouble(chiSquaredDistribution((double)(rows - 1)*(cols - 1), q));
}
