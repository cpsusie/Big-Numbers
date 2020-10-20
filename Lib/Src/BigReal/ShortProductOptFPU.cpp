#include "pch.h"

#define ASM_OPTIMIZED

#if(SP_OPT_METHOD == SP_OPT_BY_FPU)

#if defined(IS64BIT)
#error SP_OPT_BY_FPU cannot be used in x64-mode
#endif

#if defined(_DEBUG)
BigReal &BigReal::shortProductNoZeroCheckDebug(const BigReal &x, const BigReal &y, UINT loopCount) { // return *this
#else
BigReal &BigReal::shortProductNoZeroCheck(     const BigReal &x, const BigReal &y, UINT loopCount) { // return *this
#endif
  if(!m_digitPool.continueCalculation()) throwBigRealException(_T("Operation was cancelled"));
  assert(isNormalProduct(x, y) && (loopCount > 0));
  m_low = (m_expo = x.m_expo + y.m_expo) + 1;
  clearDigits();

  unsigned short ctrlWord;
  __asm {
    fnstcw ctrlWord
    mov    ax, ctrlWord
    or     ax, 0x300
    mov    ctrlWord, ax
    fldcw  ctrlWord
  }
  int loopCounter = loopCount;
  for(Digit *xk = x.m_first, *yk = y.m_first;;) {

#if !defined(ASM_OPTIMIZED)
    Double80 tmp(0);
    for(const Digit *xp = xk, *yp = yk; xp && yp; xp = xp->next, yp = yp->prev) {
//      _tprintf(_T("    multiply %2d * %2d = %d\n"),xp->n,yp->n,xp->n*yp->n);
      tmp += (unsigned __int64)xp->n * yp->n;
    }
    appendZero();
    if(!tmp.isZero()) {
      addSubProduct(getUint64(tmp));
    }

#else // ASM_OPTIMIZED
    bool resultIsZero = false;
    __asm {
      mov         ecx, dword ptr [xk]     // xp = xk
      mov         edx, dword ptr [yk]     // yp = yk
      fldz                                // st(0) = 0
InnerLoop:                                // do { // we know that the first time both xp and yp are not nullptr.
      fild        dword ptr [ecx]         //   st(1) =  xp->n
      fimul       dword ptr [edx]         //   st(1) *= yp->n
      fadd                                //   st(0) += st(1)
      mov         edx, dword ptr [edx+8]  //   yp = yp->prev
      or          edx, edx                //
      je          ExitLoop                //   if(yp == nullptr) break
      mov         ecx, dword ptr [ecx+4]  //   xp = xp->next
      or          ecx, ecx                //
      jne         InnerLoop               // } while(xp)
ExitLoop:
      fldz
      fcomip      st, st(1)
      jne         End
      fstp        st(0)                   // pop st(0). No addition necessary
      mov         resultIsZero, 1
End:
    }
    appendZero();
    if(!resultIsZero) {
      addFPUReg0();
    }
#endif // ASM_OPTIMIZED

    if(--loopCounter <= 0) break;
    if(yk->next) {
      yk = yk->next;
    } else if(!(xk = xk->next)) {
      break; // we are done
    }
  }
  m_low -= loopCount - loopCounter;
  return setSignByProductRule(x,y).trimZeroes();
}

#if defined(ASM_OPTIMIZED)

void BigReal::addFPUReg0() {
  BR2DigitType n;
  BRDigitType  carry = 0;
  _asm {
    fistp n;
  }

  for(Digit *tp = m_last; n || carry;) {
    if(tp) {
      carry += (BRDigitType)(tp->n + n % BIGREALBASE);
      tp->n = carry % BIGREALBASE;
      tp = tp->prev;
    } else {
      carry += (BRDigitType)n % BIGREALBASE;
      insertDigit(carry);
      m_expo++;
    }
    carry /= BIGREALBASE;
    n     /= BIGREALBASE;
  }
// dont call trimZeroes() !!;
}

#else // ASM_OPTIMIZED

void BigReal::addSubProduct(BR2DigitType n) {
  BRDigitType carry = 0;
  for(Digit *tp = m_last; n || carry;) {
    if(tp) {
      carry += (BRDigitType)(tp->n + n % BIGREALBASE);
      tp->n = carry % BIGREALBASE;
      tp = tp->prev;
    } else {
      carry += (BRDigitType)n % BIGREALBASE;
      insertDigit(carry);
      m_expo++;
    }
    carry /= BIGREALBASE;
    n     /= BIGREALBASE;
  }
// dont call trimZeroes() !!;
}

#endif // ASM_OPTIMIZED

#define SPLIT_LENGTH 200

#define MAX_DIGITVALUE (BIGREALBASE-1)

size_t BigReal::s_splitLength = SPLIT_LENGTH; // Value found by experiments with measureSplitFactor in testnumber.cpp

UINT BigReal::getMaxSplitLength() { // static
  return _UI64_MAX / ((unsigned __int64)MAX_DIGITVALUE * MAX_DIGITVALUE);
}

#endif // SP_OPT_METHOD == SP_OPT_BY_FPU
