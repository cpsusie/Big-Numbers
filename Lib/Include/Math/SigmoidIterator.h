#pragma once

#include <Iterator.h>
#include <NumberInterval.h>

class SigmoidIterator : public Iterator<double> {
public:
  SigmoidIterator(const DoubleInterval &interval, UINT steps);
  SigmoidIterator(double from, double to, UINT steps);
  const DoubleInterval &getinterval() const;
  UINT getSteps() const;
};
