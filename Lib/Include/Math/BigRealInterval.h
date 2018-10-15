#pragma once

#include <NumberInterval.h>
#include "BigReal.h"

class BigRealInterval : public NumberInterval<BigReal> {
public:
  BigRealInterval(DigitPool *digitPool=NULL)
    : NumberInterval(BigReal(0,digitPool), BigReal(0, digitPool))
  {
  }
  BigRealInterval(const BigReal &from, const BigReal &to, DigitPool *digitPool=NULL)
    : NumberInterval(BigReal(from,digitPool?digitPool:from.getDigitPool())
                    ,BigReal(to  ,digitPool?digitPool:from.getDigitPool()))
  {
  }
  BigRealInterval(const BigRealInterval &src, DigitPool *digitPool=NULL)
    : NumberInterval(BigReal(src.getFrom(),digitPool?digitPool:src.getDigitPool())
                    ,BigReal(src.getTo()  ,digitPool?digitPool:src.getDigitPool()))
  {
  }
  inline DigitPool *getDigitPool() const {
    return getFrom().getDigitPool();
  }

  template<class T> BigRealInterval &operator=(const T &src) {
    DigitPool *dp = getDigitPool();
    setFrom(BigReal(src.getFrom(),dp));
    setTo(  BigReal(src.getTo()  ,dp));
    return *this;
  }
  // Find needed decimal digitCount to use, so that add/sub of from and to will be done with at
  // least precision significant decimal digits.
  inline size_t getNeededDecimalDigits(size_t digits) const {
    const BigReal length = getLength(); // don't care about sign. need only expo10
    const BigReal &m     = (compareAbs(getFrom(),getTo()) > 0) ? getFrom() : getTo();
    return BigReal::getExpo10(m) - BigReal::getExpo10(length) + digits;
  }
};
