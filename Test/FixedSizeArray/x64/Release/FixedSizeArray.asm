; Listing generated by Microsoft (R) Optimizing Compiler Version 19.27.29112.0 

include listing.inc

INCLUDELIB OLDNAMES

EXTRN	__imp___std_terminate:PROC
EXTRN	__security_check_cookie:PROC
EXTRN	__imp___CxxFrameHandler4:PROC
PUBLIC	?__autoclassinit2@String@@QEAAX_K@Z		; String::__autoclassinit2
PUBLIC	??YString@@QEAAAEAV0@_W@Z			; String::operator+=
PUBLIC	??$initList@VDouble80@@@?$FixedSizeVectorTemplate@N$02@@AEAAXIAEBVDouble80@@@Z ; FixedSizeVectorTemplate<double,3>::initList<Double80>
PUBLIC	??$initList@VDouble80@@V1@@?$FixedSizeVectorTemplate@N$02@@AEAAXIAEBVDouble80@@V1@@Z ; FixedSizeVectorTemplate<double,3>::initList<Double80,Double80>
PUBLIC	??$initList@VDouble80@@V1@V1@@?$FixedSizeVectorTemplate@N$02@@AEAAXIAEBVDouble80@@V1@1@Z ; FixedSizeVectorTemplate<double,3>::initList<Double80,Double80,Double80>
PUBLIC	??$?0VDouble80@@V0@V0@@?$FixedSizeVectorTemplate@N$02@@QEAA@AEBVDouble80@@V1@1@Z ; FixedSizeVectorTemplate<double,3>::FixedSizeVectorTemplate<double,3><Double80,Double80,Double80>
PUBLIC	?toString@?$FixedSizeVectorTemplate@N$02@@QEBA?AVString@@H@Z ; FixedSizeVectorTemplate<double,3>::toString
PUBLIC	main
;	COMDAT pdata
pdata	SEGMENT
$pdata$??YString@@QEAAAEAV0@_W@Z DD imagerel $LN4
	DD	imagerel $LN4+59
	DD	imagerel $unwind$??YString@@QEAAAEAV0@_W@Z
pdata	ENDS
;	COMDAT pdata
pdata	SEGMENT
$pdata$??$initList@VDouble80@@@?$FixedSizeVectorTemplate@N$02@@AEAAXIAEBVDouble80@@@Z DD imagerel $LN4
	DD	imagerel $LN4+28
	DD	imagerel $unwind$??$initList@VDouble80@@@?$FixedSizeVectorTemplate@N$02@@AEAAXIAEBVDouble80@@@Z
pdata	ENDS
;	COMDAT pdata
pdata	SEGMENT
$pdata$??$initList@VDouble80@@V1@@?$FixedSizeVectorTemplate@N$02@@AEAAXIAEBVDouble80@@V1@@Z DD imagerel $LN6
	DD	imagerel $LN6+53
	DD	imagerel $unwind$??$initList@VDouble80@@V1@@?$FixedSizeVectorTemplate@N$02@@AEAAXIAEBVDouble80@@V1@@Z
pdata	ENDS
;	COMDAT pdata
pdata	SEGMENT
$pdata$??$initList@VDouble80@@V1@V1@@?$FixedSizeVectorTemplate@N$02@@AEAAXIAEBVDouble80@@V1@1@Z DD imagerel $LN8
	DD	imagerel $LN8+105
	DD	imagerel $unwind$??$initList@VDouble80@@V1@V1@@?$FixedSizeVectorTemplate@N$02@@AEAAXIAEBVDouble80@@V1@1@Z
pdata	ENDS
;	COMDAT pdata
pdata	SEGMENT
$pdata$??$?0VDouble80@@V0@V0@@?$FixedSizeVectorTemplate@N$02@@QEAA@AEBVDouble80@@V1@1@Z DD imagerel $LN10
	DD	imagerel $LN10+125
	DD	imagerel $unwind$??$?0VDouble80@@V0@V0@@?$FixedSizeVectorTemplate@N$02@@QEAA@AEBVDouble80@@V1@1@Z
pdata	ENDS
;	COMDAT pdata
pdata	SEGMENT
$pdata$?toString@?$FixedSizeVectorTemplate@N$02@@QEBA?AVString@@H@Z DD imagerel $LN46
	DD	imagerel $LN46+595
	DD	imagerel $unwind$?toString@?$FixedSizeVectorTemplate@N$02@@QEBA?AVString@@H@Z
pdata	ENDS
;	COMDAT pdata
pdata	SEGMENT
$pdata$?dtor$0@?0??toString@?$FixedSizeVectorTemplate@N$02@@QEBA?AVString@@H@Z@4HA DD imagerel ?dtor$0@?0??toString@?$FixedSizeVectorTemplate@N$02@@QEBA?AVString@@H@Z@4HA
	DD	imagerel ?dtor$0@?0??toString@?$FixedSizeVectorTemplate@N$02@@QEBA?AVString@@H@Z@4HA+38
	DD	imagerel $unwind$?dtor$0@?0??toString@?$FixedSizeVectorTemplate@N$02@@QEBA?AVString@@H@Z@4HA
pdata	ENDS
;	COMDAT pdata
pdata	SEGMENT
$pdata$main DD	imagerel $LN18
	DD	imagerel $LN18+206
	DD	imagerel $unwind$main
pdata	ENDS
;	COMDAT xdata
xdata	SEGMENT
$unwind$main DD	052019H
	DD	086811H
	DD	0120109H
	DD	03002H
	DD	imagerel __GSHandlerCheck
	DD	070H
xdata	ENDS
;	COMDAT xdata
xdata	SEGMENT
$unwind$?dtor$0@?0??toString@?$FixedSizeVectorTemplate@N$02@@QEBA?AVString@@H@Z@4HA DD 020601H
	DD	050023206H
xdata	ENDS
;	COMDAT xdata
xdata	SEGMENT
$ip2state$?toString@?$FixedSizeVectorTemplate@N$02@@QEBA?AVString@@H@Z DB 06H
	DB	09aH
	DB	02H
	DB	0c1H, 03H
	DB	04H
	DB	018H
	DB	02H
xdata	ENDS
;	COMDAT xdata
xdata	SEGMENT
$stateUnwindMap$?toString@?$FixedSizeVectorTemplate@N$02@@QEBA?AVString@@H@Z DB 04H
	DB	0eH
	DD	imagerel ?dtor$0@?0??toString@?$FixedSizeVectorTemplate@N$02@@QEBA?AVString@@H@Z@4HA
	DB	02aH
	DD	imagerel ??1String@@QEAA@XZ
	DB	080H
xdata	ENDS
;	COMDAT xdata
xdata	SEGMENT
$cppxdata$?toString@?$FixedSizeVectorTemplate@N$02@@QEBA?AVString@@H@Z DB 028H
	DD	imagerel $stateUnwindMap$?toString@?$FixedSizeVectorTemplate@N$02@@QEBA?AVString@@H@Z
	DD	imagerel $ip2state$?toString@?$FixedSizeVectorTemplate@N$02@@QEBA?AVString@@H@Z
xdata	ENDS
;	COMDAT xdata
xdata	SEGMENT
$unwind$?toString@?$FixedSizeVectorTemplate@N$02@@QEBA?AVString@@H@Z DD 0a2319H
	DD	0183414H
	DD	0f010d214H
	DD	0d00ce00eH
	DD	07008c00aH
	DD	050066007H
	DD	imagerel __GSHandlerCheck_EH4
	DD	imagerel $cppxdata$?toString@?$FixedSizeVectorTemplate@N$02@@QEBA?AVString@@H@Z
	DD	062H
xdata	ENDS
;	COMDAT xdata
xdata	SEGMENT
$unwind$??$?0VDouble80@@V0@V0@@?$FixedSizeVectorTemplate@N$02@@QEAA@AEBVDouble80@@V1@1@Z DD 062801H
	DD	046828H
	DD	0c340aH
	DD	07006920aH
xdata	ENDS
;	COMDAT xdata
xdata	SEGMENT
$unwind$??$initList@VDouble80@@V1@V1@@?$FixedSizeVectorTemplate@N$02@@AEAAXIAEBVDouble80@@V1@1@Z DD 060f01H
	DD	0a640fH
	DD	09340fH
	DD	0700b520fH
xdata	ENDS
;	COMDAT xdata
xdata	SEGMENT
$unwind$??$initList@VDouble80@@V1@@?$FixedSizeVectorTemplate@N$02@@AEAAXIAEBVDouble80@@V1@@Z DD 040a01H
	DD	07340aH
	DD	07006320aH
xdata	ENDS
;	COMDAT xdata
xdata	SEGMENT
$unwind$??$initList@VDouble80@@@?$FixedSizeVectorTemplate@N$02@@AEAAXIAEBVDouble80@@@Z DD 020601H
	DD	030023206H
xdata	ENDS
;	COMDAT xdata
xdata	SEGMENT
$unwind$??YString@@QEAAAEAV0@_W@Z DD 011319H
	DD	06204H
	DD	imagerel __GSHandlerCheck
	DD	028H
; Function compile flags: /Ogtpy
; File C:\Mytools2015\Test\FixedSizeArray\FixedSizeArray.cpp
; File C:\Mytools2015\Lib\Src\Util\MyString.cpp
; File C:\Mytools2015\Test\FixedSizeArray\FixedSizeArray.cpp
;	COMDAT main
_TEXT	SEGMENT
$T1 = 32
$T2 = 48
v1$ = 64
str$ = 88
__$ArrayPad$ = 112
argc$ = 160
argv$ = 168
main	PROC						; COMDAT

; 243  : int main(int argc, TCHAR **argv) {

$LN18:
	push	rbx
	sub	rsp, 144				; 00000090H
	movaps	XMMWORD PTR [rsp+128], xmm6
	mov	rax, QWORD PTR __security_cookie
	xor	rax, rsp
	mov	QWORD PTR __$ArrayPad$[rsp], rax

; 39   :     initList(0, x0, tail...);

	movsd	xmm0, QWORD PTR ??_C@_0L@HIODJCBP@?$AA?$AA?$AA?$AA?$AA?$AA?$AA?$IA?$AB?$AA@

; 15   :     m_v[i] = (T)v;

	lea	rcx, OFFSET FLAT:??_C@_0L@HFGOBLGC@?$PP?$PP?$PP?$PP?$PP?$PP?$PP?$PP?$PO?$HP@

; 39   :     initList(0, x0, tail...);

	movzx	eax, WORD PTR ??_C@_0L@HIODJCBP@?$AA?$AA?$AA?$AA?$AA?$AA?$AA?$IA?$AB?$AA@+8
	movsd	xmm6, QWORD PTR ??_C@_0L@LAFAEEFE@?$AA?$AA?$AA?$AA?$AA?$AA?$AA?$IA?$MA?$DP@
	movzx	ebx, WORD PTR ??_C@_0L@LAFAEEFE@?$AA?$AA?$AA?$AA?$AA?$AA?$AA?$IA?$MA?$DP@+8
	movsd	QWORD PTR $T1[rsp], xmm0
	mov	WORD PTR $T1[rsp+8], ax

; 15   :     m_v[i] = (T)v;

	call	??BDouble80@@QEBANXZ			; Double80::operator double
	lea	rcx, QWORD PTR $T1[rsp]
	movsd	QWORD PTR v1$[rsp], xmm0

; 16   :     initList(i+1, tail...);

	movsd	QWORD PTR $T2[rsp], xmm6
	mov	WORD PTR $T2[rsp+8], bx

; 15   :     m_v[i] = (T)v;

	call	??BDouble80@@QEBANXZ			; Double80::operator double

; 11   :     m_v[i] = (T)v;

	lea	rcx, QWORD PTR $T2[rsp]

; 15   :     m_v[i] = (T)v;

	movsd	QWORD PTR v1$[rsp+8], xmm0

; 11   :     m_v[i] = (T)v;

	call	??BDouble80@@QEBANXZ			; Double80::operator double
	movsd	QWORD PTR v1$[rsp+16], xmm0

; 244  :   const DVector3D v1(DBL80_MAX, DBL80_MIN, DBL80_EPSILON);
; 245  : /*
; 246  :   int ssi3[] = { 2,4,8 };
; 247  :   double ssd3[] = { 2.6,-4.4,8.3 };
; 248  :   double ssd2[] = { 2.6,-4.4 };
; 249  :   const Vector3D v2(5, 6, 10);
; 250  :   const Vector3D v3(ssi3);
; 251  :   const Vector3D v4(ssd3);
; 252  :   Vector3D v5(ssd2);
; 253  :   const Vector3D v6(v2);
; 254  :   v5 = v1;
; 255  :   Vector3D s = v1 + v2;
; 256  :   FVector3D fv(v1);
; 257  :   double p1 = v1 * v2;
; 258  :   double p2 = fv * v2;
; 259  :   FVector3D fv1 = 2 * v1;
; 260  :   FVector3D fv2 = -v1;
; 261  : */
; 262  :   String str = v1.toString();

	lea	rdx, QWORD PTR str$[rsp]
	xorps	xmm0, xmm0
	lea	rcx, QWORD PTR v1$[rsp]
	xor	eax, eax
	movups	XMMWORD PTR str$[rsp], xmm0
	mov	QWORD PTR str$[rsp+16], rax
	call	?toString@?$FixedSizeVectorTemplate@N$02@@QEBA?AVString@@H@Z ; FixedSizeVectorTemplate<double,3>::toString
; File C:\Mytools2015\Lib\Src\Util\MyString.cpp

; 28   :   delete[] m_buf;

	mov	rcx, QWORD PTR str$[rsp+16]
	call	??_V@YAXPEAX@Z				; operator delete[]
; File C:\Mytools2015\Test\FixedSizeArray\FixedSizeArray.cpp

; 264  :   return 0;

	xor	eax, eax

; 265  : }

	mov	rcx, QWORD PTR __$ArrayPad$[rsp]
	xor	rcx, rsp
	call	__security_check_cookie
	movaps	xmm6, XMMWORD PTR [rsp+128]
	add	rsp, 144				; 00000090H
	pop	rbx
	ret	0
main	ENDP
_TEXT	ENDS
; Function compile flags: /Ogtpy
; File C:\Mytools2015\Test\FixedSizeArray\FixedSizeArray.cpp
; File C:\Mytools2015\Lib\Src\Util\MyString.cpp
; File C:\Mytools2015\Test\FixedSizeArray\FixedSizeArray.cpp
; File C:\mytools2015\Lib\Include\MyString.h
; File C:\Mytools2015\Lib\Src\Util\MyString.cpp
; File C:\Program Files (x86)\Windows Kits\10\Include\10.0.18362.0\ucrt\wchar.h
; File C:\Mytools2015\Lib\Src\Util\MyString.cpp
; File C:\Mytools2015\Test\FixedSizeArray\FixedSizeArray.cpp
; File C:\Mytools2015\Lib\Src\Util\MyString.cpp
; File C:\Mytools2015\Test\FixedSizeArray\FixedSizeArray.cpp
; File C:\mytools2015\Lib\Include\MyString.h
; File C:\Mytools2015\Lib\Src\Util\MyString.cpp
; File C:\Program Files (x86)\Windows Kits\10\Include\10.0.18362.0\ucrt\wchar.h
; File C:\Mytools2015\Lib\Src\Util\MyString.cpp
; File C:\Mytools2015\Test\FixedSizeArray\FixedSizeArray.cpp
;	COMDAT ?toString@?$FixedSizeVectorTemplate@N$02@@QEBA?AVString@@H@Z
_TEXT	SEGMENT
$T1 = 48
__$ReturnUdt$GSCopy$ = 56
$T2 = 64
tmp$3 = 88
tmp$4 = 88
__$ArrayPad$ = 96
this$ = 176
__$ReturnUdt$ = 184
precision$dead$ = 192
?toString@?$FixedSizeVectorTemplate@N$02@@QEBA?AVString@@H@Z PROC ; FixedSizeVectorTemplate<double,3>::toString, COMDAT

; 227  :   inline String toString(int precision = 3) const {

$LN46:
	mov	QWORD PTR [rsp+24], rbx
	push	rbp
	push	rsi
	push	rdi
	push	r12
	push	r13
	push	r14
	push	r15
	sub	rsp, 112				; 00000070H
	mov	rax, QWORD PTR __security_cookie
	xor	rax, rsp
	mov	QWORD PTR __$ArrayPad$[rsp], rax
	mov	rdi, rdx
	mov	rbp, rcx
	mov	QWORD PTR __$ReturnUdt$GSCopy$[rsp], rdx
	xor	r13d, r13d
	mov	DWORD PTR $T1[rsp], r13d
	xorps	xmm0, xmm0
	xor	eax, eax
	movups	XMMWORD PTR [rdx], xmm0
	mov	QWORD PTR [rdx+16], rax
; File C:\Mytools2015\Lib\Src\Util\MyString.cpp

; 13   :   m_buf = newCharBuffer(EMPTYSTRING, m_len, m_capacity);

	lea	r8, QWORD PTR [rdx+8]
	lea	rcx, OFFSET FLAT:??_C@_11LOCGONAA@@
	call	?newCharBuffer@String@@CAPEA_WPEB_WAEA_K1@Z ; String::newCharBuffer
	mov	QWORD PTR [rdi+16], rax
; File C:\Mytools2015\Test\FixedSizeArray\FixedSizeArray.cpp

; 235  :     return result;

	mov	DWORD PTR $T1[rsp], 1

; 228  :     String result;
; 229  :     TCHAR delim = '(';

	mov	eax, 40					; 00000028H

; 227  :   inline String toString(int precision = 3) const {

	lea	r12d, QWORD PTR [r13+3]
	mov	r8, -1
	npad	2
$LL4@toString:
; File C:\mytools2015\Lib\Include\MyString.h

; 197  :     const TCHAR tmp[] = { ch, 0 };

	mov	WORD PTR tmp$4[rsp], ax
	mov	WORD PTR tmp$4[rsp+2], r13w
; File C:\Mytools2015\Lib\Src\Util\MyString.cpp

; 83   :   const size_t length = _tcsclen(mrhs);

	lea	rax, QWORD PTR tmp$4[rsp]
	mov	rbx, r8
$LL42@toString:
	inc	rbx
	cmp	WORD PTR [rax+rbx*2], 0
	jne	SHORT $LL42@toString

; 84   :   if(length == 0) {

	test	rbx, rbx
	je	$LN18@toString

; 85   :     return *this;
; 86   :   }
; 87   :   const size_t newLength = m_len + length;

	mov	rcx, QWORD PTR [rdi]
	lea	rsi, QWORD PTR [rcx+rbx]

; 88   :   if(m_capacity < newLength + 1) {

	mov	r14, QWORD PTR [rdi+16]
	lea	rax, QWORD PTR [rsi+1]
	cmp	QWORD PTR [rdi+8], rax
	jae	SHORT $LN17@toString

; 89   :     TCHAR *old = m_buf;
; 90   :     m_buf = new TCHAR[m_capacity = (newLength + 1) * 3];

	lea	rcx, QWORD PTR [rsi+1]
	lea	rcx, QWORD PTR [rcx+rcx*2]
	mov	QWORD PTR [rdi+8], rcx
	mov	eax, 2
	mul	rcx
	cmovo	rax, r8
	mov	rcx, rax
	call	??_U@YAPEAX_K@Z				; operator new[]
	mov	QWORD PTR [rdi+16], rax

; 91   :     TMEMCPY(m_buf, old, m_len);

	mov	r8, QWORD PTR [rdi]
; File C:\Program Files (x86)\Windows Kits\10\Include\10.0.18362.0\ucrt\wchar.h

; 233  :         return (wchar_t*)memcpy(_S1, _S2, _N*sizeof(wchar_t));

	add	r8, r8
	mov	rdx, r14
	mov	rcx, rax
	call	memcpy
; File C:\Mytools2015\Lib\Src\Util\MyString.cpp

; 92   :     _tcsncpy(m_buf + m_len, mrhs, length);

	mov	rdx, QWORD PTR [rdi]
	mov	rax, QWORD PTR [rdi+16]
	lea	rcx, QWORD PTR [rax+rdx*2]
	mov	r8, rbx
	lea	rdx, QWORD PTR tmp$4[rsp]
	call	QWORD PTR __imp_wcsncpy

; 93   :     m_buf[m_len = newLength] = '\0';

	mov	QWORD PTR [rdi], rsi
	mov	rax, QWORD PTR [rdi+16]
	mov	WORD PTR [rax+rsi*2], r13w

; 94   :     delete[] old; // Dont delete old before now, rhs and old might overlap

	mov	rcx, r14
	call	??_V@YAXPEAX@Z				; operator delete[]

; 95   :   } else {

	jmp	SHORT $LN18@toString
$LN17@toString:

; 96   :     _tcsncpy(m_buf + m_len, mrhs, length); // Dont use strcpy. rhs and m_buf might overlap, which would give an infinite loop

	lea	rcx, QWORD PTR [r14+rcx*2]
	mov	r8, rbx
	lea	rdx, QWORD PTR tmp$4[rsp]
	call	QWORD PTR __imp_wcsncpy

; 97   :     m_buf[m_len = newLength] = '\0';

	mov	QWORD PTR [rdi], rsi
	mov	rax, QWORD PTR [rdi+16]
	mov	WORD PTR [rax+rsi*2], r13w
$LN18@toString:
; File C:\Mytools2015\Test\FixedSizeArray\FixedSizeArray.cpp

; 232  :       result += ::toString(m_v[i], precision);

	movsd	xmm1, QWORD PTR [rbp]
	lea	rcx, QWORD PTR $T2[rsp]
	call	?toString@@YA?AVString@@N_J0H@Z		; toString
	npad	1
	mov	rdx, rax
	mov	rcx, rdi
	call	??YString@@QEAAAEAV0@AEBV0@@Z		; String::operator+=
	npad	1
; File C:\Mytools2015\Lib\Src\Util\MyString.cpp

; 28   :   delete[] m_buf;

	mov	rcx, QWORD PTR $T2[rsp+16]
	call	??_V@YAXPEAX@Z				; operator delete[]
; File C:\Mytools2015\Test\FixedSizeArray\FixedSizeArray.cpp

; 230  :     for(UINT i = 0; i < dimension; i++, delim = ',') {

	add	rbp, 8
	mov	eax, 44					; 0000002cH
	sub	r12, 1
	mov	r8, -1
	jne	$LL4@toString
; File C:\mytools2015\Lib\Include\MyString.h

; 197  :     const TCHAR tmp[] = { ch, 0 };

	mov	DWORD PTR tmp$3[rsp], 41		; 00000029H
; File C:\Mytools2015\Lib\Src\Util\MyString.cpp

; 83   :   const size_t length = _tcsclen(mrhs);

	lea	rax, QWORD PTR tmp$3[rsp]
	mov	rbx, r8
	npad	3
$LL41@toString:
	inc	rbx
	cmp	WORD PTR [rax+rbx*2], 0
	jne	SHORT $LL41@toString

; 84   :   if(length == 0) {

	test	rbx, rbx
	je	$LN43@toString

; 85   :     return *this;
; 86   :   }
; 87   :   const size_t newLength = m_len + length;

	mov	rdx, QWORD PTR [rdi]
	lea	rsi, QWORD PTR [rdx+rbx]

; 88   :   if(m_capacity < newLength + 1) {

	mov	r14, QWORD PTR [rdi+16]
	lea	rcx, QWORD PTR [rsi+1]
	cmp	QWORD PTR [rdi+8], rcx
	jae	SHORT $LN30@toString

; 89   :     TCHAR *old = m_buf;
; 90   :     m_buf = new TCHAR[m_capacity = (newLength + 1) * 3];

	lea	rcx, QWORD PTR [rsi+1]
	lea	rcx, QWORD PTR [rcx+rcx*2]
	mov	QWORD PTR [rdi+8], rcx
	mov	eax, 2
	mul	rcx
	cmovo	rax, r8
	mov	rcx, rax
	call	??_U@YAPEAX_K@Z				; operator new[]
	mov	QWORD PTR [rdi+16], rax
; File C:\Program Files (x86)\Windows Kits\10\Include\10.0.18362.0\ucrt\wchar.h

; 233  :         return (wchar_t*)memcpy(_S1, _S2, _N*sizeof(wchar_t));

	mov	r8, QWORD PTR [rdi]
	add	r8, r8
	mov	rdx, r14
	mov	rcx, rax
	call	memcpy
; File C:\Mytools2015\Lib\Src\Util\MyString.cpp

; 92   :     _tcsncpy(m_buf + m_len, mrhs, length);

	mov	r9, QWORD PTR [rdi]
	mov	rdx, QWORD PTR [rdi+16]
	lea	rcx, QWORD PTR [rdx+r9*2]
	mov	r8, rbx
	lea	rdx, QWORD PTR tmp$3[rsp]
	call	QWORD PTR __imp_wcsncpy

; 93   :     m_buf[m_len = newLength] = '\0';

	mov	QWORD PTR [rdi], rsi
	mov	rdx, QWORD PTR [rdi+16]
	mov	WORD PTR [rdx+rsi*2], r13w

; 94   :     delete[] old; // Dont delete old before now, rhs and old might overlap

	mov	rcx, r14
	call	??_V@YAXPEAX@Z				; operator delete[]

; 95   :   } else {

	jmp	SHORT $LN43@toString
$LN30@toString:

; 96   :     _tcsncpy(m_buf + m_len, mrhs, length); // Dont use strcpy. rhs and m_buf might overlap, which would give an infinite loop

	lea	rcx, QWORD PTR [r14+rdx*2]
	mov	r8, rbx
	lea	rdx, QWORD PTR tmp$3[rsp]
	call	QWORD PTR __imp_wcsncpy

; 97   :     m_buf[m_len = newLength] = '\0';

	mov	QWORD PTR [rdi], rsi
	mov	rax, QWORD PTR [rdi+16]
	mov	WORD PTR [rax+rsi*2], r13w
$LN43@toString:
; File C:\Mytools2015\Test\FixedSizeArray\FixedSizeArray.cpp

; 236  :   }

	mov	rax, rdi
	mov	rcx, QWORD PTR __$ArrayPad$[rsp]
	xor	rcx, rsp
	call	__security_check_cookie
	mov	rbx, QWORD PTR [rsp+192]
	add	rsp, 112				; 00000070H
	pop	r15
	pop	r14
	pop	r13
	pop	r12
	pop	rdi
	pop	rsi
	pop	rbp
	ret	0
	int	3
?toString@?$FixedSizeVectorTemplate@N$02@@QEBA?AVString@@H@Z ENDP ; FixedSizeVectorTemplate<double,3>::toString
_TEXT	ENDS
;	COMDAT text$x
text$x	SEGMENT
$T1 = 48
__$ReturnUdt$GSCopy$ = 56
$T2 = 64
tmp$3 = 88
tmp$4 = 88
__$ArrayPad$ = 96
this$ = 176
__$ReturnUdt$ = 184
precision$dead$ = 192
?dtor$0@?0??toString@?$FixedSizeVectorTemplate@N$02@@QEBA?AVString@@H@Z@4HA PROC ; `FixedSizeVectorTemplate<double,3>::toString'::`1'::dtor$0
	push	rbp
	sub	rsp, 32					; 00000020H
	mov	rbp, rdx
	mov	eax, DWORD PTR $T1[rbp]
	and	eax, 1
	test	eax, eax
	je	SHORT $LN7@dtor$0
	and	DWORD PTR $T1[rbp], -2
	mov	rcx, QWORD PTR __$ReturnUdt$GSCopy$[rbp]
	call	??1String@@QEAA@XZ			; String::~String
$LN7@dtor$0:
	add	rsp, 32					; 00000020H
	pop	rbp
	ret	0
?dtor$0@?0??toString@?$FixedSizeVectorTemplate@N$02@@QEBA?AVString@@H@Z@4HA ENDP ; `FixedSizeVectorTemplate<double,3>::toString'::`1'::dtor$0
text$x	ENDS
;	COMDAT text$x
text$x	SEGMENT
$T1 = 48
__$ReturnUdt$GSCopy$ = 56
$T2 = 64
tmp$3 = 88
tmp$4 = 88
__$ArrayPad$ = 96
this$ = 176
__$ReturnUdt$ = 184
precision$dead$ = 192
?dtor$1@?0??toString@?$FixedSizeVectorTemplate@N$02@@QEBA?AVString@@H@Z@4HA PROC ; `FixedSizeVectorTemplate<double,3>::toString'::`1'::dtor$1
	lea	rcx, QWORD PTR $T2[rdx]
	jmp	??1String@@QEAA@XZ			; String::~String
?dtor$1@?0??toString@?$FixedSizeVectorTemplate@N$02@@QEBA?AVString@@H@Z@4HA ENDP ; `FixedSizeVectorTemplate<double,3>::toString'::`1'::dtor$1
text$x	ENDS
; Function compile flags: /Ogtpy
;	COMDAT text$x
text$x	SEGMENT
$T1 = 48
__$ReturnUdt$GSCopy$ = 56
$T2 = 64
tmp$3 = 88
tmp$4 = 88
__$ArrayPad$ = 96
this$ = 176
__$ReturnUdt$ = 184
precision$dead$ = 192
?dtor$0@?0??toString@?$FixedSizeVectorTemplate@N$02@@QEBA?AVString@@H@Z@4HA PROC ; `FixedSizeVectorTemplate<double,3>::toString'::`1'::dtor$0
	push	rbp
	sub	rsp, 32					; 00000020H
	mov	rbp, rdx
	mov	eax, DWORD PTR $T1[rbp]
	and	eax, 1
	test	eax, eax
	je	SHORT $LN7@dtor$0
	and	DWORD PTR $T1[rbp], -2
	mov	rcx, QWORD PTR __$ReturnUdt$GSCopy$[rbp]
	call	??1String@@QEAA@XZ			; String::~String
$LN7@dtor$0:
	add	rsp, 32					; 00000020H
	pop	rbp
	ret	0
?dtor$0@?0??toString@?$FixedSizeVectorTemplate@N$02@@QEBA?AVString@@H@Z@4HA ENDP ; `FixedSizeVectorTemplate<double,3>::toString'::`1'::dtor$0
text$x	ENDS
; Function compile flags: /Ogtpy
;	COMDAT text$x
text$x	SEGMENT
$T1 = 48
__$ReturnUdt$GSCopy$ = 56
$T2 = 64
tmp$3 = 88
tmp$4 = 88
__$ArrayPad$ = 96
this$ = 176
__$ReturnUdt$ = 184
precision$dead$ = 192
?dtor$1@?0??toString@?$FixedSizeVectorTemplate@N$02@@QEBA?AVString@@H@Z@4HA PROC ; `FixedSizeVectorTemplate<double,3>::toString'::`1'::dtor$1
	lea	rcx, QWORD PTR $T2[rdx]
	jmp	??1String@@QEAA@XZ			; String::~String
?dtor$1@?0??toString@?$FixedSizeVectorTemplate@N$02@@QEBA?AVString@@H@Z@4HA ENDP ; `FixedSizeVectorTemplate<double,3>::toString'::`1'::dtor$1
text$x	ENDS
; Function compile flags: /Ogtpy
; File C:\Mytools2015\Test\FixedSizeArray\FixedSizeArray.cpp
;	COMDAT ??$?0VDouble80@@V0@V0@@?$FixedSizeVectorTemplate@N$02@@QEAA@AEBVDouble80@@V1@1@Z
_TEXT	SEGMENT
$T1 = 32
$T2 = 48
this$ = 96
x0$dead$ = 104
<tail_0>$ = 112
<tail_1>$ = 120
??$?0VDouble80@@V0@V0@@?$FixedSizeVectorTemplate@N$02@@QEAA@AEBVDouble80@@V1@1@Z PROC ; FixedSizeVectorTemplate<double,3>::FixedSizeVectorTemplate<double,3><Double80,Double80,Double80>, COMDAT

; 38   :   template<typename S, typename... Tail> inline FixedSizeVectorTemplate(const S &x0, Tail... tail) {

$LN10:
	mov	QWORD PTR [rsp+8], rbx
	push	rdi
	sub	rsp, 80					; 00000050H

; 39   :     initList(0, x0, tail...);

	movsd	xmm0, QWORD PTR [r8]
	mov	rdi, rcx
	movzx	eax, WORD PTR [r8+8]

; 15   :     m_v[i] = (T)v;

	lea	rcx, OFFSET FLAT:??_C@_0L@HFGOBLGC@?$PP?$PP?$PP?$PP?$PP?$PP?$PP?$PP?$PO?$HP@

; 39   :     initList(0, x0, tail...);

	movzx	ebx, WORD PTR [r9+8]
	movaps	XMMWORD PTR [rsp+64], xmm6
	movsd	xmm6, QWORD PTR [r9]
	movsd	QWORD PTR $T1[rsp], xmm0
	mov	WORD PTR $T1[rsp+8], ax

; 15   :     m_v[i] = (T)v;

	call	??BDouble80@@QEBANXZ			; Double80::operator double
	lea	rcx, QWORD PTR $T1[rsp]
	movsd	QWORD PTR [rdi], xmm0

; 16   :     initList(i+1, tail...);

	movsd	QWORD PTR $T2[rsp], xmm6
	mov	WORD PTR $T2[rsp+8], bx

; 15   :     m_v[i] = (T)v;

	call	??BDouble80@@QEBANXZ			; Double80::operator double

; 11   :     m_v[i] = (T)v;

	lea	rcx, QWORD PTR $T2[rsp]

; 15   :     m_v[i] = (T)v;

	movsd	QWORD PTR [rdi+8], xmm0

; 11   :     m_v[i] = (T)v;

	call	??BDouble80@@QEBANXZ			; Double80::operator double

; 40   :   }

	mov	rbx, QWORD PTR [rsp+96]
	mov	rax, rdi
	movaps	xmm6, XMMWORD PTR [rsp+64]

; 11   :     m_v[i] = (T)v;

	movsd	QWORD PTR [rdi+16], xmm0

; 40   :   }

	add	rsp, 80					; 00000050H
	pop	rdi
	ret	0
??$?0VDouble80@@V0@V0@@?$FixedSizeVectorTemplate@N$02@@QEAA@AEBVDouble80@@V1@1@Z ENDP ; FixedSizeVectorTemplate<double,3>::FixedSizeVectorTemplate<double,3><Double80,Double80,Double80>
_TEXT	ENDS
; Function compile flags: /Ogtpy
; File C:\Mytools2015\Test\FixedSizeArray\FixedSizeArray.cpp
;	COMDAT ??$initList@VDouble80@@V1@V1@@?$FixedSizeVectorTemplate@N$02@@AEAAXIAEBVDouble80@@V1@1@Z
_TEXT	SEGMENT
$T1 = 32
this$ = 64
i$dead$ = 72
v$dead$ = 80
<tail_0>$ = 88
<tail_1>$ = 96
??$initList@VDouble80@@V1@V1@@?$FixedSizeVectorTemplate@N$02@@AEAAXIAEBVDouble80@@V1@1@Z PROC ; FixedSizeVectorTemplate<double,3>::initList<Double80,Double80,Double80>, COMDAT

; 13   :   template<typename V, typename... Tail> inline void initList(UINT i, const V &v, Tail... tail) {

$LN8:
	mov	QWORD PTR [rsp+16], rbx
	mov	QWORD PTR [rsp+24], rsi
	push	rdi
	sub	rsp, 48					; 00000030H
	mov	rbx, QWORD PTR <tail_1>$[rsp]
	mov	rsi, rcx

; 14   :     assert(i < dimension);
; 15   :     m_v[i] = (T)v;

	lea	rcx, OFFSET FLAT:??_C@_0L@HFGOBLGC@?$PP?$PP?$PP?$PP?$PP?$PP?$PP?$PP?$PO?$HP@
	mov	rdi, r9
	call	??BDouble80@@QEBANXZ			; Double80::operator double

; 16   :     initList(i+1, tail...);

	movsd	xmm1, QWORD PTR [rbx]

; 15   :     m_v[i] = (T)v;

	mov	rcx, rdi

; 16   :     initList(i+1, tail...);

	movzx	eax, WORD PTR [rbx+8]
	movsd	QWORD PTR $T1[rsp], xmm1
	mov	WORD PTR $T1[rsp+8], ax
	movsd	QWORD PTR [rsi], xmm0

; 15   :     m_v[i] = (T)v;

	call	??BDouble80@@QEBANXZ			; Double80::operator double

; 11   :     m_v[i] = (T)v;

	lea	rcx, QWORD PTR $T1[rsp]

; 15   :     m_v[i] = (T)v;

	movsd	QWORD PTR [rsi+8], xmm0

; 11   :     m_v[i] = (T)v;

	call	??BDouble80@@QEBANXZ			; Double80::operator double

; 17   :   }

	mov	rbx, QWORD PTR [rsp+72]

; 11   :     m_v[i] = (T)v;

	movsd	QWORD PTR [rsi+16], xmm0

; 17   :   }

	mov	rsi, QWORD PTR [rsp+80]
	add	rsp, 48					; 00000030H
	pop	rdi
	ret	0
??$initList@VDouble80@@V1@V1@@?$FixedSizeVectorTemplate@N$02@@AEAAXIAEBVDouble80@@V1@1@Z ENDP ; FixedSizeVectorTemplate<double,3>::initList<Double80,Double80,Double80>
_TEXT	ENDS
; Function compile flags: /Ogtpy
; File C:\Mytools2015\Test\FixedSizeArray\FixedSizeArray.cpp
;	COMDAT ??$initList@VDouble80@@V1@@?$FixedSizeVectorTemplate@N$02@@AEAAXIAEBVDouble80@@V1@@Z
_TEXT	SEGMENT
this$ = 48
i$dead$ = 56
v$ = 64
<tail_0>$ = 72
??$initList@VDouble80@@V1@@?$FixedSizeVectorTemplate@N$02@@AEAAXIAEBVDouble80@@V1@@Z PROC ; FixedSizeVectorTemplate<double,3>::initList<Double80,Double80>, COMDAT

; 13   :   template<typename V, typename... Tail> inline void initList(UINT i, const V &v, Tail... tail) {

$LN6:
	mov	QWORD PTR [rsp+16], rbx
	push	rdi
	sub	rsp, 32					; 00000020H
	mov	rbx, rcx
	mov	rdi, r9

; 14   :     assert(i < dimension);
; 15   :     m_v[i] = (T)v;

	mov	rcx, r8
	call	??BDouble80@@QEBANXZ			; Double80::operator double

; 11   :     m_v[i] = (T)v;

	mov	rcx, rdi

; 14   :     assert(i < dimension);
; 15   :     m_v[i] = (T)v;

	movsd	QWORD PTR [rbx+8], xmm0

; 11   :     m_v[i] = (T)v;

	call	??BDouble80@@QEBANXZ			; Double80::operator double
	movsd	QWORD PTR [rbx+16], xmm0

; 16   :     initList(i+1, tail...);
; 17   :   }

	mov	rbx, QWORD PTR [rsp+56]
	add	rsp, 32					; 00000020H
	pop	rdi
	ret	0
??$initList@VDouble80@@V1@@?$FixedSizeVectorTemplate@N$02@@AEAAXIAEBVDouble80@@V1@@Z ENDP ; FixedSizeVectorTemplate<double,3>::initList<Double80,Double80>
_TEXT	ENDS
; Function compile flags: /Ogtpy
; File C:\Mytools2015\Test\FixedSizeArray\FixedSizeArray.cpp
;	COMDAT ??$initList@VDouble80@@@?$FixedSizeVectorTemplate@N$02@@AEAAXIAEBVDouble80@@@Z
_TEXT	SEGMENT
this$ = 48
i$dead$ = 56
v$ = 64
??$initList@VDouble80@@@?$FixedSizeVectorTemplate@N$02@@AEAAXIAEBVDouble80@@@Z PROC ; FixedSizeVectorTemplate<double,3>::initList<Double80>, COMDAT

; 9    :   template<typename V> inline void initList(UINT i, const V &v) {

$LN4:
	push	rbx
	sub	rsp, 32					; 00000020H
	mov	rbx, rcx

; 10   :     assert(i == dimension - 1);
; 11   :     m_v[i] = (T)v;

	mov	rcx, r8
	call	??BDouble80@@QEBANXZ			; Double80::operator double
	movsd	QWORD PTR [rbx+16], xmm0

; 12   :   }

	add	rsp, 32					; 00000020H
	pop	rbx
	ret	0
??$initList@VDouble80@@@?$FixedSizeVectorTemplate@N$02@@AEAAXIAEBVDouble80@@@Z ENDP ; FixedSizeVectorTemplate<double,3>::initList<Double80>
_TEXT	ENDS
; Function compile flags: /Ogtpy
; File C:\mytools2015\Lib\Include\MyString.h
;	COMDAT ??YString@@QEAAAEAV0@_W@Z
_TEXT	SEGMENT
tmp$ = 32
__$ArrayPad$ = 40
this$ = 64
ch$ = 72
??YString@@QEAAAEAV0@_W@Z PROC				; String::operator+=, COMDAT

; 196  :   inline String &operator+=(TCHAR ch) {

$LN4:
	sub	rsp, 56					; 00000038H
	mov	rax, QWORD PTR __security_cookie
	xor	rax, rsp
	mov	QWORD PTR __$ArrayPad$[rsp], rax

; 197  :     const TCHAR tmp[] = { ch, 0 };

	mov	WORD PTR tmp$[rsp], dx
	xor	eax, eax

; 198  :     return *this += tmp;

	lea	rdx, QWORD PTR tmp$[rsp]
	mov	WORD PTR tmp$[rsp+2], ax
	call	??YString@@QEAAAEAV0@PEB_W@Z		; String::operator+=

; 199  :   }

	mov	rcx, QWORD PTR __$ArrayPad$[rsp]
	xor	rcx, rsp
	call	__security_check_cookie
	add	rsp, 56					; 00000038H
	ret	0
??YString@@QEAAAEAV0@_W@Z ENDP				; String::operator+=
_TEXT	ENDS
; Function compile flags: /Ogtpy
;	COMDAT ?__autoclassinit2@String@@QEAAX_K@Z
_TEXT	SEGMENT
this$ = 8
classSize$dead$ = 16
?__autoclassinit2@String@@QEAAX_K@Z PROC		; String::__autoclassinit2, COMDAT
	xorps	xmm0, xmm0
	xor	eax, eax
	movups	XMMWORD PTR [rcx], xmm0
	mov	QWORD PTR [rcx+16], rax
	ret	0
?__autoclassinit2@String@@QEAAX_K@Z ENDP		; String::__autoclassinit2
_TEXT	ENDS
END
