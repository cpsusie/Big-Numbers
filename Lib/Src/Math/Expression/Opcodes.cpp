#include "pch.h"
#include <Math/Expression/NewOpCode.h>

#pragma warning(disable : 4073)
#pragma init_seg(lib)

// Set Byte on Condition
OpcodeSetxx             SETO  (0x0F90);                    // Set byte   if overflow
OpcodeSetxx             SETNO (0x0F91);                    // Set byte   if not overflow
OpcodeSetxx             SETB  (0x0F92);                    // Set byte   if below                 (unsigned)
OpcodeSetxx             SETAE (0x0F93);                    // Set byte   if above or equal        (unsigned)
OpcodeSetxx             SETE  (0x0F94);                    // Set byte   if equal                 (signed/unsigned)
OpcodeSetxx             SETNE (0x0F95);                    // Set byte   if not equal             (signed/unsigned)
OpcodeSetxx             SETBE (0x0F96);                    // Set byte   if below or equal        (unsigned)
OpcodeSetxx             SETA  (0x0F97);                    // Set byte   if above                 (unsigned)
OpcodeSetxx             SETS  (0x0F98);                    // Set byte   if sign
OpcodeSetxx             SETNS (0x0F99);                    // Set byte   if not sign
OpcodeSetxx             SETPE (0x0F9A);                    // Set byte   if parity even
OpcodeSetxx             SETPO (0x0F9B);                    // Set byte   if parity odd
OpcodeSetxx             SETL  (0x0F9C);                    // Set byte   if less                  (signed  )
OpcodeSetxx             SETGE (0x0F9D);                    // Set byte   if greater or equal      (signed  )
OpcodeSetxx             SETLE (0x0F9E);                    // Set byte   if less or equal         (signed  )
OpcodeSetxx             SETG  (0x0F9F);                    // Set byte   if greater               (signed  );

Instruction0Arg         RET   (0xC3);                      // Near return to calling procedure

Instruction0Arg         CMC   (0xF5);                      // Complement carry flag
Instruction0Arg         CLC   (0xF8);                      // Clear carry flag     CF = 0
Instruction0Arg         STC   (0xF9);                      // Set   carry flag     CF = 1
Instruction0Arg         CLI   (0xFA);                      // Clear interrupt flag IF = 0
Instruction0Arg         STI   (0xFB);                      // Set   interrupt flag IF = 1
Instruction0Arg         CLD   (0xFC);                      // Clear direction flag DF = 0
Instruction0Arg         STD   (0xFD);                      // Set   direction flag DF = 1

#ifdef IS64BIT
Instruction0Arg         CLGI  (0x0F01DD);                  // Clear Global Interrupt Flag
Instruction0Arg         STGI  (0x0F01DC);                  // Set Global Interrupt Flag
#endif // IS64BIT

#ifdef IS32BIT
Instruction0Arg         PUSHFD(0x9C);                      // Push EFLAGS onto stack *--SP = FLAGS;
Instruction0Arg         POPFD (0x9D);                      // Pop  EFLAGS register from stack FLAGS = *SP++
Instruction0Arg         PUSHF (PUSHFD,REGSIZE_WORD);       // Push FLAGS  onto stack *--SP = FLAGS;
Instruction0Arg         POPF  (PUSHFD,REGSIZE_WORD);       // Pop  FLAGS  register from stack FLAGS = *SP++
#else
Instruction0Arg         PUSHFQ(0x9C);                      // Push RFLAGS onto stack *--SP = FLAGS;
Instruction0Arg         POPFQ (0x9D);                      // Pop  RFLAGS register from stack FLAGS = *SP++
Instruction0Arg         PUSHF (PUSHFQ,REGSIZE_WORD);       // Push FLAGS  onto stack *--SP = FLAGS;
Instruction0Arg         POPF  (PUSHFQ,REGSIZE_WORD);       // Pop  FLAGS  register from stack FLAGS = *SP++
#endif // IS46BIT

Instruction0Arg         SAHF  (0x9E);                      // Store AH into FLAGS
Instruction0Arg         LAHF  (0x9F);                      // Load FLAGS into AH register
#ifdef IS32BIT
Instruction0Arg         PUSHAD(0x60);                      // Push all double-word (32-bit) registers onto stack
Instruction0Arg         POPAD (0x61);                      // Pop  all double-word (32-bit) registers from stack
#endif // IS32BIT

Instruction0Arg         NOOP  (0x90);
Opcode2Arg              ADD(0x00),OR(0x08),ADC(0x10),SBB(0x18),AND(0x20),SUB(0x28),XOR(0x30),CMP(0x38);
OpcodeMov               MOV(0x88, 0xB0, 0xC6);

//        Non existing opcode (0xF6,1);
Opcode1Arg              NOT   (0xF6,2);                    // Negate the operand, logical NOT
Opcode1Arg              NEG   (0xF6,3);                    // Two's complement negation
Opcode1Arg              MUL   (0xF6,4);                    // Unsigned multiply (ax = al*src, dx:ax=ax*src, edx:eax=eax*src, rdx:rax=rax*src)
Opcode1Arg              IMUL  (0xF6,5);                    // Signed multiply   (ax = al*src, dx:ax=ax*src, edx:eax=eax*src, rdx:rax=rax*src)
Opcode1Arg              DIV   (0xF6,6);                    // Unsigned divide   (ax/=src,al=quot,ah=rem,    edx:eax/=src,eax=quot,edx=rem,  rdx:rax/=src,rax=quit,rdx=rem
Opcode1Arg              IDIV  (0xF6,7);                    // // Signed divide   ax      /= src, ah  must contain sign extension of al . al =quot, ah =rem
                                                           //                    dk:ax   /= src. dx  must contain sign extension of ax . ax =quot, dx =rem
                                                           //                    edx:eax /= src. edx must contain sign extension of eax. eax=quot, edx=rem
                                                           //                    rdx:rax /= src. rdx must contain sign extension of rax. rax=quot, rdx=rem


OpcodeShiftRot          ROL   (0    );                     // Rotate left  by cl/imm
OpcodeShiftRot          ROR   (1    );                     // Rotate right by cl/imm
OpcodeShiftRot          RCL   (2    );                     // Rotate left  by cl/imm (with carry)
OpcodeShiftRot          RCR   (3    );                     // Rotate right by cl/imm (with carry)
OpcodeShiftRot          SHL   (4    );                     // Shift left   by cl/imm                 (unsigned shift left )
OpcodeShiftRot          SHR   (5    );                     // Shift right  by cl/imm                 (unsigned shift right)
// OpcodeShiftRot          SH??   (6    );                 // DO WHAT?
OpcodeShiftRot          SAR   (7    );                     // Shift Arithmetically right by cl/imm   (signed shift   right)

Instruction0Arg         CWDE  (0x98);                      // Convert word to dword   Copy sign (bit 15) of AX  into higher 16 bits of EAX
Instruction0Arg         CBW   (CWDE,REGSIZE_WORD);         // Convert byte to word    Copy sign (bit 7)  of AL  into every bit of AH
Instruction0Arg         CDQ   (0x99);                      // Convert dword to qword  Copy sign (bit 31) of EAX into every bit of EDX
Instruction0Arg         CWD   (CDQ ,REGSIZE_WORD);         // Convert word to dword   Copy sign (bit 15) of AX  into every bit of DX

#ifdef IS64BIT
Instruction0Arg         CDQE  (CWDE,REGSIZE_QWORD);        // Sign extend EAX into RAX
Instruction0Arg         CQO   (CDQ ,REGSIZE_QWORD);        // Sign extend RAX into RDX:RAX
#endif // IS64BIT
