#include "pch.h"

namespace IntelCPU {

#pragma warning(disable : 4073)
#pragma init_seg(lib)

#define DEFNAME(type,id,...) type id(_T(#id),__VA_ARGS__)

DEFNAME(Opcode0Arg       ,  RET   ,0xC3    );              // Near return to calling procedure
DEFNAME(Opcode0Arg       ,  CMC   ,0xF5    );              // Complement carry flag
DEFNAME(Opcode0Arg       ,  CLC   ,0xF8    );              // Clear carry flag     CF = 0
DEFNAME(Opcode0Arg       ,  STC   ,0xF9    );              // Set   carry flag     CF = 1
DEFNAME(Opcode0Arg       ,  CLI   ,0xFA    );              // Clear interrupt flag IF = 0
DEFNAME(Opcode0Arg       ,  STI   ,0xFB    );              // Set   interrupt flag IF = 1
DEFNAME(Opcode0Arg       ,  CLD   ,0xFC    );              // Clear direction flag DF = 0
DEFNAME(Opcode0Arg       ,  STD   ,0xFD    );              // Set   direction flag DF = 1

#if defined(IS64BIT)
DEFNAME(Opcode0Arg       ,  CLGI  ,0x0F01DD);              // Clear Global Interrupt Flag
DEFNAME(Opcode0Arg       ,  STGI  ,0x0F01DC);              // Set Global Interrupt Flag
#endif // IS64BIT

#define ARG0_SIZEABLE (WORDPTR_ALLOWED|QWORDPTR_ALLOWED|HAS_NONBYTE_SIZEBITS)

#if defined(IS32BIT)
DEFNAME(Opcode0Arg       ,  PUSHFD,0x9C  ,ARG0_SIZEABLE);  // Push EFLAGS onto stack *--SP = FLAGS;
DEFNAME(Opcode0Arg       ,  POPFD ,0x9D  ,ARG0_SIZEABLE);  // Pop  EFLAGS register from stack FLAGS = *SP++
DEFNAME(Opcode0Arg       ,  PUSHF ,PUSHFD,REGSIZE_WORD);   // Push FLAGS  onto stack *--SP = FLAGS;
DEFNAME(Opcode0Arg       ,  POPF  ,POPFD ,REGSIZE_WORD);   // Pop  FLAGS  register from stack FLAGS = *SP++
#else
DEFNAME(Opcode0Arg       ,  PUSHFQ,0x9C  ,ARG0_SIZEABLE);  // Push RFLAGS onto stack *--SP = FLAGS;
DEFNAME(Opcode0Arg       ,  POPFQ ,0x9D  ,ARG0_SIZEABLE);  // Pop  RFLAGS register from stack FLAGS = *SP++
DEFNAME(Opcode0Arg       ,  PUSHF ,PUSHFQ,REGSIZE_WORD);   // Push FLAGS  onto stack *--SP = FLAGS;
DEFNAME(Opcode0Arg       ,  POPF  ,POPFQ ,REGSIZE_WORD);   // Pop  FLAGS  register from stack FLAGS = *SP++
#endif // IS46BIT

DEFNAME(Opcode0Arg       ,  SAHF  ,0x9E);                  // Store AH into FLAGS
DEFNAME(Opcode0Arg       ,  LAHF  ,0x9F);                  // Load FLAGS into AH register
#if defined(IS32BIT)
DEFNAME(Opcode0Arg       ,  PUSHAD,0x60);                  // Push all double-word (32-bit) registers onto stack
DEFNAME(Opcode0Arg       ,  POPAD ,0x61);                  // Pop  all double-word (32-bit) registers from stack
#endif // IS32BIT

DEFNAME(Opcode0Arg       ,  NOOP  ,0x90);
DEFNAME(OpcodeStd2Arg    ,  ADD   ,0x00,0x04,0x80,0,HAS_IMM_XBIT|HAS_DIRBIT1);
DEFNAME(OpcodeStd2Arg    ,  OR    ,0x08,0x0C,0x80,1,HAS_IMM_XBIT|HAS_DIRBIT1);
DEFNAME(OpcodeStd2Arg    ,  ADC   ,0x10,0x14,0x80,2,HAS_IMM_XBIT|HAS_DIRBIT1);
DEFNAME(OpcodeStd2Arg    ,  SBB   ,0x18,0x1C,0x80,3,HAS_IMM_XBIT|HAS_DIRBIT1);
DEFNAME(OpcodeStd2Arg    ,  AND   ,0x20,0x24,0x80,4,HAS_IMM_XBIT|HAS_DIRBIT1);
DEFNAME(OpcodeStd2Arg    ,  SUB   ,0x28,0x2C,0x80,5,HAS_IMM_XBIT|HAS_DIRBIT1);
DEFNAME(OpcodeStd2Arg    ,  XOR   ,0x30,0x34,0x80,6,HAS_IMM_XBIT|HAS_DIRBIT1);
DEFNAME(OpcodeStd2Arg    ,  CMP   ,0x38,0x3C,0x80,7,HAS_IMM_XBIT|HAS_DIRBIT1);

DEFNAME(OpcodeXchg       ,  XCHG       );                  // Exchange Two operands
DEFNAME(Opcode1ArgNoMode ,  BSWAP ,0x0FC8,        0,REGTYPE_GPR_ALLOWED|DWORDGPR_ALLOWED|QWORDGPR_ALLOWED|HAS_REXWBIT);
DEFNAME(OpcodeStd2Arg    ,  TEST  ,0x84,0xA8,0xF6,0);      // Logical Compare. same as AND but doesn't change dst. set SF,ZF,PF according to result

DEFNAME(OpcodeMov        ,  MOV        );
DEFNAME(OpcodeMovExtend  ,  MOVZX ,0x0FB6, 0x0FB7, 0   );  // Move with zero-extend
DEFNAME(OpcodeMovExtend  ,  MOVSX ,0x0FBE, 0x0FBF, 0x63);  // Move with sign-extend

DEFNAME(OpcodeLea        ,  LEA   ,0x8D);                  // Load effective address

DEFNAME(OpcodePushPop    ,  PUSH  ,0x50, 0xFF,6, 0x68);
DEFNAME(OpcodePushPop    ,  POP   ,0x58, 0x8F,0, 0   );

DEFNAME(OpcodeIncDec     ,  INC   ,0xFE,0);
DEFNAME(OpcodeIncDec     ,  DEC   ,0xFE,1);

//        Non existing opcode (0xF6,1);
DEFNAME(Opcode1Arg       ,  NOT   ,0xF6,2);                // Negate the operand, logical NOT
DEFNAME(Opcode1Arg       ,  NEG   ,0xF6,3);                // Two's complement negation, op = -op
DEFNAME(OpcodeIMul       ,  IMUL         );                // Signed multiply   (ax = al*src, dx:ax=ax*src, edx:eax=eax*src, rdx:rax=rax*src)
DEFNAME(Opcode1Arg       ,  IDIV  ,0xF6,7);                // Signed divide   ax      /= src, ah  must contain sign extension of al . al =quot, ah =rem
                                                           //                 dk:ax   /= src. dx  must contain sign extension of ax . ax =quot, dx =rem
                                                           //                 edx:eax /= src. edx must contain sign extension of eax. eax=quot, edx=rem
                                                           //                 rdx:rax /= src. rdx must contain sign extension of rax. rax=quot, rdx=rem

DEFNAME(Opcode2ArgM      ,  MUL   ,0xF6,4);                // Unsigned multiply (ax = al*src, dx:ax=ax*src, edx:eax=eax*src, rdx:rax=rax*src)
DEFNAME(Opcode2ArgM      ,  DIV   ,0xF6,6);                // Unsigned divide   (ax/=src,al=quot,ah=rem,    edx:eax/=src,eax=quot,edx=rem,  rdx:rax/=src,rax=quot,rdx=rem

DEFNAME(OpcodeShiftRot   ,  ROL   ,0     );                // Rotate left  by cl/imm
DEFNAME(OpcodeShiftRot   ,  ROR   ,1     );                // Rotate right by cl/imm
DEFNAME(OpcodeShiftRot   ,  RCL   ,2     );                // Rotate left  by cl/imm (with carry)
DEFNAME(OpcodeShiftRot   ,  RCR   ,3     );                // Rotate right by cl/imm (with carry)
DEFNAME(OpcodeShiftRot   ,  SHL   ,4     );                // Shift left   by cl/imm                 (unsigned shift left )
DEFNAME(OpcodeShiftRot   ,  SHR   ,5     );                // Shift right  by cl/imm                 (unsigned shift right)
// OpcodeShiftRot          SH??   (6     );                // DO WHAT?
DEFNAME(OpcodeShiftRot   ,  SAR   ,7     );                // Shift Arithmetically right by cl/imm   (signed shift   right)

DEFNAME(OpcodeDoubleShift,  SHLD  ,0x0FA5,0x0FA4);         // Shift left  by cl/imm, filling opened bitpositions, by most significant bits of reg
DEFNAME(OpcodeDoubleShift,  SHRD  ,0x0FAD,0x0FAC);         // Shift right by cl/imm, filling opened bitpositions, by least significant bits of reg


#define BITSCAN_FLAGS (NONBYTE_GPR_ALLOWED | NONBYTE_GPRPTR_ALLOWED | HAS_NONBYTE_SIZEBITS | OP1_REGONLY)

DEFNAME(Opcode2Arg       ,  BSF   ,0x0FBC, BITSCAN_FLAGS);                            // Bitscan forward
DEFNAME(Opcode2Arg       ,  BSR   ,0x0FBD, BITSCAN_FLAGS);                            // Bitscan reverse

DEFNAME(OpcodeBitTest    ,  BT    ,0x0FA3, 4);                                        // Bit Test
DEFNAME(OpcodeBitTest    ,  BTS   ,0x0FAB, 5);                                        // Bit Test and Set
DEFNAME(OpcodeBitTest    ,  BTR   ,0x0FB3, 6);                                        // Bit Test and Reset
DEFNAME(OpcodeBitTest    ,  BTC   ,0x0FBB, 7);                                        // Bit Test and Complement

DEFNAME(OpcodeJmp        ,  JMP );                                                    // Jump
DEFNAME(OpcodeCall       ,  CALL);                                                    // Call Procedure

DEFNAME(OpcodeJcc        ,  JO    ,0x70);                                             // Jump      if overflow           (OF==1 )
DEFNAME(OpcodeJcc        ,  JNO   ,0x71);                                             // Jump      if not overflow       (OF==0 )
DEFNAME(OpcodeJcc        ,  JB    ,0x72);                                             // Jump      if below              (CF==1 )          (unsigned)
DEFNAME(OpcodeJcc        ,  JAE   ,0x73);                                             // Jump      if above or equal     (CF==0 )          (unsigned)
DEFNAME(OpcodeJcc        ,  JE    ,0x74);                                             // Jump      if equal              (ZF==1 )
DEFNAME(OpcodeJcc        ,  JNE   ,0x75);                                             // Jump      if not equal          (ZF==0 )
DEFNAME(OpcodeJcc        ,  JBE   ,0x76);                                             // Jump      if below or equal     (CF==1 || ZF==1)  (unsigned)
DEFNAME(OpcodeJcc        ,  JA    ,0x77);                                             // Jump      if above              (CF==0 && ZF==0)  (unsigned)
DEFNAME(OpcodeJcc        ,  JS    ,0x78);                                             // Jump      if sign               (SF==1 )
DEFNAME(OpcodeJcc        ,  JNS   ,0x79);                                             // Jump      if not sign           (SF==0 )
DEFNAME(OpcodeJcc        ,  JP    ,0x7A);                                             // Jump      if parity even        (PF==1 )
DEFNAME(OpcodeJcc        ,  JNP   ,0x7B);                                             // Jump      if parity odd         (PF==0 )
DEFNAME(OpcodeJcc        ,  JL    ,0x7C);                                             // Jump      if less               (SF!=OF)          (signed  )
DEFNAME(OpcodeJcc        ,  JGE   ,0x7D);                                             // Jump      if greater or equal   (SF==OF)          (signed  )
DEFNAME(OpcodeJcc        ,  JLE   ,0x7E);                                             // Jump      if less or equal      (ZF==1 || SF!=OF) (signed  )
DEFNAME(OpcodeJcc        ,  JG    ,0x7F);                                             // Jump      if greater            (ZF==0 && SF==OF) (signed  )

#if defined(IS32BIT)
DEFNAME(Opcode1Arg       , JCXZ   ,0x67E3,0,IMM8_ALLOWED);                            // Jump if CX  register is 0. 1 byte PC relative offset
DEFNAME(Opcode1Arg       , JECXZ  ,0xE3  ,0,IMM8_ALLOWED);                            // Jump if ECX register is 0. 1 byte PC relative offset
#else
DEFNAME(Opcode1Arg       , JECXZ  ,0x67E3,0,IMM8_ALLOWED);                            // Jump if ECX register is 0. 1 byte PC relative offset
DEFNAME(Opcode1Arg       , JRCXZ  ,0xE3  ,0,IMM8_ALLOWED);                            // Jump if RCX register is 0. 1 byte PC relative offset
#endif // IS64BIT

DEFNAME(Opcode1Arg       , LOOP   ,0xE2  ,0,IMM8_ALLOWED);                            // Decrement count jump if count != 0
DEFNAME(Opcode1Arg       , LOOPE  ,0xE1  ,0,IMM8_ALLOWED);                            // Decrement count jump if count != 0 and ZF = 1
DEFNAME(Opcode1Arg       , LOOPNE ,0xE0  ,0,IMM8_ALLOWED);                            // Decrement count jump if count != 0 and ZF = 0

#define SETCC_FLAGS   (REGTYPE_GPR_ALLOWED | BYTEGPR_ALLOWED | BYTEPTR_ALLOWED)
// Set Byte on Condition
DEFNAME(Opcode1Arg       ,  SETO  ,0x0F90 ,0,SETCC_FLAGS);                            // Set byte  if overflow           (OF==1 )
DEFNAME(Opcode1Arg       ,  SETNO ,0x0F91 ,0,SETCC_FLAGS);                            // Set byte  if not overflow       (OF==0 )
DEFNAME(Opcode1Arg       ,  SETB  ,0x0F92 ,0,SETCC_FLAGS);                            // Set byte  if below              (CF==1 )          (unsigned)
DEFNAME(Opcode1Arg       ,  SETAE ,0x0F93 ,0,SETCC_FLAGS);                            // Set byte  if above or equal     (CF==0 )          (unsigned)
DEFNAME(Opcode1Arg       ,  SETE  ,0x0F94 ,0,SETCC_FLAGS);                            // Set byte  if equal              (ZF==1 )
DEFNAME(Opcode1Arg       ,  SETNE ,0x0F95 ,0,SETCC_FLAGS);                            // Set byte  if not equal          (ZF==0 )
DEFNAME(Opcode1Arg       ,  SETBE ,0x0F96 ,0,SETCC_FLAGS);                            // Set byte  if below or equal     (CF==1 || ZF==1)  (unsigned)
DEFNAME(Opcode1Arg       ,  SETA  ,0x0F97 ,0,SETCC_FLAGS);                            // Set byte  if above              (CF==0 && ZF==0)  (unsigned)
DEFNAME(Opcode1Arg       ,  SETS  ,0x0F98 ,0,SETCC_FLAGS);                            // Set byte  if sign               (SF==1 )
DEFNAME(Opcode1Arg       ,  SETNS ,0x0F99 ,0,SETCC_FLAGS);                            // Set byte  if not sign           (SF==0 )
DEFNAME(Opcode1Arg       ,  SETP  ,0x0F9A ,0,SETCC_FLAGS);                            // Set byte  if parity even        (PF==1 )
DEFNAME(Opcode1Arg       ,  SETNP ,0x0F9B ,0,SETCC_FLAGS);                            // Set byte  if parity odd         (PF==0 )
DEFNAME(Opcode1Arg       ,  SETL  ,0x0F9C ,0,SETCC_FLAGS);                            // Set byte  if less               (SF!=OF)          (signed  )
DEFNAME(Opcode1Arg       ,  SETGE ,0x0F9D ,0,SETCC_FLAGS);                            // Set byte  if greater or equal   (SF==OF)          (signed  )
DEFNAME(Opcode1Arg       ,  SETLE ,0x0F9E ,0,SETCC_FLAGS);                            // Set byte  if less or equal      (ZF==1 || SF!=OF) (signed  )
DEFNAME(Opcode1Arg       ,  SETG  ,0x0F9F ,0,SETCC_FLAGS);                            // Set byte  if greater            (ZF==0 && SF==OF) (signed  )


#define CMOV_FLAGS   (NONBYTE_GPR_ALLOWED|NONBYTE_GPRPTR_ALLOWED|OP1_REGONLY|HAS_NONBYTE_SIZEBITS)

DEFNAME(Opcode2Arg       ,  CMOVO ,0x0F40 ,0,CMOV_FLAGS );                            // Move      if overflow           (OF==1 )
DEFNAME(Opcode2Arg       ,  CMOVNO,0x0F41 ,0,CMOV_FLAGS );                            // Move      if not overflow       (OF==0 )
DEFNAME(Opcode2Arg       ,  CMOVB ,0x0F42 ,0,CMOV_FLAGS );                            // Move      if below              (CF==1 )          (unsigned)
DEFNAME(Opcode2Arg       ,  CMOVAE,0x0F43 ,0,CMOV_FLAGS );                            // Move      if above or equal     (CF==0 )          (unsigned)
DEFNAME(Opcode2Arg       ,  CMOVE ,0x0F44 ,0,CMOV_FLAGS );                            // Move      if equal              (ZF==1 )
DEFNAME(Opcode2Arg       ,  CMOVNE,0x0F45 ,0,CMOV_FLAGS );                            // Move      if not equal          (ZF==0 )
DEFNAME(Opcode2Arg       ,  CMOVBE,0x0F46 ,0,CMOV_FLAGS );                            // Move      if below or equal     (CF==1 || ZF==1)  (unsigned)
DEFNAME(Opcode2Arg       ,  CMOVA ,0x0F47 ,0,CMOV_FLAGS );                            // Move      if above              (CF==0 && ZF==0)  (unsigned)
DEFNAME(Opcode2Arg       ,  CMOVS ,0x0F48 ,0,CMOV_FLAGS );                            // Move      if sign               (SF==1 )
DEFNAME(Opcode2Arg       ,  CMOVNS,0x0F49 ,0,CMOV_FLAGS );                            // Move      if not sign           (SF==0 )
DEFNAME(Opcode2Arg       ,  CMOVP ,0x0F4A ,0,CMOV_FLAGS );                            // Move      if parity even        (PF==1 )
DEFNAME(Opcode2Arg       ,  CMOVNP,0x0F4B ,0,CMOV_FLAGS );                            // Move      if parity odd         (PF==0 )
DEFNAME(Opcode2Arg       ,  CMOVL ,0x0F4C ,0,CMOV_FLAGS );                            // Move      if less               (SF!=OF)          (signed  )
DEFNAME(Opcode2Arg       ,  CMOVGE,0x0F4D ,0,CMOV_FLAGS );                            // Move      if greater or equal   (SF==OF)          (signed  )
DEFNAME(Opcode2Arg       ,  CMOVLE,0x0F4E ,0,CMOV_FLAGS );                            // Move      if less or equal      (ZF==1 || SF!=OF) (signed  )
DEFNAME(Opcode2Arg       ,  CMOVG ,0x0F4F ,0,CMOV_FLAGS );                            // Move      if greater            (ZF==0 && SF==OF) (signed  )



DEFNAME(Opcode0Arg       ,  CWDE  ,0x98,ARG0_SIZEABLE);    // Convert word  to dword  Copy sign (bit 15) of AX  into higher 16 bits of EAX
DEFNAME(Opcode0Arg       ,  CDQ   ,0x99,ARG0_SIZEABLE);    // Convert dword to qword  Copy sign (bit 31) of EAX into every bit of EDX
DEFNAME(Opcode0Arg       ,  CBW   ,CWDE,REGSIZE_WORD );    // Convert byte  to word   Copy sign (bit 7)  of AL  into higher  8 bits of AX (AH)
DEFNAME(Opcode0Arg       ,  CWD   ,CDQ ,REGSIZE_WORD );    // Convert word  to dword  Copy sign (bit 15) of AX  into every bit of DX

#if defined(IS64BIT)
DEFNAME(Opcode0Arg       ,  CDQE  ,CWDE,REGSIZE_QWORD );   // Sign extend EAX into RAX
DEFNAME(Opcode0Arg       ,  CQO   ,CDQ ,REGSIZE_QWORD );   // Sign extend RAX into RDX:RAX
#endif // IS64BIT

DEFNAME(StringInstruction,  MOVSB ,0xA4);
DEFNAME(StringInstruction,  CMPSB ,0xA6);
DEFNAME(StringInstruction,  STOSB ,0xAA);
DEFNAME(StringInstruction,  LODSB ,0xAC);
DEFNAME(StringInstruction,  SCASB ,0xAE);

DEFNAME(StringInstruction,  MOVSW ,MOVSB,REGSIZE_WORD);
DEFNAME(StringInstruction,  CMPSW ,CMPSB,REGSIZE_WORD);
DEFNAME(StringInstruction,  STOSW ,STOSB,REGSIZE_WORD);
DEFNAME(StringInstruction,  LODSW ,LODSB,REGSIZE_WORD);
DEFNAME(StringInstruction,  SCASW ,SCASB,REGSIZE_WORD);

DEFNAME(StringInstruction,  MOVSD ,MOVSB,REGSIZE_DWORD);
DEFNAME(StringInstruction,  CMPSD ,CMPSB,REGSIZE_DWORD);
DEFNAME(StringInstruction,  STOSD ,STOSB,REGSIZE_DWORD);
DEFNAME(StringInstruction,  LODSD ,LODSB,REGSIZE_DWORD);
DEFNAME(StringInstruction,  SCASD ,SCASB,REGSIZE_DWORD);

#if defined(IS64BIT)
DEFNAME(StringInstruction,  MOVSQ ,MOVSB,REGSIZE_QWORD);
DEFNAME(StringInstruction,  CMPSQ ,CMPSB,REGSIZE_QWORD);
DEFNAME(StringInstruction,  STOSQ ,STOSB,REGSIZE_QWORD);
DEFNAME(StringInstruction,  LODSQ ,LODSB,REGSIZE_QWORD);
DEFNAME(StringInstruction,  SCASQ ,SCASB,REGSIZE_QWORD);
#endif // IS64BIT

DEFNAME(StringPrefix     ,  REP   ,0xF3);                         // Apply to INS, OUTS, MOVS, LODS, and STOS
DEFNAME(StringPrefix     ,  REPE  ,0xF3);                         // Apply to CMPS and SCAS instructions
DEFNAME(StringPrefix     ,  REPNE ,0xF2);                         // Apply to CMPS and SCAS instructions

// ----------------------------- FPU opcodes -------------------------------------

DEFNAME(OpcodeFPUTransfer,FLD     ,0xD9C0,0xD9,0,0xDD,0,0xDB,5, 1);                   // FLD( src).  Push  src   into st(0). src={st(i),Real4/8/10 in memory}
DEFNAME(OpcodeFPUTransfer,FST     ,0xDDD0,0xD9,2,0xDD,2,0   ,0, 0);                   // FST( dst).  Store st(0) into dst.   dst={st(i),Real4/8    in memory}
DEFNAME(OpcodeFPUTransfer,FSTP    ,0xDDD8,0xD9,3,0xDD,3,0xDB,7,-1);                   // FSTP(dst).  Pop   st(0) into dst.   dst={st(i),Real4/8/10 in memory}

DEFNAME(OpcodeFPU1Arg    ,FBLD    ,0xDF,4, 1,TBYTEPTR_ALLOWED);                       // FBLD( src). Push  BCD data from src into st(0). src=tbyte ptr
DEFNAME(OpcodeFPU1Arg    ,FBSTP   ,0xDF,6,-1,TBYTEPTR_ALLOWED);                       // FBSTP(dst). Pop   st(0) as BCD data into dst.   dst=tbyte ptr

static const char s_FPUArithmStackDelta[3] = {-1,0,0 };

DEFNAME(OpcodeFPUArithm  ,FADD    ,0xDEC1,0xDCC0,0xD8C0,0xD8,0,0xDC,0,s_FPUArithmStackDelta);
DEFNAME(OpcodeFPUArithm  ,FMUL    ,0xDEC9,0xDCC8,0xD8C8,0xD8,1,0xDC,1,s_FPUArithmStackDelta);
DEFNAME(OpcodeFPUArithm  ,FSUB    ,0xDEE9,0xDCE8,0xD8E0,0xD8,4,0xDC,4,s_FPUArithmStackDelta);
DEFNAME(OpcodeFPUArithm  ,FSUBR   ,0xDEE1,0xDCE0,0xD8E8,0xD8,5,0xDC,5,s_FPUArithmStackDelta);
DEFNAME(OpcodeFPUArithm  ,FDIV    ,0xDEF9,0xDCF8,0xD8F0,0xD8,6,0xDC,6,s_FPUArithmStackDelta);
DEFNAME(OpcodeFPUArithm  ,FDIVR   ,0xDEF1,0xDCF0,0xD8F8,0xD8,7,0xDC,7,s_FPUArithmStackDelta);

DEFNAME(OpcodeFPU1Arg    ,FADDP   ,0xDEC0,0,-1,REGTYPE_FPU_ALLOWED);                  // FADDP(st(i)).  st(i) += st(0);       pop st(0);
DEFNAME(OpcodeFPU1Arg    ,FMULP   ,0xDEC8,0,-1,REGTYPE_FPU_ALLOWED);                  // FMULP(st(i)).  st(i) *= st(0);       pop st(0);
DEFNAME(OpcodeFPU1Arg    ,FSUBP   ,0xDEE8,0,-1,REGTYPE_FPU_ALLOWED);                  // FSUBP(st(i)).  st(i) -= st(0);       pop st(0);
DEFNAME(OpcodeFPU1Arg    ,FDIVP   ,0xDEF8,0,-1,REGTYPE_FPU_ALLOWED);                  // FDIVP(st(i)).  st(i) /= st(0);       pop st(0);
DEFNAME(OpcodeFPU1Arg    ,FSUBRP  ,0xDEE0,0,-1,REGTYPE_FPU_ALLOWED);                  // FSUBRP(st(i)). st(i)  = st(0)-st(i); pop st(0);
DEFNAME(OpcodeFPU1Arg    ,FDIVRP  ,0xDEF0,0,-1,REGTYPE_FPU_ALLOWED);                  // FDIVRP(st(i)). st(i)  = st(0)/st(i); pop st(0);

// ----------------------------- FPU compare opcodes ----------------------------
DEFNAME(OpcodeFPUCompare ,FCOM    ,0xD8D0,0xD8,2,0xDC,2, 0);                          // FCOM(  src  ). Compare st(0) to src, src={st(i),Real4/8 in memory}
DEFNAME(OpcodeFPUCompare ,FCOMP   ,0xD8D8,0xD8,3,0xDC,3,-1);                          // Same as FCOM,  but pop st(0) after compare
DEFNAME(OpcodeFPU1Arg    ,FCOMI   ,0xDBF0,0, 0,REGTYPE_FPU_ALLOWED);                  // FCOMI( st(i)). Compare st(0) to st(i) and set CPU-flags
DEFNAME(OpcodeFPU1Arg    ,FCOMIP  ,0xDFF0,0,-1,REGTYPE_FPU_ALLOWED);                  // Same as FCOMI, but pop st(0) after compare
DEFNAME(OpcodeFPU1Arg    ,FUCOM   ,0xDDE0,0, 0,REGTYPE_FPU_ALLOWED);                  // FUCOM( st(i)). Unordered compare st(0) to st(i)
DEFNAME(OpcodeFPU1Arg    ,FUCOMP  ,0xDDE8,0,-1,REGTYPE_FPU_ALLOWED);                  // Same as FUCOM, but pop st(0) after compare
DEFNAME(OpcodeFPU1Arg    ,FUCOMI  ,0xDBE8,0, 0,REGTYPE_FPU_ALLOWED);                  // FUCOMI(st(i)). Unordered compare st(0) to st(i) and set CPU-flags
DEFNAME(OpcodeFPU1Arg    ,FUCOMIP ,0xDFE8,0,-1,REGTYPE_FPU_ALLOWED);                  // Same as FUCOMI, but pop st(0) after compare

DEFNAME(OpcodeFPU0Arg    ,FCOMPP  ,0xDED9,-2);                                        // Compare st(0) to st(1); pop both
DEFNAME(OpcodeFPU0Arg    ,FUCOMPP ,0xDAE9,-2);                                        // Unordered compare st(0) to st(1); pop both

// ------------------------ FPU integer opcodes ---------------------------------
DEFNAME(OpcodeFPUIArithm ,FILD    ,0xDF,0,0xDB,0,0xDF,5, 1);                          // FILD(  src). Push src into st(0). src=(signed int16/32/64 in memory)
DEFNAME(OpcodeFPUIArithm ,FISTP   ,0xDF,3,0xDB,3,0xDF,7,-1);                          // FISTP( dst). Pop st(0) into dst, rounding according to RC-field FPU control word. dst=(signed int16/32/64 in memory)
DEFNAME(OpcodeFPUIArithm ,FISTTP  ,0xDF,1,0xDB,1,0xDD,1,-1);                          // FISTTP(dst). Same as fistp, but truncates to nearest integer, regardless of RC-field in FPU control word
DEFNAME(OpcodeFPUIArithm ,FIST    ,0xDF,2,0xDB,2,0,   0, 0);                          // FIST(  dst). Store st(0) into dst, rounding according to RC-field FPU control word. dst=(signed int16/32 in memory)
DEFNAME(OpcodeFPUIArithm ,FIADD   ,0xDE,0,0xDA,0,0,   0, 0);                          // FIADD( src). st(0) += src.      src=(signed int16/32 in memory)
DEFNAME(OpcodeFPUIArithm ,FIMUL   ,0xDE,1,0xDA,1,0,   0, 0);                          // FIMUL( src). st(0) *= src.      src=(signed int16/32 in memory)
DEFNAME(OpcodeFPUIArithm ,FISUB   ,0xDE,4,0xDA,4,0,   0, 0);                          // FISUB( src). st(0) -= src.      src=(signed int16/32 in memory)
DEFNAME(OpcodeFPUIArithm ,FIDIV   ,0xDE,6,0xDA,6,0,   0, 0);                          // FIDIV( src). st(0) /= src.      src=(signed int16/32 in memory)
DEFNAME(OpcodeFPUIArithm ,FISUBR  ,0xDE,5,0xDA,5,0,   0, 0);                          // FISUBR(src). st(0) = src-st(0). src=(signed int16/32 in memory)
DEFNAME(OpcodeFPUIArithm ,FIDIVR  ,0xDE,7,0xDA,7,0,   0, 0);                          // FIDIVR(src). st(0) = src/st(0). src=(signed int16/32 in memory)
DEFNAME(OpcodeFPUIArithm ,FICOM   ,0xDE,2,0xDA,2,0,   0, 0);                          // FICOM( src). Compare st(0) to signed int16/32 in memory
DEFNAME(OpcodeFPUIArithm ,FICOMP  ,0xDE,3,0xDA,3,0,   0,-1);                          // Same as ficom, but pop st(0) after compare


// ------------------- Conditional Copy st(i) to st(0) --------------------------
DEFNAME(OpcodeFPU1Arg    ,FCMOVB  ,0xDAC0,0,0,REGTYPE_FPU_ALLOWED);                   // Copy      if below              (CF==1 )
DEFNAME(OpcodeFPU1Arg    ,FCMOVE  ,0xDAC8,0,0,REGTYPE_FPU_ALLOWED);                   // Copy      if equal              (ZF==1 )
DEFNAME(OpcodeFPU1Arg    ,FCMOVBE ,0xDAD0,0,0,REGTYPE_FPU_ALLOWED);                   // Copy      if below or equal     (CF==1 || ZF==1)
DEFNAME(OpcodeFPU1Arg    ,FCMOVU  ,0xDAD8,0,0,REGTYPE_FPU_ALLOWED);                   // Copy      if unordered          (PF==1 )
DEFNAME(OpcodeFPU1Arg    ,FCMOVAE ,0xDBC0,0,0,REGTYPE_FPU_ALLOWED);                   // Copy      if above or equal     (CF==0 )
DEFNAME(OpcodeFPU1Arg    ,FCMOVNE ,0xDBC8,0,0,REGTYPE_FPU_ALLOWED);                   // Copy      if not equal          (ZF==0 )
DEFNAME(OpcodeFPU1Arg    ,FCMOVA  ,0xDBD0,0,0,REGTYPE_FPU_ALLOWED);                   // Copy      if above              (CF==0 && ZF==0)
DEFNAME(OpcodeFPU1Arg    ,FCMOVNU ,0xDBD8,0,0,REGTYPE_FPU_ALLOWED);                   // Copy      if not unordered      (PF==0 )

DEFNAME(OpcodeFPU1Arg    ,FFREE   ,0xDDC0,0,0,REGTYPE_FPU_ALLOWED);                   // Free a data register
DEFNAME(OpcodeFPU1Arg    ,FXCH    ,0xD9C8,0,0,REGTYPE_FPU_ALLOWED);                   // Swap st(0) and st(i)

DEFNAME(OpcodeFPU0Arg    ,FWAIT   ,0x9B  , 0);                                        // Wait while FPU is busy
DEFNAME(OpcodeFPU0Arg    ,FNOP    ,0xD9D0, 0);                                        // No operation
DEFNAME(OpcodeFPU0Arg    ,FCHS    ,0xD9E0, 0);                                        // st(0); = -st(0)
DEFNAME(OpcodeFPU0Arg    ,FABS    ,0xD9E1, 0);                                        // st(0) = abs(st(0))
DEFNAME(OpcodeFPU0Arg    ,FTST    ,0xD9E4, 0);                                        // Compare st(0) to 0.0
DEFNAME(OpcodeFPU0Arg    ,FXAM    ,0xD9E5, 0);                                        // Examine the content of st(0)
DEFNAME(OpcodeFPU0Arg    ,FLD1    ,0xD9E8, 1);                                        // push 1.0
DEFNAME(OpcodeFPU0Arg    ,FLDL2T  ,0xD9E9, 1);                                        // push log2(10)
DEFNAME(OpcodeFPU0Arg    ,FLDL2E  ,0xD9EA, 1);                                        // push log2(e)
DEFNAME(OpcodeFPU0Arg    ,FLDPI   ,0xD9EB, 1);                                        // push pi
DEFNAME(OpcodeFPU0Arg    ,FLDLG2  ,0xD9EC, 1);                                        // push log10(2)
DEFNAME(OpcodeFPU0Arg    ,FLDLN2  ,0xD9ED, 1);                                        // push ln(2)
DEFNAME(OpcodeFPU0Arg    ,FLDZ    ,0xD9EE, 1);                                        // push 0.0
DEFNAME(OpcodeFPU0Arg    ,F2XM1   ,0xD9F0, 0);                                        // st(0) = 2^st(0)-1, assume -1 <= st(0) <= 1
DEFNAME(OpcodeFPU0Arg    ,FYL2X   ,0xD9F1,-1);                                        // st(1) = log2(st(0))*st(1); pop st(0)
DEFNAME(OpcodeFPU0Arg    ,FPTAN   ,0xD9F2, 1);                                        // st(0) = tan(st(0)); push 1.0
DEFNAME(OpcodeFPU0Arg    ,FPATAN  ,0xD9F3,-1);                                        // st(1) = atan(st(1)/st(0)); pop st(0)
DEFNAME(OpcodeFPU0Arg    ,FXTRACT ,0xD9F4, 1);                                        // st(0) = unbiased exponent in floating point format of st(0). then push signinificant wiht exponent 0
DEFNAME(OpcodeFPU0Arg    ,FPREM1  ,0xD9F5, 0);                                        // As FPREM. Magnitude of the remainder <= ST(1) / 2
DEFNAME(OpcodeFPU0Arg    ,FDECSTP ,0xD9F6, 0);                                        // Decrement stack pointer. st0->st1, st7->st0, ..., st1->st2
DEFNAME(OpcodeFPU0Arg    ,FINCSTP ,0xD9F7, 0);                                        // Increment stack pointer. st0->st7, st1->st0, ..., st7->st6
DEFNAME(OpcodeFPU0Arg    ,FPREM   ,0xD9F8, 0);                                        // Partial remainder. st(0) %= st(1). Exponent of st(0) reduced with at most 63
DEFNAME(OpcodeFPU0Arg    ,FYL2XP1 ,0xD9F9,-1);                                        // st(1) = log2(st(0)+1)*st(1); pop st(0)
DEFNAME(OpcodeFPU0Arg    ,FSQRT   ,0xD9FA, 0);                                        // st(0) = sqrt(st(0))
DEFNAME(OpcodeFPU0Arg    ,FSINCOS ,0xD9FB, 1);                                        // Sine and cosine of the angle value in ST(0), st(0)=sin; push(cos)
DEFNAME(OpcodeFPU0Arg    ,FRNDINT ,0xD9FC, 0);                                        // st(0) = nearest integral value according to the rounding mode
DEFNAME(OpcodeFPU0Arg    ,FSCALE  ,0xD9FD, 0);                                        // st(0) *= 2^int(st(1))
DEFNAME(OpcodeFPU0Arg    ,FSIN    ,0xD9FE, 0);                                        // st(0) = sin(ST(0))
DEFNAME(OpcodeFPU0Arg    ,FCOS    ,0xD9FF, 0);                                        // st(0) = cos(ST(0))

DEFNAME(Opcode1Arg       ,FLDCW   ,0xD9  ,5,WORDPTR_ALLOWED);                         // FLDCW( m16   ). Load  control word.
DEFNAME(Opcode1Arg       ,FNSTCW  ,0xD9  ,7,WORDPTR_ALLOWED);                         // FNSTCW(m16   ). Store control word.
DEFNAME(Opcode1ArgFNSTSW ,FNSTSW  ,0xDD  );                                           // FNSTSW(m16/AX). Store status  word.

// --------------------------- XMM opcodes --------------------------------
DEFNAME(Opcode2Arg        ,MOVAPS ,0x0F28 , REGTYPE_XMM_ALLOWED | HAS_DIRBIT0 | XMMWORDPTR_ALLOWED);
Opcode2ArgPfxF2SD          MOVSD1(_T("movsd") ,0x0F10 , HAS_DIRBIT0 );
DEFNAME(Opcode2ArgPfxF2SD ,ADDSD  ,0x0F58 , OP1_REGONLY   );
DEFNAME(Opcode2ArgPfxF2SD ,MULSD  ,0x0F59 , OP1_REGONLY   );
DEFNAME(Opcode2ArgPfxF2SD ,SUBSD  ,0x0F5C , OP1_REGONLY   );
DEFNAME(Opcode2ArgPfxF2SD ,DIVSD  ,0x0F5E , OP1_REGONLY   );

}; // namespace
