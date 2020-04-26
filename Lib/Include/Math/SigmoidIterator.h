#pragma once

#include <Iterator.h>

class SigmoidIterator : public Iterator<double> {
public:
  SigmoidIterator(double from, double to, UINT steps);
  UINT getSteps() const;
};
