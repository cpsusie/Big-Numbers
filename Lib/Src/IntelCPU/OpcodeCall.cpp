#include "pch.h"

namespace IntelCPU {

#if defined(IS32BIT)
#define JMP_NEAR_ABSRM_FLAGS (REGTYPE_GPR_ALLOWED    | DWORDGPR_ALLOWED   | WORDGPR_ALLOWED | DWORDPTR_ALLOWED | WORDPTR_ALLOWED | HAS_WORDPREFIX)
#define JMP_NEAR_IMM_FLAGS   (IMMEDIATEVALUE_ALLOWED | HAS_IMM_XBIT       | HAS_WORDPREFIX)
#else // IS64BIT
#define JMP_NEAR_ABSRM_FLAGS (REGTYPE_GPR_ALLOWED    | QWORDGPR_ALLOWED   | QWORDPTR_ALLOWED)
#define JMP_NEAR_IMM_FLAGS   (IMMEDIATEVALUE_ALLOWED | HAS_IMM_XBIT)
#endif // IS64BIT

OpcodeJmpImm::OpcodeJmpImm(const String &mnemonic)
  : Opcode1Arg(mnemonic, 0xE9, 0, JMP_NEAR_IMM_FLAGS)
{
}

InstructionBase OpcodeJmpImm::operator()(const InstructionOperand &op) const {
  isValidOperand(op, true);
#if defined(IS32BIT)
  switch(op.getSize()) {
  case REGSIZE_BYTE :
    return InstructionBuilder(*this).setImmediateOperand(op);
  case REGSIZE_WORD :
// NEVER USE THIS. it will clear high 16 bits of EIP
//    return InstructionBuilder(*this).setImmediateOperand(op,REGSIZE_WORD).wordIns();
  case REGSIZE_DWORD:
    return InstructionBuilder(*this).setImmediateOperand(op,REGSIZE_DWORD);
  }
#else  // IS64BIT
  switch(op.getSize()) {
  case REGSIZE_BYTE :
    return InstructionBuilder(*this).setImmediateOperand(op);
  case REGSIZE_WORD :
  case REGSIZE_DWORD:
    return InstructionBuilder(*this).setImmediateOperand(op,REGSIZE_DWORD);
  }
#endif // IS64BIT
  throwUnknownOperandType(op,1);
  return __super::operator()(op);
}

OpcodeJmp::OpcodeJmp(const String &mnemonic)
  : Opcode1Arg( mnemonic, 0xFF, 4, JMP_NEAR_ABSRM_FLAGS)
  , m_jmpRelImm(mnemonic)
{
}

bool OpcodeJmp::isValidOperand(const InstructionOperand &op, bool throwOnError) const {
  switch(op.getType()) {
  case REGISTER      :
  case MEMORYOPERAND : return __super::isValidOperand(   op, throwOnError);
  case IMMEDIATEVALUE: return m_jmpRelImm.isValidOperand(op, throwOnError);
  }
  throwUnknownOperandType(op,1);
  return false;
}

InstructionBase OpcodeJmp::operator()(const InstructionOperand &op) const {
  switch(op.getType()) {
  case REGISTER      :
  case MEMORYOPERAND : return __super::operator()(op);
  case IMMEDIATEVALUE: return m_jmpRelImm(op);
  }
  throwUnknownOperandType(op,1);
  return __super::operator()(op);
};

OpcodeJcc::OpcodeJcc(const String &mnemonic, UINT op)
  : Opcode1Arg( mnemonic, 0x0F00 | ((op)+0x10), 0, IMMEDIATEVALUE_ALLOWED | HAS_WORDPREFIX)
  , m_shortCode(mnemonic, op                  , 0, IMM8_ALLOWED)
{
}

InstructionBase OpcodeJcc::operator()(const InstructionOperand &op) const {
  isValidOperand(op, true);
  if(op.isImmByte()) {
    return InstructionBuilder(m_shortCode).setImmediateOperand(op, REGSIZE_BYTE);
  } else {
#if defined(IS32BIT)
    switch(op.getSize()) {
    case REGSIZE_WORD :
// NEVER USE THIS. it will clear high 16 bits of EIP !!
//    return InstructionBuilder(*this).setImmediateOperand(op, REGSIZE_WORD).wordIns();
    case REGSIZE_DWORD:
      return InstructionBuilder(*this).setImmediateOperand(op, REGSIZE_DWORD);
    }
#else // IS64BIT
    return InstructionBuilder(*this).setImmediateOperand(op, REGSIZE_DWORD);
#endif // IS64BIT
  }
  throwUnknownOperandType(op,1);
  return __super::operator()(op);
}

InstructionBase OpcodeCallImm::operator()(const InstructionOperand &op) const {
  isValidOperand(op,true);
  switch(op.getSize()) {
  case REGSIZE_BYTE :
  case REGSIZE_WORD :
  case REGSIZE_DWORD:
    return InstructionBuilder(*this).setImmediateOperand(op,REGSIZE_DWORD);
  }
  throwUnknownOperandType(op,1);
  return __super::operator()(op);
}

bool OpcodeCall::isValidOperand(const InstructionOperand &op, bool throwOnError) const {
  switch(op.getType()) {
  case REGISTER      :
  case MEMORYOPERAND : return __super::isValidOperand(        op, throwOnError);
  case IMMEDIATEVALUE: return m_callNearRelImm.isValidOperand(op, throwOnError);
  }
  throwUnknownOperandType(op,1);
  return false;
}

InstructionBase OpcodeCall::operator()(const InstructionOperand &op) const {
  switch(op.getType()) {
  case REGISTER      :
  case MEMORYOPERAND : return __super::operator()(op);
  case IMMEDIATEVALUE: return m_callNearRelImm(op);
  }
  throwUnknownOperandType(op,1);
  return __super::operator()(op);
}

}; // namespace
