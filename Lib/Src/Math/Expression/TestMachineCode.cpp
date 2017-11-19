#include "pch.h"
#include <Math/Expression/Expression.h>
#include <Math/MathFunctions.h>

//#define GENERATE_ASSEMBLER_CODE

#ifdef GENERATE_ASSEMBLER_CODE

void fisk() {
  __asm {

    imul ecx, dword ptr[esi+8*edi+0xabcddbca],0x12345678
    imul cx, word ptr[esi+8*edi+0xabcddbca],0x1234

    cbw
    cwde
    cwd
    cdq

    add ax, word ptr[esp+0x12345678] // 8 bytes instruction

    idiv al
    idiv cl
    idiv dl
    idiv bl
    idiv ah
    idiv ch
    idiv dh
    idiv bh

    idiv byte ptr[eax]
    idiv byte ptr[ecx]
    idiv byte ptr[edx]
    idiv byte ptr[ebx]
    idiv byte ptr[esp]
    idiv byte ptr[ebp]
    idiv byte ptr[esi]
    idiv byte ptr[edi]

    idiv eax
    idiv ecx
    idiv edx
    idiv ebx
    idiv esp
    idiv ebp
    idiv esi
    idiv edi

    idiv dword ptr[eax]
    idiv dword ptr[ecx]
    idiv dword ptr[edx]
    idiv dword ptr[ebx]
    idiv dword ptr[esp]
    idiv dword ptr[ebp]
    idiv dword ptr[esi]
    idiv dword ptr[edi]

    idiv ax
    idiv cx
    idiv dx
    idiv bx
    idiv sp
    idiv bp
    idiv si
    idiv di

    idiv word ptr[eax]
    idiv word ptr[ecx]
    idiv word ptr[edx]
    idiv word ptr[ebx]
    idiv word ptr[esp]
    idiv word ptr[ebp]
    idiv word ptr[esi]
    idiv word ptr[edi]

    imul cl
    imul dl
    imul bl
    imul byte ptr[ecx]

    imul ecx
    imul edx
    imul ebx
    imul dword ptr[eax]
    imul dword ptr[ecx]
    imul dword ptr[edx]
    imul dword ptr[ebx]
    imul dword ptr[esp]
    imul dword ptr[ebp]
    imul dword ptr[esi]
    imul dword ptr[edi]
    imul eax, eax
    imul eax, ecx
    imul eax, edx
    imul eax, ebx
    imul eax, esp
    imul eax, ebp
    imul eax, esi
    imul eax, edi
    imul ecx, eax
    imul ecx, ecx
    imul ecx, edx
    imul ecx, ebx
    imul ecx, esp
    imul ecx, ebp
    imul ecx, esi
    imul ecx, edi
    imul eax, dword ptr[eax], 2
    imul eax, ecx, 2
    imul eax, edx, 2
    imul eax, ebx, 2
    imul eax, esp, 2
    imul eax, ebp, 2
    imul eax, esi, 2
    imul eax, edi, 2
    imul ecx, eax, 2
    imul ecx, ecx, 2
    imul ecx, edx, 2
    imul ecx, ebx, 2
    imul ecx, esp, 2
    imul ecx, ebp, 2
    imul ecx, esi, 2
    imul ecx, edi, 2
    imul edx, eax, 2
    imul edx, ecx, 2
    imul edx, edx, 2
    imul edx, ebx, 2
    imul edx, esp, 2
    imul edx, ebp, 2
    imul edx, esi, 2
    imul edx, edi, 2
    imul ebx, eax, 2
    imul ebx, ecx, 2
    imul ebx, edx, 2
    imul ebx, ebx, 2
    imul ebx, esp, 2
    imul ebx, ebp, 2
    imul ebx, esi, 2
    imul ebx, edi, 2
    imul eax, dword ptr[esp+0xaaaaaaaa], 0x12345678
    imul eax, ecx, 0x12345678
    imul eax, edx, 0x12345678
    imul eax, ebx, 0x12345678
    imul eax, esp, 0x12345678
    imul eax, ebp, 0x12345678
    imul eax, esi, 0x12345678
    imul eax, edi, 0x12345678
    imul ecx, eax, 0x12345678
    imul ecx, ecx, 0x12345678
    imul ecx, edx, 0x12345678
    imul ecx, ebx, 0x12345678
    imul ecx, esp, 0x12345678
    imul ecx, ebp, 0x12345678
    imul ecx, esi, 0x12345678
    imul ecx, edi, 0x12345678
    imul edx, eax, 0x12345678
    imul edx, ecx, 0x12345678
    imul edx, edx, 0x12345678
    imul edx, ebx, 0x12345678
    imul edx, esp, 0x12345678
    imul edx, ebp, 0x12345678
    imul edx, esi, 0x12345678
    imul edx, edi, 0x12345678
    imul ebx, eax, 0x12345678
    imul ebx, ecx, 0x12345678
    imul ebx, edx, 0x12345678
    imul ebx, ebx, 0x12345678
    imul ebx, esp, 0x12345678
    imul ebx, ebp, 0x12345678
    imul ebx, esi, 0x12345678
    imul ebx, edi, 0x12345678

    imul cx
    imul dx
    imul bx
    imul word ptr[ecx]
    imul ax, ax
    imul ax, cx
    imul ax, dx
    imul ax, bx
    imul ax, sp
    imul ax, bp
    imul ax, si
    imul ax, di
    imul cx, ax
    imul cx, cx
    imul cx, dx
    imul cx, bx
    imul cx, sp
    imul cx, bp
    imul cx, si
    imul cx, di
    imul ax, 2
    imul cx, 2
    imul dx, 2
    imul bx, 2
    imul ax, 0x1234
    imul cx, 0x1234
    imul dx, 0x1234
    imul bx, 0x1234

    mov byte ptr[eax], al
    mov byte ptr[eax], cl
    mov byte ptr[eax], dl
    mov byte ptr[eax], bl
    mov byte ptr[eax], ah
    mov byte ptr[eax], ch
    mov byte ptr[eax], dh
    mov byte ptr[eax], bh

    mov dword ptr[eax], eax
    mov dword ptr[eax], ecx
    mov dword ptr[eax], edx
    mov dword ptr[eax], ebx
    mov dword ptr[eax], esp
    mov dword ptr[eax], ebp
    mov dword ptr[eax], esi
    mov dword ptr[eax], edi

    mov al, byte ptr[eax]
    mov cl, byte ptr[eax]
    mov dl, byte ptr[eax]
    mov bl, byte ptr[eax]
    mov ah, byte ptr[eax]
    mov ch, byte ptr[eax]
    mov dh, byte ptr[eax]
    mov bh, byte ptr[eax]

    mov eax, dword ptr[eax]
    mov ecx, dword ptr[eax]
    mov edx, dword ptr[eax]
    mov ebx, dword ptr[eax]
    mov esp, dword ptr[eax]
    mov ebp, dword ptr[eax]
    mov esi, dword ptr[eax]
    mov edi, dword ptr[eax]

    mov al,1
    mov cl,1
    mov dl,1
    mov bl,1
    mov ah,1
    mov ch,1
    mov dh,1
    mov bh,1

    mov eax,0x12345678
    mov ecx,0x12345678
    mov edx,0x12345678
    mov ebx,0x12345678
    mov esp,0x12345678
    mov ebp,0x12345678
    mov esi,0x12345678
    mov edi,0x12345678

    mov eax,1
    mov ecx,1
    mov edx,1
    mov ebx,1
    mov esp,1
    mov ebp,1
    mov esi,1
    mov edi,1

    mov ax,word ptr[eax]
    mov cx,word ptr[eax]
    mov dx,word ptr[eax]
    mov bx,word ptr[eax]
    mov sp,word ptr[eax]
    mov bp,word ptr[eax]
    mov si,word ptr[eax]
    mov di,word ptr[eax]

    mov word ptr[eax],ax
    mov word ptr[eax],cx
    mov word ptr[eax],dx
    mov word ptr[eax],bx
    mov word ptr[eax],sp
    mov word ptr[eax],bp
    mov word ptr[eax],si
    mov word ptr[eax],di

    mov ax,0x1234
    mov cx,0x1234
    mov dx,0x1234
    mov bx,0x1234
    mov sp,0x1234
    mov bp,0x1234
    mov si,0x1234
    mov di,0x1234

    mov ax,1
    mov cx,1
    mov dx,1
    mov bx,1
    mov sp,1
    mov bp,1
    mov si,1
    mov di,1


  }
}

#endif // GENERATE_ASSEMBLER_CODE

#ifdef TEST_MACHINECODE

#ifdef IS32BIT
#define OP_1ARGX64_QWORD(name)
#else
#define OP_1ARGX64_QWORD(name) ,name##_QWORD
#endif // IS32BIT

#define OP_1ARG(name) name##_BYTE ,name##_DWORD ,name##_WORD  OP_1ARGX64_QWORD(name)

#ifdef IS32BIT
#define OP_2ARGX64(name)
#else // IS64BIT
#define OP_2ARGX64(name)                                                                       \
   ,name##_R64_QWORD(RAX), name##_R64_QWORD(RCX), name##_R64_QWORD(RDX), name##_R64_QWORD(RBX) \
   ,name##_R64_QWORD(RSP), name##_R64_QWORD(RBP), name##_R64_QWORD(RSI), name##_R64_QWORD(RDI) \
   ,name##_R64_QWORD( R8), name##_R64_QWORD( R9), name##_R64_QWORD(R10), name##_R64_QWORD(R11) \
   ,name##_R64_QWORD(R12), name##_R64_QWORD(R13), name##_R64_QWORD(R14), name##_R64_QWORD(R15) \
   ,name##_QWORD_R64(RAX), name##_QWORD_R64(RCX), name##_QWORD_R64(RDX), name##_QWORD_R64(RBX) \
   ,name##_QWORD_R64(RSP), name##_QWORD_R64(RBP), name##_QWORD_R64(RSI), name##_QWORD_R64(RDI) \
   ,name##_QWORD_R64( R8), name##_QWORD_R64( R9), name##_QWORD_R64(R10), name##_QWORD_R64(R11) \
   ,name##_QWORD_R64(R12), name##_QWORD_R64(R13), name##_QWORD_R64(R14), name##_QWORD_R64(R15)
#endif // IS32BIT

#define OP_2ARG(name)                                                                          \
    name##_R8_BYTE(   AL), name##_R8_BYTE(   CL), name##_R8_BYTE(   DL), name##_R8_BYTE(   BL) \
   ,name##_R8_BYTE(   AH), name##_R8_BYTE(   CH), name##_R8_BYTE(   DH), name##_R8_BYTE(   BH) \
   ,name##_BYTE_R8(   AL), name##_BYTE_R8(   CL), name##_BYTE_R8(   DL), name##_BYTE_R8(   BL) \
   ,name##_BYTE_R8(   AH), name##_BYTE_R8(   CH), name##_BYTE_R8(   DH), name##_BYTE_R8(   BH) \
   ,name##_R16_WORD(  AX), name##_R16_WORD(  CX), name##_R16_WORD(  DX), name##_R16_WORD(  BX) \
   ,name##_R16_WORD(  SP), name##_R16_WORD(  BP), name##_R16_WORD(  SI), name##_R16_WORD(  DI) \
   ,name##_WORD_R16(  AX), name##_WORD_R16(  CX), name##_WORD_R16(  DX), name##_WORD_R16(  BX) \
   ,name##_WORD_R16(  SP), name##_WORD_R16(  BP), name##_WORD_R16(  SI), name##_WORD_R16(  DI) \
   ,name##_R32_DWORD(EAX), name##_R32_DWORD(ECX), name##_R32_DWORD(EDX), name##_R32_DWORD(EBX) \
   ,name##_R32_DWORD(ESP), name##_R32_DWORD(EBP), name##_R32_DWORD(ESI), name##_R32_DWORD(EDI) \
   ,name##_DWORD_R32(EAX), name##_DWORD_R32(ECX), name##_DWORD_R32(EDX), name##_DWORD_R32(EBX) \
   ,name##_DWORD_R32(ESP), name##_DWORD_R32(EBP), name##_DWORD_R32(ESI), name##_DWORD_R32(EDI) \
    OP_2ARGX64(name)

#ifdef IS32BIT
#define EMIT_ALLR8X64( name)
#define EMIT_ALLR16X64(name)
#define EMIT_ALLR32X64(name)
#else // IS64BIT
#define EMIT_ALLR8X64(name)                                                                   \
  emit(name##_R8( R8 )); emit(name##_R8( R9 )); emit(name##_R8( R10)); emit(name##_R8( R11)); \
  emit(name##_R8( R12)); emit(name##_R8( R13)); emit(name##_R8( R14)); emit(name##_R8( R15))

#define EMIT_ALLR16X64(name)                                                                  \
  emit(name##_R16(R8 )); emit(name##_R16(R9 )); emit(name##_R16(R10)); emit(name##_R16(R11)); \
  emit(name##_R16(R12)); emit(name##_R16(R13)); emit(name##_R16(R14)); emit(name##_R16(R15))

#define EMIT_ALLR32X64(name)                                                                  \
  emit(name##_R32(R8 )); emit(name##_R32(R9 )); emit(name##_R32(R10)); emit(name##_R32(R11)); \
  emit(name##_R32(R12)); emit(name##_R32(R13)); emit(name##_R32(R14)); emit(name##_R32(R15))
#endif // IS64BIT

#define EMIT_ALLR8(name)                                                                      \
  emit(name##_R8( AL )); emit(name##_R8( CL )); emit(name##_R8( DL )); emit(name##_R8( BL )); \
  emit(name##_R8( AH )); emit(name##_R8( CH )); emit(name##_R8( DH )); emit(name##_R8( BH )); \
  EMIT_ALLR8X64(name)

#define EMIT_ALLR16(name)                                                                     \
  emit(name##_R16(AX )); emit(name##_R16(CX )); emit(name##_R16(DX )); emit(name##_R16(BX )); \
  emit(name##_R16(SP )); emit(name##_R16(BP )); emit(name##_R16(SI )); emit(name##_R16(DI )); \
  EMIT_ALLR16X64(name)

#define EMIT_ALLR32(name)                                                                     \
  emit(name##_R32(EAX)); emit(name##_R32(ECX)); emit(name##_R32(EDX)); emit(name##_R32(EBX)); \
  emit(name##_R32(ESP)); emit(name##_R32(EBP)); emit(name##_R32(ESI)); emit(name##_R32(EDI)); \
  EMIT_ALLR32X64(name)

#define EMIT_ALLR64(name)                                                                     \
  emit(name##_R64(RAX)); emit(name##_R64(RCX)); emit(name##_R64(RDX)); emit(name##_R64(RBX)); \
  emit(name##_R64(RSP)); emit(name##_R64(RBP)); emit(name##_R64(RSI)); emit(name##_R64(RDI)); \
  emit(name##_R64( R8)); emit(name##_R64( R9)); emit(name##_R64(R10)); emit(name##_R64(R11)); \
  emit(name##_R64(R12)); emit(name##_R64(R13)); emit(name##_R64(R14)); emit(name##_R64(R15));

#define UNKNOWN_OPCODE(  dst)               B2INSA(0x8700 + ((dst)<<3))                     // Build src with MEM_ADDR-macros, REG_SRC
//Real FMOD(Real x, Real y) {
//  return fmod(x,y);
//}

static BYTE   staticInt8  = 0x12;
static USHORT staticInt16 = 0x1234;
static UINT   staticInt32 = 0x12345678;

#ifdef IS64BIT
static UINT64 staticInt64 = 0x123456789abcdef1;
#endif // IS64BIT

void MachineCode::genTestSequence() {
#ifdef IS32BIT
  void *addr = (void*)&staticInt32;
#else // IS64BIT
  void *addr = (void*)&staticInt64;
#endif // IS64BIT

  EMIT_ALLR8(INC);
  EMIT_ALLR8(DEC);
  EMIT_ALLR16(INC);
  EMIT_ALLR16(DEC);
  EMIT_ALLR32(INC);
  EMIT_ALLR32(DEC);

#ifdef IS64BIT
  EMIT_ALLR64(INC);
  EMIT_ALLR64(DEC);
#endif // IS64BIT

#ifdef IS32BIT
  EMIT_ALLR16(PUSH);
  EMIT_ALLR16(POP );
  EMIT_ALLR32(PUSH);
  EMIT_ALLR32(POP );
#else // IS64BIT
  EMIT_ALLR64(PUSH);
  EMIT_ALLR64(POP );
#endif // IS64BIT

  emit(REG_SRC(MOV_BYTE_R8(  BL ), BH ));
  emit(REG_SRC(MOV_BYTE_R8(  BH ), DH ));
  emit(REG_SRC(MOV_WORD_R16( DI ), SI ));
  emit(REG_SRC(MOV_DWORD_R32(EDI), ESI));

  emit(REG_SRC(MOV_R8_BYTE(  BL ), BH ));
  emit(REG_SRC(MOV_R8_BYTE(  BH ), DH ));
  emit(REG_SRC(MOV_R16_WORD( DI ), SI ));
  emit(REG_SRC(MOV_R32_DWORD(EDI), ESI));

  emit(MOV_R8_IMM_BYTE(  BL));       addBytes(addr,1);
  emit(MOV_R8_IMM_BYTE(  BH));       addBytes((char*)addr+1,1);
  emit(MOV_R16_IMM_WORD( DI));       addBytes(addr,2);
  emit(MOV_R32_IMM_DWORD(EDI));      addBytes(addr,4);

  emit(MOV_TO_AL_IMM_ADDR_BYTE    ); addBytes(addr,sizeof(addr));
  emit(MOV_TO_AX_IMM_ADDR_WORD    ); addBytes(addr,sizeof(addr));
  emit(MOV_TO_EAX_IMM_ADDR_DWORD  ); addBytes(addr,sizeof(addr));
  emit(MOV_FROM_AL_IMM_ADDR_BYTE  ); addBytes(addr,sizeof(addr));
  emit(MOV_FROM_AX_IMM_ADDR_WORD  ); addBytes(addr,sizeof(addr));
  emit(MOV_FROM_EAX_IMM_ADDR_DWORD); addBytes(addr,sizeof(addr));

#ifdef IS64BIT
  emit(REG_SRC(MOV_QWORD_R64(RDI), RSI));
  emit(REG_SRC(MOV_R64_QWORD(RDI), RSI));
  emit(MOV_R64_IMM_QWORD(RDI));      addBytes(addr,8);
  emit(MOV_TO_RAX_IMM_ADDR_QWORD  ); addBytes(addr,sizeof(addr));
  emit(MOV_FROM_RAX_IMM_ADDR_QWORD); addBytes(addr,sizeof(addr));
#endif

  static int j = 0x12345678;
  emit(MEM_ADDR_PTR(ADD_R32_DWORD(EDI), ESI));
#ifdef IS64BIT
  emit(MEM_ADDR_PTR(ADD_R64_QWORD(RDI), RSI));
  emit(REG_SRC(ADD_R64_QWORD(RDX),RAX));
  emit(ADD_R64_IMM_DWORD(RAX)); addBytes(&j, 4);
  emit(ADD_R64_IMM_BYTE(RBX)),  append(0x34);
#endif

  emit(MEM_ADDR_PTR(OR_R32_DWORD(EDI), ESI));
#ifdef IS64BIT
  emit(MEM_ADDR_PTR(OR_R64_QWORD(RDI), RSI));
  emit(REG_SRC(OR_QWORD_R64(RDI)     , RSI));
  emit(OR_R64_IMM_DWORD(RSI));  addBytes(&j, 4);
#endif

  emit(MEM_ADDR_PTR(XOR_R32_DWORD(EDI), ESI));
#ifdef IS64BIT
  emit(MEM_ADDR_PTR(XOR_R64_QWORD(RDI), RSI));
  emit(REG_SRC(XOR_QWORD_R64(RDI)     , RSI));
  emit(XOR_R64_IMM_DWORD(RSI)); addBytes(&j, 4);
#endif

  emit(MEM_ADDR_PTR( MUL_DWORD,EDI));
  emit(MEM_ADDR_PTR(IMUL_DWORD,EDI));
#ifdef IS64BIT
  emit(MEM_ADDR_PTR(MUL_QWORD,RDI));
  emit(MEM_ADDR_PTR(IMUL_QWORD,RDI));
#endif

  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_DWORD(EAX),ECX)); addBytes(&staticInt32,4);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_DWORD(ECX),ECX)); addBytes(&staticInt32,4);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_DWORD(EDX),ECX)); addBytes(&staticInt32,4);

#ifdef IS64BIT
  emit(MEM_ADDR_PTR(IMUL3_QWORD_IMM_DWORD(RAX),RCX)); addBytes(&staticInt32,4);
  emit(MEM_ADDR_PTR(IMUL3_QWORD_IMM_DWORD(RCX),RCX)); addBytes(&staticInt32,4);
  emit(MEM_ADDR_PTR(IMUL3_QWORD_IMM_DWORD(RDX),RCX)); addBytes(&staticInt32,4);
#endif // IS64BIT

  emit(REG_SRC(XOR_DWORD_R32(EDX),EDX));
  emit(MOV_R32_IMM_DWORD(EDI)); addBytes(addr,4);
  emit(MEM_ADDR_PTR(MUL_DWORD,EDI));

  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_DWORD(EAX),ECX)); addBytes(&staticInt32,4);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_DWORD(ECX),ECX)); addBytes(&staticInt32,4);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_DWORD(EDX),ECX)); addBytes(&staticInt32,4);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_DWORD(EBX),ECX)); addBytes(&staticInt32,4);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_DWORD(ESP),ECX)); addBytes(&staticInt32,4);
  emit(MEM_ADDR_ESP(IMUL3_DWORD_IMM_DWORD(EAX)));     addBytes(&staticInt32,4);
  emit(MEM_ADDR_ESP(IMUL3_DWORD_IMM_DWORD(ECX)));     addBytes(&staticInt32,4);
  emit(MEM_ADDR_ESP(IMUL3_DWORD_IMM_DWORD(EDX)));     addBytes(&staticInt32,4);
  emit(MEM_ADDR_ESP(IMUL3_DWORD_IMM_DWORD(EBX)));     addBytes(&staticInt32,4);
  emit(MEM_ADDR_ESP(IMUL3_DWORD_IMM_DWORD(ESP)));     addBytes(&staticInt32,4);

  emit(REG_SRC(IMUL3_DWORD_IMM_DWORD(EAX),ECX));      addBytes(&staticInt32,4);
  emit(REG_SRC(IMUL3_DWORD_IMM_DWORD(ECX),ECX));      addBytes(&staticInt32,4);
  emit(REG_SRC(IMUL3_DWORD_IMM_DWORD(EDX),ECX));      addBytes(&staticInt32,4);
  emit(REG_SRC(IMUL3_DWORD_IMM_DWORD(EBX),ECX));      addBytes(&staticInt32,4);
  emit(REG_SRC(IMUL3_DWORD_IMM_DWORD(ESP),ECX));      addBytes(&staticInt32,4);

  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(EAX),ECX));  addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(ECX),ECX));  addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(EDX),ECX));  addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(EBX),ECX));  addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(ESP),ECX));  addBytes(&staticInt32,1);
  emit(MEM_ADDR_ESP(IMUL3_DWORD_IMM_BYTE(EAX)));      addBytes(&staticInt32,1);
  emit(MEM_ADDR_ESP(IMUL3_DWORD_IMM_BYTE(ECX)));      addBytes(&staticInt32,1);
  emit(MEM_ADDR_ESP(IMUL3_DWORD_IMM_BYTE(EDX)));      addBytes(&staticInt32,1);
  emit(MEM_ADDR_ESP(IMUL3_DWORD_IMM_BYTE(EBX)));      addBytes(&staticInt32,1);
  emit(MEM_ADDR_ESP(IMUL3_DWORD_IMM_BYTE(ESP)));      addBytes(&staticInt32,1);

  emit(REG_SRC(IMUL3_DWORD_IMM_BYTE(EAX),ECX));       addBytes(&staticInt32,1);
  emit(REG_SRC(IMUL3_DWORD_IMM_BYTE(ECX),ECX));       addBytes(&staticInt32,1);
  emit(REG_SRC(IMUL3_DWORD_IMM_BYTE(EDX),ECX));       addBytes(&staticInt32,1);
  emit(REG_SRC(IMUL3_DWORD_IMM_BYTE(EBX),ECX));       addBytes(&staticInt32,1);
  emit(REG_SRC(IMUL3_DWORD_IMM_BYTE(ESP),ECX));       addBytes(&staticInt32,1);


  emit(MEM_ADDR_PTR(IMUL3_WORD_IMM_WORD(AX),CX));     addBytes(&staticInt32,2);
  emit(MEM_ADDR_PTR(IMUL3_WORD_IMM_WORD(CX),CX));     addBytes(&staticInt32,2);
  emit(MEM_ADDR_PTR(IMUL3_WORD_IMM_WORD(DX),CX));     addBytes(&staticInt32,2);
  emit(MEM_ADDR_PTR(IMUL3_WORD_IMM_WORD(BX),CX));     addBytes(&staticInt32,2);
  emit(MEM_ADDR_PTR(IMUL3_WORD_IMM_WORD(SP),CX));     addBytes(&staticInt32,2);
  emit(MEM_ADDR_ESP(IMUL3_WORD_IMM_WORD(AX)));        addBytes(&staticInt32,2);
  emit(MEM_ADDR_ESP(IMUL3_WORD_IMM_WORD(CX)));        addBytes(&staticInt32,2);
  emit(MEM_ADDR_ESP(IMUL3_WORD_IMM_WORD(DX)));        addBytes(&staticInt32,2);
  emit(MEM_ADDR_ESP(IMUL3_WORD_IMM_WORD(BX)));        addBytes(&staticInt32,2);
  emit(MEM_ADDR_ESP(IMUL3_WORD_IMM_WORD(SP)));        addBytes(&staticInt32,2);

  emit(REG_SRC(IMUL3_WORD_IMM_WORD(AX),CX));          addBytes(&staticInt32,2);
  emit(REG_SRC(IMUL3_WORD_IMM_WORD(CX),CX));          addBytes(&staticInt32,2);
  emit(REG_SRC(IMUL3_WORD_IMM_WORD(DX),CX));          addBytes(&staticInt32,2);
  emit(REG_SRC(IMUL3_WORD_IMM_WORD(BX),CX));          addBytes(&staticInt32,2);
  emit(REG_SRC(IMUL3_WORD_IMM_WORD(SP),CX));          addBytes(&staticInt32,2);

  emit(MEM_ADDR_PTR(IMUL3_WORD_IMM_BYTE(AX),CX));     addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_WORD_IMM_BYTE(CX),CX));     addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_WORD_IMM_BYTE(DX),CX));     addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_WORD_IMM_BYTE(BX),CX));     addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_WORD_IMM_BYTE(SP),CX));     addBytes(&staticInt32,1);
  emit(MEM_ADDR_ESP(IMUL3_WORD_IMM_BYTE(AX)));        addBytes(&staticInt32,1);
  emit(MEM_ADDR_ESP(IMUL3_WORD_IMM_BYTE(CX)));        addBytes(&staticInt32,1);
  emit(MEM_ADDR_ESP(IMUL3_WORD_IMM_BYTE(DX)));        addBytes(&staticInt32,1);
  emit(MEM_ADDR_ESP(IMUL3_WORD_IMM_BYTE(BX)));        addBytes(&staticInt32,1);
  emit(MEM_ADDR_ESP(IMUL3_WORD_IMM_BYTE(SP)));        addBytes(&staticInt32,1);

  emit(REG_SRC(IMUL3_WORD_IMM_BYTE(AX),CX));          addBytes(&staticInt32,1);
  emit(REG_SRC(IMUL3_WORD_IMM_BYTE(CX),CX));          addBytes(&staticInt32,1);
  emit(REG_SRC(IMUL3_WORD_IMM_BYTE(DX),CX));          addBytes(&staticInt32,1);
  emit(REG_SRC(IMUL3_WORD_IMM_BYTE(BX),CX));          addBytes(&staticInt32,1);
  emit(REG_SRC(IMUL3_WORD_IMM_BYTE(SP),CX));          addBytes(&staticInt32,1);

  emit(NOOP);

  const IntelOpcode opcodes[] = {
    OP_1ARG(NOT )
   ,OP_1ARG(NEG )
   ,OP_1ARG(INC )
   ,OP_1ARG(DEC )
   ,OP_1ARG(MUL )
   ,OP_1ARG(IMUL)
   ,OP_1ARG(DIV )
   ,OP_1ARG(IDIV)

   ,IMUL2_R32_DWORD(EAX),IMUL2_R32_DWORD(ECX),IMUL2_R32_DWORD(EDX),IMUL2_R32_DWORD(EBX)
   ,IMUL2_R32_DWORD(ESP),IMUL2_R32_DWORD(EBP),IMUL2_R32_DWORD(ESI),IMUL2_R32_DWORD(EDI)

   ,OP_2ARG(ADD)
   ,OP_2ARG(OR )
   ,OP_2ARG(AND)
   ,OP_2ARG(SUB)
   ,OP_2ARG(XOR)
   ,OP_2ARG(CMP)
   ,OP_2ARG(MOV)

#ifdef IS64BIT
   ,MOVSD_XMM_MMWORD(XMM0)
   ,MOVSD_XMM_MMWORD(XMM2)
   ,MOVSD_MMWORD_XMM(XMM3)
   ,MOVSD_MMWORD_XMM(XMM7)
#endif //  IS64BIT

   ,LEA_R32_DWORD(EAX)  ,LEA_R32_DWORD(ECX),LEA_R32_DWORD(EDX),LEA_R32_DWORD(EBX)
   ,LEA_R32_DWORD(ESP)  ,LEA_R32_DWORD(EBP),LEA_R32_DWORD(ESI),LEA_R32_DWORD(EDI)

   ,FLDCW_WORD   ,FNSTCW_WORD  ,FNSTSW_WORD
   ,FLD_DWORD    ,FST_DWORD    ,FSTP_DWORD
   ,FLD_QWORD    ,FST_QWORD    ,FSTP_QWORD
   ,FLD_TBYTE                  ,FSTP_TBYTE
   ,FILD_WORD    ,FIST_WORD    ,FISTP_WORD
   ,FILD_DWORD   ,FIST_DWORD   ,FISTP_DWORD
   ,FILD_QWORD                 ,FISTP_QWORD
   ,FADD_DWORD   ,FSUB_DWORD   ,FSUBR_DWORD  ,FMUL_DWORD   ,FDIV_DWORD   ,FDIVR_DWORD  ,FCOM_DWORD   ,FCOMP_DWORD
   ,FADD_QWORD   ,FSUB_QWORD   ,FSUBR_QWORD  ,FMUL_QWORD   ,FDIV_QWORD   ,FDIVR_QWORD  ,FCOM_QWORD   ,FCOMP_QWORD
   ,FIADD_WORD   ,FISUB_WORD   ,FISUBR_WORD  ,FIMUL_WORD   ,FIDIV_WORD   ,FIDIVR_WORD  ,FICOM_WORD   ,FICOMP_WORD
   ,FIADD_DWORD  ,FISUB_DWORD  ,FISUBR_DWORD ,FIMUL_DWORD  ,FIDIV_DWORD  ,FIDIVR_DWORD ,FICOM_DWORD  ,FICOMP_DWORD
  };

  const UINT registers[] = {
    EAX
   ,ECX
   ,EDX
   ,EBX
   ,ESP
   ,EBP
   ,ESI
   ,EDI
#ifdef IS64BIT
   ,R8
   ,R9
   ,R10
   ,R11
   ,R12
   ,R13
   ,R14
   ,R15
#endif
  };

  for(int i = 0; i < ARRAYSIZE(opcodes); i++) {

    const IntelOpcode &op = opcodes[i];

    int r;

    if(op.hasRegSrcMode()) {
      const int maxReg = op.hasRex() ? 15 : 7;
      for(r = 0; r < maxReg; r++) {
        const int reg = registers[r];
        emit(REG_SRC(op, reg));                                           // size=2 ex:add eax,ecx
      }
    }
    emit(NOOP);

    // ptr[eax]
    for(r = 0; r < ARRAYSIZE(registers); r++) {
      const int reg = registers[r];
      if((reg&7) == ESP || (reg&7) == EBP) continue;
      emit(MEM_ADDR_PTR(op, reg));                                        // size=2 reg!=ESP,EBP                         ex:fld DWORD PTR[eax]
    }

    emit(NOOP);
    // ptr[eax+127]
    for(r = 0; r < ARRAYSIZE(registers); r++) {
      const int reg = registers[r];
      if((reg&7) == ESP) continue;
      emit(MEM_ADDR_PTR1(op, reg, -1));                                   // size=3 reg!=ESP        offs1=1 byte signed  ex.fld DWORD PTR[eax+127]
      emit(MEM_ADDR_PTR1(op, reg,  1));
    }

    emit(NOOP);
    // ptr[eax+0x12345678]
    for(r = 0; r < ARRAYSIZE(registers); r++) {
      const int reg = registers[r];
      if((reg&7) == ESP) continue;
      emit(MEM_ADDR_PTR4(op, reg, 0x12345678));                           // size=6 reg!=ESP        offs4=4 bytes signed ex fld DWORD PTR[  eax+0x12345678]
      emit(MEM_ADDR_PTR4(op, reg,-0x12345678));
    }

    emit(NOOP);
    // ptr[(eax<<p2)+0x12345678]
    for(int p2 = 0; p2 < 4; p2++) {
      for(int r = 0; r < ARRAYSIZE(registers); r++) {
        const int reg = registers[r];
        if((reg&7) == ESP) continue;
        emit(MEM_ADDR_MP2PTR4(op, reg, p2, 0x12345678));                  // size=7 reg!=ESP p2=0-3 offs4=4 bytes signed ex fld DWORD PTR[2*eax+0x12345678]
        emit(MEM_ADDR_MP2PTR4(op, reg, p2,-0x12345678));
      }
    }

    emit(NOOP);
    // ptr[eax+ecx]
    for(int r1 = 0; r1 < ARRAYSIZE(registers); r1++) {
      const int addReg = registers[r1];
      if((addReg&7) == ESP) continue;
      for(int r = 0; r < ARRAYSIZE(registers); r++) {
        const int reg = registers[r];
        if((reg&7) == EBP) continue;
        emit(MEM_ADDR_PTRREG(op, reg, addReg));                           // size=3 reg!=EBP addReg!=ESP                 ex fld DWORD PTR[esp+  ecx]
      }
    }

    emit(NOOP);
    // ptr[eax+(ecx<<p2)]
    for(int r1 = 0; r1 < ARRAYSIZE(registers); r1++) {
      const int addReg = registers[r1];
      if((addReg&7) == ESP) continue;
      for(int p2 = 0; p2 < 4; p2++) {
        for(int r = 0; r < ARRAYSIZE(registers); r++) {
          const int reg = registers[r];
          if((reg&7) == EBP) continue;
          emit(MEM_ADDR_PTRMP2REG(op, reg, addReg, p2));                  // size=3 reg!=EBP addReg!=ESP p2=0-3          ex fld DWORD PTR[esp+2*ecx]
        }
      }
    }

    emit(NOOP);
    // ptr[eax+ecx+127]
    for(int r1 = 0; r1 < ARRAYSIZE(registers); r1++) {
      const int addReg = registers[r1];
      if((addReg&7) == ESP) continue;
      for(int r = 0; r < ARRAYSIZE(registers); r++) {
        const int reg = registers[r];
        emit(MEM_ADDR_PTRREG1(op, reg, addReg,-1));                       // size=4 addReg!=ESP offs1=1 byte signed      ex fld DWORD PTR[ebp+  ecx+127]
        emit(MEM_ADDR_PTRREG1(op, reg, addReg, 1));
      }
    }

    emit(NOOP);
    // ptr[eax+2^p2*ecx+127]
    for(int r1 = 0; r1 < ARRAYSIZE(registers); r1++) {
      const int addReg = registers[r1];
      if((addReg&7) == ESP) continue;
      for(int p2 = 0; p2 < 4; p2++) {
        for(int r = 0; r < ARRAYSIZE(registers); r++) {
          const int reg = registers[r];
          emit(MEM_ADDR_PTRMP2REG1(op, reg, addReg, p2, 1));              // size=4 addReg!=ESP p2=0-3                   ex fld DWORD PTR[ebp+2*ecx+127]
          emit(MEM_ADDR_PTRMP2REG1(op, reg, addReg, p2,-1));
        }
      }
    }

    emit(NOOP);
    // ptr[eax+ecx+0x12345678]
    for(int r1 = 0; r1 < ARRAYSIZE(registers); r1++) {
      const int addReg = registers[r1];
      if((addReg&7) == ESP) continue;
      for(int r = 0; r < ARRAYSIZE(registers); r++) {
        const int reg = registers[r];
        emit(MEM_ADDR_PTRREG4(op, reg, addReg, 0x12345678));              // size=7 addReg!=ESP offs1=4 bytes signed     ex fld DWORD PTR[esp+  eax+0x12345678]
        emit(MEM_ADDR_PTRREG4(op, reg, addReg,-0x12345678));
      }
    }

    emit(NOOP);
    // ptr[eax+2^p2*ecx+0x12345678]
    for(int r1 = 0; r1 < ARRAYSIZE(registers); r1++) {
      const int addReg = registers[r1];
      if((addReg&7) == ESP) continue;
      for(int p2 = 0; p2 < 4; p2++) {
        for(int r = 0; r < ARRAYSIZE(registers); r++) {
          const int reg = registers[r];
          emit(MEM_ADDR_PTRMP2REG4(op, reg, addReg, p2, 0x12345678));     // size=7 addR32!=ESP p2=0-3 offs4=4 bytes signed ex fld DWORD PTR[esp+2*eax+0x12345678]
          emit(MEM_ADDR_PTRMP2REG4(op, reg, addReg, p2,-0x12345678));
        }
      }
    }

    emit(NOOP);
    emit(MEM_ADDR_ESP( op            ));                                  // size=3                      ex fld DWORD PTR[esp}

    emit(MEM_ADDR_ESP1(op,          1));                                  // size=4 offst=1 byte signed  ex fld DWORD PTR[esp+128}
    emit(MEM_ADDR_ESP1(op,         -1));

    emit(MEM_ADDR_ESP4(op, 0x12345678));                                  // size=7 offst=4 bytes signed ex fld DWORD PTR[esp+0x12345678]
    emit(MEM_ADDR_ESP4(op,-0x12345678));

    emit(MEM_ADDR_DS(  op   ));  addBytes(addr,4);                        // size=2 + 4 byte address
    emit(NOOP);
  }

  emit(MOV_TO_AL_IMM_ADDR_BYTE     ); addBytes(&addr,sizeof(addr));
  emit(MOV_TO_AX_IMM_ADDR_WORD     ); addBytes(&addr,sizeof(addr));
  emit(MOV_TO_EAX_IMM_ADDR_DWORD   ); addBytes(&addr,sizeof(addr));

  emit(MOV_FROM_AL_IMM_ADDR_BYTE   ); addBytes(&addr,sizeof(addr));
  emit(MOV_FROM_AX_IMM_ADDR_WORD   ); addBytes(&addr,sizeof(addr));
  emit(MOV_FROM_EAX_IMM_ADDR_DWORD ); addBytes(&addr,sizeof(addr));


  emit(ADD_AL_IMM_BYTE      );  addBytes(&staticInt8 ,1);
  emit(SUB_AL_IMM_BYTE      );  addBytes(&staticInt8 ,1);
  emit(ADD_AX_IMM_WORD      );  addBytes(&staticInt16,2);
  emit(SUB_AX_IMM_WORD      );  addBytes(&staticInt16,2);
  emit(ADD_EAX_IMM_DWORD    );  addBytes(&staticInt32,4);
  emit(SUB_EAX_IMM_DWORD    );  addBytes(&staticInt32,4);
  emit(ADD_R8_IMM_BYTE(   CL)); addBytes(&staticInt8 ,1);
  emit(SUB_R8_IMM_BYTE(   CL)); addBytes(&staticInt8 ,1);
  emit(ADD_R16_IMM_BYTE(  CX)); addBytes(&staticInt8 ,1);
  emit(SUB_R16_IMM_BYTE(  CX)); addBytes(&staticInt8 ,1);
  emit(ADD_R16_IMM_WORD(  CX)); addBytes(&staticInt16,2);
  emit(SUB_R16_IMM_WORD(  CX)); addBytes(&staticInt16,2);
  emit(ADD_R32_IMM_BYTE( EAX)); addBytes(&staticInt8 ,1);
  emit(SUB_R32_IMM_BYTE( EAX)); addBytes(&staticInt8 ,1);
  emit(ADD_R32_IMM_DWORD(EAX)); addBytes(&staticInt32,4);
  emit(SUB_R32_IMM_DWORD(EAX)); addBytes(&staticInt32,4);
}

#endif // TEST_MACHINECODE
