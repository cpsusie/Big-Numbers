#include "pch.h"
#include <NewOpCode.h>

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

#ifdef IS64BIT
DEFNAME(Opcode0Arg       ,  CLGI  ,0x0F01DD);              // Clear Global Interrupt Flag
DEFNAME(Opcode0Arg       ,  STGI  ,0x0F01DC);              // Set Global Interrupt Flag
#endif // IS64BIT

#define ARG0_SIZEABLE (WORDPTR_ALLOWED|QWORDPTR_ALLOWED|HAS_NONBYTE_SIZEBITS)

#ifdef IS32BIT
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
#ifdef IS32BIT
DEFNAME(Opcode0Arg       ,  PUSHAD,0x60);                  // Push all double-word (32-bit) registers onto stack
DEFNAME(Opcode0Arg       ,  POPAD ,0x61);                  // Pop  all double-word (32-bit) registers from stack
#endif // IS32BIT

DEFNAME(Opcode0Arg       ,  NOOP  ,0x90);
DEFNAME(OpcodeStd2Arg    ,  ADD   ,0x00,0x04,0x80,0,HAS_IMM_XBIT|HAS_DIRECTIONBIT1);
DEFNAME(OpcodeStd2Arg    ,  OR    ,0x08,0x0C,0x80,1,HAS_IMM_XBIT|HAS_DIRECTIONBIT1);
DEFNAME(OpcodeStd2Arg    ,  ADC   ,0x10,0x14,0x80,2,HAS_IMM_XBIT|HAS_DIRECTIONBIT1);
DEFNAME(OpcodeStd2Arg    ,  SBB   ,0x18,0x1C,0x80,3,HAS_IMM_XBIT|HAS_DIRECTIONBIT1);
DEFNAME(OpcodeStd2Arg    ,  AND   ,0x20,0x24,0x80,4,HAS_IMM_XBIT|HAS_DIRECTIONBIT1);
DEFNAME(OpcodeStd2Arg    ,  SUB   ,0x28,0x2C,0x80,5,HAS_IMM_XBIT|HAS_DIRECTIONBIT1);
DEFNAME(OpcodeStd2Arg    ,  XOR   ,0x30,0x34,0x80,6,HAS_IMM_XBIT|HAS_DIRECTIONBIT1);
DEFNAME(OpcodeStd2Arg    ,  CMP   ,0x38,0x3C,0x80,7,HAS_IMM_XBIT|HAS_DIRECTIONBIT1);
/*
DEFNAME(Opcode2Arg       ,  ADD   ,0x00);
DEFNAME(Opcode2Arg       ,  OR    ,0x08);
DEFNAME(Opcode2Arg       ,  ADC   ,0x10);
DEFNAME(Opcode2Arg       ,  SBB   ,0x18);
DEFNAME(Opcode2Arg       ,  AND   ,0x20);
DEFNAME(Opcode2Arg       ,  SUB   ,0x28);
DEFNAME(Opcode2Arg       ,  XOR   ,0x30);
DEFNAME(Opcode2Arg       ,  CMP   ,0x38);
*/
DEFNAME(OpcodeXchg       ,  XCHG       );                  // Exchange Two operands
DEFNAME(OpcodeStd2Arg    ,  TEST  ,0x84,0xA8,0xF6,0);      // Logical Compare. same as AND but doesn't change dst. set SF,ZF,PF according to result

DEFNAME(OpcodeMov        ,  MOV        );
DEFNAME(OpcodeLea        ,  LEA   ,0x8D);                  // Load effective address

DEFNAME(OpcodePushPop    ,  PUSH  ,0x50, 0xFF,6, 0x68);
DEFNAME(OpcodePushPop    ,  POP   ,0x58, 0x8F,0, 0   );

DEFNAME(OpcodeIncDec     ,  INC   ,0xFE,0);
DEFNAME(OpcodeIncDec     ,  DEC   ,0xFE,1);

//        Non existing opcode (0xF6,1);
DEFNAME(Opcode1Arg       ,  NOT   ,0xF6,2);                // Negate the operand, logical NOT
DEFNAME(Opcode1Arg       ,  NEG   ,0xF6,3);                // Two's complement negation
DEFNAME(OpcodeIMul       ,  IMUL         );                // Signed multiply   (ax = al*src, dx:ax=ax*src, edx:eax=eax*src, rdx:rax=rax*src)
DEFNAME(Opcode1Arg       ,  IDIV  ,0xF6,7);                // Signed divide   ax      /= src, ah  must contain sign extension of al . al =quot, ah =rem
                                                           //                 dk:ax   /= src. dx  must contain sign extension of ax . ax =quot, dx =rem
                                                           //                 edx:eax /= src. edx must contain sign extension of eax. eax=quot, edx=rem
                                                           //                 rdx:rax /= src. rdx must contain sign extension of rax. rax=quot, rdx=rem

DEFNAME(Opcode2ArgM      ,  MUL   ,0xF6,4);                // Unsigned multiply (ax = al*src, dx:ax=ax*src, edx:eax=eax*src, rdx:rax=rax*src)
DEFNAME(Opcode2ArgM      ,  DIV   ,0xF6,6);                // Unsigned divide   (ax/=src,al=quot,ah=rem,    edx:eax/=src,eax=quot,edx=rem,  rdx:rax/=src,rax=quit,rdx=rem

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

DEFNAME(OpcodeBitScan    ,  BSF   ,0x0FBC);                // Bitscan forward
DEFNAME(OpcodeBitScan    ,  BSR   ,0x0FBD);                // Bitscan reversed

// Set Byte on Condition
DEFNAME(OpcodeSetcc      ,  SETO  ,0x0F90  );              // Set byte if overflow
DEFNAME(OpcodeSetcc      ,  SETNO ,0x0F91  );              // Set byte if not overflow
DEFNAME(OpcodeSetcc      ,  SETB  ,0x0F92  );              // Set byte if below                 (unsigned)
DEFNAME(OpcodeSetcc      ,  SETAE ,0x0F93  );              // Set byte if above or equal        (unsigned)
DEFNAME(OpcodeSetcc      ,  SETE  ,0x0F94  );              // Set byte if equal                 (signed/unsigned)
DEFNAME(OpcodeSetcc      ,  SETNE ,0x0F95  );              // Set byte if not equal             (signed/unsigned)
DEFNAME(OpcodeSetcc      ,  SETBE ,0x0F96  );              // Set byte if below or equal        (unsigned)
DEFNAME(OpcodeSetcc      ,  SETA  ,0x0F97  );              // Set byte if above                 (unsigned)
DEFNAME(OpcodeSetcc      ,  SETS  ,0x0F98  );              // Set byte if sign
DEFNAME(OpcodeSetcc      ,  SETNS ,0x0F99  );              // Set byte if not sign
DEFNAME(OpcodeSetcc      ,  SETP  ,0x0F9A  );              // Set byte if parity even
DEFNAME(OpcodeSetcc      ,  SETNP ,0x0F9B  );              // Set byte if parity odd
DEFNAME(OpcodeSetcc      ,  SETL  ,0x0F9C  );              // Set byte if less                  (signed  )
DEFNAME(OpcodeSetcc      ,  SETGE ,0x0F9D  );              // Set byte if greater or equal      (signed  )
DEFNAME(OpcodeSetcc      ,  SETLE ,0x0F9E  );              // Set byte if less or equal         (signed  )
DEFNAME(OpcodeSetcc      ,  SETG  ,0x0F9F  );              // Set byte if greater               (signed  );

DEFNAME(Opcode0Arg       ,  CWDE  ,0x98,ARG0_SIZEABLE);    // Convert word  to dword  Copy sign (bit 15) of AX  into higher 16 bits of EAX
DEFNAME(Opcode0Arg       ,  CDQ   ,0x99,ARG0_SIZEABLE);    // Convert dword to qword  Copy sign (bit 31) of EAX into every bit of EDX
DEFNAME(Opcode0Arg       ,  CBW   ,CWDE,REGSIZE_WORD );    // Convert byte  to word   Copy sign (bit 7)  of AL  into higher  8 bits of AX (AH)
DEFNAME(Opcode0Arg       ,  CWD   ,CDQ ,REGSIZE_WORD );    // Convert word  to dword  Copy sign (bit 15) of AX  into every bit of DX

#ifdef IS64BIT
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

#ifdef IS64BIT
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

DEFNAME(OpcodeFPUTransfer,FLD     ,0xD9C0,0xD9,0,0xDD,0,0xDB,5);
DEFNAME(OpcodeFPUTransfer,FST     ,0xDDD0,0xD9,2,0xDD,2,0   ,0);
DEFNAME(OpcodeFPUTransfer,FSTP    ,0xDDD8,0xD9,3,0xDD,3,0xDB,7);

DEFNAME(Opcode1Arg       ,FBLD    ,0xDF,4,TBYTEPTR_ALLOWED);      // LoaD BCD data from memory
DEFNAME(Opcode1Arg       ,FBSTP   ,0xDF,6,TBYTEPTR_ALLOWED);      // STore BCD data to memory

DEFNAME(OpcodeFPUArithm  ,FADD    ,0xDEC1,0xDCC0,0xD8C0,0xD8,0,0xDC,0);
DEFNAME(OpcodeFPUArithm  ,FMUL    ,0xDEC9,0xDCC8,0xD8C8,0xD8,1,0xDC,1);
DEFNAME(OpcodeFPUArithm  ,FSUB    ,0xDEE9,0xDCE8,0xD8E0,0xD8,4,0xDC,4);
DEFNAME(OpcodeFPUArithm  ,FSUBR   ,0xDEE1,0xDCE0,0xD8E8,0xD8,5,0xDC,5);
DEFNAME(OpcodeFPUArithm  ,FDIV    ,0xDEF9,0xDCF8,0xD8F0,0xD8,6,0xDC,6);
DEFNAME(OpcodeFPUArithm  ,FDIVR   ,0xDEF1,0xDCF0,0xD8F8,0xD8,7,0xDC,7);

DEFNAME(Opcode1Arg       ,FADDP   ,0xDEC0,0,REGTYPE_FPU_ALLOWED);
DEFNAME(Opcode1Arg       ,FMULP   ,0xDEC8,0,REGTYPE_FPU_ALLOWED);
DEFNAME(Opcode1Arg       ,FSUBP   ,0xDEE8,0,REGTYPE_FPU_ALLOWED);
DEFNAME(Opcode1Arg       ,FDIVP   ,0xDEF8,0,REGTYPE_FPU_ALLOWED);
DEFNAME(Opcode1Arg       ,FSUBRP  ,0xDEE0,0,REGTYPE_FPU_ALLOWED);
DEFNAME(Opcode1Arg       ,FDIVRP  ,0xDEF0,0,REGTYPE_FPU_ALLOWED);

DEFNAME(OpcodeFPUCompare ,FCOM    ,0xD8D0,0xD8,2,0xDC,2);
DEFNAME(OpcodeFPUCompare ,FCOMP   ,0xD8D8,0xD8,3,0xDC,3);
DEFNAME(Opcode1Arg       ,FCOMI   ,0xDBF0,0,REGTYPE_FPU_ALLOWED); // Compare st(0) to st(i) and set CPU-flags
DEFNAME(Opcode1Arg       ,FCOMIP  ,0xDFF0,0,REGTYPE_FPU_ALLOWED); // Compare st(0) to st(i) and set CPU-flags; pop st(0)
DEFNAME(Opcode1Arg       ,FUCOM   ,0xDDE0,0,REGTYPE_FPU_ALLOWED); // Unordered compare st(0) to st(i)
DEFNAME(Opcode1Arg       ,FUCOMP  ,0xDDE8,0,REGTYPE_FPU_ALLOWED); // Unordered compare st(0) to st(i); pop st(0)
DEFNAME(Opcode1Arg       ,FUCOMI  ,0xDBE8,0,REGTYPE_FPU_ALLOWED); // Unordered compare st(0) to st(i) and set CPU-flags
DEFNAME(Opcode1Arg       ,FUCOMIP ,0xDFE8,0,REGTYPE_FPU_ALLOWED); // Unordered compare st(0) to st(i) and set CPU-flags; pop st(0)

DEFNAME(Opcode0Arg       ,FCOMPP  ,0xDED9);                       // Compare st(0) to st(1); pop both
DEFNAME(Opcode0Arg       ,FUCOMPP ,0xDAE9);                       // Unordered compare st(0) to st(1); pop both

DEFNAME(OpcodeFPUIArithm ,FILD    ,0xDF,0,0xDB,0,0xDF,5);
DEFNAME(OpcodeFPUIArithm ,FISTTP  ,0xDF,1,0xDB,1,0xDD,1);
DEFNAME(OpcodeFPUIArithm ,FIST    ,0xDF,2,0xDB,2,0,   0);
DEFNAME(OpcodeFPUIArithm ,FISTP   ,0xDF,3,0xDB,3,0xDF,7);
DEFNAME(OpcodeFPUIArithm ,FIADD   ,0xDE,0,0xDA,0,0,   0);
DEFNAME(OpcodeFPUIArithm ,FIMUL   ,0xDE,1,0xDA,1,0,   0);
DEFNAME(OpcodeFPUIArithm ,FICOM   ,0xDE,2,0xDA,2,0,   0);
DEFNAME(OpcodeFPUIArithm ,FICOMP  ,0xDE,3,0xDA,3,0,   0);
DEFNAME(OpcodeFPUIArithm ,FISUB   ,0xDE,4,0xDA,4,0,   0);
DEFNAME(OpcodeFPUIArithm ,FISUBR  ,0xDE,5,0xDA,5,0,   0);
DEFNAME(OpcodeFPUIArithm ,FIDIV   ,0xDE,6,0xDA,6,0,   0);
DEFNAME(OpcodeFPUIArithm ,FIDIVR  ,0xDE,7,0xDA,7,0,   0);

DEFNAME(Opcode1Arg       ,FCMOVB  ,0xDAC0,0,REGTYPE_FPU_ALLOWED); // Move if below (CF=1)
DEFNAME(Opcode1Arg       ,FCMOVE  ,0xDAC8,0,REGTYPE_FPU_ALLOWED); // Move if equal (ZF=1)
DEFNAME(Opcode1Arg       ,FCMOVBE ,0xDAD0,0,REGTYPE_FPU_ALLOWED); // Move if below or equal (CF=1 or ZF=1)
DEFNAME(Opcode1Arg       ,FCMOVU  ,0xDAD8,0,REGTYPE_FPU_ALLOWED); // Move if unordered (PF=1)
DEFNAME(Opcode1Arg       ,FCMOVAE ,0xDBC0,0,REGTYPE_FPU_ALLOWED); // Move if above or equal (CF=0)
DEFNAME(Opcode1Arg       ,FCMOVNE ,0xDBC8,0,REGTYPE_FPU_ALLOWED); // Move if not equal (ZF=0)
DEFNAME(Opcode1Arg       ,FCMOVA  ,0xDBD0,0,REGTYPE_FPU_ALLOWED); // Move if above (CF=0 and ZF=0)
DEFNAME(Opcode1Arg       ,FCMOVNU ,0xDBD8,0,REGTYPE_FPU_ALLOWED); // Move if not unordered (PF=0)

DEFNAME(Opcode1Arg       ,FFREE   ,0xDDC0,0,REGTYPE_FPU_ALLOWED); // Free a data register
DEFNAME(Opcode1Arg       ,FXCH    ,0xD9C8,0,REGTYPE_FPU_ALLOWED); // Swap st(0) and st(i)

DEFNAME(Opcode0Arg       ,FNSTSWAX,0xDFE0);                       // Store status word into CPU register AX
DEFNAME(Opcode0Arg       ,FWAIT   ,0x9B  );                       // Wait while FPU is busy
DEFNAME(Opcode0Arg       ,FNOP    ,0xD9D0);                       // No operation
DEFNAME(Opcode0Arg       ,FCHS    ,0xD9E0);                       // st(0); = -st(0)
DEFNAME(Opcode0Arg       ,FABS    ,0xD9E1);                       // st(0) = abs(st(0))
DEFNAME(Opcode0Arg       ,FTST    ,0xD9E4);                       // Compare st(0) to 0.0
DEFNAME(Opcode0Arg       ,FXAM    ,0xD9E5);                       // Examine the content of st(0)
DEFNAME(Opcode0Arg       ,FLD1    ,0xD9E8);                       // push 1.0
DEFNAME(Opcode0Arg       ,FLDL2T  ,0xD9E9);                       // push log2(10)
DEFNAME(Opcode0Arg       ,FLDL2E  ,0xD9EA);                       // push log2(e)
DEFNAME(Opcode0Arg       ,FLDPI   ,0xD9EB);                       // push pi
DEFNAME(Opcode0Arg       ,FLDLG2  ,0xD9EC);                       // push log10(2)
DEFNAME(Opcode0Arg       ,FLDLN2  ,0xD9ED);                       // push ln(2)
DEFNAME(Opcode0Arg       ,FLDZ    ,0xD9EE);                       // push 0.0
DEFNAME(Opcode0Arg       ,F2XM1   ,0xD9F0);                       // st(0) = 2^st(0)-1, assume -1 <= st(0) <= 1
DEFNAME(Opcode0Arg       ,FYL2X   ,0xD9F1);                       // st(1) = log2(st(0))*st(1); pop st(0)
DEFNAME(Opcode0Arg       ,FPTAN   ,0xD9F2);                       // st(0) = tan(st(0)); push 1.0
DEFNAME(Opcode0Arg       ,FPATAN  ,0xD9F3);                       // st(1) = atan(st(1)/st(0)); pop st(0)
DEFNAME(Opcode0Arg       ,FXTRACT ,0xD9F4);                       // st(0) = unbiased exponent in floating point format of st(0). then push signinificant wiht exponent 0
DEFNAME(Opcode0Arg       ,FPREM1  ,0xD9F5);                       // As FPREM. Magnitude of the remainder <= ST(1) / 2
DEFNAME(Opcode0Arg       ,FDECSTP ,0xD9F6);                       // Decrement stack pointer. st0->st1, st7->st0, ..., st1->st2
DEFNAME(Opcode0Arg       ,FINCSTP ,0xD9F7);                       // Increment stack pointer. st0->st7, st1->st0, ..., st7->st6
DEFNAME(Opcode0Arg       ,FPREM   ,0xD9F8);                       // Partial remainder. st(0) %= st(1). Exponent of st(0) reduced with at most 63
DEFNAME(Opcode0Arg       ,FYL2XP1 ,0xD9F9);                       // st(1) = log2(st(0)+1)*st(1); pop st(0)
DEFNAME(Opcode0Arg       ,FSQRT   ,0xD9FA);                       // st(0) = sqrt(st(0))
DEFNAME(Opcode0Arg       ,FSINCOS ,0xD9FB);                       // Sine and cosine of the angle value in ST(0), st(0)=sin; push(cos)
DEFNAME(Opcode0Arg       ,FRNDINT ,0xD9FC);                       // st(0) = nearest integral value according to the rounding mode
DEFNAME(Opcode0Arg       ,FSCALE  ,0xD9FD);                       // st(0) *= 2^int(st(1))
DEFNAME(Opcode0Arg       ,FSIN    ,0xD9FE);                       // st(0) = sin(ST(0))
DEFNAME(Opcode0Arg       ,FCOS    ,0xD9FF);                       // st(0) = cos(ST(0))

DEFNAME(Opcode1Arg       ,FLDCW   ,0xD9  ,5,WORDPTR_ALLOWED);     // load control word
DEFNAME(Opcode1Arg       ,FNSTCW  ,0xD9  ,7,WORDPTR_ALLOWED);     // store control word
DEFNAME(Opcode1Arg       ,FNSTSW  ,0xDD  ,7,WORDPTR_ALLOWED);     // store status word


// --------------------------- XMM opcodes --------------------------------

DEFNAME(Opcode2Arg        ,MOVAPS ,0x0F28 , REGTYPE_XMM_ALLOWED | HAS_DIRECTIONBIT0 | XMMWORDPTR_ALLOWED);
Opcode2ArgPfxF2SD          MOVSD1(_T("movsd") ,0x0F10 , HAS_DIRECTIONBIT0 );
DEFNAME(Opcode2ArgPfxF2SD ,ADDSD  ,0x0F58 , FIRSTOP_REGONLY   );
DEFNAME(Opcode2ArgPfxF2SD ,MULSD  ,0x0F59 , FIRSTOP_REGONLY   );
DEFNAME(Opcode2ArgPfxF2SD ,SUBSD  ,0x0F5C , FIRSTOP_REGONLY   );
DEFNAME(Opcode2ArgPfxF2SD ,DIVSD  ,0x0F5E , FIRSTOP_REGONLY   );