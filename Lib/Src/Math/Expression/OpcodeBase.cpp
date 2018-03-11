#include "pch.h"
#include <Math/Expression/NewOpCode.h>
#include "RexByte.h"

// --------------------------------- OpcodeBase -----------------------------------

bool OpcodeBase::isRegisterTypeAllowed(RegType type) const {
  switch(type) {
  case REGTYPE_GP  : return (getFlags() & REGTYPE_GP_ALLOWED ) != 0;
  case REGTYPE_FPU : return (getFlags() & REGTYPE_FPU_ALLOWED) != 0;
  case REGTYPE_XMM : return (getFlags() & REGTYPE_XMM_ALLOWED) != 0;
  }
  return false;
}

bool OpcodeBase::isOperandSizeAllowed(RegSize size) const {
  switch(size) {
  case REGSIZE_BYTE  : return (getFlags() & REGSIZE_BYTE_ALLOWED ) != 0;
  case REGSIZE_WORD  : return (getFlags() & REGSIZE_WORD_ALLOWED ) != 0;
  case REGSIZE_DWORD : return (getFlags() & REGSIZE_DWORD_ALLOWED) != 0;
  case REGSIZE_QWORD : return (getFlags() & REGSIZE_QWORD_ALLOWED) != 0;
  case REGSIZE_TBYTE : return (getFlags() & REGSIZE_TBYTE_ALLOWED) != 0;
  case REGSIZE_OWORD : return (getFlags() & REGSIZE_OWORD_ALLOWED) != 0;
  }
  return false;
}

void OpcodeBase::validateOpCount(int count) const {
  if(getOpCount() != count) {
    throwInvalidArgumentException(__TFUNCTION__, _T("%d operand(s) specified. Expected %d"), count, getOpCount());
  }
}

void OpcodeBase::validateRegisterAllowed(const Register &reg) const {
  DEFINEMETHODNAME;
  if(!isRegisterAllowed()) {
    throwInvalidArgumentException(method, _T("Opcode doesn't use registers"));
  }
  if(!isRegisterTypeAllowed(reg.getType()) || !isOperandSizeAllowed(reg.getSize())) {
    throwInvalidArgumentException(method, _T("%s not allowed for this opcode"), reg.getName().cstr());
  }
}

void OpcodeBase::validateMemoryReferenceAllowed() const {
  if(!isMemoryReferenceAllowed()) {
    throwInvalidArgumentException(__TFUNCTION__, _T("Cannot reference memory for this opcode"));
  }
}

void OpcodeBase::validateImmediateValueAllowed() const {
  if(!isImmediateValueAllowed()) {
    throwInvalidArgumentException(__TFUNCTION__, _T("Immediate value not allowed this opcode"));
  }
}

void OpcodeBase::validateOperandSize(RegSize size) const {
  if(!isOperandSizeAllowed(size)) {
    throwInvalidArgumentException(__TFUNCTION__, _T("Size %s not allowed for this opcode"), toString(size).cstr());
  }
}

void OpcodeBase::validateSameSize(const InstructionOperand &op1, const InstructionOperand &op2) const {
  if(op1.getSize() != op2.getSize()) {
    throwInvalidArgumentException(__TFUNCTION__, _T("Different size:%s,%s"), op1.toString().cstr(), op2.toString().cstr());
  }
}

