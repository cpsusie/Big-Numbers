#include "pch.h"
#include <Math/Expression/Expression.h>

//#define GENERATE_ASSEMBLER_CODE

#ifdef GENERATE_ASSEMBLER_CODE

void fisk() {
  __asm {

    imul ecx, dword ptr[esi+8*edi+0xabcddbca],0x12345678
    imul cx ,  word ptr[esi+8*edi+0xabcddbca],0x1234

    cbw
    cwde
    cwd
    cdq

  }
}

#endif // GENERATE_ASSEMBLER_CODE

#ifdef TEST_MACHINECODE

#ifdef IS32BIT
#define ALLHIGHREG(op)
#else
#define ALLHIGHREG(op) ,op(8),op(9),op(10),op(11),op(12),op(13),op(14),op(15)
#endif

#define ALLREG(op) op(0),op(1),op(2),op(3),op(4),op(5),op(6),op(7) ALLHIGHREG(op)

#ifdef IS32BIT
#define OP_1ARGX64_QWORD(name)
#define FOR_ALL_REG64(op)
#else // IS64BIT
#define OP_1ARGX64_QWORD(name) ,name##_QWORD
#endif // IS32BIT

#define OP_2ARG_BYTE_1WAY(    name)  ALLREG(name##_BYTE     )
#define OP_2ARG_BYTE_2WAY(    name)  ALLREG(name##_R8_BYTE  ), ALLREG(name##_BYTE_R8  )
#define OP_2ARG_WORD_1WAY(    name)  ALLREG(name##_WORD     )
#define OP_2ARG_WORD_2WAY(    name)  ALLREG(name##_R16_WORD ), ALLREG(name##_WORD_R16 )
#define OP_2ARG_DWORD_1WAY(   name)  ALLREG(name##_DWORD    )
#define OP_2ARG_DWORD_2WAY(   name)  ALLREG(name##_R32_DWORD), ALLREG(name##_DWORD_R32)
#define OP_2ARGX64_QWORD_1WAY(name) ,ALLREG(name##_QWORD    )
#define OP_2ARGX64_QWORD_2WAY(name) ,ALLREG(name##_R64_QWORD), ALLREG(name##_R64_QWORD)

#define OP_1ARG(              name) name##_BYTE ,name##_DWORD ,name##_WORD  OP_1ARGX64_QWORD(name)

#define OP_2ARG_1WAY_NOBYTE(name) \
  OP_2ARG_WORD_1WAY(    name)     \
 ,OP_2ARG_DWORD_1WAY(   name)     \
  OP_2ARGX64_QWORD_1WAY(name)

#define OP_2ARG_2WAY(name)        \
  OP_2ARG_BYTE_2WAY(    name)     \
 ,OP_2ARG_WORD_2WAY(    name)     \
 ,OP_2ARG_DWORD_2WAY(   name)     \
  OP_2ARGX64_QWORD_2WAY(name)

#define DEFINE_INSTRUCTIONLIST(a, list) const IntelInstruction a[] = { list }
#define EMIT_INSTRUCTIONLIST(a) for(int i = 0; i < ARRAYSIZE(a); i++) { emit(a[i]); }
#define EMITALL(name) { DEFINE_INSTRUCTIONLIST(l, ALLREG(name)); EMIT_INSTRUCTIONLIST(l); }

#define EMIT_ALLR8( name) EMITALL(name##_R8 )
#define EMIT_ALLR16(name) EMITALL(name##_R16)
#define EMIT_ALLR32(name) EMITALL(name##_R32)
#ifdef IS32BIT
#define EMIT_ALLR64(name)
#else // IS64BIT
#define EMIT_ALLR64(name) EMITALL(name##_R64)
#endif

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
#ifdef GENERATE_ASSEMBLER_CODE
  fisk();
#endif
#ifdef IS32BIT
  void *addr = (void*)&staticInt32;
#else // IS64BIT
  void *addr = (void*)&staticInt64;
#endif // IS64BIT

#ifdef TEST_INCDEC_PUSHPOP

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

#endif // TEST_INCDEC_PUSHPOP

#ifdef TEST_MOV

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

#endif // TEST_MOV

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
  emit(REP); emit(SCAS_QWORD);
#endif
  emit(REP); emit(SCAS_DWORD);

#ifdef TEST_MUL

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
#endif // TEST_MUL

  const IntelOpcode opcodes[] = {
    OP_2ARG_1WAY_NOBYTE(BSF)
   ,OP_2ARG_1WAY_NOBYTE(BSR)
   ,
    SETO   ,SETNO   ,SETB    ,SETAE
   ,SETE   ,SETNE   ,SETBE   ,SETA
   ,SETS   ,SETNS   ,SETPE   ,SETPO
   ,SETL   ,SETGE   ,SETLE   ,SETG
   ,OP_1ARG(NOT )
   ,OP_1ARG(NEG )
   ,OP_1ARG(INC )
   ,OP_1ARG(DEC )
   ,OP_1ARG(MUL )
   ,OP_1ARG(IMUL)
   ,OP_1ARG(DIV )
   ,OP_1ARG(IDIV)
   ,OP_1ARG(ROL )
   ,OP_1ARG(ROR )
   ,OP_1ARG(RCL )
   ,OP_1ARG(RCR )
   ,OP_1ARG(SHL )
   ,OP_1ARG(SHR )
   ,OP_1ARG(SAR )
   ,OP_2ARG_1WAY_NOBYTE(SHLD)
   ,OP_2ARG_1WAY_NOBYTE(SHRD)

   ,IMUL2_R32_DWORD(EAX),IMUL2_R32_DWORD(ECX),IMUL2_R32_DWORD(EDX),IMUL2_R32_DWORD(EBX)
   ,IMUL2_R32_DWORD(ESP),IMUL2_R32_DWORD(EBP),IMUL2_R32_DWORD(ESI),IMUL2_R32_DWORD(EDI)

   ,OP_2ARG_2WAY(ADD)
   ,OP_2ARG_2WAY(ADC)
   ,OP_2ARG_2WAY(OR )
   ,OP_2ARG_2WAY(AND)
   ,OP_2ARG_2WAY(SUB)
   ,OP_2ARG_2WAY(SBB)
   ,OP_2ARG_2WAY(XOR)
   ,OP_2ARG_2WAY(CMP)
   ,OP_2ARG_2WAY(MOV)

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
      const int maxReg = op.hasRexMode() ? 15 : 7;
      for(r = 0; r <= maxReg; r++) {
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
