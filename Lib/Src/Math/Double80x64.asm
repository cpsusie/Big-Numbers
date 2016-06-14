.DATA

maxI32   dword 7fffffffh
maxI32P1 dword 80000000h
maxI64   qword 7fffffffffffffffh
maxI64P1 qword 8000000000000000h

.CODE

;---------------------------------------------- Constructors ------------------------------------------------

;void consD80Long(TenByteClass &s, long &x);
consD80Long PROC
    fild	DWORD PTR [rdx]
    fstp	TBYTE PTR [rcx]
    ret
consD80Long ENDP


;void consD80ULong(TenByteClass &s, unsigned long x);
consD80ULong PROC
    cmp	    edx, maxI32
    jbe	    SHORT DoSmallInt32

    and	    edx, maxI32
    push    rdx
    fild	DWORD PTR [rsp]
    push    7fffffffh
    fild    DWORD PTR [rsp]
    faddp
    fld1
    faddp
    fstp	TBYTE PTR [rcx]
    pop		rdx
    pop     rdx
    ret

DoSmallInt32:
    push    rdx
    fild	DWORD PTR [rsp]
    pop     rdx
    fstp	TBYTE PTR [rcx]
    ret
consD80ULong ENDP

; void consD80LongLong(TenByteClass &s, const __int64 &x);
consD80LongLong PROC
    fild	QWORD PTR [rdx]
    fstp	TBYTE PTR [rcx]
    ret
consD80LongLong ENDP

; void consD80ULongLong(TenByteClass &s, const unsigned __int64 x);
consD80ULongLong PROC
    cmp     rdx,0
    jg 	    SHORT DoSmallInt64

    and	    rdx, maxI64
    push    rdx
    fild	QWORD PTR [rsp]
    push    maxI64
    fild    QWORD PTR [rsp]
    faddp
    fld1
    faddp
    fstp	TBYTE PTR [rcx]
    pop     rdx
    pop     rdx
    ret

DoSmallInt64:
    push    rdx
    fild	QWORD PTR [rsp]
    pop     rdx
    fstp	TBYTE PTR [rcx]
    ret
consD80ULongLong ENDP

; void consD80Float(TenByteClass &s, float &x);
consD80Float PROC
    fld		DWORD PTR[rdx]
    fstp	TBYTE PTR[rcx]
    ret
consD80Float ENDP

;void consD80Double(TenByteClass &s, const double &x);
consD80Double PROC
    fld		QWORD PTR[rdx]
    fstp	TBYTE PTR[rcx]
    ret
consD80Double ENDP

; ---------------------- Conversion functions from Double80 -> long,ulong,longlong ulonglong,float,double

;long &D80ToLong(long &dst, const TenByteClass &src);
D80ToLong PROC
    fld		TBYTE PTR[rdx]
    fistp	DWORD PTR[rcx]
    mov		rax, rcx
    ret
D80ToLong ENDP

;unsigned long &D80ToULong(unsigned long &dst, const TenByteClass &src);
D80ToULong PROC
    fld		TBYTE PTR[rdx]
	ficom   maxI32
	fnstsw  ax
	sahf
	jbe     GetSmallInt32

	fild    maxI32P1                 ; x >= maxInt32
	fchs
	fsub
	fistp   DWORD PTR[rcx]
	mov     eax, DWORD PTR[rcx]
	add     eax, maxI32P1
	mov     DWORD PTR[rcx], eax
    mov		rax, rcx
    ret
GetSmallInt32:
    fistp	DWORD PTR[rcx]
    mov		rax, rcx
    ret
D80ToULong ENDP

;long long &D80ToLongLong(long long &dst, const TenByteClass &src);
D80ToLongLong PROC
    fld		TBYTE PTR[rdx]
    fistp	QWORD PTR[rcx]
    mov		rax, rcx
    ret
D80ToLongLong ENDP

;unsigned long long &D80ToULongLong(unsigned long long &dst, const TenByteClass &src);
D80ToULongLong PROC
    fld		TBYTE PTR[rdx]
	fild    maxI64
	fcomip  st, st(1)
	jge     GetSmallInt64

	fild    maxI64P1                 ; x >= maxInt64
	fchs
	fsub
	fistp   QWORD PTR[rcx]
	mov     rax, QWORD PTR[rcx]
	add     rax, maxI64P1
	mov     QWORD PTR[rcx], rax
    mov		rax, rcx
    ret
GetSmallInt64:
    fistp	QWORD PTR[rcx]
    mov		rax, rcx
    ret
D80ToULongLong ENDP

;float &D80ToFloat(float &dst, const TenByteClass &src);
D80ToFloat PROC
    fld		TBYTE PTR [rdx]
    fstp	DWORD PTR[rcx]
    mov		rax, rcx
    ret
D80ToFloat ENDP

;double &D80ToDouble(double &d, const TenByteClass &src);
D80ToDouble PROC
    fld		TBYTE PTR[rdx]
    fstp	QWORD PTR[rcx]
    mov		rax, rcx
    ret
D80ToDouble ENDP

;TenByteClass &sumD80D80(TenByteClass &s, const TenByteClass &x, const TenByteClass &y);
sumD80D80 PROC
    fld		TBYTE PTR[r8]
    fld		TBYTE PTR[rdx]
    faddp
    fstp	TBYTE PTR[rcx]
    mov		rax,rcx
    ret
sumD80D80 ENDP

END
