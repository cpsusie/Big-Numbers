#include "stdafx.h"
#include "FunctionTest.h"

static const char legalBias[] = { '<', '#', '>' };
static const int  biasCount   = ARRAYSIZE(legalBias);

void testAPCSum(TestStatistic &stat) {
  DigitPool          *pool           = stat.getDigitPool();
  const unsigned int  totalTestCount = (unsigned int)(((double)biasCount * sqr(MAXSCALE - MINSCALE) * (MAXLENGTH-1) * 10) / sqr(SCALESTEP));
  const BigReal       delta          = e(pool->get1(), -APC_DIGITS, pool);
  size_t              maxLength      = 0;
  size_t              sumLength      = 0;

  stat.setTotalTestCount(totalTestCount);

  for(int biasIndex = 0; biasIndex < biasCount; biasIndex++) {
    const char bias = legalBias[biasIndex];
    for(int length = 1; length < MAXLENGTH; length++) {
      for(int xScale = MINSCALE; xScale <= MAXSCALE; xScale += SCALESTEP) {
        for(int yScale = MINSCALE; yScale <= MAXSCALE; yScale += SCALESTEP) {
          for(int i = 0; i < 10; i++) {
            const BigReal x = stat.getRandom(length, xScale);
            const BigReal y = stat.getRandom(length, yScale);

            if(stat.isTimeToPrint()) {
              stat.printLoopMessage(_T("bias:'%c', length:%4d, xscale:%4d, yscale:%4d")
                                   ,bias,length,xScale,yScale);
            }

            const BigReal    tolerance   = delta * dmax(x, y);
            const BigReal    exactResult = x + y;
            const BigReal    APCResult   = BigReal::apcSum(bias, x, y, pool);
            const BigReal    error       = fabs(APCResult - exactResult);
            const int        d           = compare(APCResult, exactResult);
            bool             ok;
            const size_t     len         = APCResult.getLength();
            sumLength += len;
            if(len > maxLength) maxLength = len;
            switch(bias) {
            case '<': ok = error <= tolerance && d <= 0; break;
            case '#': ok = error <= tolerance;           break;
            case '>': ok = error <= tolerance && d >= 0; break;
            default :
              stat.out() << _T("Invalid bias:'") << bias << _T("'") << NEWLINE;
              throwException(_T("Invalid bias: =%c"), bias);
            }
            if(!ok) {
              ERRLOG << _T("Error in APCsum")                                                           << NEWLINE
                     << _T("x:")                                      << FullFormatBigReal(x)           << NEWLINE
                     << _T("y:")                                      << FullFormatBigReal(y)           << NEWLINE
                     << _T("ExactResult = x + y:")                    << FullFormatBigReal(exactResult) << NEWLINE
                     << _T("Bias:'") << bias << _T("'")                                                 << NEWLINE
                     << _T("Result = APCsum(bias,x,y):")              << FullFormatBigReal(APCResult)   << NEWLINE
                     << _T("Difference=fabs(APCResult-exactResult):") << FullFormatBigReal(error)       << NEWLINE
                     << _T("Tolerance:")                              << tolerance                      << NEWLINE;

              throwException(_T("Error in APCSum"));
            } else {
              stat.update(error, tolerance);
            }
          }
        }
      }
    }
  }
  stat.addEndMessage(_T("avgLen:%5.3lf maxLen:%3d"), (double)sumLength / stat.getTestCount(), maxLength);
}

static const ConstBigReal minTolerance = BIGREAL_1 - e(BIGREAL_1, -APC_DIGITS);
static const ConstBigReal maxTolerance = BIGREAL_1 + e(BIGREAL_1, -APC_DIGITS);

static bool checkBiasedExpr(const BigReal &APCresult, const BigReal &x0, const char bias, BigReal &quotient) {
  if(x0.isZero()) {
    return APCresult.isZero();
  }

  switch(bias) {
  case '>':
    quotient = fabs(rQuot(APCresult, x0, 15));
    return (BIGREAL_1 <= quotient) && (quotient <= maxTolerance);
  case '#':
    quotient = fabs(rQuot(APCresult, x0, 15));
    return (minTolerance <= quotient) && (quotient <= maxTolerance);
    break;
  case '<':
    quotient = fabs(rQuot(x0, APCresult, 15));
    return (BIGREAL_1 <= quotient) && (quotient <= maxTolerance);
  default:
    tcout << _T("Invalid bias:'") << bias << _T("'") << NEWLINE;
    throwException(_T("Invalid bias: =%c"), bias);
    return false;
  }
}

void testAPCProd(TestStatistic &stat) {
  DigitPool          *pool           = stat.getDigitPool();
  const unsigned int  totalTestCount = (unsigned int)(((double)biasCount * sqr(MAXSCALE - MINSCALE) * (MAXLENGTH-1) * 10) / sqr(SCALESTEP));
  size_t              maxLength      = 0;
  size_t              sumLength      = 0;

  stat.setTotalTestCount(totalTestCount);

  for(int biasIndex = 0; biasIndex < biasCount; biasIndex++) {
    const char bias = legalBias[biasIndex];
    for(int length = 1; length < MAXLENGTH; length++) {
      for(int xScale = MINSCALE; xScale <= MAXSCALE; xScale += SCALESTEP) {
        for(int yScale = MINSCALE; yScale <= MAXSCALE; yScale += SCALESTEP ) {
          for(int i = 0; i < 10; i++) {
            const BigReal x = stat.getRandom(length, xScale);
            const BigReal y = stat.getRandom(length, yScale);

            if(stat.isTimeToPrint()) {
              stat.printLoopMessage(_T("bias:'%c', length:%4d, xscale:%4d, yscale:%4d")
                                   ,bias,length,xScale,yScale);
            }

            const BigReal exactResult = x * y;
            const BigReal APCResult   = BigReal::apcProd(bias, x, y,pool);
            const size_t  len         = APCResult.getLength();
            sumLength += len;
            if(len > maxLength) maxLength = len;
            BigReal       quotient(pool);

            if(!checkBiasedExpr(APCResult, exactResult, bias, quotient)) {
              ERRLOG << _T("Error in APCprod")                                                 << NEWLINE
                     << _T("x:")                             << FullFormatBigReal(x)           << NEWLINE
                     << _T("y:")                             << FullFormatBigReal(y)           << NEWLINE
                     << _T("Exactresult = x * y:")           << FullFormatBigReal(exactResult) << NEWLINE
                     << _T("Bias:'") << bias << _T("'")                                        << NEWLINE
                     << _T("APCResult = APCprod(bias,x,y):") << FullFormatBigReal(APCResult)   << NEWLINE
                     << _T("Quotient:")                      << FullFormatBigReal(quotient)    << NEWLINE;
              throwException(_T("Error in APCprod"));
            } else {
              stat.update(fabs(quotient-BIGREAL_1));
            }
          }
        }
      }
    }
  }
  stat.addEndMessage(_T("avgLen:%5.3lf maxLen:%3d"), (double)sumLength / stat.getTestCount(), (int)maxLength);
}

void testAPCQuot(TestStatistic &stat) {
  DigitPool          *pool           = stat.getDigitPool();
  const unsigned int  totalTestCount = (unsigned int)(((double)biasCount * sqr(MAXSCALE - MINSCALE) * MAXLENGTH * 10) / sqr(SCALESTEP));
  size_t              maxLength      = 0;
  size_t              sumLength      = 0;

  stat.setTotalTestCount(totalTestCount);

  for(int biasIndex = 0; biasIndex < biasCount; biasIndex++) {
    const char bias = legalBias[biasIndex];
    for(int length = 1; length < MAXLENGTH; length++) {
      for(int xScale = MINSCALE; xScale < MAXSCALE; xScale += SCALESTEP) {
        for(int yScale = MINSCALE; yScale < MAXSCALE; yScale += SCALESTEP) {
          for(int i = 0; i < 10; i++) {
            const BigReal x = stat.getRandom(length, xScale);
            const BigReal y = stat.getRandom(length, yScale, false);

            if(stat.isTimeToPrint()) {
              stat.printLoopMessage(_T("bias:'%c', length:%4d, xscale:%4d, yscale:%4d")
                                   ,bias,length,xScale,yScale);
            }

            const BigReal exactResult = rQuot(x, y, 20);
            const BigReal APCResult   = BigReal::apcQuot(bias, x, y,pool);
            const size_t  len         = APCResult.getLength();
            sumLength += len;
            if(len > maxLength) maxLength = len;
            BigReal       quotient(pool);

            if(!checkBiasedExpr(APCResult, exactResult, bias, quotient)) {
              ERRLOG << _T("Error in APCquot")                                                 << NEWLINE
                     << _T("x:")                             << FullFormatBigReal(x)           << NEWLINE
                     << _T("y:")                             << FullFormatBigReal(y)           << NEWLINE
                     << _T("ExactResult = rQuot(x,y,20):")   << FullFormatBigReal(exactResult) << NEWLINE
                     << _T("Bias:'") << bias << _T("'")                                            << NEWLINE
                     << _T("APCResult = APCquot(bias,x,y):") << FullFormatBigReal(APCResult)   << NEWLINE
                     << _T("Quotient:")                      << FullFormatBigReal(quotient)    << NEWLINE;
              throwException(_T("Error in APCquot"));
            } else {
              stat.update(fabs(quotient-BIGREAL_1));
            }
          }
        }
      }
    }
  }
  stat.addEndMessage(_T("avgLen:%5.3lf maxLen:%3d"), (double)sumLength / stat.getTestCount(), (int)maxLength);
}

void testAPCPow(TestStatistic &stat) {
  DigitPool          *pool           = stat.getDigitPool();
  const BigInt        minY( -50, pool);
  const BigInt        maxY(  50, pool);
  const int           lengthStep     = 5;
  const unsigned int  totalTestCount = (unsigned int)(biasCount * ceil((double)(MAXSCALE - MINSCALE)/SCALESTEP) * ceil((double)(MAXLENGTH-4)/lengthStep) * getInt(maxY-minY));
  size_t              maxLength      = 0;
  size_t              sumLength      = 0;

  stat.setTotalTestCount(totalTestCount);

  for(int biasIndex = 0; biasIndex < biasCount; biasIndex++) {
    const char bias = legalBias[biasIndex];
    for(int length = 4; length < MAXLENGTH; length += lengthStep) {
      for(BigInt y = minY; y < maxY; y++) {
        for(int xScale = MINSCALE; xScale < MAXSCALE; xScale += SCALESTEP) {
          const BigReal x = stat.getRandom(length, xScale, !y.isNegative());

          if(stat.isTimeToPrint()) {
            stat.printLoopMessage(_T("bias:'%c', length:%4d, y:%4s, xscale:%4d")
                                 ,bias,length,y.toString().cstr(), xScale);
          }

          const BigReal exactResult = rPow(x, y, 20);
          const BigReal APCResult   = BigReal::apcPow(bias, x, y, pool);
          const size_t  len         = APCResult.getLength();
          sumLength += len;
          if(len > maxLength) maxLength = len;
          BigReal       quotient(pool);
          if(!checkBiasedExpr(APCResult, exactResult, bias, quotient)) {
            ERRLOG << _T("Error in APCpow")                                                 << NEWLINE
                   << _T("x:")                            << FullFormatBigReal(x)           << NEWLINE
                   << _T("y:")                            << FullFormatBigReal(y)           << NEWLINE
                   << _T("ExactResult = rPow(x,y,20):")   << FullFormatBigReal(exactResult) << NEWLINE
                   << _T("Bias:'") << bias << _T("'")                                       << NEWLINE
                   << _T("APCResult = APCpow(bias,x,y):") << FullFormatBigReal(APCResult)   << NEWLINE
                   << _T("Quotient:")                     << FullFormatBigReal(quotient)    << NEWLINE;
            throwException(_T("Error in APCpow"));
          } else {
            stat.update(fabs(quotient-BIGREAL_1));
          }
        }
      }
    }
  }
  stat.addEndMessage(_T("avgLen:%5.3lf maxLen:%3d"), (double)sumLength / stat.getTestCount(), (int)maxLength);
}
