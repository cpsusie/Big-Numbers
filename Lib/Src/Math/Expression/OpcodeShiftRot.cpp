#include "pch.h"
#include "InstructionBuilder.h"

class InstructionShiftRot : public InstructionBuilder {
public:
  InstructionShiftRot(const OpcodeBase &opcode) : InstructionBuilder(opcode)
  {
  }
  InstructionBuilder &setGPRegImm(const GPRegister    &dst, INT64 immv);
  InstructionBuilder &setMemImm(  const MemoryOperand &dst, int   immv);
};

bool OpcodeShiftRot::isValidOperandCombination(const InstructionOperand &op1, const InstructionOperand &op2, bool throwOnError) const {
  return false;
}

InstructionBase OpcodeShiftRot::operator()(const InstructionOperand &op1, const InstructionOperand &op2) const {
  throwUnsupportedOperationException(__TFUNCTION__);
  return InstructionShiftRot(*this);
}
