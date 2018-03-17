#include "pch.h"
#include <Math/Expression/NewOpCode.h>

#pragma warning(disable : 4073)
#pragma init_seg(lib)

// Set Byte on Condition
SetxxOp                 SETO  (0x0F90);                    // Set byte   if overflow
SetxxOp                 SETNO (0x0F91);                    // Set byte   if not overflow
SetxxOp                 SETB  (0x0F92);                    // Set byte   if below                 (unsigned)
SetxxOp                 SETAE (0x0F93);                    // Set byte   if above or equal        (unsigned)
SetxxOp                 SETE  (0x0F94);                    // Set byte   if equal                 (signed/unsigned)
SetxxOp                 SETNE (0x0F95);                    // Set byte   if not equal             (signed/unsigned)
SetxxOp                 SETBE (0x0F96);                    // Set byte   if below or equal        (unsigned)
SetxxOp                 SETA  (0x0F97);                    // Set byte   if above                 (unsigned)
SetxxOp                 SETS  (0x0F98);                    // Set byte   if sign
SetxxOp                 SETNS (0x0F99);                    // Set byte   if not sign
SetxxOp                 SETPE (0x0F9A);                    // Set byte   if parity even
SetxxOp                 SETPO (0x0F9B);                    // Set byte   if parity odd
SetxxOp                 SETL  (0x0F9C);                    // Set byte   if less                  (signed  )
SetxxOp                 SETGE (0x0F9D);                    // Set byte   if greater or equal      (signed  )
SetxxOp                 SETLE (0x0F9E);                    // Set byte   if less or equal         (signed  )
SetxxOp                 SETG  (0x0F9F);                    // Set byte   if greater               (signed  );

Instruction0Arg         RET   (0xC3,1);                    // Near return to calling procedure

Instruction0Arg         CMC   (0xF5,1);                    // Complement carry flag
Instruction0Arg         CLC   (0xF8,1);                    // Clear carry flag     CF = 0
Instruction0Arg         STC   (0xF9,1);                    // Set   carry flag     CF = 1
Instruction0Arg         CLI   (0xFA,1);                    // Clear interrupt flag IF = 0
Instruction0Arg         STI   (0xFB,1);                    // Set   interrupt flag IF = 1
Instruction0Arg         CLD   (0xFC,1);                    // Clear direction flag DF = 0
Instruction0Arg         STD   (0xFD,1);                    // Set   direction flag DF = 1

Instruction0Arg         PUSHF (0x9C,1);                    // Push FLAGS onto stack *--SP = FLAGS;
Instruction0Arg         POPF  (0x9D,1);                    // Pop  FLAGS register from stack FLAGS = *SP++
Instruction0Arg         SAHF  (0x9E,1);                    // Store AH into FLAGS
Instruction0Arg         LAHF  (0x9F,1);                    // Load FLAGS into AH register
Instruction0Arg         PUSHAD(0x60,1);                    // Push all double-word (32-bit) registers onto stack
Instruction0Arg         POPAD (0x61,1);                    // Pop  all double-word (32-bit) registers from stack

Instruction0Arg         NOOP  (0x90,1);
Opcode2Arg              ADD(0x00),OR(0x08),ADC(0x10),SBB(0x18),AND(0x20),SUB(0x28),XOR(0x30),CMP(0x38);
OpcodeMov               MOV(0x88, 0xB0);
