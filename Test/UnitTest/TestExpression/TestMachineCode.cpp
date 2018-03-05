#include "stdafx.h"
#include <Math/Expression/Expression.h>

#define TEST_MACHINECODE

#ifdef TEST_MACHINECODE

#define TEST_ALLREGISTERS

static const BYTE r8List[] = {
    AL
#ifndef TEST_ALLREGISTERS
   ,AH  ,CH
#else
   ,CL  ,DL  ,BL  ,AH  ,CH  ,DH  ,BH
#endif // TEST_ALLREGISTERS
#ifdef IS64BIT
   ,R8B
#ifdef TEST_ALLREGISTERS
   ,R9B  ,R10B ,R11B ,R12B ,R13B ,R14B ,R15B
#endif // TEST_ALLREGISTERS
#endif // IS64BIT
  };

  const BYTE r16List[] = {
    AX
#ifndef TEST_ALLREGISTERS
   ,SP   ,BP
#else
   ,CX   ,DX   ,BX   ,SP   ,BP   ,SI  ,DI
#endif // TEST_ALLREGISTERS

#ifdef IS64BIT
   ,R8W
#ifdef TEST_ALLREGISTERS
   ,R9W  ,R10W ,R11W ,R12W ,R13W ,R14W ,R15W
#endif // TEST_ALLREGISTERS
#endif // IS64BIT
  };

  const BYTE r32List[] = {
    EAX
#ifndef TEST_ALLREGISTERS
   ,ESP  ,EBP
#else
   ,ECX  ,EDX  ,EBX  ,ESP  ,EBP  ,ESI  ,EDI
#endif // TEST_ALLREGISTERS

#ifdef IS64BIT
   ,R8D
#ifdef TEST_ALLREGISTERS
   ,R9D  ,R10D ,R11D ,R12D ,R13D ,R14D ,R15D
#endif // TEST_ALLREGISTERS
#endif // IS64BIT
  };

#ifdef IS64BIT
  const BYTE r64List[] = {
    RAX
#ifndef TEST_ALLREGISTERS
   ,RSP  ,RBP
#else
   ,RCX  ,RDX  ,RBX  ,RSP  ,RBP  ,RSI  ,RDI
#endif // TEST_ALLREGISTERS
   ,R8
#ifdef TEST_ALLREGISTERS
   ,R9   ,R10  ,R11  ,R12  ,R13  ,R14  ,R15
#endif // TEST_ALLREGISTERS
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
#define ALLHIGHREG8(    op   )
#define ALLHIGHREG16(   op   )
#define ALLHIGHREG32(   op   )
#define ALLHIGHREG64(   op   )
#define ALLHIGHREG(     op   )
#define ALLREG64(       op   )
#define ALLHIGHREG8IMM( ins,v)
#define ALLHIGHREG16IMM(ins,v)
#define ALLHIGHREG32IMM(ins,v)
#define ALLREG64IMM(    ins,v)
#else // IS64BIT

#ifdef TEST_ALLREGISTERS
#define ALLHIGHREG8(    op)    ,op(R8B),op(R9B),op(R10B),op(R11B),op(R12B),op(R13B),op(R14B),op(R15B)
#define ALLHIGHREG16(   op)    ,op(R8W),op(R9W),op(R10W),op(R11W),op(R12W),op(R13W),op(R14W),op(R15W)
#define ALLHIGHREG32(   op)    ,op(R8D),op(R9D),op(R10D),op(R11D),op(R12D),op(R13D),op(R14D),op(R15D)
#define ALLHIGHREG64(   op)    ,op(R8 ),op(R9 ),op(R10 ),op(R11 ),op(R12 ),op(R13 ),op(R14 ),op(R15 )

#define ALLHIGHREG8IMM( ins,v) ,ins(R8B,v),ins(R9B,v),ins(R10B,v),ins(R11B,v),ins(R12B,v),ins(R13B,v),ins(R14B,v),ins(R15B,v)
#define ALLHIGHREG16IMM(ins,v) ,ins(R8W,v),ins(R9W,v),ins(R10W,v),ins(R11W,v),ins(R12W,v),ins(R13W,v),ins(R14W,v),ins(R15W,v)
#define ALLHIGHREG32IMM(ins,v) ,ins(R8D,v),ins(R9D,v),ins(R10D,v),ins(R11D,v),ins(R12D,v),ins(R13D,v),ins(R14D,v),ins(R15D,v)
#define ALLHIGHREG64IMM(ins,v) ,ins(R8 ,v),ins(R9 ,v),ins(R10 ,v),ins(R11 ,v),ins(R12 ,v),ins(R13 ,v),ins(R14 ,v),ins(R15 ,v)

#else // only subset of all registers
#define ALLHIGHREG8(    op   ) ,op( R8B  ),op( R15B  )
#define ALLHIGHREG16(   op   ) ,op( R8W  ),op( R15W  )
#define ALLHIGHREG32(   op   ) ,op( R8D  ),op( R15D  )
#define ALLHIGHREG64(   op   ) ,op( R8   ),op( R15   )

#define ALLHIGHREG8IMM( ins,v) ,ins(R8B,v),ins(R15B,v)
#define ALLHIGHREG16IMM(ins,v) ,ins(R8W,v),ins(R15W,v)
#define ALLHIGHREG32IMM(ins,v) ,ins(R8D,v),ins(R15D,v)
#define ALLHIGHREG64IMM(ins,v) ,ins(R8 ,v),ins(R15 ,v)
#endif // TEST_ALLREGISTERS
#endif // IS64BIT

#ifdef TEST_ALLREGISTERS
#define ALLREG8( op)        op( AL   ),op( CL   ),op( DL   ),op( BL   ),op( AH   ),op( CH   ),op( DH   ),op( BH   ) ALLHIGHREG8(    op   )
#define ALLREG16(op)        op( AX   ),op( CX   ),op( DX   ),op( BX   ),op( SP   ),op( BP   ),op( SI   ),op( DI   ) ALLHIGHREG16(   op   )
#define ALLREG32(op)        op( EAX  ),op( ECX  ),op( EDX  ),op( EBX  ),op( ESP  ),op( EBP  ),op( ESI  ),op( EDI  ) ALLHIGHREG32(   op   )
#define ALLREG8IMM( ins,v)  ins(AL ,v),ins(CL ,v),ins(DL ,v),ins(BL ,v),ins(AH ,v),ins(CH ,v),ins(DH ,v),ins(BH ,v) ALLHIGHREG8IMM( ins,v)
#define ALLREG16IMM(ins,v)  ins(AX ,v),ins(CX ,v),ins(DX ,v),ins(BX ,v),ins(SP ,v),ins(BP ,v),ins(SI ,v),ins(DI ,v) ALLHIGHREG16IMM(ins,v)
#define ALLREG32IMM(ins,v)  ins(EAX,v),ins(ECX,v),ins(EDX,v),ins(EBX,v),ins(ESP,v),ins(EBP,v),ins(ESI,v),ins(EDI,v) ALLHIGHREG32IMM(ins,v)
#ifdef IS64BIT
#define ALLREG64(   op   ) ,op( RAX  ),op( RCX  ),op( RDX)  ,op( RBX  ),op( RSP  ),op( RBP  ),op( RSI  ),op( RDI  ) ALLHIGHREG64(op      )
#define ALLREG64IMM(ins,v) ,ins(RAX,v),ins(RCX,v),ins(RDX,v),ins(RBX,v),ins(RSP,v),ins(RBP,v),ins(RSI,v),ins(RDI,v) ALLHIGHREG64IMM(ins,v)
#endif // IS64BIT
#else
#define ALLREG8(    op   )  op( AL   ),op( AH   ),op( CH   ),op( BH   ) ALLHIGHREG8(    op   )
#define ALLREG16(   op   )  op( AX   ),op( SP   ),op( BP   ),op( DI   ) ALLHIGHREG16(   op   )
#define ALLREG32(   op   )  op( EAX  ),op( ESP  ),op( EBP  ),op( EDI  ) ALLHIGHREG32(   op   )
#define ALLREG8IMM( ins,v)  ins(AL ,v),ins(AH ,v),ins(CH ,v),ins(BH ,v) ALLHIGHREG8IMM( ins,v)
#define ALLREG16IMM(ins,v)  ins(AX ,v),ins(SP ,v),ins(BP ,v),ins(DI ,v) ALLHIGHREG16IMM(ins,v)
#define ALLREG32IMM(ins,v)  ins(EAX,v),ins(ESP,v),ins(EBP,v),ins(EDI,v) ALLHIGHREG32IMM(ins,v)

#ifdef IS64BIT
#define ALLREG64(   op   ) ,op( RAX  ),op( RSP  ),op( RBP  ),op( RDI  ) ALLHIGHREG64(   op   )
#define ALLREG64IMM(ins,v) ,ins(RAX,v),ins(RSP,v),ins(RBP,v),ins(RDI,v) ALLHIGHREG64IMM(ins,v)
#endif
#endif // TEST_ALLREGISTERS

#ifdef IS32BIT
#define OP_1ARG_QWORD(name)
#define FOR_ALL_REG64(op)
#else // IS64BIT
#define OP_1ARG_QWORD(name) ,name##_QWORD
#endif // IS32BIT

#define ALLREG(op)      ALLREG8(   op      ),ALLREG16(   op      ),ALLREG32(   op)                                                           ALLREG64(   op )
#define ALLREGIMM( ins) ALLREG8IMM(ins,0x7f),ALLREG16IMM(ins,0x7f),ALLREG16IMM(ins,0x7fff),ALLREG32IMM(ins,0x7f),ALLREG32IMM(ins,0xffffffff) ALLREG64IMM(ins,0x7f) ALLREG64IMM(ins,0xffffffff)
#define ALLREGIMM1(ins) ALLREG8IMM(ins,0x7f),ALLREG16IMM(ins,0x7f),ALLREG16IMM(ins,0x7fff),ALLREG32IMM(ins,0x7f),ALLREG32IMM(ins,0x7fffffff) ALLREG64IMM(ins,0x7f) ALLREG64IMM(ins,0xffffffff) ALLREG64IMM(ins,0x7fffffffffffffffi64)

#define OP_2ARG_BYTE(    name)  ALLREG(name##_BYTE     )
#define OP_2ARG_WORD(    name)  ALLREG(name##_WORD     )
#define OP_2ARG_DWORD(   name)  ALLREG(name##_DWORD    )

#ifdef IS32BIT
#define OP_2ARG_QWORD(name)
#else
#define OP_2ARG_QWORD(name) ,ALLREG(name##_QWORD    )
#endif
#define OP_1ARG(          name) name##_BYTE ,name##_DWORD ,name##_WORD  OP_1ARG_QWORD(name)

#define OP_2ARG_NOBYTE(name) \
  OP_2ARG_WORD(    name)     \
 ,OP_2ARG_DWORD(   name)     \
  OP_2ARG_QWORD(   name)

#define DEFINE_INSTRUCTIONLIST(a, list) const IntelInstruction a[] = { list }
#define EMIT_INSTRUCTIONLIST(a) for(int i = 0; i < ARRAYSIZE(a); i++) { emit(a[i]); }
#define EMITALL(name)    { DEFINE_INSTRUCTIONLIST(l, ALLREG(     name)); EMIT_INSTRUCTIONLIST(l); }
#define EMITALLIMM( ins) { DEFINE_INSTRUCTIONLIST(l, ALLREGIMM(  ins )); EMIT_INSTRUCTIONLIST(l); }
#define EMITALLIMM1(ins) { DEFINE_INSTRUCTIONLIST(l, ALLREGIMM1( ins )); EMIT_INSTRUCTIONLIST(l); }

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

static char  staticInt8  = 0x12;
static short staticInt16 = 0x3412;
static int   staticInt32 = 0x78563412;

#ifdef IS64BIT
static INT64 staticInt64 = 0xf0debc9a78563412;
#endif // IS64BIT

//#define TEST_INCDEC_PUSHPOP
//#define TEST_MOV_IMM
//#define TEST_REP_SCAS
//#define TEST_MUL
#define TEST_REG_IMM
//#define TEST_MEM_ADDR

class TestMachineCode : public MachineCode {
public:
  TestMachineCode();
};

TestMachineCode::TestMachineCode() {
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

#ifdef TEST_MOV_IMM

  emit(MOV_R8_IMM_BYTE(  BL ));      addBytes(addr,1);
  emit(MOV_R8_IMM_BYTE(  BH ));      addBytes((char*)addr+1,1);
  emit(MOV_R16_IMM_WORD( DI ));      addBytes(addr,2);
  emit(MOV_R32_IMM_DWORD(EDI));      addBytes(addr,4);
#ifdef IS64BIT
  emit(MOV_R8_IMM_BYTE(  R8B));      addBytes(addr,1);
  emit(MOV_R8_IMM_BYTE(  R9B));      addBytes((char*)addr+1,1);
  emit(MOV_R16_IMM_WORD( R8W));      addBytes(addr,2);
  emit(MOV_R32_IMM_DWORD(R8D));      addBytes(addr,4);
  emit(MOV_R64_IMM_QWORD(RDI));      addBytes(addr,8);
  emit(MOV_R64_IMM_QWORD(R8 ));      addBytes(addr,8);
#endif

  emit(MOV_TO_AL_IMM_ADDR_BYTE    ); addBytes(addr,sizeof(addr));
  emit(MOV_FROM_AL_IMM_ADDR_BYTE  ); addBytes(addr,sizeof(addr));
  emit(MOV_TO_AX_IMM_ADDR_WORD    ); addBytes(addr,sizeof(addr));
  emit(MOV_FROM_AX_IMM_ADDR_WORD  ); addBytes(addr,sizeof(addr));
  emit(MOV_TO_EAX_IMM_ADDR_DWORD  ); addBytes(addr,sizeof(addr));
  emit(MOV_FROM_EAX_IMM_ADDR_DWORD); addBytes(addr,sizeof(addr));
#ifdef IS64BIT
  emit(MOV_TO_RAX_IMM_ADDR_QWORD  ); addBytes(addr,sizeof(addr));
  emit(MOV_FROM_RAX_IMM_ADDR_QWORD); addBytes(addr,sizeof(addr));
#endif

#endif // TEST_MOV_IMM

#ifdef TEST_REP_SCAS
  emit(REP); emit(SCAS_BYTE );
  emit(REP); emit(SCAS_WORD );
  emit(REP); emit(SCAS_DWORD);
#ifdef IS64BIT
  emit(REP); emit(SCAS_QWORD);
#endif
#endif // TEST_REP_SCAS


#ifdef IS32BIT
#define ADDR_REGISTER EDI
#define SP_REGISTER   ESP
#define BP_REGISTER   EBP
#else
#define ADDR_REGISTER RDI
#define SP_REGISTER   RSP
#define BP_REGISTER   RBP
#endif // IS32BIT

#ifdef TEST_MUL
  emit(MEM_ADDR_PTR( MUL_DWORD,ADDR_REGISTER,0          ));
  emit(MEM_ADDR_PTR(IMUL_DWORD,ADDR_REGISTER,0          ));
  emit(MEM_ADDR_PTR( MUL_DWORD,ADDR_REGISTER,staticInt8 ));
  emit(MEM_ADDR_PTR(IMUL_DWORD,ADDR_REGISTER,staticInt8 ));
  emit(MEM_ADDR_PTR( MUL_DWORD,ADDR_REGISTER,staticInt32));
  emit(MEM_ADDR_PTR(IMUL_DWORD,ADDR_REGISTER,staticInt32));
  emit(MEM_ADDR_PTR( MUL_DWORD,SP_REGISTER  ,0          ));
  emit(MEM_ADDR_PTR(IMUL_DWORD,SP_REGISTER  ,0          ));
  emit(MEM_ADDR_PTR( MUL_DWORD,SP_REGISTER  ,staticInt8 ));
  emit(MEM_ADDR_PTR(IMUL_DWORD,SP_REGISTER  ,staticInt8 ));
  emit(MEM_ADDR_PTR( MUL_DWORD,SP_REGISTER  ,staticInt32));
  emit(MEM_ADDR_PTR(IMUL_DWORD,SP_REGISTER  ,staticInt32));
  emit(MEM_ADDR_PTR( MUL_DWORD,BP_REGISTER  ,0          ));
  emit(MEM_ADDR_PTR(IMUL_DWORD,BP_REGISTER  ,0          ));
  emit(MEM_ADDR_PTR( MUL_DWORD,BP_REGISTER  ,staticInt8 ));
  emit(MEM_ADDR_PTR(IMUL_DWORD,BP_REGISTER  ,staticInt8 ));
  emit(MEM_ADDR_PTR( MUL_DWORD,BP_REGISTER  ,staticInt32));
  emit(MEM_ADDR_PTR(IMUL_DWORD,BP_REGISTER  ,staticInt32));
  emit(NOOP);

#ifdef IS64BIT
  emit(MEM_ADDR_PTR( MUL_QWORD,ADDR_REGISTER,0          ));
  emit(MEM_ADDR_PTR(IMUL_QWORD,ADDR_REGISTER,0          ));
  emit(MEM_ADDR_PTR( MUL_QWORD,ADDR_REGISTER,staticInt8 ));
  emit(MEM_ADDR_PTR(IMUL_QWORD,ADDR_REGISTER,staticInt8 ));
  emit(MEM_ADDR_PTR( MUL_QWORD,ADDR_REGISTER,staticInt32));
  emit(MEM_ADDR_PTR(IMUL_QWORD,ADDR_REGISTER,staticInt32));
  emit(MEM_ADDR_PTR( MUL_QWORD,SP_REGISTER,0            ));
  emit(MEM_ADDR_PTR(IMUL_QWORD,SP_REGISTER,0            ));
  emit(MEM_ADDR_PTR( MUL_QWORD,SP_REGISTER,staticInt8   ));
  emit(MEM_ADDR_PTR(IMUL_QWORD,SP_REGISTER,staticInt8   ));
  emit(MEM_ADDR_PTR( MUL_QWORD,SP_REGISTER,staticInt32  ));
  emit(MEM_ADDR_PTR(IMUL_QWORD,SP_REGISTER,staticInt32  ));
  emit(MEM_ADDR_PTR( MUL_QWORD,BP_REGISTER,0            ));
  emit(MEM_ADDR_PTR(IMUL_QWORD,BP_REGISTER,0            ));
  emit(MEM_ADDR_PTR( MUL_QWORD,BP_REGISTER,staticInt8   ));
  emit(MEM_ADDR_PTR(IMUL_QWORD,BP_REGISTER,staticInt8   ));
  emit(MEM_ADDR_PTR( MUL_QWORD,BP_REGISTER,staticInt32  ));
  emit(MEM_ADDR_PTR(IMUL_QWORD,BP_REGISTER,staticInt32  ));
  emit(NOOP);
#endif

  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_DWORD(EAX),ADDR_REGISTER,0)); addBytes(&staticInt32,4);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_DWORD(ECX),ADDR_REGISTER,0)); addBytes(&staticInt32,4);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_DWORD(EDX),ADDR_REGISTER,0)); addBytes(&staticInt32,4);

#ifdef IS64BIT
  emit(MEM_ADDR_PTR(IMUL3_QWORD_IMM_DWORD(RAX),ADDR_REGISTER,0)); addBytes(&staticInt32,4);
  emit(MEM_ADDR_PTR(IMUL3_QWORD_IMM_DWORD(RCX),ADDR_REGISTER,0)); addBytes(&staticInt32,4);
  emit(MEM_ADDR_PTR(IMUL3_QWORD_IMM_DWORD(RDX),ADDR_REGISTER,0)); addBytes(&staticInt32,4);
#endif // IS64BIT
  emit(NOOP);

  emit(MOV_R32_IMM_DWORD(EDI)); addBytes(addr,4);
  emit(MEM_ADDR_PTR(MUL_DWORD,ADDR_REGISTER,0));

  emit(NOOP);

  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_DWORD(EAX),ADDR_REGISTER,staticInt8)); addBytes(&staticInt32,4);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_DWORD(ECX),ADDR_REGISTER,staticInt8)); addBytes(&staticInt32,4);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_DWORD(EDX),ADDR_REGISTER,staticInt8)); addBytes(&staticInt32,4);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_DWORD(EBX),ADDR_REGISTER,staticInt8)); addBytes(&staticInt32,4);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_DWORD(ESP),ADDR_REGISTER,staticInt8)); addBytes(&staticInt32,4);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_DWORD(EAX),ADDR_REGISTER,staticInt8)); addBytes(&staticInt32,4);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_DWORD(ECX),ADDR_REGISTER,staticInt8)); addBytes(&staticInt32,4);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_DWORD(EDX),ADDR_REGISTER,staticInt8)); addBytes(&staticInt32,4);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_DWORD(EBX),ADDR_REGISTER,staticInt8)); addBytes(&staticInt32,4);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_DWORD(ESP),ADDR_REGISTER,staticInt8)); addBytes(&staticInt32,4);

  emit(NOOP);

  emit(REGREG(IMUL3_DWORD_IMM_DWORD(EAX),ECX));                 addBytes(&staticInt32,4);
  emit(REGREG(IMUL3_DWORD_IMM_DWORD(ECX),ECX));                 addBytes(&staticInt32,4);
  emit(REGREG(IMUL3_DWORD_IMM_DWORD(EDX),ECX));                 addBytes(&staticInt32,4);
  emit(REGREG(IMUL3_DWORD_IMM_DWORD(EBX),ECX));                 addBytes(&staticInt32,4);
  emit(REGREG(IMUL3_DWORD_IMM_DWORD(ESP),ECX));                 addBytes(&staticInt32,4);

  emit(NOOP);

  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(EAX),ADDR_REGISTER,0            ));  addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(ECX),ADDR_REGISTER,0            ));  addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(EDX),ADDR_REGISTER,0            ));  addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(EBX),ADDR_REGISTER,0            ));  addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(ESP),ADDR_REGISTER,0            ));  addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(EAX),ADDR_REGISTER,staticInt8   ));  addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(ECX),ADDR_REGISTER,staticInt8   ));  addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(EDX),ADDR_REGISTER,staticInt8   ));  addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(EBX),ADDR_REGISTER,staticInt8   ));  addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(ESP),ADDR_REGISTER,staticInt8   ));  addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(EAX),ADDR_REGISTER,staticInt32  ));  addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(ECX),ADDR_REGISTER,staticInt32  ));  addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(EDX),ADDR_REGISTER,staticInt32  ));  addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(EBX),ADDR_REGISTER,staticInt32  ));  addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(ESP),ADDR_REGISTER,staticInt32  ));  addBytes(&staticInt32,1);

  emit(NOOP);

  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(EAX),SP_REGISTER  ,0            ));  addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(ECX),SP_REGISTER  ,0            ));  addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(EDX),SP_REGISTER  ,0            ));  addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(EBX),SP_REGISTER  ,0            ));  addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(ESP),SP_REGISTER  ,0            ));  addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(EAX),SP_REGISTER  ,staticInt8   ));  addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(ECX),SP_REGISTER  ,staticInt8   ));  addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(EDX),SP_REGISTER  ,staticInt8   ));  addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(EBX),SP_REGISTER  ,staticInt8   ));  addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(ESP),SP_REGISTER  ,staticInt8   ));  addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(EAX),SP_REGISTER  ,staticInt32  ));  addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(ECX),SP_REGISTER  ,staticInt32  ));  addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(EDX),SP_REGISTER  ,staticInt32  ));  addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(EBX),SP_REGISTER  ,staticInt32  ));  addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(ESP),SP_REGISTER  ,staticInt32  ));  addBytes(&staticInt32,1);

  emit(NOOP);

  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(EAX),BP_REGISTER  ,0            ));  addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(ECX),BP_REGISTER  ,0            ));  addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(EDX),BP_REGISTER  ,0            ));  addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(EBX),BP_REGISTER  ,0            ));  addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(ESP),BP_REGISTER  ,0            ));  addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(EAX),BP_REGISTER  ,staticInt8   ));  addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(ECX),BP_REGISTER  ,staticInt8   ));  addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(EDX),BP_REGISTER  ,staticInt8   ));  addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(EBX),BP_REGISTER  ,staticInt8   ));  addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(ESP),BP_REGISTER  ,staticInt8   ));  addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(EAX),BP_REGISTER  ,staticInt32  ));  addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(ECX),BP_REGISTER  ,staticInt32  ));  addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(EDX),BP_REGISTER  ,staticInt32  ));  addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(EBX),BP_REGISTER  ,staticInt32  ));  addBytes(&staticInt32,1);
  emit(MEM_ADDR_PTR(IMUL3_DWORD_IMM_BYTE(ESP),BP_REGISTER  ,staticInt32  ));  addBytes(&staticInt32,1);

  emit(NOOP);

  emit(REGREG(IMUL3_DWORD_IMM_BYTE(EAX),ECX));                  addBytes(&staticInt32,1);
  emit(REGREG(IMUL3_DWORD_IMM_BYTE(ECX),ECX));                  addBytes(&staticInt32,1);
  emit(REGREG(IMUL3_DWORD_IMM_BYTE(EDX),ECX));                  addBytes(&staticInt32,1);
  emit(REGREG(IMUL3_DWORD_IMM_BYTE(EBX),ECX));                  addBytes(&staticInt32,1);
  emit(REGREG(IMUL3_DWORD_IMM_BYTE(ESP),ECX));                  addBytes(&staticInt32,1);

  emit(REGREG(IMUL3_WORD_IMM_WORD(AX),CX));                     addBytes(&staticInt32,2);
  emit(REGREG(IMUL3_WORD_IMM_WORD(CX),CX));                     addBytes(&staticInt32,2);
  emit(REGREG(IMUL3_WORD_IMM_WORD(DX),CX));                     addBytes(&staticInt32,2);
  emit(REGREG(IMUL3_WORD_IMM_WORD(BX),CX));                     addBytes(&staticInt32,2);
  emit(REGREG(IMUL3_WORD_IMM_WORD(SP),CX));                     addBytes(&staticInt32,2);

  emit(REGREG(IMUL3_WORD_IMM_BYTE(AX),CX));                     addBytes(&staticInt32,1);
  emit(REGREG(IMUL3_WORD_IMM_BYTE(CX),CX));                     addBytes(&staticInt32,1);
  emit(REGREG(IMUL3_WORD_IMM_BYTE(DX),CX));                     addBytes(&staticInt32,1);
  emit(REGREG(IMUL3_WORD_IMM_BYTE(BX),CX));                     addBytes(&staticInt32,1);
  emit(REGREG(IMUL3_WORD_IMM_BYTE(SP),CX));                     addBytes(&staticInt32,1);

  emit(NOOP);
#endif // TEST_MUL

#ifdef TEST_REG_IMM
  EMITALLIMM1(MOV_REG_IMM)
  EMITALLIMM( ADD_REG_IMM)
  EMITALLIMM( OR_REG_IMM )
  EMITALLIMM( ADC_REG_IMM)
  EMITALLIMM( SBB_REG_IMM)
  EMITALLIMM( AND_REG_IMM)
  EMITALLIMM( SUB_REG_IMM)
  EMITALLIMM( XOR_REG_IMM)
  EMITALLIMM( CMP_REG_IMM)
#endif

#ifdef TEST_MEM_ADDR
  const IntelOpcode opcodes[] = {
    ALLREG(    MOV_MEM_REG)
   ,ALLREG(    MOV_REG_MEM)
   ,ALLREG(    ADD_MEM_REG)
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
    if(op.hasRegRegMode()) {
      const BYTE *regList = regListArray[op.getOpSize()];
      const int maxReg = REF_REGLISTSIZE;
      for(int r = 0; r < maxReg; r++) {
        const BYTE reg = regList[r];
        emit(REGREG(op, reg));                                           // size=2 ex:add eax,ecx
      }
    }
    emit(NOOP);

    const int addrOffsets[] = {
      0,127,-128,0x7fffffff,-1
    };
    // ptr[reg+offset]
    for(int j = 0; j < ARRAYSIZE(addrOffsets); j++) {
      for(int r = 0; r < ARRAYSIZE(refRegisters); r++) {
        const BYTE reg = refRegisters[r];
        emit(MEM_ADDR_PTR(op, reg, addrOffsets[j]));
      }
      emit(NOOP);
    }

    // ptr[(eax<<p2)+0x12345678]
    for(int j = 0; j < ARRAYSIZE(addrOffsets); j++) {
      for(int r = 0; r < ARRAYSIZE(refRegisters); r++) {
        const BYTE reg = refRegisters[r];
        if((reg&7) == 4) continue;
        for(int p2 = 0; p2 < 4; p2++) {
          emit(MEM_ADDR_MP2PTR4(op, reg, p2,addrOffsets[j]));
        }
      }
    }
    emit(NOOP);
    // ptr[eax+(ecx<<p2)+offset]
    for(int j = 0; j < ARRAYSIZE(addrOffsets); j++) {
      for(int r = 0; r < ARRAYSIZE(refRegisters); r++) {
        const BYTE reg = refRegisters[r];
        for(int r1 = 0; r1 < ARRAYSIZE(refRegisters); r1++) {
          const BYTE addReg = refRegisters[r1];
          if((addReg&7) == 4) continue;
          for(int p2 = 0; p2 < 4; p2++) {
            emit(MEM_ADDR_PTRMP2REG(op, reg, addReg, p2, addrOffsets[j]));                  // size=3 reg!=EBP addReg!=ESP p2=0-3          ex fld DWORD PTR[esp+2*ecx]
          }
        }
      }
      emit(NOOP);
    }
    emit(MEM_ADDR_DS_IMM(  op,staticInt32   ));
    emit(NOOP);
  }
#endif TEST_MEM_ADDR

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

void generateTestSequence() {
#ifdef TEST_MACHINECODE
  TestMachineCode test;
#endif // TEST_MACHINECODE
}

#ifdef _DEBUG
#ifdef TEST_MACHINECODE

#ifdef IS64BIT
extern "C" void assemblerCode();
#else
void assemblerCode() {
  __asm {
    jmp         End
    add         byte ptr es:[78563412h],al
    add         byte ptr cs:[78563412h],al
    add         byte ptr ss:[78563412h],al
    add         byte ptr ds:[78563412h],al
    imul ecx, dword ptr[esi+8*edi+0xabcddbca],0x12345678
    imul cx ,  word ptr[esi+8*edi+0xabcddbca],0x1234

    cbw
    cwde
    cwd
    cdq
  }
End:;
}
#endif // IS64BIT
#endif // TEST_MACHINECODE
#endif // _DEBUG

void callAssemblerCode() {
#ifdef _DEBUG
#ifdef TEST_MACHINECODE
  assemblerCode();
#endif // TEST_MACHINECODE
#endif // _DEBUG
}
