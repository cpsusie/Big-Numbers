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

// #define TEST_ALLREGISTERS

static const BYTE r8List[] = {
    AL
#ifdef TEST_ALLREGISTERS
   ,CL   ,DL   ,BL   ,AH   ,CH   ,DH
#endif // TEST_ALLREGISTERS
   ,BH
#ifdef IS64BIT
   ,R8B
#ifdef TEST_ALLREGISTERS
   ,R9B  ,R10B ,R11B ,R12B ,R13B ,R14B
#endif // TEST_ALLREGISTERS
   ,R15B
#endif // IS64BIT
  };

  const BYTE r16List[] = {
    AX
#ifdef TEST_ALLREGISTERS
   ,CX   ,DX   ,BX   ,SP   ,BP   ,SI
#endif // TEST_ALLREGISTERS
   ,DI
#ifdef IS64BIT
   ,R8W
#ifdef TEST_ALLREGISTERS
   ,R9W  ,R10W ,R11W ,R12W ,R13W ,R14W
#endif // TEST_ALLREGISTERS
   ,R15W
#endif // IS64BIT
  };

  const BYTE r32List[] = {
    EAX
#ifdef TEST_ALLREGISTERS
   ,ECX  ,EDX  ,EBX  ,ESP  ,EBP  ,ESI
#endif // TEST_ALLREGISTERS
   ,EDI
#ifdef IS64BIT
   ,R8D
#ifdef TEST_ALLREGISTERS
   ,R9D  ,R10D ,R11D ,R12D ,R13D ,R14D
#endif // TEST_ALLREGISTERS
   ,R15D
#endif // IS64BIT
  };

#ifdef IS64BIT
  const BYTE r64List[] = {
    RAX
#ifdef TEST_ALLREGISTERS
   ,RCX  ,RDX  ,RBX  ,RSP  ,RBP  ,RSI
#endif // TEST_ALLREGISTERS
   ,RDI
   ,R8
#ifdef TEST_ALLREGISTERS
   ,R9   ,R10  ,R11  ,R12  ,R13  ,R14
#endif // TEST_ALLREGISTERS
   ,R15
  };
#endif // IS64BIT

  const BYTE *regListArray[] = {
    r8List, r16List, r32List
#ifdef IS64BIT
   ,r64List
#endif
  };
#ifdef IS64BIT
#define refRegisters r64List
#else
#define refRegisters r32List
#endif

#define REF_REGLISTSIZE ARRAYSIZE(r32List)

#ifdef IS32BIT
#define ALLHIGHREG(op)
#define ALLREG64(  op)
#else
#ifdef TEST_ALLREGISTERS
#define ALLHIGHREG8( op) ,op(R8B),op(R9B),op(R10B),op(R11B),op(R12B),op(R13B),op(R14B),op(R15B)
#define ALLHIGHREG16(op) ,op(R8W),op(R9W),op(R10W),op(R11W),op(R12W),op(R13W),op(R14W),op(R15W)
#define ALLHIGHREG32(op) ,op(R8D),op(R9D),op(R10D),op(R11D),op(R12D),op(R13D),op(R14D),op(R15D)
#define ALLHIGHREG64(op) ,op(R8 ),op(R9 ),op(R10 ),op(R11 ),op(R12 ),op(R13 ),op(R14 ),op(R15 )
#else
#define ALLHIGHREG8(    op   ) ,op( R8B  ),op( R15B  )
#define ALLHIGHREG16(   op   ) ,op( R8W  ),op( R15W  )
#define ALLHIGHREG32(   op   ) ,op( R8D  ),op( R15D  )
#define ALLHIGHREG64(   op   ) ,op( R8   ),op( R15   )

#define ALLHIGHREG8IMM( ins,v) ,ins(R8B,v),ins(R15B,v)
#define ALLHIGHREG16IMM(ins,v) ,ins(R8W,v),ins(R15W,v)
#define ALLHIGHREG32IMM(ins,v) ,ins(R8D,v),ins(R15D,v)
#define ALLHIGHREG64IMM(ins,v) ,ins(R8 ,v),ins(R15 ,v)

#endif // TEST_ALLREGISTERS
#endif // IS32BIT

#ifdef TEST_ALLREGISTERS
#define ALLREG8( op)  op(AL ),op(CL ),op(DL ),op(BL ),op(AH ),op(CH ),op(DH ),op(BH ) ALLHIGHREG8( op)
#define ALLREG16(op)  op(AX ),op(CX ),op(DX ),op(BX ),op(SP ),op(BP ),op(SI ),op(DI ) ALLHIGHREG16(op)
#define ALLREG32(op)  op(EAX),op(ECX),op(EDX),op(EBX),op(ESP),op(EBP),op(ESI),op(EDI) ALLHIGHREG32(op)
#define ALLREG64(op) ,op(RAX),op(RCX),op(RDX),op(RBX),op(RSP),op(RBP),op(RSI),op(RDI) ALLHIGHREG64(op)
#else
#define ALLREG8(    op   )  op( AL   ),op( BH   ) ALLHIGHREG8(    op   )
#define ALLREG16(   op   )  op( AX   ),op( DI   ) ALLHIGHREG16(   op   )
#define ALLREG32(   op   )  op( EAX  ),op( EDI  ) ALLHIGHREG32(   op   )
#define ALLREG64(   op   ) ,op( RAX  ),op( RDI  ) ALLHIGHREG64(   op   )
#define ALLREG8IMM( ins,v)  ins(AL ,v),ins(BH ,v) ALLHIGHREG8IMM( ins,v)
#define ALLREG16IMM(ins,v)  ins(AX ,v),ins(DI ,v) ALLHIGHREG16IMM(ins,v)
#define ALLREG32IMM(ins,v)  ins(EAX,v),ins(EDI,v) ALLHIGHREG32IMM(ins,v)
#define ALLREG64IMM(ins,v) ,ins(RAX,v),ins(RDI,v) ALLHIGHREG64IMM(ins,v)

#endif // TEST_ALLREGISTERS

#ifdef IS32BIT
#define OP_1ARGX64_QWORD(name)
#define FOR_ALL_REG64(op)
#else // IS64BIT
#define OP_1ARGX64_QWORD(name) ,name##_QWORD
#endif // IS32BIT

#define ALLREG(op)     ALLREG8(   op      ),ALLREG16(   op      ),ALLREG32(   op)                                                           ALLREG64(   op )
#define ALLREGIMM(ins) ALLREG8IMM(ins,0xff),ALLREG16IMM(ins,0xff),ALLREG16IMM(ins,0xffff),ALLREG32IMM(ins,0xff),ALLREG32IMM(ins,0xffffffff) ALLREG64IMM(ins,0xff) ALLREG64IMM(ins,0xffffffff)

#define OP_2ARG_BYTE(    name)  ALLREG(name##_BYTE     )
#define OP_2ARG_WORD(    name)  ALLREG(name##_WORD     )
#define OP_2ARG_DWORD(   name)  ALLREG(name##_DWORD    )
#define OP_2ARGX64_QWORD(name) ,ALLREG(name##_QWORD    )

#define OP_1ARG(              name) name##_BYTE ,name##_DWORD ,name##_WORD  OP_1ARGX64_QWORD(name)

#define OP_2ARG_NOBYTE(name) \
  OP_2ARG_WORD(    name)     \
 ,OP_2ARG_DWORD(   name)     \
  OP_2ARGX64_QWORD(name)

#define DEFINE_INSTRUCTIONLIST(a, list) const IntelInstruction a[] = { list }
#define EMIT_INSTRUCTIONLIST(a) for(int i = 0; i < ARRAYSIZE(a); i++) { emit(a[i]); }
#define EMITALL(name)   { DEFINE_INSTRUCTIONLIST(l, ALLREG(   name)); EMIT_INSTRUCTIONLIST(l); }
#define EMITALLIMM(ins) { DEFINE_INSTRUCTIONLIST(l, ALLREGIMM(ins )); EMIT_INSTRUCTIONLIST(l); }

#define EMIT_ALLR8( name) EMITALL(name##_R8 )
#define EMIT_ALLR16(name) EMITALL(name##_R16)
#define EMIT_ALLR32(name) EMITALL(name##_R32)
#ifdef IS32BIT
#define EMIT_ALLR64(name)
#else // IS64BIT
#define EMIT_ALLR64(name) EMITALL(name##_R64)
#endif

#define UNKNOWN_OPCODE(  dst)               B2INSA(0x8700 + ((dst)<<3))                     // Build src with MEM_ADDR-macros, REGREG
//Real FMOD(Real x, Real y) {
//  return fmod(x,y);
//}

static BYTE   staticInt8  = 0x12;
static USHORT staticInt16 = 0x1234;
static UINT   staticInt32 = 0x12345678;

#ifdef IS64BIT
static UINT64 staticInt64 = 0x123456789abcdef1;
#endif // IS64BIT

#define TEST_INCDEC_PUSHPOP
#define TEST_MOV
#define TEST_MUL

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

  emit(REGREG(MOV_MEM_REG(  BL), BH ));
  emit(REGREG(MOV_MEM_REG(  BH), DH ));
  emit(REGREG(MOV_MEM_REG(  DI), SI ));
  emit(REGREG(MOV_MEM_REG( EDI), ESI));

  emit(REGREG(MOV_MEM_REG(  BL), BH ));
  emit(REGREG(MOV_MEM_REG(  BH), DH ));
  emit(REGREG(MOV_MEM_REG(  DI), SI ));
  emit(REGREG(MOV_MEM_REG( EDI), ESI));

  emit(MOV_R8_IMM_BYTE(  BL ));      addBytes(addr,1);
  emit(MOV_R8_IMM_BYTE(  BH ));      addBytes((char*)addr+1,1);
  emit(MOV_R16_IMM_WORD( DI ));      addBytes(addr,2);
  emit(MOV_R32_IMM_DWORD(EDI));      addBytes(addr,4);

  emit(MOV_TO_AL_IMM_ADDR_BYTE    ); addBytes(addr,sizeof(addr));
  emit(MOV_TO_AX_IMM_ADDR_WORD    ); addBytes(addr,sizeof(addr));
  emit(MOV_TO_EAX_IMM_ADDR_DWORD  ); addBytes(addr,sizeof(addr));
  emit(MOV_FROM_AL_IMM_ADDR_BYTE  ); addBytes(addr,sizeof(addr));
  emit(MOV_FROM_AX_IMM_ADDR_WORD  ); addBytes(addr,sizeof(addr));
  emit(MOV_FROM_EAX_IMM_ADDR_DWORD); addBytes(addr,sizeof(addr));

#ifdef IS64BIT
  emit(REGREG(MOV_MEM_REG(RDI), RSI));
  emit(REGREG(MOV_MEM_REG(RDI), RSI));
  emit(MOV_R64_IMM_QWORD(RDI));      addBytes(addr,8);
  emit(MOV_TO_RAX_IMM_ADDR_QWORD  ); addBytes(addr,sizeof(addr));
  emit(MOV_FROM_RAX_IMM_ADDR_QWORD); addBytes(addr,sizeof(addr));
#endif

#endif // TEST_MOV

  static int j = 0x12345678;
#ifdef IS64BIT
  emit(REP); emit(SCAS_QWORD);
#endif
  emit(REP); emit(SCAS_DWORD);

#ifdef IS32BIT
#define ADDR_REGISTER EDI
#else
#define ADDR_REGISTER RDI
#endif // IS32BIT

#ifdef TEST_MUL
  emit(MEM_ADDR_PTR( MUL_DWORD,ADDR_REGISTER));
  emit(MEM_ADDR_PTR(IMUL_DWORD,ADDR_REGISTER));
#ifdef IS64BIT
  emit(MEM_ADDR_PTR(MUL_QWORD ,ADDR_REGISTER));
  emit(MEM_ADDR_PTR(IMUL_QWORD,ADDR_REGISTER));
#endif

  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_DWORD(EAX),ADDR_REGISTER)); addBytes(&staticInt32,4);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_DWORD(ECX),ADDR_REGISTER)); addBytes(&staticInt32,4);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_DWORD(EDX),ADDR_REGISTER)); addBytes(&staticInt32,4);

#ifdef IS64BIT
  emit(MEM_ADDR_PTR(IMUL3_QWORD_IMM_DWORD(RAX),ADDR_REGISTER)); addBytes(&staticInt32,4);
  emit(MEM_ADDR_PTR(IMUL3_QWORD_IMM_DWORD(RCX),ADDR_REGISTER)); addBytes(&staticInt32,4);
  emit(MEM_ADDR_PTR(IMUL3_QWORD_IMM_DWORD(RDX),ADDR_REGISTER)); addBytes(&staticInt32,4);
#endif // IS64BIT

  emit(MOV_R32_IMM_DWORD(EDI)); addBytes(addr,4);
  emit(MEM_ADDR_PTR(MUL_DWORD,ADDR_REGISTER));

  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_DWORD(EAX),ADDR_REGISTER)); addBytes(&staticInt32,4);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_DWORD(ECX),ADDR_REGISTER)); addBytes(&staticInt32,4);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_DWORD(EDX),ADDR_REGISTER)); addBytes(&staticInt32,4);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_DWORD(EBX),ADDR_REGISTER)); addBytes(&staticInt32,4);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_DWORD(ESP),ADDR_REGISTER)); addBytes(&staticInt32,4);
  emit(MEM_ADDR_ESP(IMUL3_DWORD_IMM_DWORD(EAX)));     addBytes(&staticInt32,4);
  emit(MEM_ADDR_ESP(IMUL3_DWORD_IMM_DWORD(ECX)));     addBytes(&staticInt32,4);
  emit(MEM_ADDR_ESP(IMUL3_DWORD_IMM_DWORD(EDX)));     addBytes(&staticInt32,4);
  emit(MEM_ADDR_ESP(IMUL3_DWORD_IMM_DWORD(EBX)));     addBytes(&staticInt32,4);
  emit(MEM_ADDR_ESP(IMUL3_DWORD_IMM_DWORD(ESP)));     addBytes(&staticInt32,4);

  emit(REGREG(IMUL3_DWORD_IMM_DWORD(EAX),ECX));       addBytes(&staticInt32,4);
  emit(REGREG(IMUL3_DWORD_IMM_DWORD(ECX),ECX));       addBytes(&staticInt32,4);
  emit(REGREG(IMUL3_DWORD_IMM_DWORD(EDX),ECX));       addBytes(&staticInt32,4);
  emit(REGREG(IMUL3_DWORD_IMM_DWORD(EBX),ECX));       addBytes(&staticInt32,4);
  emit(REGREG(IMUL3_DWORD_IMM_DWORD(ESP),ECX));       addBytes(&staticInt32,4);
 
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(EAX),ADDR_REGISTER));   addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(ECX),ADDR_REGISTER));   addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(EDX),ADDR_REGISTER));   addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(EBX),ADDR_REGISTER));   addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(ESP),ADDR_REGISTER));   addBytes(&staticInt32,1);
  emit(MEM_ADDR_ESP(IMUL3_DWORD_IMM_BYTE(EAX)));                 addBytes(&staticInt32,1);
  emit(MEM_ADDR_ESP(IMUL3_DWORD_IMM_BYTE(ECX)));                 addBytes(&staticInt32,1);
  emit(MEM_ADDR_ESP(IMUL3_DWORD_IMM_BYTE(EDX)));                 addBytes(&staticInt32,1);
  emit(MEM_ADDR_ESP(IMUL3_DWORD_IMM_BYTE(EBX)));                 addBytes(&staticInt32,1);
  emit(MEM_ADDR_ESP(IMUL3_DWORD_IMM_BYTE(ESP)));                 addBytes(&staticInt32,1);

  emit(REGREG(IMUL3_DWORD_IMM_BYTE(EAX),ECX));                   addBytes(&staticInt32,1);
  emit(REGREG(IMUL3_DWORD_IMM_BYTE(ECX),ECX));                   addBytes(&staticInt32,1);
  emit(REGREG(IMUL3_DWORD_IMM_BYTE(EDX),ECX));                   addBytes(&staticInt32,1);
  emit(REGREG(IMUL3_DWORD_IMM_BYTE(EBX),ECX));                   addBytes(&staticInt32,1);
  emit(REGREG(IMUL3_DWORD_IMM_BYTE(ESP),ECX));                   addBytes(&staticInt32,1);

  emit(MEM_ADDR_PTR(IMUL3_WORD_IMM_WORD(AX),ADDR_REGISTER));     addBytes(&staticInt32,2);
  emit(MEM_ADDR_PTR(IMUL3_WORD_IMM_WORD(CX),ADDR_REGISTER));     addBytes(&staticInt32,2);
  emit(MEM_ADDR_PTR(IMUL3_WORD_IMM_WORD(DX),ADDR_REGISTER));     addBytes(&staticInt32,2);
  emit(MEM_ADDR_PTR(IMUL3_WORD_IMM_WORD(BX),ADDR_REGISTER));     addBytes(&staticInt32,2);
  emit(MEM_ADDR_PTR(IMUL3_WORD_IMM_WORD(SP),ADDR_REGISTER));     addBytes(&staticInt32,2);
  emit(MEM_ADDR_ESP(IMUL3_WORD_IMM_WORD(AX)));                   addBytes(&staticInt32,2);
  emit(MEM_ADDR_ESP(IMUL3_WORD_IMM_WORD(CX)));                   addBytes(&staticInt32,2);
  emit(MEM_ADDR_ESP(IMUL3_WORD_IMM_WORD(DX)));                   addBytes(&staticInt32,2);
  emit(MEM_ADDR_ESP(IMUL3_WORD_IMM_WORD(BX)));                   addBytes(&staticInt32,2);
  emit(MEM_ADDR_ESP(IMUL3_WORD_IMM_WORD(SP)));                   addBytes(&staticInt32,2);

  emit(REGREG(IMUL3_WORD_IMM_WORD(AX),CX));                      addBytes(&staticInt32,2);
  emit(REGREG(IMUL3_WORD_IMM_WORD(CX),CX));                      addBytes(&staticInt32,2);
  emit(REGREG(IMUL3_WORD_IMM_WORD(DX),CX));                      addBytes(&staticInt32,2);
  emit(REGREG(IMUL3_WORD_IMM_WORD(BX),CX));                      addBytes(&staticInt32,2);
  emit(REGREG(IMUL3_WORD_IMM_WORD(SP),CX));                      addBytes(&staticInt32,2);

  emit(MEM_ADDR_PTR(IMUL3_WORD_IMM_BYTE(AX),ADDR_REGISTER));     addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_WORD_IMM_BYTE(CX),ADDR_REGISTER));     addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_WORD_IMM_BYTE(DX),ADDR_REGISTER));     addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_WORD_IMM_BYTE(BX),ADDR_REGISTER));     addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_WORD_IMM_BYTE(SP),ADDR_REGISTER));     addBytes(&staticInt32,1);
  emit(MEM_ADDR_ESP(IMUL3_WORD_IMM_BYTE(AX)));                   addBytes(&staticInt32,1);
  emit(MEM_ADDR_ESP(IMUL3_WORD_IMM_BYTE(CX)));                   addBytes(&staticInt32,1);
  emit(MEM_ADDR_ESP(IMUL3_WORD_IMM_BYTE(DX)));                   addBytes(&staticInt32,1);
  emit(MEM_ADDR_ESP(IMUL3_WORD_IMM_BYTE(BX)));                   addBytes(&staticInt32,1);
  emit(MEM_ADDR_ESP(IMUL3_WORD_IMM_BYTE(SP)));                   addBytes(&staticInt32,1);

  emit(REGREG(IMUL3_WORD_IMM_BYTE(AX),CX));                      addBytes(&staticInt32,1);
  emit(REGREG(IMUL3_WORD_IMM_BYTE(CX),CX));                      addBytes(&staticInt32,1);
  emit(REGREG(IMUL3_WORD_IMM_BYTE(DX),CX));                      addBytes(&staticInt32,1);
  emit(REGREG(IMUL3_WORD_IMM_BYTE(BX),CX));                      addBytes(&staticInt32,1);
  emit(REGREG(IMUL3_WORD_IMM_BYTE(SP),CX));                      addBytes(&staticInt32,1);

  emit(NOOP);
#endif // TEST_MUL

  EMITALLIMM(ADD_REG_IMM)
  EMITALLIMM(OR_REG_IMM )
  EMITALLIMM(ADC_REG_IMM)
  EMITALLIMM(SBB_REG_IMM)
  EMITALLIMM(AND_REG_IMM)
  EMITALLIMM(SUB_REG_IMM)
  EMITALLIMM(XOR_REG_IMM)
  EMITALLIMM(CMP_REG_IMM)

  const IntelOpcode opcodes[] = {
    ALLREG(    ADD_MEM_REG)
   ,ALLREG(    ADD_REG_MEM)

   ,SETO   ,SETNO   ,SETB    ,SETAE
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
   ,OP_2ARG_NOBYTE(SHLD)
   ,OP_2ARG_NOBYTE(SHRD)
   ,OP_2ARG_NOBYTE(BSF)
   ,OP_2ARG_NOBYTE(BSR)

   ,IMUL2_R32_DWORD(EAX),IMUL2_R32_DWORD(ECX),IMUL2_R32_DWORD(EDX),IMUL2_R32_DWORD(EBX)
   ,IMUL2_R32_DWORD(ESP),IMUL2_R32_DWORD(EBP),IMUL2_R32_DWORD(ESI),IMUL2_R32_DWORD(EDI)

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

  for(int i = 0; i < ARRAYSIZE(opcodes); i++) {

    const IntelOpcode &op = opcodes[i];

    int r;
    if(op.hasRegRegMode()) {
      const BYTE *regList = regListArray[op.getOpSize()];
      const int maxReg = REF_REGLISTSIZE;
      for(r = 0; r < maxReg; r++) {
        const BYTE reg = regList[r];
        emit(REGREG(op, reg));                                           // size=2 ex:add eax,ecx
      }
    }
    emit(NOOP);

    // ptr[eax]
    for(r = 0; r < ARRAYSIZE(refRegisters); r++) {
      const BYTE reg = refRegisters[r];
      if(((reg&7) == 4) || ((reg&7) == 5)) continue;
      emit(MEM_ADDR_PTR(op, reg));                                        // size=2 reg!=ESP,EBP                         ex:fld DWORD PTR[eax]
    }

    emit(NOOP);
    // ptr[eax+127]
    for(r = 0; r < ARRAYSIZE(refRegisters); r++) {
      const BYTE reg = refRegisters[r];
      if((reg&7) == 4) continue;
      emit(MEM_ADDR_PTR1(op, reg, -1));                                   // size=3 reg!=ESP        offs1=1 byte signed  ex.fld DWORD PTR[eax+127]
      emit(MEM_ADDR_PTR1(op, reg,  1));
    }

    emit(NOOP);
    // ptr[eax+0x12345678]
    for(r = 0; r < ARRAYSIZE(refRegisters); r++) {
      const BYTE reg = refRegisters[r];
      if((reg&7) == 4) continue;
      emit(MEM_ADDR_PTR4(op, reg,-0x12345678));                           // size=6 reg!=ESP        offs4=4 bytes signed ex fld DWORD PTR[  eax+0x12345678]
      emit(MEM_ADDR_PTR4(op, reg, 0x12345678));
    }

    emit(NOOP);
    // ptr[(eax<<p2)+0x12345678]
    for(int p2 = 0; p2 < 4; p2++) {
      for(int r = 0; r < ARRAYSIZE(refRegisters); r++) {
        const BYTE reg = refRegisters[r];
        if((reg&7) == 4) continue;
        emit(MEM_ADDR_MP2PTR4(op, reg, p2,-0x12345678));
        emit(MEM_ADDR_MP2PTR4(op, reg, p2, 0x12345678));                  // size=7 reg!=ESP p2=0-3 offs4=4 bytes signed ex fld DWORD PTR[2*eax+0x12345678]
      }
    }

    emit(NOOP);
    // ptr[eax+(ecx<<p2)]
    for(int r1 = 0; r1 < ARRAYSIZE(refRegisters); r1++) {
      const BYTE addReg = refRegisters[r1];
      if((addReg&7) == 4) continue;
      for(int p2 = 0; p2 < 4; p2++) {
        for(int r = 0; r < ARRAYSIZE(refRegisters); r++) {
          const BYTE reg = refRegisters[r];
          if((reg&7) == 5) continue;
          emit(MEM_ADDR_PTRMP2REG(op, reg, addReg, p2));                  // size=3 reg!=EBP addReg!=ESP p2=0-3          ex fld DWORD PTR[esp+2*ecx]
        }
      }
    }

    emit(NOOP);

    // ptr[eax+(ecx<<p2)+127]
    for(int r1 = 0; r1 < ARRAYSIZE(refRegisters); r1++) {
      const BYTE addReg = refRegisters[r1];
      if((addReg&7) == 4) continue;
      for(int p2 = 0; p2 < 4; p2++) {
        for(int r = 0; r < ARRAYSIZE(refRegisters); r++) {
          const BYTE reg = refRegisters[r];
          emit(MEM_ADDR_PTRMP2REG1(op, reg, addReg, p2, 1));              // size=4 addReg!=ESP p2=0-3                   ex fld DWORD PTR[ebp+2*ecx+127]
          emit(MEM_ADDR_PTRMP2REG1(op, reg, addReg, p2,-1));
        }
      }
    }

    emit(NOOP);
    // ptr[eax+(ecx<<p2)+0x12345678]
    for(int r1 = 0; r1 < ARRAYSIZE(refRegisters); r1++) {
      const BYTE addReg = refRegisters[r1];
      if((addReg&7) == 4) continue;
      for(int p2 = 0; p2 < 4; p2++) {
        for(int r = 0; r < ARRAYSIZE(refRegisters); r++) {
          const BYTE reg = refRegisters[r];
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
/*
  emit(MOV_TO_AL_IMM_ADDR_BYTE     ); addBytes(&addr,sizeof(addr));
  emit(MOV_TO_AX_IMM_ADDR_WORD     ); addBytes(&addr,sizeof(addr));
  emit(MOV_TO_EAX_IMM_ADDR_DWORD   ); addBytes(&addr,sizeof(addr));

  emit(MOV_FROM_AL_IMM_ADDR_BYTE   ); addBytes(&addr,sizeof(addr));
  emit(MOV_FROM_AX_IMM_ADDR_WORD   ); addBytes(&addr,sizeof(addr));
  emit(MOV_FROM_EAX_IMM_ADDR_DWORD ); addBytes(&addr,sizeof(addr));
*/
}

#endif // TEST_MACHINECODE
