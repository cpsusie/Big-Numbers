.DATA

BASE      qword 1000000000000000000             ; BIGREALBASE (=1e18)
MaxSum    qword 0DE0B6B3A763FFFFh               ; max highorder QWORD = (0xffffffffffffffff * BASE) >> 64

.CODE

; int BigRealMultiplyColumn(const Digit *yk, const Digit *xk, Digit *dst, _uint128 &bigSum);
; rcx yk
; rdx xk
; r8  dst (=current digit to copy to)
; r9  &bigsum
BigRealMultiplyColumn PROC
      push        rbx
      push        rsi
      push        rdi

      mov         rbx, rdx                        ; xp = xk
      xor         rsi, rsi                        ;
      xor         rdi, rdi                        ; rsi:rdi accumulates sum. Init to 0
MultiplyLoop:                                     ; do { ; we know that the first time both xp and yp are not NULL.
      mov         rax, QWORD PTR[rbx]             ;   rax        = xp->n
      mul         QWORD PTR [rcx]                 ;   [rdx:rax] *= yp->n
      add         rdi, rax                        ;
      adc         rsi, rdx                        ;
      mov         rcx, QWORD PTR[rcx+16]          ;   yp = yp->prev
      jrcxz       AddSubProduct                   ;   if(yp == NULL) exit loop to AddSubProduct
      mov         rbx, QWORD PTR[rbx+8]           ;   xp = xp->next
      or          rbx, rbx                        ;
      jne         MultiplyLoop                    ; } while(xp);

AddSubProduct:
      cmp         rsi, MaxSum                     ; if(sum >= maxui64 * BASE) goto SumTooBig (or we will have a division by zero exception)
      jae         SumTooBig                       ; Use jae for unsigned compare, jge is for signed !
                                                  ;
      mov         rbx, BASE                       ; rbx remains constant BASE
      mov         rdx, rsi                        ;
      mov         rax, rdi                        ; [rdx:rax] = sum
      div         rbx                             ; rax = sum / BASE, rdx = sum % BASE
      mov         rdi, rax                        ; rdi = sum / BASE
                                                  ; No need to set carry = currentDigit->n (it's just been added => currentDigit->n == 0)
      mov         QWORD PTR[r8], rdx              ; currentDigit->n = carry % BASE = sum % BASE = rdx
      or          rdi, rdi                        ;
      je          NextDigit                       ; if (sum == 0) we're done. Carry is always 0 at this point
                                                  ;
AddIntNoCarry:                                    ; Assume 0 < sum in rdi <= maxui64), r8 is addr of last updated digit, Carry = 0
      mov         r8, QWORD PTR[r8+16]            ; currentDigit = currentDigit->prev
      xor         rdx, rdx                        ;
      mov         rax, rdi                        ; [rdx:rax] = [0:sum}
      div         rbx                             ; rax = sum / BASE, rdx = sum % BASE
      mov         rdi, rax                        ; rdi = new sum (sum / BASE) <= maxui64/BASE = 18)
      mov         rax, QWORD PTR[r8]              ; rax = currentDigit->n + (old sum) % BASE
      add         rax, rdx                        ;
      xor         rdx, rdx                        ; [rdx:rax] = carry = currentDigit->n + (old sum) % BASE) <= (BASE-1) * 2
      div         rbx                             ; rax = carry / BASE, rdx = carry % BASE
      mov         QWORD PTR[r8], rdx              ; currentDigit->n = carry % BASE
                                                  ;
      or          rdi, rdi                        ;
      jne         AddIntPossibleCarry             ;
      or          rax, rax                        ; sum = 0
      jne         FinalizeCarryLoop               ; if(carry != 0) handle it in FinalizeCarryLoop, which assumes carry in rax
      jmp         NextDigit                       ; else we're done
                                                  ;
AddIntPossibleCarry:                              ; Assume carry in rax. can be 0 or 1
      or          rax, rax                        ;
      je          AddIntNoCarry                   ;
                                                  ; Assume 0 < sum in rdi <= maxui64/BASE (=18), rsi is addr of last updated digit, Carry in rax != 0 (maxvalue = 1)
      mov         r8, QWORD PTR[r8+16]            ; currentDigit = currentDigit->prev
      add         rax, QWORD PTR[r8]              ; carry += currentDigit->n
      add         rax, rdi                        ; carry += sum % BASE (No need to calculate sum % BASE because sum<BASE=>sum%BASE==sum = edi
      xor         rdx, rdx                        ;
      div         rbx                             ; rax = carry / BASE, rdx = carry % BASE
      mov         QWORD PTR[r8], rdx              ; currentDigit->n = carry % BASE
      or          rax, rax                        ;
      je          NextDigit                       ; if (carry == 0) we're done
                                                  ;
FinalizeCarryLoop:                                ; do { // Assume r8 is addr of last updated digit, Carry in rax (!= 0)
      mov         r8, QWORD PTR[r8+16]            ;   currentDigit = currentDigit->prev
      add         rax, QWORD PTR[r8]              ;   carry += currentDigit->n
      xor         rdx, rdx                        ;
      div         rbx                             ;   rax = carry / BASE , rdx = carry % BASE
      mov         QWORD PTR[r8], rdx              ;   currentDigit->n = carry % BASE
      or          rax, rax                        ;
      je          NextDigit                       ;
      jmp         FinalizeCarryLoop               ; } while(carry != 0)
SumTooBig:                                        ;
      mov         QWORD PTR[r9]  , rdi            ; low order  QWORD to bigSum.lo
      mov         QWORD PTR[r9+8], rsi            ; high order QWORD to bigSum.hi
      xor         rax, rax                        ; return 0
      jmp         Epilog                          ;
NextDigit:                                        ;
      mov         rax, 1                          ; return 1
Epilog:                                           ; restore used registers and stack pointer
      pop         rdi
      pop         rsi
      pop         rbx
      ret
BigRealMultiplyColumn ENDP

; class SubProductSum {
; public:
;   Digit        *m_cd;     // pointer to current digit
;   BR2DigitType  m_bigSum; // result stored here if too big for simple division.
; };

; int BigRealSquareColumn(  const Digit *yp, const Digit *xp, SubProductSum *sps, int sumLength);
; rcx yp
; rdx xp
; r8  sps address of SubProductSum
; r9  sumLength
BigRealSquareColumn PROC
      push        rbx
      push        rsi
      push        rdi

      xor         rsi, rsi                        ;
      xor         rdi, rdi                        ; rsi:rdi accumulates sum. Init to 0
      mov         rbx, rdx                        ; rbx = xp; need rdx for multiplication
      mov         eax, r9d                        ; eax = sumLength
      and         eax, 1                          ;
      jz          EvenSumLength                   ; if(isEven(sumLength)) goto EvenSumLength
      cmp         rbx, rcx                        ;
      je          AddMiddleElement                ; while(xp != yp) {
OddLengthLoop:                                    ;
      mov         rax, QWORD PTR[rbx]             ;   rax        = xp->n
      mul         QWORD PTR [rcx]                 ;   [rdx:rax] *= yp->n
      add         rdi, rax                        ;
      adc         rsi, rdx                        ;   [rsi:rdi] += [rdx:rax]   sum += xp->n * yp->n
      mov         rcx, QWORD PTR[rcx+16]          ;   yp = yp->prev
      mov         rbx, QWORD PTR[rbx+8]           ;   xp = xp->next
      cmp         rbx, rcx                        ;
      jne         OddLengthLoop                   ; } while(xp != yp);
      shld        rsi, rdi, 1                     ;
      shl         rdi, 1                          ; [rsi:rdi] *= 2             sum *= 2
AddMiddleElement:
      mov         rax, QWORD PTR[rbx]             ; rax        = xp->n
      mul         QWORD PTR [rbx]                 ; [rdx:rax] *= yp->n
      add         rdi, rax                        ;
      adc         rsi, rdx                        ; rsi:rdi += [rdx:rax]       sum += xp->n * yp->n
      jmp         AddSubProduct                   ;
EvenSumLength:                                    ; Assume: r9d is even, rbx = xp
      shr         r9d, 1                          ; r9d /= 2;                  sumLength /= 2
EvenLengthLoop:                                   ; for(;;) {
      mov         rax, QWORD PTR[rbx]             ;   rax       = xp->n
      mul         QWORD PTR [rcx]                 ;   [rdx:rax] *= yp->n
      add         rdi, rax                        ;
      adc         rsi, rdx                        ;   rsi:rdi += [rdx:rax]     sum += xp->n * yp->n;
      sub         r9d, 1                          ;
      jz          EndEvenLengthLoop               ;   if(!--numLength) break;
      mov         rcx, QWORD PTR[rcx+16]          ;   yp = yp->prev
      mov         rbx, QWORD PTR[rbx+8]           ;   xp = xp->next
      jmp         EvenLengthLoop                  ; }
EndEvenLengthLoop:                                ;
      shld        rsi, rdi, 1                     ;
      shl         rdi, 1                          ; [rsi:rdi] *= 2             sum *= 2
AddSubProduct:                                    ;
      cmp         rsi, MaxSum                     ; if(sum >= maxui64 * BASE) goto SumTooBig (or we will have a division by zero exception)
      jae         SumTooBig                       ; Use jae for unsigned compare, jge is for signed !

      mov         rbx, BASE                       ; rbx remains constant BASE
      mov         rdx, rsi                        ;
      mov         rax, rdi                        ; [rdx:rax] = sum
      div         rbx                             ; rax = sum / BASE, rdx = sum % BASE
      mov         rdi, rax                        ; rdi = sum / BASE
                                                  ; No need to set carry = currentDigit->n (it's just been added => currentDigit->n == 0)
      mov         r8, QWORD PTR[r8]               ; r8 = address of sps->m_cd = currentDigit
      mov         QWORD PTR[r8], rdx              ; currentDigit->n = carry % BASE = sum % BASE = rdx
      or          rdi, rdi                        ;
      je          NextDigit                       ; if (sum == 0) we're done. Carry is always 0 at this point
                                                  ;
AddIntNoCarry:                                    ; Assume 0 < sum in rdi <= maxui64), r8 is addr of last updated digit, Carry = 0
      mov         r8, QWORD PTR[r8+16]            ; currentDigit = currentDigit->prev
      xor         rdx, rdx                        ;
      mov         rax, rdi                        ; [rdx:rax] = [0:sum}
      div         rbx                             ; rax = sum / BASE, rdx = sum % BASE
      mov         rdi, rax                        ; rdi = new sum (sum / BASE) <= maxui64/BASE = 18)
      mov         rax, QWORD PTR[r8]              ; rax = currentDigit->n + (old sum) % BASE
      add         rax, rdx                        ;
      xor         rdx, rdx                        ; [rdx:rax] = carry = currentDigit->n + (old sum) % BASE) <= (BASE-1) * 2
      div         rbx                             ; rax = carry / BASE, rdx = carry % BASE
      mov         QWORD PTR[r8], rdx              ; currentDigit->n = carry % BASE
                                                  ;
      or          rdi, rdi                        ;
      jne         AddIntPossibleCarry             ;
      or          rax, rax                        ; sum = 0
      jne         FinalizeCarryLoop               ; if(carry != 0) handle it in FinalizeCarryLoop, which assumes carry in rax
      jmp         NextDigit                       ; else we're done
                                                  ;
AddIntPossibleCarry:                              ; Assume carry in rax. can be 0 or 1
      or          rax, rax                        ;
      je          AddIntNoCarry                   ;
                                                  ; Assume 0 < sum in rdi <= maxui64/BASE (=18), rsi is addr of last updated digit, Carry in rax != 0 (maxvalue = 1)
      mov         r8, QWORD PTR[r8+16]            ; currentDigit = currentDigit->prev
      add         rax, QWORD PTR[r8]              ; carry += currentDigit->n
      add         rax, rdi                        ; carry += sum % BASE (No need to calculate sum % BASE because sum<BASE=>sum%BASE==sum = edi
      xor         rdx, rdx                        ;
      div         rbx                             ; rax = carry / BASE, rdx = carry % BASE
      mov         QWORD PTR[r8], rdx              ; currentDigit->n = carry % BASE
      or          rax, rax                        ;
      je          NextDigit                       ; if (carry == 0) we're done
                                                  ;
FinalizeCarryLoop:                                ; do { // Assume r8 is addr of last updated digit, Carry in rax (!= 0)
      mov         r8, QWORD PTR[r8+16]            ;   currentDigit = currentDigit->prev
      add         rax, QWORD PTR[r8]              ;   carry += currentDigit->n
      xor         rdx, rdx                        ;
      div         rbx                             ;   rax = carry / BASE , rdx = carry % BASE
      mov         QWORD PTR[r8], rdx              ;   currentDigit->n = carry % BASE
      or          rax, rax                        ;
      je          NextDigit                       ;
      jmp         FinalizeCarryLoop               ; } while(carry != 0)

SumTooBig:
      mov         QWORD PTR[r8+8] , rdi           ; low order  QWORD to bigSum.lo
      mov         QWORD PTR[r8+16], rsi           ; high order QWORD to bigSum.hi
      xor         rax, rax                        ; return 0
      jmp         Epilog                          ;
NextDigit:                                        ;
      mov         rax, 1                          ; return 1
Epilog:                                           ; restore used registers and stack pointer
      pop         rdi
      pop         rsi
      pop         rbx
      ret
BigRealSquareColumn ENDP

END
