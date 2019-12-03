#pragma once

#include <NumberInterval.h>
#include "BigReal.h"

// Find needed decimal digitCount to use, so that add/sub of from and to will be done with at
// least precision significant decimal digits.
template<class T> size_t neededDecimalDigitsTemplate(const NumberInterval<T> *i, size_t digits) {
  const BigReal  len = i->getLength(); // don't care about sign. need only expo10
  const BigReal &m   = (BigReal::compareAbs(i->getFrom(), i->getTo()) > 0) ? i->getFrom() : i->getTo();
  return BigReal::getExpo10(m) - BigReal::getExpo10(len) + digits;
}

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

  inline BigReal getLength(DigitPool *digitPool = NULL) const {
    _SELECTDIGITPOOL(getFrom());
    return dif(getTo(),getFrom(),pool->_0(), pool);
  }

  template<class T> BigRealInterval &operator=(const T &src) {
    DigitPool *dp = getDigitPool();
    setFrom(BigReal(src.getFrom(),dp));
    setTo(  BigReal(src.getTo()  ,dp));
    return *this;
  }
  inline size_t getNeededDecimalDigits(size_t digits) const {
    return neededDecimalDigitsTemplate(this, digits);
  }
};

class ConstBigRealInterval : public NumberInterval<ConstBigReal> {
private:
  ConstBigRealInterval &setFrom(const ConstBigReal &v);    // not defined
  ConstBigRealInterval &setTo(  const ConstBigReal &v);    // not defined
  ConstBigRealInterval &operator+=(const NumberInterval<ConstBigReal> &rhs); // union operator..not defined
  ConstBigRealInterval &operator=(ConstBigRealInterval &); // not defined
public:
  ConstBigRealInterval(const BigReal &from, const BigReal &to) : NumberInterval(ConstBigReal(from), ConstBigReal(to)) {
  }
  ConstBigRealInterval(const ConstBigRealInterval &src) : NumberInterval(ConstBigReal(src.getFrom()), ConstBigReal(src.getTo())) {
  }
  ConstBigRealInterval(const BigRealInterval &src) : NumberInterval(ConstBigReal(src.getFrom()), ConstBigReal(src.getTo())) {
  }
  inline DigitPool *getDigitPool() const {
    return getFrom().getDigitPool();
  }

  inline BigReal getLength(DigitPool *digitPool = NULL) const {
    _SELECTDIGITPOOL(getFrom());
    return dif(getTo(), getFrom(), pool->_0(), pool);
  }
  // Find needed decimal digitCount to use, so that add/sub of from and to will be done with at
  // least precision significant decimal digits.
  inline size_t getNeededDecimalDigits(size_t digits) const {
    return neededDecimalDigitsTemplate(this, digits);
  }
};
