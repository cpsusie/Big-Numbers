#include "pch.h"

#if defined(__NEVER__)

BigReal &BigReal::shortProductNoZeroCheck(const BigReal &x, const BigReal &y, size_t loopCount) { // version 1
  return (s_useShortProdReferenceVersion) ? shortProductNoZeroCheckReference(x, y,loopCount) : shortProductNoZeroCheckDebug(x, y, loopCount);
}

#if(BIGREALBASE != 100000000)
#error BIGREALBASE must be 10000000
#endif


BigReal &BigReal::shortProductNoZeroCheckReference(const BigReal &x, const BigReal &y, int loopCount) { // return *this
  m_low = m_expo = x.m_expo + y.m_expo;
  clearDigits();

  int digitsAdded = 0;
  Digit *xk, *yk;
  for(xk = x.m_first, yk = y.m_first;;) { // loopcondition at the end
    appendZero();
    digitsAdded++;
    Digit *tp = m_last;

    unsigned __int64 sum64;
    __asm {
      mov         ebx, DWORD PTR [xk]     // xp = xk
      mov         ecx, DWORD PTR [yk]     // yp = yk
      xor         esi, esi                //
      xor         edi, edi                // esi:edi accumulates sum. Init to 0
MultiplyLoop:                             // do { // we know that the first time both xp and yp are not nullptr.
      mov         eax, DWORD PTR [ebx]    //   eax     =  xp->n
      mul         DWORD PTR [ecx]         //   [edx:eax] *= yp->n
      add         edi, eax                //
      adc         esi, edx                //
      mov         ecx, DWORD PTR [ecx+8]  //   yp = yp->prev
      jecxz       AddSubProduct           //   if(yp == nullptr) exit loop to AddSubProduct
      mov         ebx, DWORD ptr [ebx+4]  //   xp = xp->next
      cmp         ebx, 0                  //
      jne         MultiplyLoop            // } while(xp);

      xor         ecx, ecx                // carry in ecx. If we jmp directly to AddSubProduct, because yp==nullptr, we dont need this!!

AddSubProduct:
      cmp         esi, 0x5f5e0ff          // if(sum >= 0xffffffff * NUMBEBASE) goto SumTooBig (or we will have a division by zero exception)
      jae         SumTooBig               // Use jae for unsigned compare, jge is for signed !

      mov         ebx, 100000000          // ebx remains constant BIGREALBASE
      mov         edx, esi                //
      mov         eax, edi                // [edx:eax] = sum
      div         ebx                     // eax = sum / BIGREALBASE, edx = sum % BIGREALBASE
      mov         edi, eax                // edi = sum / BIGREALBASE
      mov         esi, tp                 // esi is currentDigit from here. No more need for highorder DWORD of sum
                                          // No need to set carry = currentDigit->n (it's just been added => currentDigit->n == 0)
      mov         DWORD PTR[esi], edx     // currentDigit->n = carry % BIGREALBASE = sum % BIGREALBASE = edx
      mov         esi, DWORD PTR[esi+8]   // currentDigit = currentDigit->prev
      cmp         edi, 0                  //
      je          NextDigit               // if(sum == 0) we're Done. Carry is always 0 at this point

AddInt32:                                 // do { // assume sum <= 0xffffffff)
      cmp         esi, 0                  //
      je          NoDigitAddInt32         //   if(currentDigit == nullptr) goto NoDigitAddInt32
      xor         edx, edx                //
      mov         eax, edi                //   [edx:eax] = sum
      div         ebx                     //   eax = sum / BIGREALBASE, edx = sum % BIGREALBASE
      mov         edi, eax                //   edi = new sum (sum / BIGREALBASE)
      add         ecx, DWORD PTR [esi]    //   carry += currentDigit->n
      add         ecx, edx                //   carry += sum % BIGREALBASE
      xor         edx, edx                //
      mov         eax, ecx                //   [edx:eax] = carry
      div         ebx                     //   eax = cary / BIGREALBASE, edx = cary % BIGREALBASE
      mov         ecx, eax                //   carry = carry / BIGREALBASE
      mov         DWORD PTR[esi], edx     //   currentDigit->n = carry % BIGREALBASE
      mov         esi, DWORD PTR[esi+8]   //   currentDigit = currentDigit->prev
      cmp         edi, 0                  //
      jne         AddInt32                // } while(sum != 0);
      cmp         ecx, 0                  //
      je          NextDigit               // if(carry == 0) we're done

//FinalizeCarry:                          // First iteration. Assume currentDigit in esi, Carry in ecx
      cmp         esi, 0                  //
      je          NoDigitFinalyzeCarryECX // if(currentDigit == nullptr) goto NoDigitFinalyzeCarry (No looping there)
      add         ecx, DWORD PTR [esi]    // carry += currentDigit->n
      xor         edx, edx                //
      mov         eax, ecx                //
      div         ebx                     // eax = carry / BIGREALBASE , edx = carry % BIGREALBASE
      mov         DWORD PTR[esi], edx     // currentDigit->n = carry % BIGREALBASE
      cmp         eax, 0
      je          NextDigit               // if(new carry == 0) we're done
      mov         ecx, DWORD PTR[esi+8]   // currentDigit = currentDigit->prev. currentDigit now in ecx. Carry in eax
      jecxz       NoDigitFinalyzeCarryEAX // if(tp == nullptr) goto NoDigitFinalyzeCarry (no looping there)

FinalizeCarryLoop:                        // do { // Assume currentDigit in ecx. Carry in eax and currentDigit != nullptr
      add         eax, DWORD PTR [ecx]    //   carry += currentDigit->n
      xor         edx, edx                //
      div         ebx                     //   eax = carry / BIGREALBASE , edx = carry % BIGREALBASE
      mov         DWORD PTR[ecx], edx     //   currentDigit->n = carry % BIGREALBASE
      cmp         eax, 0
      je          NextDigit               //   if(carry == 0) we're done
      mov         ecx, DWORD PTR[ecx+8]   //   currentDigit = currentDigit->prev
      cmp         ecx, 0
      jne         FinalizeCarryLoop       // } while(currentDigit != nullptr)

NoDigitFinalyzeCarryEAX:                  // Same as NoDigitFinalyzeCarry, but pushes eax instead of ecx
      push        eax                     // push carry, assumed to be in eax
      push        this                    // puch this
      call        insertDigitAndIncrExpo  // insertDigitAndIncrExpo(this, carry);
      add         esp, 8
      jmp         NextDigit

NoDigitAddInt32:                          // (Assume sum <= 0xffffffff) and no more digits. So they must be inserted at the head
                                          // In fact, the max value of sum at this point is 99999999^2 / BIGREALBASE = 99999998
                                          // So after the division by BIGREALBASE sum will be 0 => this is not a loop
      push        edi                     // push sum. Carry is 0 at this point
      push        this                    // push this
      call        insertDigitAndIncrExpo  // insertDigitAndIncrExpo(this, sum);
      add         esp, 8                  //
      jmp         NextDigit

NoDigitFinalyzeCarryECX:                  // No loop required, as only one digit will be added. and after that, carry == 0
      push        ecx                     // push carry, assumed to be in ecx
      push        this                    // puch this
      call        insertDigitAndIncrExpo  // insertDigitAndIncrExpo(this, carry);
      add         esp, 8
      jmp         NextDigit

SumTooBig:
      lea	     eax, sum64               // sum = esi:edi
      mov        DWORD PTR[eax]  , edi
      mov        DWORD PTR[eax+4], esi
  }

  unsigned long carry = 0;
  while(sum64 || carry) {
    if(tp) {
      carry += (unsigned long)(tp->n + sum64 % BIGREALBASE);
      tp->n = carry % BIGREALBASE;
      tp = tp->prev;
    } else {
      carry += (unsigned long)sum64 % BIGREALBASE;
      insertDigit(carry);
      m_expo++;
    }
    carry /= BIGREALBASE;
    sum64 /= BIGREALBASE;
  }
// dont call trimZeroes() !!;

NextDigit:
//    addSPTotal.incr();
    if(--loopCount <= 0) break;
    if(yk->next) {
      yk = yk->next;
    } else if(!(xk = xk->next)) {
      break; // we are done
    }
  }

  m_low -= digitsAdded - 1;
  if(m_last->n == 0) { //( only trim the end for zeroes
    for(m_low++, xk = m_last->prev; xk->n == 0; xk = xk->prev, m_low++);
    deleteDigits(xk->next, m_last);
    (m_last = xk)->next = nullptr;
  }
  return setSignByProductRule(x, y);
}

inline void insertDigitAndIncrExpo(BigReal &v, const BRDigitType n) {
  v.insertDigit(n);
  v.m_expo++; // we know m_expo != ZERO_EXPO
}

BigReal &BigReal::shortProductNoZeroCheckReference(const BigReal &x, const BigReal &y, int loopCount) { // return *this
  m_low = (m_expo = x.m_expo + y.m_expo) + 1;
  clearDigits();

  for(Digit *xk = x.m_first, *yk = y.m_first;;) { // loopcondition at the end
    unsigned __int64 tmp;
    __asm {
      mov         ecx, dword ptr [xk]     // xp = xk
      mov         ebx, dword ptr [yk]     // yp = yk
      xor         edi, edi                // edi = 0, edi:edi is accumulator
      xor         esi, esi                // esi = 0
MultiplyLoop:                             // do { // we know that the first time both xp and yp are not nullptr.
      mov         eax, dword ptr [ecx]    //   eax     =  xp->n
      mul         dword ptr [ebx]         //   edx:eax *= yp->n
      add         edi, eax                //
      adc         esi, edx                //
      mov         ebx, dword ptr [ebx+8]  //   yp = yp->prev
      cmp         ebx, 0                  //
      je          ExitLoop                //   if(yp == nullptr) break
      mov         ecx, dword ptr [ecx+4]  //   xp = xp->next
      cmp         ecx, 0                  //
      jne         MultiplyLoop            // } while(xp)
ExitLoop:
      mov         dword ptr [tmp]  , edi  // tmp = (esi:edi)
      mov         dword ptr [tmp+4], esi
    }
    appendZero();
    if(tmp) {
      addSubProductReference1(tmp);
    }
    m_low--;

    if(--loopCount <= 0) break;
    if(yk->next) {
      yk = yk->next;
    } else if(!(xk = xk->next)) {
      break; // we are done
    }
  }

  if(m_last->n == 0) { //( only trim the end for zeroes
    for(m_low++, xk = m_last->prev; xk->n == 0; xk = xk->prev, m_low++);
    deleteDigits(xk->next, m_last);
    (m_last = xk)->next = nullptr;
  }
  return setSignByProductRule(x, y);
}

void BigReal::addSubProductReference1(unsigned __int64 &n) {
  Digit *tp = m_last;
  _asm {
    mov ecx, DWORD PTR n            // ecx = &n
    mov edx, DWORD PTR [ecx+4]      // edx: high DWORD of n
    cmp edx, 0x5f5e0ff              // if(n >= 0xffffffff * NUMBEBASE) goto NTooBig (or we will have a division by zero exception)
    jae NTooBig                     // use jae for unsigned, jge is for signed !
                                    //
    mov ebx, 100000000              // ebx remains constant BIGREALBASE
    xor edi, edi                    // carry in edi
    mov esi, DWORD PTR tp           // tp    in esi
    cmp edx, 0                      //
    je  AddInt32                    // if(highorder DWORD of n == 0 do it in int division
                                    //
//    cmp esi, 0                    //
//    je NoDigitAddInt64            // if(tp != nullptr) { When called from shortProduct tp always != nullptr!
                                    //
    mov eax, DWORD PTR [ecx]        //   edx:eax = n. edx already contains highorder DWORD of n
    div ebx                         //   eax = n / BIGREALBASE, edx = n % BIGREALBASE
    mov DWORD PTR[ecx], eax         //   n = n / BIGREALBASE. eax is free to use
    mov edi, DWORD PTR [esi]        //   carry = tp->n
    add edi, edx                    //   carry += n % BIGREALBASE
    xor edx, edx                    //   edx:eax = 0:carry
    mov eax, edi                    //
    div ebx                         //   eax = cary / BIGREALBASE, edx = cary % BIGREALBASE
    mov DWORD PTR[esi], edx         //   tp->n = carry % BIGREALBASE
    mov esi, DWORD PTR[esi+8]       //   tp = tp->prev
    mov edi, eax                    //   carry = carry / BIGREALBASE
    cmp DWORD PTR[ecx], 0           //
    jne AddInt32                    // if(n) goto AddInt32
    cmp edi, 0                      // else (n==0)
    jne FinalizeCarry               // if(carry) goto FinalizeCarry
    jmp Done                        // else Done

AddInt32:                           // while(n) { (n <= 0xffffffff)
    cmp esi, 0                      //
    je NoDigitAddInt32              //   if(tp == nullptr) goto NoDigitAddInt32
                                    //
    xor edx, edx                    //
    mov eax, DWORD PTR [ecx]        //   eax = n
    div ebx                         //   eax = n / BIGREALBASE, edx = n % BIGREALBASE
    mov DWORD PTR [ecx], eax        //   n = n / BIGREALBASE
    add edi, DWORD PTR [esi]        //   carry += tp->n
    add edi, edx                    //   carry += n % BIGREALBASE
    xor edx, edx                    //
    mov eax, edi                    //   edx:eax = 0:carry
    div ebx                         //   eax = cary / BIGREALBASE, edx = cary % BIGREALBASE
    mov DWORD PTR[esi], edx         //   tp->n = carry % BIGREALBASE
    mov edi, eax                    //   carry = carry / BIGREALBASE
    mov esi, DWORD PTR[esi+8]       //   tp = tp->prev
    cmp DWORD PTR [ecx], 0          //
    jne AddInt32                    // }
    cmp edi, 0                      // while(carry) { // n is 0 so we dont need it any more
    je  Done                        //

FinalizeCarry:
    cmp esi, 0                      //   if tp == nullptr goto NoDigitHandleCarry (no looping there)
    je NoDigitHandleCarry           //
                                    //
    add edi, DWORD PTR [esi]        //   carry += tp->n
    xor edx, edx                    //
    mov eax, edi                    //
    div ebx                         //   eax = carry / BIGREALBASE , edx = carry % BIGREALBASE
    mov DWORD PTR[esi], edx         //   tp->n = carry % BIGREALBASE
    mov esi, DWORD PTR[esi+8]       //   tp = tp->prev
    cmp eax, 0                      //   if(carry == 0) we're done
    je  Done                        // }
    mov edi, eax                    //   edi = new carry
    jmp FinalizeCarry               //

NoDigitAddInt32:
    mov esi, DWORD PTR[ecx]         // put n into esi.

NoDigitAddInt32Loop:                // while(n) { edi = carry, esi = n (> 0 && <= 0xffffffff)
    xor edx, edx                    //
    mov eax, esi                    //   edx:eax = 0:n
    div ebx                         //   eax = n / BIGREALBASE, edx = n % BIGREALBASE
    mov esi, eax
    add edi, edx                    //   carry += n % BIGREALBASE
    push edi                        //   push carry
    push this                       //   push this
    call insertDigitAndIncrExpo     //   insertDigitAnd(this, carry); No more need for n (=0)
    add esp, 4                      //
    pop edi                         //   restore edi
    xor edx, edx                    //
    mov eax, edi                    //
    div ebx                         //   eax = carry / NUMBERBAE
    mov edi, eax                    //   carry /= BIGREALBASE
    cmp esi, 0
    jne NoDigitAddInt32Loop         // }
    cmp edi, 0                      // if(carry == 0) goto Done
    je Done                         //

NoDigitHandleCarry:                 // No loop required, as only one digit will be added. and after that, carry == 0
    push edi                        // push carry
    push this                       // puch this
    call insertDigitAndIncrExpo     // insertDigitAnd(this, carry);
    add esp, 8
    jmp Done
  }

NTooBig:
  unsigned long carry = 0;
  while(n || carry) {
    if(tp) {
      carry += (unsigned long)(tp->n + n % BIGREALBASE);
      tp->n = carry % BIGREALBASE;
      tp = tp->prev;
    } else {
      carry += (unsigned long)n % BIGREALBASE;
      insertDigit(carry);
      m_expo++;
    }
    carry /= BIGREALBASE;
    n     /= BIGREALBASE;
  }
// dont call trimZeroes() !!;
Done:;
}

void BigReal::addSubProductReference2(unsigned __int64 &n) {
  unsigned long carry = 0;

  for(Digit *tp = m_last; n || carry;) {
    if(tp) {
      carry += (unsigned long)(tp->n + n % BIGREALBASE);
      tp->n = carry % BIGREALBASE;
//      printf("new tp:%08d(%08x)\n", tp->n, tp->n);
      tp = tp->prev;
    } else {
      carry += (unsigned long)(n % BIGREALBASE);
      insertDigit(carry % BIGREALBASE);
//      printf("new tp:%08d(%08x)\n", m_first->n, m_first->n);
      m_expo++;
    }
    carry /= BIGREALBASE;
    n     /= BIGREALBASE;
  }
}

#endif // __NEVER__
