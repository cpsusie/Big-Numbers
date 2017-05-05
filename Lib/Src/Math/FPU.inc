ROUND      EQU "0" 
ROUNDDOWN  EQU "1"
ROUNDUP    EQU "2" 
TRUNCATE   EQU "3" 

pushRoundMode MACRO roundMode
    push	0
    fnstcw	  WORD PTR[rsp]      ; save FPU ctrlWord in cwSave (=*rsp)
    mov		ax, WORD PTR[rsp]
ifidn     <roundMode>,<ROUND>
   and    ah, 0f3h		           ; clear bit 10,11
elseifidn <roundMode>,<ROUNDDOWN>
    or	  ah, 04h                ; set bit 10
    and	  ah, 0f7h               ; clear bit 11
elseifidn <roundMode>,<ROUNDUP>
    and   ah, 0fbh               ; clear bit 10
    or    ah, 8h                 ; set bit 11
else
    or    ah, 0ch                ; set bit 10, 11
endif
    mov		WORD PTR[rsp+2], ax    ; crtlflags in rsp[2]
    fldcw	WORD PTR[rsp+2]        ; load new ctrlword
ENDM

popRoundMode MACRO
    fldcw	WORD PTR[rsp]          ; restore FPU  ctrlWord (=cwSave)
    add		rsp, 8
ENDM
