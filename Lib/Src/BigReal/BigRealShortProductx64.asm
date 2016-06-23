.DATA

BASE      qword 1000000000000000000             ; BIGREALBASE (=1e18)
MaxSum    qword 0DE0B6B3A763FFFeh               ; max highorder QWORD 

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
      mov         rax, QWORD PTR[rbx]             ;   rax     =  xp->n
      mul         QWORD PTR [rcx]                 ;   [rdx:rax] *= yp->n
      add         rdi, rax                        ;
      adc         rsi, rdx                        ;
      mov         rcx, QWORD PTR[rcx+16]          ;   yp = yp->prev
      jrcxz       AddSubProduct                   ;   if(yp == NULL) exit loop to AddSubProduct
      mov         rbx, QWORD PTR[rbx+8]           ;   xp = xp->next
      cmp         rbx, 0                          ;
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
      cmp         rdi, 0                          ;
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
      cmp         rdi, 0                          ;
      jne         AddIntPossibleCarry             ;
      cmp         rax, 0                          ; sum = 0
      jne         FinalizeCarryLoop               ; if(carry != 0) handle it in FinalizeCarryLoop, which assumes carry in eax
      jmp         NextDigit                       ; else we're done
                                                  ;
AddIntPossibleCarry:                              ; Assume carry in rax. can be 0 or 1
      cmp         rax, 0                          ;
      je          AddIntNoCarry                   ;
                                                  ; Assume 0 < sum in rdi <= maxui64/BASE (=18), rsi is addr of last updated digit, Carry in rax != 0 (maxvalue = 1)
      mov         r8, QWORD PTR[r8+16]            ; currentDigit = currentDigit->prev
      add         rax, QWORD PTR[r8]              ; carry += currentDigit->n
      add         rax, rdi                        ; carry += sum % BASE (No need to calculate sum % BASE because sum<BASE=>sum%BASE==sum = edi
      xor         rdx, rdx                        ;
      div         rbx                             ; eax = carry / BASE, edx = carry % BASE
      mov         QWORD PTR[r8], rdx              ; currentDigit->n = carry % BASE
      cmp         rax, 0                          ;
      je          NextDigit                       ; if (carry == 0) we're done
                                                  ;
FinalizeCarryLoop:                                ; do { // Assume r8 is addr of last updated digit, Carry in eax (!= 0)
      mov         r8, QWORD PTR[r8+16]            ;   currentDigit = currentDigit->prev
      add         rax, QWORD PTR[r8]              ;   carry += currentDigit->n
      xor         rdx, rdx                        ;
      div         rbx                             ;   eax = carry / BASE , edx = carry % BASE
      mov         QWORD PTR[r8], rdx              ;   currentDigit->n = carry % BASE
      cmp         rax, 0                          ;
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

END
