#include "pch.h"

#if(SP_OPT_METHOD == SP_OPT_BY_REG64)

#ifdef IS32BIT
#error SP_OPT_BY_REG64 cannot be used in x86-mode
#endif

#if(BIGREALBASE != 1000000000000000000)
#error For SP_OPT_METHOD == SP_OPT_BY_REG64 BIGREALBASE must be 1e18
#endif


// used to prevent 5 paramters.
class SubProductSum {
public:
  Digit        *m_cd;     // pointer to current digit
  BR2DigitType  m_bigSum; // result stored here if too big for simple division.
  SubProductSum(Digit *cd) : m_cd(cd) {
  }
};

#define ASM_OPTIMIZED
#ifdef ASM_OPTIMIZED

// return 1 if sum of products has been added. 0 if we need to do it in C-code. if so, the sum is returned in bigsum
extern "C" char BigRealMultiplyColumn(const Digit *yk, const Digit *xk, SubProductSum *sps);
extern "C" char BigRealSquareColumn(  const Digit *yp, const Digit *xp, SubProductSum *sps, int sumLength);

#else
static char BigRealMultiplyColumn(const Digit *yp, const Digit *xp, SubProductSum *sps) {
  for(sps->m_bigSum = 0; xp && yp; xp = xp->next, yp = yp->prev) {
    sps->m_bigSum += (BR2DigitType)xp->n * yp->n;
  }
  return 0;
}
char BigRealSquareColumn(const Digit *yp, const Digit *xp, SubProductSum *sps, int sumLength) {
  sps->m_bigSum = 0;
  if(isOdd(sumLength)) {
    for(;xp != yp; xp = xp->next, yp = yp->prev) {
      sps->m_bigSum += (BR2DigitType)xp->n * yp->n;
    }
    if(sumLength>1) sps->m_bigSum <<= 1;
    sps->m_bigSum += (BR2DigitType)yp->n * yp->n;
  } else { // sumLength even
    for(sumLength /= 2;; xp = xp->next, yp = yp->prev) {
      sps->m_bigSum += (BR2DigitType)xp->n * yp->n;
      if(!--sumLength) break;
    }
    sps->m_bigSum <<= 1;
  }
  return 0;
}
#endif

DECLARE_CALLCOUNTER(shortProdCallTotal  );
DECLARE_CALLCOUNTER(shortProdLoopTotal  );
DECLARE_CALLCOUNTER(shortProdSumTooBig  );
DECLARE_CALLCOUNTER(shortSquareCallTotal);
DECLARE_CALLCOUNTER(shortSquareLoopTotal);
DECLARE_CALLCOUNTER(shortSquareSumTooBig);

// Return *this. Assume x._isnormal() && y._isnormal() && loopCount > 0
BigReal &BigReal::shortProductNoZeroCheck(const BigReal &x, const BigReal &y, UINT loopCount) {
  if(!m_digitPool.continueCalculation()) throwBigRealException(_T("Operation was cancelled"));
  assert(isNormalProduct(x, y) && (loopCount > 0));
  SubProductSum   sps(clearDigits1());
  int             digitsAdded;
  m_expo = m_low = x.m_expo + y.m_expo;

  if(&x == &y) {
    COUNTCALL(shortSquareCallTotal);
    digitsAdded   = 0;
    int sumLength = 1;
    for(const Digit *xk = x.m_first, *yk = x.m_first;;) { // loopcondition at the end
      sps.m_cd = fastAppendDigit(sps.m_cd);
      COUNTCALL(shortSquareLoopTotal);
      if(!BigRealSquareColumn(yk, xk, &sps, sumLength)) {
        COUNTCALL(shorSquareSumTooBig);
        BRDigitType carry = 0;
        Digit      *d     = sps.m_cd;
        d->n = sps.m_bigSum % s_BIGREALBASEBR2;
        sps.m_bigSum /= s_BIGREALBASEBR2;
        do {
          d = d->prev;
          carry  += d->n + (BRDigitType)(sps.m_bigSum % s_BIGREALBASEBR2);
          d->n   = carry % BIGREALBASE;
          carry  /= BIGREALBASE;
          sps.m_bigSum /= s_BIGREALBASEBR2;
        } while(sps.m_bigSum || carry);
        // dont call trimZeroes() !!;
      }
      if(++digitsAdded == loopCount) break;
      if(yk->next) {
        yk = yk->next;
        sumLength++;
      } else if(xk = xk->next) {
        sumLength--;
      } else {
        break; // we are done
      }
    }
  } else {
    COUNTCALL(shortProdCallTotal);
    int loopCounter = loopCount;
    for(const Digit *xk = x.m_first, *yk = y.m_first;;) { // loopcondition at the end
      sps.m_cd = fastAppendDigit(sps.m_cd);
      COUNTCALL(shortProdLoopTotal);
      if(!BigRealMultiplyColumn(yk, xk, &sps)) {
        COUNTCALL(shortProdSumTooBig);
        BRDigitType carry = 0;
        Digit      *d     = sps.m_cd;
        d->n = sps.m_bigSum % s_BIGREALBASEBR2;
        sps.m_bigSum /= s_BIGREALBASEBR2;
        do {
          d = d->prev;
          carry  += d->n + (BRDigitType)(sps.m_bigSum % s_BIGREALBASEBR2);
          d->n   = carry % BIGREALBASE;
          carry  /= BIGREALBASE;
          sps.m_bigSum /= s_BIGREALBASEBR2;
        } while(sps.m_bigSum || carry);
        // dont call trimZeroes() !!;
      }
      if(--loopCounter <= 0) break;
      if(yk->next) {
        yk = yk->next;
      } else if(!(xk = xk->next)) {
        break; // we are done
      }
    }
    digitsAdded = loopCount - loopCounter;
  }
  if(sps.m_cd->n == 0) { // Fixup both ends of digit chain, to reestablish invariant (See comments in assertIsValid)
    m_last = sps.m_cd;
    for(digitsAdded--, sps.m_cd = m_last->prev; sps.m_cd->n == 0; sps.m_cd = sps.m_cd->prev, digitsAdded--);
    deleteDigits(sps.m_cd->next, m_last);
  }
  m_low -= digitsAdded - 1;
  (m_last = sps.m_cd)->next = NULL;
  if(m_first->n == 0) {
    sps.m_cd = m_first;
    (m_first = m_first->next)->prev = NULL;
    deleteDigits(sps.m_cd,sps.m_cd);
  } else {
    m_first->prev = NULL;
    m_expo++;
  }
  return setSignByProductRule(x, y);
}

#define SPLIT_LENGTH 240

size_t BigReal::s_splitLength = SPLIT_LENGTH; // Value found by experiments with measureSplitFactor in TestBigReal.cpp

#define MAX_DIGITVALUE (BIGREALBASE-1)

UINT BigReal::getMaxSplitLength() { // static
  return 340; // floor(_UI128_MAX / (MAX_DIGITVALUE * MAX_DIGITVALUE))
}

#endif // SP_OPT_METHOD == SP_OPT_BY_REG32
