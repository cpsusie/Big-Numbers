#include "pch.h"
#include <Math/Expression/NewOpCode.h>

// Set Byte on Condition
SetxxOp SETO( 0x0F90);                          // Set byte   if overflow
SetxxOp SETNO(0x0F91);                          // Set byte   if not overflow
SetxxOp SETB (0x0F92);                          // Set byte   if below                 (unsigned)
SetxxOp SETAE(0x0F93);                          // Set byte   if above or equal        (unsigned)
SetxxOp SETE (0x0F94);                          // Set byte   if equal                 (signed/unsigned)
SetxxOp SETNE(0x0F95);                          // Set byte   if not equal             (signed/unsigned)
SetxxOp SETBE(0x0F96);                          // Set byte   if below or equal        (unsigned)
SetxxOp SETA (0x0F97);                          // Set byte   if above                 (unsigned)
SetxxOp SETS (0x0F98);                          // Set byte   if sign
SetxxOp SETNS(0x0F99);                          // Set byte   if not sign
SetxxOp SETPE(0x0F9A);                          // Set byte   if parity even
SetxxOp SETPO(0x0F9B);                          // Set byte   if parity odd
SetxxOp SETL (0x0F9C);                          // Set byte   if less                  (signed  )
SetxxOp SETGE(0x0F9D);                          // Set byte   if greater or equal      (signed  )
SetxxOp SETLE(0x0F9E);                          // Set byte   if less or equal         (signed  )
SetxxOp SETG (0x0F9F);                          // Set byte   if greater               (signed  );
