#include "pch.h"

#if(SP_OPT_METHOD == SP_OPT_BY_FPU2)

#ifdef IS64BIT
#error SP_OPT_BY_FPU2 cannot be used in x64-mode
#endif

#if(BIGREALBASE != 100000000)
#error For SP_OPT_METHOD == SP_OPT_BY_FPU2 BIGREALBASE must be 100000000
#endif

#ifdef _DEBUG
BigReal &BigReal::shortProductNoZeroCheckDebug(const BigReal &x, const BigReal &y, UINT loopCount) { // return *this
#else
BigReal &BigReal::shortProductNoZeroCheck(     const BigReal &x, const BigReal &y, UINT loopCount) { // return *this
#endif
  if(!m_digitPool.continueCalculation()) throwBigRealException(_T("Operation was cancelled"));
  assert(isNormalProduct(x, y) && (loopCount > 0));

  int              loopCounter = loopCount;
  Digit           *cd          = clearDigits1();
  const UINT       BASE        = BIGREALBASE;
  m_expo = m_low = x.m_expo + y.m_expo;

  unsigned short ctrlWord, saveCtrlWord;
  __asm {
    fnstcw        ctrlWord
    mov           ax, ctrlWord
    mov           saveCtrlWord, ax
    or            ax, 0xf00               // Set bit 8-11 of FPU control word, giving truncate mode and high precision. We use all 64 bits in significant
    mov           ctrlWord, ax
    fldcw         ctrlWord
    fldz                                  // push 2 zeroes. one is carry, the other to check for zero with fcomi
    fldz
    fild          BASE                    // push BASE
  }

// Name convention in comment. NewS = new Sum, OldS = old Sum, etc. NewS' = NewS * BASE (need division by BASE before used).S for Sum, C for Carry, D for Digit

  for(const Digit *xk = x.m_first, *yk = y.m_first;;) {
    cd = fastAppendDigit(cd);

    __asm {
      mov         ebx, dword ptr [xk]       // xp = xk
      mov         ecx, dword ptr [yk]       // yp = yk
      fldz                                  // push 0
MultiplyLoop:                               // do { First iteration both xp and yp are not NULL.
      fild        dword ptr [ebx]           //   push xp->n
      fimul       dword ptr [ecx]           //   st(0) *= yp->n
      fadd                                  //   st(0) += st(1)
      mov         ecx, dword ptr [ecx+8]    //   yp = yp->prev
      jecxz       AddSubProduct             //   if(yp == NULL) break
      mov         ebx, dword ptr [ebx+4]    //   xp = xp->next
      or          ebx, ebx                  //
      jne         MultiplyLoop              // } while(xp)

AddSubProduct:                              // Assume                         st0:Sum(xp*yp)         st1:BASE               st2:Carry=0            st3:0
      mov         ecx  , cd
      fst         st(4)                     // Save sum in st4                st0:Sum(xp*yp)         st1:BASE               st2:Carry=0            st3:0               st4:Sum
      fprem                                 // st0 %= BASE                    st0:Sum%BASE           st1:BASE               st2:Carry=0            st3:0               st4:Sum
      fist        dword ptr[ecx]            // ecx->n = sum % BASE            st0:Sum%BASE           st1:BASE               st2:Carry=0            st3:0               st4:Sum
      fsubr       st(0), st(4)              // st0 = st4 - st0                st0:Sum-Sum%BASE       st1:BASE               st2:Carry              st3:0
      fcomi       st(0), st(3)              //                                st0:NewS'              st1:BASE               st2:Carry              st3:0
      je          NextDigit                 // if(NewS' == 0) we're done

//AddNoCarry:                               // Assume                         st0:OldS'              st1:BASE               st2:OldC=0             st3:0
      fdiv        st(0), st(1)              // st0 /= BASE                    st0:OldS               st1:BASE               st2:OldC               st3:0
      mov         ecx, dword ptr[ecx+8]     // ecx = ecx->prev
      fst         st(4)                     // Save OldS in st4                                                                                                        st4:OldS
      fprem                                 // st0 %= BASE                    st0:OldS%BASE          st1:BASE               st2:OldC               st3:0               st4:OldS
      fsub        st(4), st                 // Saved sum -= st0               st0:OldS%BASE          st1:BASE               st2:OldC               st3:0               st4:OldS-OldS%BASE
      fiadd       dword ptr[ecx]            // st0 += ecx->n                  st0:OldS%BASE+OldD     st1:BASE               st2:OldC               st3:0               st4:OldS-OldS%BASE
      fst         st(2)                     // Save Carry'                                                                  st2:OldS%BASE+OldD     st3:0               st4:OldS-OldS%BASE
      fprem                                 // st0 %= BASE                    st0:NewD               st1:BASE               st2:OldS%BASE+OldD     st3:0               st4:OldS-OldS%BASE
      fist        dword ptr[ecx]            // ecx->n = NewD
      fsubp       st(2), st(0)              // st2 -= NewD, pop st0           st0:BASE               st1:NewC'              st2:0                  st3:OldS-OldS%BASE
      fld         st(3)                     // push st3                       st0:NewS'              st1:BASE               st2:NewC'              st3:0              st4:----
      fcomi       st(0), st(3)              //
      jne         AddWithCarry              // if(NewS' != 0) goto AddWithCarry
      fstp        st(0)                     // pop NewS'(=0)                  st0:BASE               st1:NewC'              st2:0
      fld         st(1)                     // push NewC'                     st0:NewC'              st1:BASE               st2:NewC'              st3:0
      fcomi       st(0), st(3)              //
      jne         FinalizeCarryLoop         // if(NewC'!= 0) goto FinalizeCarryLoop
      jmp         NextDigit                 // else we're done

AddWithCarry:                               // Assume                         st0:OldS' <= 1844*BASE st1:BASE               st2:OldC'              st3:0
      fdiv        st(0), st(1)              // st0 /= BASE                    st0:OldS               st1:BASE               st2:OldC'              st3:0
      mov         ecx, dword ptr[ecx+8]     // ecx = ecx->prev
      fiadd       dword ptr[ecx]            // st0 += ecx->n                  st0:OldS+OldD          st1:BASE               st2;OldC'              st3:0
      fld         st(2)                     // push Carry'                    st0:Carry'             st1:OldS+OldD          st2:BASE               st3:----           st4:0
      fdiv        st(0), st(2)              // st0 /= BASE                    st0:Carry              st1:OldS+OldD          st2:BASE               st3:----           st4:0
      faddp       st(1), st(0)              // st1 += Carry, pop st0          st0:OldS+OldD+OldC     st1:BASE               st2:----               st3:0
      fst         st(2)                     // Save carry'                                                                  st2:SaveC(OldS+OldD+OldC)
      fprem                                 // st0 %= BASE                    st0:NewD               st1:BASE               st2:SaveC              st3:0
      fist        dword ptr[ecx]            // ecx->n = NewD                  st0:NewD               st1:BASE               st2:SaveC              st3:0
      fsubr       st(0), st(2)              // st0 = SaveC - NewD             st0:NewC'              st1:BASE               st2:----               st3:0

//FinalizeCarry:                            // Assume                         st1:Carry'             st1:BASE               st2:----               st3:0
      fcomi       st(0), st(3)
      je          NextDigit                 // if(Carry' == 0) we're done
FinalizeCarryLoop:                          // do { Assume                    st0:Carry'             st1:BASE               st2:----               st3:0
      fdiv        st(0), st(1)              //   st0 /= BASE                  st0:Carry              st1:BASE               st2:----               st3:0
      mov         ecx, dword ptr[ecx+8]     //   ecx = ecx->prev
      fiadd       dword ptr[ecx]            //   st0 += Digit                 st0:OldC+OldD          st1:BASE               st2:----               st3:0
      fst         st(2)                     //   Save carry in st2            st0:OldC+OldD          st1:BASE               st2:SaveC(=OldC+OldD)  st3:0
      fprem                                 //   st0 %= BASE                  st0:NewD               st1:BASE               st2:SaveC              st3:0
      fist        dword ptr[ecx]            //   ecx->n = NewD                st0:NewD               st1:BASE               st2:SaveC              st3:0
      fsubr       st(0), st(2)              //   st0 = SaveC - NewD           st0:NewC'              st1:BASE               st2:----               st3:0
      fcomi       st(0), st(3)              //
      jne         FinalizeCarryLoop         // } while(NewC' != 0)
NextDigit:
      fstp st(2)                            // Reset carry (st(0) is always zero at this point)
    }                                       // Assume:                        st0:BASE              st1:Carry=0             st2:0

    if(--loopCounter <= 0) break;
    if(yk->next) {
      yk = yk->next;
    } else if(!(xk = xk->next)) {
      break; // we are done
    }
  }

  __asm {
    fstp          st(0)                   // pop BASE
    fstp          st(0)
    fstp          st(0)                   // pop the 2 last registers
    fldcw  saveCtrlWord                   // restore control word, leaving FPU as it was
  }

  intptr_t digitsAdded = loopCount - loopCounter;
  if(cd->n == 0) {                        // Fixup both ends of digit chain, to reestablish invariant (See comments in assertIsValil)
    m_last = cd;
    for(digitsAdded--, cd = m_last->prev; cd->n == 0; cd = cd->prev, digitsAdded--);
    deleteDigits(cd->next, m_last);
  }
  m_low -= digitsAdded - 1;
  (m_last = cd)->next = NULL;

  if(m_first->n == 0) {
    cd = m_first;
    (m_first = m_first->next)->prev = NULL;
    deleteDigits(cd,cd);
  } else {
    m_first->prev = NULL;
    m_expo++;
  }
  return setSignByProductRule(x, y);
}

#define SPLIT_LENGTH 200

#define MAX_DIGITVALUE (BIGREALBASE-1)

size_t BigReal::s_splitLength = SPLIT_LENGTH; // Value found by experiments with measureSplitFactor in testnumber.cpp

UINT BigReal::getMaxSplitLength() { // static
  return _UI64_MAX / ((BR2DigitType)MAX_DIGITVALUE * MAX_DIGITVALUE);
}

#endif // SP_OPT_METHOD == SP_OPT_BY_FPU
