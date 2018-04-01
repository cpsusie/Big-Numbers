#include "pch.h"
#include "InstructionBuilder.h"

// --------------------------------- OpcodeBase -----------------------------------

#define _SWAP2(op) ((((op)&0xff)<< 8) | (((op)>> 8)&0xff ))
#define _SWAP3(op) ((_SWAP2(op) << 8) | (((op)>>16)&0xff ))
#define _SWAP4(op) ((_SWAP2(op) <<16) | (_SWAP2((op)>>16)))

static inline UINT swapBytes(UINT bytes, int sz) {
  switch(sz) {
  case 1 : return bytes;
  case 2 : return _SWAP2(bytes);
  case 3 : return _SWAP3(bytes);
  case 4 : return _SWAP4(bytes);
  default: throwInvalidArgumentException(__TFUNCTION__, _T("sz=%d"), sz);
           return bytes;
  }
}

static inline BYTE findSize(UINT op) {
  if( op == (BYTE)op     ) return 1;
  if((op&0xffff0000) == 0) return 2;
  if((op&0xff000000) == 0) return 3;
  return 4;
}

OpcodeBase::OpcodeBase(const String &mnemonic, UINT op, BYTE extension, BYTE opCount, UINT flags)
  : m_mnemonic(  toLowerCase(mnemonic))
  , m_size(      findSize(op)         )
  , m_extension( extension            )
  , m_opCount(   opCount              )
  , m_flags(     flags                )
{
  assert(extension <= 7);
  m_bytes = swapBytes(op,m_size);
  if(getFlags() & HAS_SIZEBIT) {
    if(op & 1) {
      throwInvalidArgumentException(_T("%s:HAS_SIZEBIT set for opcode %X (bit 0 already set)")
                                   ,getMnemonic().cstr()
                                   ,op
                                   );
    }
    if(ISWORDPTR_ONLY(getFlags())) {
      throwInvalidArgumentException(_T("%s:HAS_SIZEBIT set for opcode %X, and only Word size allowed")
                                   ,getMnemonic().cstr()
                                   ,op
                                   );
    }
  }
  if(getFlags() & HAS_DIRECTIONBIT) {
    if(op & 2) {
      throwInvalidArgumentException(_T("%s:HAS_DIRECTIONBIT set for opcode %X (bit 1 already set)")
                                   ,getMnemonic().cstr()
                                   ,op
                                   );
    }
    if(getFlags() & FIRSTOP_REGONLY) {
      throwInvalidArgumentException(_T("%s:HAS_DIRECTIONBIT and FIRSTOP_REGONLY cannot both be set")
                                   ,getMnemonic().cstr()
                                   );
    }
  }

#define ALL_REGTYPES (REGTYPE_GPR0_ALLOWED|REGTYPE_GPR_ALLOWED|REGTYPE_SEG_ALLOWED|REGTYPE_FPU_ALLOWED|REGTYPE_XMM_ALLOWED)

  if((getFlags() & FIRSTOP_REGONLY) && ((getFlags() & ALL_REGTYPES) == 0)) {
    throwInvalidArgumentException(_T("%s:FIRSTOP_REGONLY is set, but no registertypes allowed")
                                 ,getMnemonic().cstr()
                                 );
  }
  if((getFlags() & LASTOP_IMMONLY) && ((getFlags() & (IMMEDIATEVALUE_ALLOWED | IMM64_ALLOWED)) == 0)) {
      throwInvalidArgumentException(_T("%s:LASTOP_IMMONLY is set, but no immediate operands allowed")
                                   ,getMnemonic().cstr()
                                   );
  }
}

OpcodeBase::OpcodeBase(const String &mnemonic, const InstructionBase &src, BYTE extension, UINT flags)
  : m_mnemonic( toLowerCase(mnemonic))
  , m_size(     src.size()           )
  , m_extension(extension            )
  , m_opCount(  0                    )
  , m_flags(    flags                )
{
  assert(extension  <= 7);
  assert(src.size() <= 4);
  m_bytes = 0;
  memcpy(&m_bytes, src.getBytes(), src.size()); // no byte swap
}

bool OpcodeBase::isRegisterAllowed(const Register &reg) const {
  switch(reg.getType()) {
  case REGTYPE_GPR:
    if(!isRegisterSizeAllowed(reg.getSize())) {
      return false;
    }
    return  ((getFlags() & REGTYPE_GPR_ALLOWED ) != 0)
        || (((getFlags() & REGTYPE_GPR0_ALLOWED) != 0) && (reg.getIndex() == 0));
  case REGTYPE_SEG: return (getFlags() & REGTYPE_SEG_ALLOWED) != 0;
  case REGTYPE_FPU: return (getFlags() & REGTYPE_FPU_ALLOWED) != 0;
  case REGTYPE_XMM: return (getFlags() & REGTYPE_XMM_ALLOWED) != 0;
  default         : NODEFAULT;
  }
  throwUnknownRegisterType(__TFUNCTION__,reg.getType());
  return false;
}

bool OpcodeBase::isRegisterSizeAllowed(RegSize size) const {
  switch(size) {
  case REGSIZE_BYTE : return (getFlags() & REGSIZE_BYTE_ALLOWED ) != 0;
  case REGSIZE_WORD : return (getFlags() & REGSIZE_WORD_ALLOWED ) != 0;
  case REGSIZE_DWORD: return (getFlags() & REGSIZE_DWORD_ALLOWED) != 0;
  case REGSIZE_QWORD: return (getFlags() & REGSIZE_QWORD_ALLOWED) != 0;
  case REGSIZE_TBYTE: return (getFlags() & REGSIZE_TBYTE_ALLOWED) != 0;
  case REGSIZE_OWORD: return (getFlags() & REGSIZE_OWORD_ALLOWED) != 0;
  }
  return false;
}

bool OpcodeBase::isMemoryOperandSizeAllowed(OperandSize size) const {
  switch(size) {
  case REGSIZE_BYTE : return (getFlags() & BYTEPTR_ALLOWED ) != 0;
  case REGSIZE_WORD : return (getFlags() & WORDPTR_ALLOWED ) != 0;
  case REGSIZE_DWORD: return (getFlags() & DWORDPTR_ALLOWED) != 0;
  case REGSIZE_QWORD: return (getFlags() & QWORDPTR_ALLOWED) != 0;
  case REGSIZE_TBYTE: return (getFlags() & TBYTEPTR_ALLOWED) != 0;
  case REGSIZE_OWORD: return (getFlags() & OWORDPTR_ALLOWED) != 0;
  case REGSIZE_VOID : return (getFlags() & VOIDPTR_ALLOWED ) != 0;
  }
  return false;
}

bool OpcodeBase::isImmediateSizeAllowed(OperandSize size) const {
  switch(size) {
  case REGSIZE_BYTE :
    return (getFlags() & (IMM64_ALLOWED|IMM32_ALLOWED|IMM16_ALLOWED|IMM8_ALLOWED)) != 0;
  case REGSIZE_WORD :
    return (getFlags() & (IMM64_ALLOWED|IMM32_ALLOWED|IMM16_ALLOWED)) != 0;
  case REGSIZE_DWORD:
    return (getFlags() & (IMM64_ALLOWED|IMM32_ALLOWED)) != 0;
  case REGSIZE_QWORD:
    return (getFlags() & (IMM64_ALLOWED)) != 0;
  }
  return false;
}

void OpcodeBase::throwInvalidOperandCombination(const InstructionOperand &op1, const InstructionOperand &op2) const {
  throwException(_T("%s:Invalid combination of operands:%s,%s")
                ,getMnemonic().cstr()
                ,op1.toString().cstr()
                ,op2.toString().cstr()
                );
}

void OpcodeBase::throwInvalidOperandCombination(const InstructionOperand &op1, const InstructionOperand &op2, const InstructionOperand &op3) const {
  throwException(_T("%s:Invalid combination of operands:%s,%s,%s")
                ,getMnemonic().cstr()
                ,op1.toString().cstr()
                ,op2.toString().cstr()
                ,op3.toString().cstr()
                );
}

void OpcodeBase::throwInvalidOperandType(const InstructionOperand &op, BYTE index) const {
  throwException(_T("%s:%s not a valid %d. operand")
                ,getMnemonic().cstr()
                ,op.toString().cstr()
                ,index
                );
}

void OpcodeBase::throwUnknownOperandType(const InstructionOperand &op, BYTE index) const {
  throwException(_T("%s:Operand %d has unknown type=%s")
                ,getMnemonic().cstr()
                ,index
                ,::toString(op.getType()).cstr());
}

void OpcodeBase::throwUnknownRegisterType(const TCHAR *method, RegType type) { // static
  throwInvalidArgumentException(method, _T("RegisterType=%s"), ::toString(type).cstr());
}

#define RAISEERROR(...)                                                                         \
{ if(throwOnError) throwException(_T("%s:%s"),getMnemonic().cstr(),format(__VA_ARGS__).cstr()); \
  return false;                                                                                 \
}

#define CHECKFIRSTOP_REGONLY(op)                                                                \
{ if((getFlags() & FIRSTOP_REGONLY) && (op.getType() != REGISTER)) {                            \
    RAISEERROR(_T("Operand 1 must be register. Type=%s"), ::toString(op.getType()).cstr());     \
  }                                                                                             \
}

#define CHECKLASTOP_IMMONLY(op)                                                                 \
{ if((getFlags() & LASTOP_IMMONLY) && (op.getType() != IMMEDIATEVALUE)) {                       \
    RAISEERROR(_T("Last operand must be immediate value. Type=%s")                              \
              ,::toString(op.getType()).cstr());                                                \
  }                                                                                             \
}

bool OpcodeBase::validateOpCount(int count, bool throwOnError) const {
  if((count < getOpCount()) || (count > getMaxOpCount())) {
    if(getOpCount() == getMaxOpCount()) {
      RAISEERROR(_T("%d operand(s) specified. Expected %d"), count, getOpCount());
    } else {
      RAISEERROR(_T("%d operand(s) specified. Expected %d-%d"), count, getOpCount(), getMaxOpCount());
    }
  }
  return true;
}

bool OpcodeBase::validateRegisterAllowed(const Register &reg, bool throwOnError) const {
  if(!isRegisterAllowed(reg)) {
    RAISEERROR(_T("%s not allowed"), reg.getName().cstr());
  }
  return true;
}

bool OpcodeBase::validateMemoryOperandAllowed(const MemoryOperand &mem, bool throwOnError) const {
  if(!isMemoryOperandAllowed(mem)) {
    RAISEERROR(_T("%s not allowed"), mem.toString().cstr());
  }
  return true;
}

bool OpcodeBase::validateImmediateOperandAllowed(const InstructionOperand &imm, bool throwOnError) const {
  if(!isImmediateSizeAllowed(imm.getSize())) {
    RAISEERROR(_T("Immediate value %s not allowed"), imm.toString().cstr());
  }
  return true;
}

bool OpcodeBase::validateImmediateValue(const Register &reg, const InstructionOperand &imm, bool throwOnError) const {
  if(!validateImmediateOperandAllowed(imm, throwOnError)) {
    return false;
  }
  if(!reg.containsSize(imm.getSize())) {
    RAISEERROR(_T("%s"), getImmSizeErrorString(reg.toString(), imm.getImmInt64()).cstr());
  }
  return true;
}

bool OpcodeBase::validateImmediateValue(const MemoryOperand &mem, const InstructionOperand &imm, bool throwOnError) const {
  if(!validateImmediateOperandAllowed(imm, throwOnError)) {
    return false;
  }
  if(!mem.containsSize(imm.getSize())) {
    RAISEERROR(_T("%s"), getImmSizeErrorString(mem.toString(), imm.getImmInt64()).cstr());
  }
  return true;
}

bool OpcodeBase::validateSameSize(const Register &reg1, const Register &reg2, bool throwOnError) const {
  if(reg1.getSize() != reg2.getSize()) {
    RAISEERROR(_T("Different size:%s,%s"), reg1.toString().cstr(), reg2.toString().cstr());
  }
  if(!validateIsRexCompatible(reg1,reg2,throwOnError) || !validateIsRexCompatible(reg2,reg1,throwOnError)) {
    return false;
  }
  return true;
}

bool OpcodeBase::validateSameSize(const Register &reg, const InstructionOperand &op, bool throwOnError) const {
  if(op.getType() == REGISTER) {
    return validateSameSize(reg, op.getRegister(), throwOnError);
  } else {
    if(reg.getSize() != op.getSize()) {
      RAISEERROR(_T("Different size:%s,%s"), reg.toString().cstr(), op.toString().cstr());
    }
    if(!validateIsRexCompatible(reg,op, throwOnError)) {
      return false;
    }
  }
  return true;
}

bool OpcodeBase::validateSameSize(const InstructionOperand &op1, const InstructionOperand &op2, bool throwOnError) const {
  if(op1.getType() == REGISTER) {
    if(!validateSameSize(op1.getRegister(),op2, throwOnError)) {
      return false;
    }
  } else if(op2.getType() == REGISTER) {
    if(!validateSameSize(op2.getRegister(),op1, throwOnError)) {
      return false;
    }
  } else {
    if(!throwOnError) {
      return false;
    }
    throwInvalidOperandCombination(op1,op2);
  }
  return true;
}

#ifdef IS64BIT
bool OpcodeBase::validateIsRexCompatible(const Register &reg, const InstructionOperand &op, bool throwOnError) const {
  if(!reg.isREXCompatible(op.needREXByte())) {
    RAISEERROR(_T("%s not allowed together with %s (Use %s)")
              ,reg.toString().cstr()
              ,op.toString().cstr()
              ,Register::getREXCompatibleRegisterNames(op.needREXByte())
             );
  }
  return true;
}

bool OpcodeBase::validateIsRexCompatible(const Register &reg1, const Register &reg2, bool throwOnError) const {
  if(!reg1.isREXCompatible(reg2.indexNeedREXByte())) {
    RAISEERROR(_T("%s not allowed together with %s (Use %s)")
              ,reg1.toString().cstr()
              ,reg2.toString().cstr()
              ,Register::getREXCompatibleRegisterNames(reg2.indexNeedREXByte())
              );
  }
  return true;
}
#endif // IS64BIT

bool OpcodeBase::validateIsRegisterOperand(const InstructionOperand &op, BYTE index, bool throwOnError) const {
  switch(op.getType()) {
  case REGISTER      :
    if(!validateRegisterAllowed(op.getRegister(), throwOnError)) {
      return false;
    }
    break;
  case MEMORYOPERAND :
  case IMMEDIATEVALUE:
    if(!throwOnError) return false;
    throwInvalidOperandType(op,index);
  default            :
    throwUnknownOperandType(op,index);
  }
  return true;
}

bool OpcodeBase::validateIsMemoryOperand(const InstructionOperand &op, BYTE index, bool throwOnError) const {
  switch(op.getType()) {
  case MEMORYOPERAND :
    if(!validateMemoryOperandAllowed((MemoryOperand&)op, throwOnError)) {
      return false;
    }
    break;
  case REGISTER      :
  case IMMEDIATEVALUE:
    if(!throwOnError) return false;
    throwInvalidOperandType(op,index);
  default            :
    throwUnknownOperandType(op,index);
  }
  return true;
}

bool OpcodeBase::validateIsRegisterOrMemoryOperand(const InstructionOperand &op, BYTE index, bool throwOnError) const {
  switch(op.getType()) {
  case REGISTER      :
    if(!validateIsRegisterOperand(op, index, throwOnError)) {
      return false;
    }
    break;
  case MEMORYOPERAND :
    if(!validateIsMemoryOperand(op, index, throwOnError)) {
      return false;
    }
    break;
  case IMMEDIATEVALUE:
    if(!throwOnError) return false;
    throwInvalidOperandType(op,index);
  default            :
    throwUnknownOperandType(op,index);
  }
  return true;
}

bool OpcodeBase::validateIsImmediateOperand(const InstructionOperand &op, BYTE index, bool throwOnError) const {
  switch(op.getType()) {
  case REGISTER      :
  case MEMORYOPERAND :
    if(!throwOnError) return false;
    throwInvalidOperandType(op,index);
    break;
  case IMMEDIATEVALUE:
    if(!validateImmediateOperandAllowed(op, throwOnError)) {
      return false;
    }
    break;
  default            :
    throwUnknownOperandType(op,index);
  }
  return true;
}

bool OpcodeBase::validateIsShiftAmountOperand(const InstructionOperand &op, BYTE index, bool throwOnError) const {
  switch(op.getType()) {
  case REGISTER      :
    if(op.getRegister() != CL) {
      RAISEERROR(_T("Register=%s. Must be cl"), op.toString().cstr());
    }
    break;
  case MEMORYOPERAND :
    if(!throwOnError) return false;
    throwInvalidOperandType(op,index);
  case IMMEDIATEVALUE:
    if(op.getSize() != REGSIZE_BYTE) {
      RAISEERROR(_T("Immediate value must be BYTE"));
    }
    break;
  default:
    throwUnknownOperandType(op,index);
  }
  return true;
}

const OperandTypeSet &OpcodeBase::getValidOperandTypes(BYTE index) const {
  validateOperandIndex(index);
  switch(index) {
  case 1: return InstructionOperand::RM;
  case 2: return InstructionOperand::RM;
  case 3: return InstructionOperand::S;
  default:return InstructionOperand::EMPTY;
  }
}

bool OpcodeBase::isValidOperandType(const InstructionOperand &op, BYTE index) const {
  if(!getValidOperandTypes(index).contains(op.getType())) {
    return false;
  }
  const bool throwOnError = false;
  if(index == 1) {
    CHECKFIRSTOP_REGONLY(op);
  }
  if(index == getMaxOpCount()) {
    CHECKLASTOP_IMMONLY(op);
  }
  switch(op.getType()) {
  case REGISTER      : return isRegisterAllowed(op.getRegister());
  case MEMORYOPERAND : return isMemoryOperandAllowed((MemoryOperand&)op);
  case IMMEDIATEVALUE: return isImmediateSizeAllowed(op.getSize());
  }
  throwInvalidOperandType(op,index);
  return false;
}

bool OpcodeBase::isValidOperand(const InstructionOperand &op, bool throwOnError) const {
  if(!validateOpCount(1, throwOnError)) {
    return false;
  }
  CHECKFIRSTOP_REGONLY(op);
  switch(op.getType()) {
  case REGISTER       :
    if(!validateRegisterAllowed(op.getRegister(), throwOnError)) {
      return false;
    }
    break;
  case MEMORYOPERAND  :
    if(!validateMemoryOperandAllowed((MemoryOperand&)op, throwOnError)) {
      return false;
    }
    break;
  case IMMEDIATEVALUE :
    if(!validateImmediateOperandAllowed(op, throwOnError)) {
      return false;
    }
    break;
  default             :
    throwUnknownOperandType(op,1);
  }
  return true;
}

bool OpcodeBase::isValidOperandCombination(const Register &reg, const InstructionOperand &op, bool throwOnError) const {
  if(!validateOpCount(2, throwOnError)) {
    return false;
  }
  CHECKLASTOP_IMMONLY(    op);
  if(!validateRegisterAllowed(reg, throwOnError)) {
    return false;
  }
  switch(op.getType()) {
  case REGISTER       : // reg <- reg
    { if(!validateRegisterAllowed(op.getRegister(), throwOnError)) {
        return false;
      }
      if(!validateSameSize(reg, op, throwOnError)) {
        return false;
      }
    }
    break;
  case MEMORYOPERAND  : // reg <- mem
    if(!validateMemoryOperandAllowed((MemoryOperand&)op, throwOnError)) {
      return false;
    }
    if(!validateSameSize(reg, op, throwOnError)) {
      return false;
    }
    break;
  case IMMEDIATEVALUE : // reg <- imm
    if(!validateImmediateValue(reg, op, throwOnError)) {
      return false;
    }
    break;
  default             :
    throwUnknownOperandType(op,2);
  }
  return true;
}

bool OpcodeBase::isValidOperandCombination(const InstructionOperand &op1, const InstructionOperand &op2, bool throwOnError) const {
  if(op1.getType() == REGISTER) {
    return isValidOperandCombination(op1.getRegister(), op2, throwOnError);
  }
  if(!validateOpCount(2, throwOnError)) {
    return false;
  }
  CHECKFIRSTOP_REGONLY(   op1);
  CHECKLASTOP_IMMONLY(    op2);
  switch(op1.getType()) {
  case MEMORYOPERAND  :
    if(!validateMemoryOperandAllowed((MemoryOperand&)op1, throwOnError)) {
      return false;
    }
    switch(op2.getType()) {
    case REGISTER       : // mem <- reg
      { const Register &regSrc = op2.getRegister();
        if(!validateRegisterAllowed(regSrc, throwOnError)) {
          return false;
        }
        if(!validateSameSize(op1,op2, throwOnError)) {
          return false;
        }
      }
      break;
    case MEMORYOPERAND  : // mem <- mem:ERROR
      RAISEERROR(_T("%s:Invalid combination of operands:%s,%s"), getMnemonic().cstr(), op1.toString().cstr(), op2.toString().cstr());
      break;
    case IMMEDIATEVALUE : // mem <- imm
      if(!validateImmediateValue((MemoryOperand&)op1, op2, throwOnError)) {
        return false;
      }
      break;
    default              :
      throwUnknownOperandType(op2,2);
    }
    break;
  case IMMEDIATEVALUE : // imm <- reg/mem:ERROR
    RAISEERROR(_T("%s:Invalid combination of operands:%s,%s"), getMnemonic().cstr(), op1.toString().cstr(), op2.toString().cstr());
    break;
  default              :
    throwUnknownOperandType(op1,1);
  }
  return true;
}

bool OpcodeBase::isValidOperandCombination(const InstructionOperand &op1, const InstructionOperand &op2, const InstructionOperand &op3, bool throwOnError) const {
  if(!validateOpCount(3, throwOnError)) {
    return false;
  }
  CHECKFIRSTOP_REGONLY(   op1);
  CHECKLASTOP_IMMONLY(    op3);
  switch(op1.getType()) {
  case REGISTER       :
    if(!validateRegisterAllowed(op1.getRegister(), throwOnError)) {
      return false;
    }
    if(!validateIsRegisterOrMemoryOperand(op2, 2, throwOnError)) {
      return false;
    }
    if(!validateSameSize(op1,op2, throwOnError)) {
      return false;
    }
    if(op3.getType() == IMMEDIATEVALUE) {
      if(!validateImmediateValue(op1.getRegister(), op3, throwOnError)) {
        return false;
      }
    }
    break;
  default:
    RAISEERROR(_T("%s:Invalid combination of operands:%s,%s,%s")
              ,getMnemonic().cstr(), op1.toString().cstr(), op2.toString().cstr(), op3.toString().cstr());
  }
  return true;
}

InstructionBase OpcodeBase::operator()(const InstructionOperand &op) const {
  throwUnsupportedOperationException(__TFUNCTION__);
  return InstructionBuilder(*this);
}

InstructionBase OpcodeBase::operator()(const InstructionOperand &op1, const InstructionOperand &op2) const {
  throwUnsupportedOperationException(__TFUNCTION__);
  return InstructionBuilder(*this);
}

InstructionBase OpcodeBase::operator()(const InstructionOperand &op1, const InstructionOperand &op2, const InstructionOperand &op3) const {
  throwUnsupportedOperationException(__TFUNCTION__);
  return InstructionBuilder(*this);
}
