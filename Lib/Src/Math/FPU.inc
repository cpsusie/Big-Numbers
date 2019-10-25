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

popRoundMode MACRO
    fldcw	WORD PTR[rsp]          ; restore FPU  ctrlWord (=cwSave)
    add		rsp, 8
ENDM

FILDUINT16 MACRO reg
  cmp     reg, 07fffh
  jbe     _LoadSmallUI16
  and     reg, 07fffh
  mov     QWORD ptr[rsp-8], reg
  fild    WORD PTR[rsp-8]
  fadd    _Dmaxi16P1
  jmp     _Done
_LoadSmallUI16:
  mov     QWORD ptr[rsp-8], reg
  fild    WORD PTR[rsp-8]
_Done:
ENDM

FILDUINT32 MACRO reg
  cmp     reg, 07fffffffh
  jbe     _LoadSmallUI32
  and     reg, 07fffffffh
  mov     QWORD ptr[rsp-8], reg
  fild    DWORD PTR[rsp-8]
  fadd    _DmaxI32P1
  jmp     _Done
_LoadSmallUI32:
  mov     QWORD ptr[rsp-8], reg
  fild    DWORD PTR[rsp-8]
_Done:
ENDM

FILDUINT64 MACRO reg
  cmp     reg, _QWmaxI64
  jbe     _LoadSmallUI64
  and     reg, _QWmaxI64
  mov     QWORD ptr[rsp-8], reg
  fild    QWORD PTR[rsp-8]
  fld     _D80maxI64P1
  fadd
  jmp     _Done
_LoadSmallUI64:
  mov     QWORD ptr[rsp-8], reg
  fild    QWORD PTR[rsp-8]
_Done:
ENDM
