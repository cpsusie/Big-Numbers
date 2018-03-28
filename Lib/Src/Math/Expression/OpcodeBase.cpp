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
  : m_mnemonic( toLowerCase(mnemonic))
  , m_size(     findSize(op))
  , m_extension(extension   )
  , m_opCount(  opCount     )
  , m_flags(    flags       )
{
  assert(extension <= 7);
  m_bytes = swapBytes(op,m_size);
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

bool OpcodeBase::isRegisterTypeAllowed(RegType type) const {
  switch(type) {
  case REGTYPE_GPR: return (getFlags() & REGTYPE_GPR_ALLOWED) != 0;
  case REGTYPE_FPU: return (getFlags() & REGTYPE_FPU_ALLOWED) != 0;
  case REGTYPE_XMM: return (getFlags() & REGTYPE_XMM_ALLOWED) != 0;
  }
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

const RegSizeSet OpcodeBase::s_wordRegCapacity( REGSIZE_BYTE, REGSIZE_WORD, REGSIZE_END);
const RegSizeSet OpcodeBase::s_dwordRegCapacity(REGSIZE_BYTE, REGSIZE_WORD, REGSIZE_DWORD, REGSIZE_END);
const RegSizeSet OpcodeBase::s_qwordRegCapacity(REGSIZE_BYTE, REGSIZE_WORD, REGSIZE_DWORD, REGSIZE_QWORD, REGSIZE_END);

bool OpcodeBase::sizeContainsSrcSize(OperandSize dstSize, OperandSize srcSize) { // static
  switch(dstSize) {
  case REGSIZE_BYTE : return srcSize == REGSIZE_BYTE;
  case REGSIZE_WORD : return s_wordRegCapacity.contains( srcSize);
  case REGSIZE_DWORD: return s_dwordRegCapacity.contains(srcSize);
  case REGSIZE_QWORD: return s_qwordRegCapacity.contains(srcSize);
  default           : return false;
  }
}

const RegSizeSet OpcodeBase::s_validImmSizeToMem(   REGSIZE_BYTE, REGSIZE_WORD, REGSIZE_DWORD, REGSIZE_END);
const RegSizeSet OpcodeBase::s_validImmSizeToReg(   REGSIZE_BYTE, REGSIZE_WORD, REGSIZE_DWORD, REGSIZE_END);

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

void OpcodeBase::throwUnknownOperandType(const TCHAR *method, OperandType type) { // static
  throwInvalidArgumentException(method, _T("OperandTye=%s"), ::toString(type).cstr());
}

#define RAISEERROR(...)                                                       \
{ if(throwOnError) throwInvalidArgumentException(__TFUNCTION__,__VA_ARGS__); \
  return false;                                                              \
}

bool OpcodeBase::validateOpCount(int count, bool throwOnError) const {
  if(getOpCount() != count) {
    RAISEERROR(_T("%s:%d operand(s) specified. Expected %d"), getMnemonic().cstr(), count, getOpCount());
  }
  return true;
}

bool OpcodeBase::validateRegisterAllowed(const Register &reg, bool throwOnError) const {
  if(!isRegisterAllowed(reg)) {
    RAISEERROR(_T("%s:%s not allowed"), getMnemonic().cstr(), reg.getName().cstr());
  }
  return true;
}

bool OpcodeBase::validateMemoryOperandAllowed(const MemoryOperand &mem, bool throwOnError) const {
  if(!isMemoryOperandAllowed(mem)) {
    RAISEERROR(_T("%s:%s not allowed"), getMnemonic().cstr(), mem.toString().cstr());
  }
  return true;
}

bool OpcodeBase::validateImmediateValueAllowed(bool throwOnError) const {
  if(!isImmediateValueAllowed()) {
    RAISEERROR(_T("%s:Immediate value not allowed"), getMnemonic().cstr());
  }
  return true;
}

bool OpcodeBase::validateImmediateValue(OperandSize dstSize, const InstructionOperand &imm, const RegSizeSet &immSizeSet, bool throwOnError) const {
  if(!validateImmediateValueAllowed(throwOnError)) {
    return false;
  }
  if(!sizeContainsSrcSize(dstSize, imm.getSize()) || !immSizeSet.contains(imm.getSize())) {
    RAISEERROR(_T("%s"), getImmSizeErrorString(::toString(dstSize), imm.getImmInt64()).cstr());
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

bool OpcodeBase::validateRegisterOperand(const InstructionOperand &op, int index, bool throwOnError) const {
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
    throwUnknownOperandType(__TFUNCTION__,op.getType());
  }
  return true;
}

bool OpcodeBase::validateMemoryOperand(const InstructionOperand &op, int index, bool throwOnError) const {
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
    throwUnknownOperandType(__TFUNCTION__,op.getType());
  }
  return true;
}

bool OpcodeBase::validateRegisterOrMemoryOperand(const InstructionOperand &op, int index, bool throwOnError) const {
  switch(op.getType()) {
  case REGISTER      :
    if(!validateRegisterOperand(op, index, throwOnError)) {
      return false;
    }
    break;
  case MEMORYOPERAND :
    if(!validateMemoryOperand(op, index, throwOnError)) {
      return false;
    }
    break;
  case IMMEDIATEVALUE:
    if(!throwOnError) return false;
    throwInvalidOperandType(op,index);
  default            :
    throwUnknownOperandType(__TFUNCTION__,op.getType());
  }
  return true;
}

bool OpcodeBase::validateShiftAmountOperand(const InstructionOperand &op, int index, bool throwOnError) const {
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
    throwUnknownOperandType(__TFUNCTION__,op.getType());
  }
  return true;
}

bool OpcodeBase::isValidOperand(const InstructionOperand &op, bool throwOnError) const {
  if(!validateOpCount(1, throwOnError)) {
    return false;
  }
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
    if(!validateImmediateValueAllowed(throwOnError)) {
      return false;
    }
    break;
  default             :
    throwUnknownOperandType(__TFUNCTION__,op.getType());
  }
  return true;
}

bool OpcodeBase::isValidOperandCombination(const Register &reg, const InstructionOperand &op, bool throwOnError) const {
  if(!validateOpCount(2, throwOnError)) {
    return false;
  }
  if(!validateRegisterAllowed(reg, throwOnError)) {
    return false;
  }
  switch(op.getType()) {
  case REGISTER       : // reg <- reg
    { if(!validateRegisterAllowed(op.getRegister(), throwOnError)) {
        return false;
      }
      if(!validateSameSize(reg,op, throwOnError)) {
        return false;
      }
    }
    break;
  case MEMORYOPERAND  : // reg <- mem
    if(!validateMemoryOperandAllowed((MemoryOperand&)op, throwOnError)) {
      return false;
    }
    if(!validateSameSize(reg,op, throwOnError)) {
      return false;
    }
    break;
  case IMMEDIATEVALUE : // reg <- imm
    if(!validateImmediateValue(reg.getSize(), op, s_validImmSizeToReg, throwOnError)) {
      return false;
    }
    break;
  default             :
    throwUnknownOperandType(__TFUNCTION__,op.getType());
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
      if(!validateImmediateValue(op1.getSize(), op2, s_validImmSizeToMem, throwOnError)) {
        return false;
      }
      break;
    default              :
      throwUnknownOperandType(__TFUNCTION__,op2.getType());
    }
    break;
  case IMMEDIATEVALUE : // imm <- reg/mem:ERROR
    RAISEERROR(_T("%s:Invalid combination of operands:%s,%s"), getMnemonic().cstr(), op1.toString().cstr(), op2.toString().cstr());
    break;
  default              :
    throwUnknownOperandType(__TFUNCTION__,op1.getType());
  }
  return true;
}

bool OpcodeBase::isValidOperandCombination(const InstructionOperand &op1, const InstructionOperand &op2, const InstructionOperand &op3, bool throwOnError) const {
  throwUnsupportedOperationException(__TFUNCTION__);
  return false;
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
