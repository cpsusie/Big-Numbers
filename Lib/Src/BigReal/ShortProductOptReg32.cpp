#include "pch.h"
#include <CallCounter.h>

#if(SP_OPT_METHOD == SP_OPT_BY_REG32)

#if(BIGREALBASE != 100000000)
#error For SP_OPT_METHOD == SP_OPT_BY_REG32 BIGREALBASE must be 100000000
#endif

//static CallCounter addSPCpp("addSubProductCpp");
//static CallCounter addSPTotal("addSubProductTotal");


// assume x != 0 and y != 0. and loopCount > 0
#ifdef _DEBUG
BigReal &BigReal::shortProductNoZeroCheckDebug(const BigReal &x, const BigReal &y, int loopCount) { // return *this
#else
BigReal &BigReal::shortProductNoZeroCheck(     const BigReal &x, const BigReal &y, int loopCount) { // return *this
#endif

  int              digitsAdded = 0;
  Digit           *cd;
  unsigned __int64 bigSum64;

  clearDigits();
  (cd = m_first = newDigit())->n = 0;
  m_expo = m_low = x.m_expo + y.m_expo;

  for(const Digit *xk = x.m_first, *yk = y.m_first;;) { // loopcondition at the end
    cd = fastAppendDigit(cd);
    digitsAdded++;

    __asm {
      mov         ebx, DWORD PTR [xk]     // xp = xk
      mov         ecx, DWORD PTR [yk]     // yp = yk
      xor         esi, esi                //
      xor         edi, edi                // esi:edi accumulates sum. Init to 0
MultiplyLoop:                             // do { // we know that the first time both xp and yp are not NULL.
      mov         eax, DWORD PTR [ebx]    //   eax     =  xp->n
      mul         DWORD PTR [ecx]         //   [edx:eax] *= yp->n
      add         edi, eax                //
      adc         esi, edx                //
      mov         ecx, DWORD PTR [ecx+8]  //   yp = yp->prev
      jecxz       AddSubProduct           //   if(yp == NULL) exit loop to AddSubProduct
      mov         ebx, DWORD ptr [ebx+4]  //   xp = xp->next
      cmp         ebx, 0                  //
      jne         MultiplyLoop            // } while(xp);

AddSubProduct:
      cmp         esi, 0x5f5e0ff          // if(sum >= 0xffffffff * NUMBEBASE) goto SumTooBig (or we will have a division by zero exception)
      jae         SumTooBig               // Use jae for unsigned compare, jge is for signed !

      mov         ebx, 100000000          // ebx remains constant BASE
      mov         edx, esi                //
      mov         eax, edi                // [edx:eax] = sum
      div         ebx                     // eax = sum / BASE, edx = sum % BASE
      mov         edi, eax                // edi = sum / BASE
      mov         esi, cd                 // esi is currentDigit from here. No more need for highorder DWORD of sum
                                          // No need to set carry = currentDigit->n (it's just been added => currentDigit->n == 0)
      mov         DWORD PTR[esi], edx     // currentDigit->n = carry % BASE = sum % BASE = edx
      cmp         edi, 0                  //
      je          NextDigit               // if(sum == 0) we're done. Carry is always 0 at this point

AddIntNoCarry:                            // Assume 0 < sum in edi <= 0xffffffff), esi is addr of last updated digit, Carry = 0
      mov         esi, DWORD PTR[esi+8]   // currentDigit = currentDigit->prev
      xor         edx, edx                //
      mov         eax, edi                // [edx:eax] = [0:sum}
      div         ebx                     // eax = sum / BASE, edx = sum % BASE
      mov         edi, eax                // edi = new sum (sum / BASE) <= 0xffffffff/BASE = 42)
      mov         eax, DWORD PTR [esi]    // eax = currentDigit->n + (old sum) % BASE
      add         eax, edx
      xor         edx, edx                // [edx:eax] = carry = currentDigit->n + (old sum) % BASE) <= (BASE-1) * 2
      div         ebx                     // eax = carry / BASE, edx = carry % BASE
      mov         DWORD PTR[esi], edx     // currentDigit->n = carry % BASE

      cmp         edi, 0                  //
      jne         AddIntPossibleCarry     //
      cmp         eax, 0                  // sum = 0
      jne         FinalizeCarryLoop       // if(carry != 0) handle it in FinalizeCarryLoop, which assumes carry in eax
      jmp         NextDigit               // else we're done

AddIntPossibleCarry:                      // Assume carry in eax. can be 0 or 1
      cmp         eax, 0
      je          AddIntNoCarry
                                          // Assume 0 < sum in edi <= 0xffffffff/BASE (=42), esi is addr of last updated digit, Carry in eax != 0 (maxvalue = 1)
      mov         esi, DWORD PTR[esi+8]   // currentDigit = currentDigit->prev
      add         eax, DWORD PTR [esi]    // carry += currentDigit->n
      add         eax, edi                // carry += sum % BASE (No need to calculate sum % BASE because sum<BASE=>sum%BASE==sum = edi
      xor         edx, edx                //
      div         ebx                     // eax = carry / BASE, edx = carry % BASE
      mov         DWORD PTR[esi], edx     // currentDigit->n = carry % BASE
      cmp         eax, 0                  //
      je          NextDigit               // if(carry == 0) we're done

FinalizeCarryLoop:                        // do { // Assume esi is addr of last updated digit, Carry in eax (!= 0)
      mov         esi, DWORD PTR[esi+8]   //   currentDigit = currentDigit->prev
      add         eax, DWORD PTR [esi]    //   carry += currentDigit->n
      xor         edx, edx                //
      div         ebx                     //   eax = carry / BASE , edx = carry % BASE
      mov         DWORD PTR[esi], edx     //   currentDigit->n = carry % BASE
      cmp         eax, 0
      je          NextDigit               //
      jmp         FinalizeCarryLoop       // } while(carry != 0)

SumTooBig:
      lea	     eax, bigSum64            // sum = esi:edi
      mov        DWORD PTR[eax]  , edi
      mov        DWORD PTR[eax+4], esi
  }

  unsigned long carry = 0;
  Digit *d = cd;
  d->n = bigSum64 % BIGREALBASE;
  bigSum64 /= BIGREALBASE;
  do {
    d = d->prev;
    carry += (unsigned long)(d->n + bigSum64 % BIGREALBASE);
    d->n = carry % BIGREALBASE;
    carry    /= BIGREALBASE;
    bigSum64 /= BIGREALBASE;
  } while(bigSum64 || carry);
// dont call trimZeroes() !!;

NextDigit:
    if(--loopCount <= 0) break;
    if(yk->next) {
      yk = yk->next;
    } else if(!(xk = xk->next)) {
      break; // we are done
    }
  }

  if(cd->n == 0) { // Fixup both ends of digit chain, to reestablish invariant (See comments in assertIsValidBigReal)
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

#define SPLIT_LENGTH 240

int BigReal::s_splitLength = SPLIT_LENGTH; // Value found by experiments with measureSplitFactor in testnumber.cpp

#define MAX_DIGITVALUE (BIGREALBASE-1)

unsigned int BigReal::getMaxSplitLength() { // static
  return _UI64_MAX / ((unsigned __int64)MAX_DIGITVALUE * MAX_DIGITVALUE);
}

#endif // SP_OPT_METHOD == SP_OPT_BY_REG32
