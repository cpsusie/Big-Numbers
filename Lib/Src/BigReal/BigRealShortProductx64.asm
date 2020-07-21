.DATA

BASE      qword 1000000000000000000             ; BIGREALBASE (=1e18)
MAXSUM    qword 0DE0B6B3A763FFFFh               ; max highorder QWORD = (0xffffffffffffffff * BASE) >> 64

.CODE

; class SubProductSum {
; public:
;   Digit        *m_cd;     // pointer to current digit
;   BR2DigitType  m_bigSum; // result stored here if too big for simple division.
; };

; char BigRealMultiplyColumn(const Digit *yp, const Digit *xp, SubProductSum *sps);
; rcx yp
; rdx xp
; r8  sps address of SubProductSum
BigRealMultiplyColumn PROC
      push        rbx                             ;
      push        rsi                             ;
      push        rdi                             ;
                                                  ;
      mov         rbx, rdx                        ; rbx = xp; need rdx for multiplication
      xor         rsi, rsi                        ;
      xor         rdi, rdi                        ; rsi:rdi accumulates sum. Init to 0
MultiplyLoop:                                     ; do { ; we know that the first time both xp and yp are not NULL.
      mov         rax, QWORD PTR[rbx]             ;   rax        = xp->n
      mul         QWORD PTR [rcx]                 ;   rdx:rax   *= yp->n
      add         rdi, rax                        ;
      adc         rsi, rdx                        ;   rsi:rdi += rdx:rax        sum += xp->n*yp->n
      mov         rcx, QWORD PTR[rcx+16]          ;   yp = yp->prev
      jrcxz       AddSubProduct                   ;   if(yp == NULL) exit loop to AddSubProduct
      mov         rbx, QWORD PTR[rbx+ 8]          ;   xp = xp->next
      or          rbx, rbx                        ;
      jne         MultiplyLoop                    ; } while(xp);
                                                  ;
AddSubProduct:                                    ;
      cmp         rsi, MAXSUM                     ; if(sum >= maxui64 * BASE) goto SumTooBig (or we will have a division by zero exception)
      jae         SumTooBig                       ; Use jae for unsigned compare, jge is for signed !
                                                  ;
      mov         rbx, BASE                       ; rbx remains constant BASE
      mov         rax, rdi                        ;
      mov         rdx, rsi                        ; rdx:rax = rsi:rdi = sum
      div         rbx                             ; rax:rdx = sum/BASE : sum%BASE
      mov         rdi, rax                        ; rdi = sum / BASE (= new sum)
                                                  ; No need to set carry = currentDigit->n (it's just been added => currentDigit->n == 0)
      mov         r8, QWORD PTR[r8]               ; r8 = address of sps->m_cd = currentDigit
      mov         QWORD PTR[r8], rdx              ; currentDigit->n = rdx = carry % BASE = sum % BASE
      or          rdi, rdi                        ;
      je          NextDigit                       ; if(new sum == 0) we're done. Carry is always 0 at this point
AddIntNoCarry:                                    ; Assume 0 < sum in rdi <= maxui64), r8 is addr of last updated digit, Carry = 0
      mov         r8 , QWORD PTR[r8+16]           ; currentDigit = currentDigit->prev
      mov         rax, rdi                        ;
      xor         rdx, rdx                        ; rdx:rax = 0:sum
      div         rbx                             ; rax:rdx = sum/BASE : sum%BASE
      mov         rdi, rax                        ; rdi = new sum (=sum/BASE <= maxui64/BASE = 18)
      mov         rax, QWORD PTR[r8]              ; rax = currentDigit->n + (old sum) % BASE
      add         rax, rdx                        ;
      xor         rdx, rdx                        ; rdx:rax = 0:carry = currentDigit->n + (old sum % BASE) <= (BASE-1) * 2
      div         rbx                             ; rax:rdx = carry/BASE : carry%BASE
      mov         QWORD PTR[r8], rdx              ; currentDigit->n = carry % BASE
                                                  ;
      or          rdi, rdi                        ;
      jne         AddIntPossibleCarry             ;
      or          rax, rax                        ; sum = 0
      jne         FinalizeCarryLoop               ; if(carry != 0) handle it in FinalizeCarryLoop, which assumes carry in rax
      jmp         NextDigit                       ; else we're done
AddIntPossibleCarry:                              ; Assume carry in rax. can be 0 or 1
      or          rax, rax                        ;
      je          AddIntNoCarry                   ;
                                                  ; Assume 0 < sum in rdi <= maxui64/BASE (=18), rsi is addr of last updated digit, Carry in rax != 0 (maxvalue = 1)
      mov         r8 , QWORD PTR[r8+16]           ; currentDigit = currentDigit->prev
      add         rax, QWORD PTR[r8]              ; carry += currentDigit->n
      add         rax, rdi                        ; carry += sum % BASE (No need to calculate sum % BASE because sum<BASE=>sum%BASE==sum = edi
      xor         rdx, rdx                        ; rdx:rax = 0:carry
      div         rbx                             ; rax:rdx = carry/BASE : carry%BASE
      mov         QWORD PTR[r8], rdx              ; currentDigit->n = carry % BASE
      or          rax, rax                        ;
      je          NextDigit                       ; if(carry == 0) we're done
FinalizeCarryLoop:                                ; do { // Assume r8 is addr of last updated digit, Carry in rax (!= 0)
      mov         r8 , QWORD PTR[r8+16]           ;   currentDigit = currentDigit->prev
      add         rax, QWORD PTR[r8]              ;   carry += currentDigit->n
      xor         rdx, rdx                        ;   rdx:rax = 0:carry
      div         rbx                             ;   rax:rdx = carry/BASE : carry%BASE
      mov         QWORD PTR[r8], rdx              ;   currentDigit->n = carry % BASE
      or          rax, rax                        ;
      jne         FinalizeCarryLoop               ; } while(carry != 0)
NextDigit:                                        ;
      mov         al, 1                           ; return 1
Epilog:                                           ; restore used registers
      pop         rdi                             ;
      pop         rsi                             ;
      pop         rbx                             ;
      ret                                         ;
SumTooBig:                                        ;
      mov         QWORD PTR[r8+ 8], rdi           ; low order  QWORD to bigSum.lo
      mov         QWORD PTR[r8+16], rsi           ; high order QWORD to bigSum.hi
      xor         al, al                          ; return 0
      jmp         Epilog                          ;
BigRealMultiplyColumn ENDP

; class SubProductSum {
; public:
;   Digit        *m_cd;     // pointer to current digit
;   BR2DigitType  m_bigSum; // result stored here if too big for simple division.
; };

; char BigRealSquareColumn(  const Digit *yp, const Digit *xp, SubProductSum *sps, int sumLength);
; rcx yp
; rdx xp
; r8  sps address of SubProductSum
; r9  sumLength
BigRealSquareColumn PROC
      push        rbx                             ;
      push        rsi                             ;
      push        rdi                             ;
                                                  ;
      mov         rbx, rdx                        ; rbx = xp; need rdx for multiplication
      xor         rsi, rsi                        ;
      xor         rdi, rdi                        ; rsi:rdi accumulates sum. Init to 0
      mov         eax, r9d                        ; eax = sumLength
      and         eax, 1                          ;
      jz          EvenSumLength                   ; if(isEven(sumLength)) goto EvenSumLength
      cmp         rbx, rcx                        ;
      je          AddMiddleElement                ; while(xp != yp) {
OddLengthLoop:                                    ;
      mov         rax, QWORD PTR[rbx]             ;   rax      = xp->n
      mul         QWORD PTR[rcx]                  ;   rdx:rax *= yp->n
      add         rdi, rax                        ;
      adc         rsi, rdx                        ;   rsi:rdi += rdx:rax   sum += xp->n * yp->n
      mov         rcx, QWORD PTR[rcx+16]          ;   yp = yp->prev
      mov         rbx, QWORD PTR[rbx+8]           ;   xp = xp->next
      cmp         rbx, rcx                        ;
      jne         OddLengthLoop                   ; } while(xp != yp);
      shld        rsi, rdi, 1                     ;
      shl         rdi, 1                          ; rsi:rdi *= 2             sum *= 2
AddMiddleElement:                                 ;
      mov         rax, QWORD PTR[rbx]             ; rax      = xp->n
      mul         rax                             ; rdx:rax *= yp->n
      add         rdi, rax                        ;
      adc         rsi, rdx                        ; rsi:rdi += rdx:rax       sum += xp->n * yp->n
      jmp         AddSubProduct                   ;
EvenSumLength:                                    ; Assume: r9d is even, rbx = xp
      shr         r9d, 1                          ; r9d /= 2;                sumLength /= 2
EvenLengthLoop:                                   ; for(;;) {
      mov         rax, QWORD PTR[rbx]             ;   rax      = xp->n
      mul         QWORD PTR[rcx]                  ;   rdx:rax *= yp->n
      add         rdi, rax                        ;
      adc         rsi, rdx                        ;   rsi:rdi += rdx:rax     sum += xp->n * yp->n;
      sub         r9d, 1                          ;
      jz          EndEvenLengthLoop               ;   if(!--sumLength) break;
      mov         rcx, QWORD PTR[rcx+16]          ;   yp = yp->prev
      mov         rbx, QWORD PTR[rbx+ 8]          ;   xp = xp->next
      jmp         EvenLengthLoop                  ; }
EndEvenLengthLoop:                                ;
      shld        rsi, rdi, 1                     ;
      shl         rdi, 1                          ; rsi:rdi *= 2             sum *= 2
AddSubProduct:                                    ;
      cmp         rsi, MAXSUM                     ; if(sum >= maxui64 * BASE) goto SumTooBig (or we will have a division by zero exception)
      jae         SumTooBig                       ; Use jae for unsigned compare, jge is for signed !
                                                  ;
      mov         rbx, BASE                       ; rbx remains constant BASE
      mov         rax, rdi                        ;
      mov         rdx, rsi                        ; rdx:rax = rsi:rdi = sum
      div         rbx                             ; rax:rdx = sum/BASE : sum%BASE
      mov         rdi, rax                        ; rdi = sum / BASE (= new sum)
                                                  ; No need to set carry = currentDigit->n (it's just been added => currentDigit->n == 0)
      mov         r8, QWORD PTR[r8]               ; r8 = address of sps->m_cd = currentDigit
      mov         QWORD PTR[r8], rdx              ; currentDigit->n = rdx = carry%BASE = sum%BASE
      or          rdi, rdi                        ;
      je          NextDigit                       ; if(new sum == 0) we're done. Carry is always 0 at this point
AddIntNoCarry:                                    ; Assume 0 < sum in rdi <= maxui64), r8 is addr of last updated digit, Carry = 0
      mov         r8, QWORD PTR[r8+16]            ; currentDigit = currentDigit->prev
      mov         rax, rdi                        ;
      xor         rdx, rdx                        ; rdx:rax = 0:sum
      div         rbx                             ; rax:rdx = sum/BASE : sum%BASE
      mov         rdi, rax                        ; rdi = new sum (=sum/BASE <= maxui64/BASE = 18)
      mov         rax, QWORD PTR[r8]              ; rax = currentDigit->n + (old sum) % BASE
      add         rax, rdx                        ;
      xor         rdx, rdx                        ; rdx:rax = 0:carry = currentDigit->n + (old sum%BASE) <= (BASE-1) * 2
      div         rbx                             ; rax:rdx = carry/BASE : carry%BASE
      mov         QWORD PTR[r8], rdx              ; currentDigit->n = carry%BASE
                                                  ;
      or          rdi, rdi                        ;
      jne         AddIntPossibleCarry             ;
      or          rax, rax                        ; sum = 0
      jne         FinalizeCarryLoop               ; if(carry != 0) handle it in FinalizeCarryLoop, which assumes carry in rax
      jmp         NextDigit                       ; else we're done
AddIntPossibleCarry:                              ; Assume carry in rax. can be 0 or 1
      or          rax, rax                        ;
      je          AddIntNoCarry                   ;
                                                  ; Assume 0 < sum in rdi <= maxui64/BASE (=18), rsi is addr of last updated digit, Carry in rax != 0 (maxvalue = 1)
      mov         r8 , QWORD PTR[r8+16]           ; currentDigit = currentDigit->prev
      add         rax, QWORD PTR[r8]              ; carry += currentDigit->n
      add         rax, rdi                        ; carry += sum % BASE (No need to calculate sum % BASE because sum<BASE=>sum%BASE==sum = edi
      xor         rdx, rdx                        ; rdx:rax = 0:carry
      div         rbx                             ; rax:rdx = carry/BASE : carry%BASE
      mov         QWORD PTR[r8], rdx              ; currentDigit->n = carry % BASE
      or          rax, rax                        ;
      je          NextDigit                       ; if(carry == 0) we're done
FinalizeCarryLoop:                                ; do { // Assume r8 is addr of last updated digit, Carry in rax (!= 0)
      mov         r8 , QWORD PTR[r8+16]           ;   currentDigit = currentDigit->prev
      add         rax, QWORD PTR[r8]              ;   carry += currentDigit->n
      xor         rdx, rdx                        ;   rdx:rax = 0:carry
      div         rbx                             ;   rax:rdx = carry/BASE : carry%BASE
      mov         QWORD PTR[r8], rdx              ;   currentDigit->n = carry % BASE
      or          rax, rax                        ;
      jne         FinalizeCarryLoop               ; } while(carry != 0)
NextDigit:                                        ;
      mov         al, 1                           ; return 1
Epilog:                                           ; restore used registers
      pop         rdi                             ;
      pop         rsi                             ;
      pop         rbx                             ;
      ret                                         ;
SumTooBig:                                        ;
      mov         QWORD PTR[r8+ 8], rdi           ; low order  QWORD to bigSum.lo
      mov         QWORD PTR[r8+16], rsi           ; high order QWORD to bigSum.hi
      xor         al, al                          ; return 0
      jmp         Epilog                          ;
BigRealSquareColumn ENDP

END
