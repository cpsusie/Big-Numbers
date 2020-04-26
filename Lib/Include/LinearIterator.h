#pragma once

#include <Iterator.h>
#include <NumberInterval.h>

class LinearIterator : public Iterator<double> {
public:
  LinearIterator(const DoubleInterval &interval, UINT steps);
  LinearIterator(double from, double to, UINT steps);
  const DoubleInterval &getinterval() const;
  UINT getSteps() const;
};
