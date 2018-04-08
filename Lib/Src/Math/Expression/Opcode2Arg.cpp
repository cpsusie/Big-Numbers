#include "pch.h"
#include "InstructionBuilder.h"

class Instruction2ArgImm : public InstructionBuilder {
public:
  Instruction2ArgImm(const OpcodeBase &opcode) : InstructionBuilder(opcode) {
  }
  InstructionBuilder &setRegImm(const Register      &reg, const InstructionOperand &imm);
  InstructionBuilder &setMemImm(const MemoryOperand &mem, const InstructionOperand &imm);
};

#define IMMOP 0x80
InstructionBuilder &Instruction2ArgImm::setRegImm(const Register &reg, const InstructionOperand &imm) {
  const BYTE    regIndex  = reg.getIndex();
  const RegSize regSize   = reg.getSize();
  const bool    immIsByte = Register::sizeContainsSrcSize(REGSIZE_BYTE,imm.getSize());
  switch(regSize) {
  case REGSIZE_BYTE :
    if(regIndex == 0) {
      or(0x04).setOperandSize(regSize).addImmediateOperand(imm,regSize);
    } else {
      prefixImm(IMMOP,reg,true ).setModeBits(MR_REG(regIndex)).addImmediateOperand(imm,regSize);
    }
    break;
  case REGSIZE_WORD :
    if(immIsByte) {
      prefixImm(IMMOP,reg,true ).setModeBits(MR_REG(regIndex)).addImmediateOperand(imm,REGSIZE_BYTE);
    } else if(regIndex == 0) {
      or(0x04).setOperandSize(regSize).addImmediateOperand(imm,regSize);
    } else {
      prefixImm(IMMOP,reg,false).setModeBits(MR_REG(regIndex)).addImmediateOperand(imm,regSize);
    }
    break;
  default           :
    if(immIsByte) {
      prefixImm(IMMOP,reg,true ).setModeBits(MR_REG(regIndex)).addImmediateOperand(imm,REGSIZE_BYTE);
    } else if(regIndex == 0) {
      or(0x04).setOperandSize(regSize).addImmediateOperand(imm,REGSIZE_DWORD);
    } else {
      prefixImm(IMMOP,reg,false).setModeBits(MR_REG(regIndex)).addImmediateOperand(imm,REGSIZE_DWORD);
    }
    break;
  }
  SETREXBITS(HIGHINDEXTOREX(regIndex,0));
  return *this;
}

InstructionBuilder &Instruction2ArgImm::setMemImm(const MemoryOperand &mem, const InstructionOperand &imm) {
  const OperandSize size      = mem.getSize();
  const bool        immIsByte = Register::sizeContainsSrcSize(REGSIZE_BYTE,imm.getSize());
  switch(size) {
  case REGSIZE_BYTE :
    prefixImm(IMMOP,mem,true).setMemoryOperand(mem).addImmediateOperand(imm,size);
    break;
  case REGSIZE_WORD :
    prefixImm(IMMOP,mem,immIsByte).setMemoryOperand(mem).addImmediateOperand(imm,immIsByte?REGSIZE_BYTE:size);
    break;
  default           :
    prefixImm(IMMOP,mem,immIsByte).setMemoryOperand(mem).addImmediateOperand(imm,immIsByte?REGSIZE_BYTE:REGSIZE_DWORD);
    break;
  }
  return *this;
}

InstructionBase Opcode2Arg::operator()(const InstructionOperand &op1, const InstructionOperand &op2) const {
  isValidOperandCombination(op1,op2,true);
  switch(op1.getType()) {
  case REGISTER       :
    switch(op2.getType()) {
    case REGISTER       : // reg <- reg
      return InstructionBuilder(*this).setRegRegOperands(op1.getRegister(),op2.getRegister());
    case MEMORYOPERAND  : // reg <- mem
      return InstructionBuilder(*this).setMemoryRegOperands((MemoryOperand&)op2, op1.getRegister()).setDirectionBit(REGISTER,MEMORYOPERAND);
    case IMMEDIATEVALUE : // reg <- imm
      return Instruction2ArgImm(*this).setRegImm(op1.getRegister(), op2);
    }
    break;
  case MEMORYOPERAND    :
    switch(op2.getType()) {
    case REGISTER       : // mem <- reg
      return InstructionBuilder(*this).setMemoryRegOperands((MemoryOperand&)op1, op2.getRegister()).setDirectionBit(MEMORYOPERAND,REGISTER);
    case IMMEDIATEVALUE : // mem <- imm
      return Instruction2ArgImm(*this).setMemImm((MemoryOperand&)op1, op2);
    }
  }
  throwInvalidOperandCombination(op1,op2);
  return __super::operator()(op1,op2);
}

InstructionBase Opcode2ArgPfxF2::operator()(const InstructionOperand &op1, const InstructionOperand &op2) const {
  return InstructionBuilder(__super::operator()(op1,op2)).prefix(0xF2);
}

bool Opcode2ArgPfxF2SD::validateSameSize(const Register &reg, const InstructionOperand &op, bool throwOnError) const {
  if((reg.getType() == REGTYPE_XMM) && (op.getType() == MEMORYOPERAND)) {
    if(op.getSize() == REGSIZE_MMWORD) {
      return true;
    }
  }
  return __super::validateSameSize(reg,op,throwOnError);
}
