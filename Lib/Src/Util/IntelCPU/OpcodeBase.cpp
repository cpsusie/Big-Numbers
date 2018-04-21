#include "pch.h"
#include <NewOpCode.h>
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

#define THROWINVALIDFLAGS(form,...)                                             \
  throwInvalidArgumentException(method                                          \
                               ,format(_T("%s:%s")                              \
                                      ,getMnemonic().cstr()                     \
                                      ,format(form,__VA_ARGS__).cstr()).cstr())

OpcodeBase::OpcodeBase(const String &mnemonic, UINT op, BYTE extension, BYTE opCount, UINT flags)
  : m_mnemonic(  toLowerCase(mnemonic))
  , m_size(      findSize(op)         )
  , m_extension( extension            )
  , m_opCount(   opCount              )
  , m_flags(     flags                )
{
  DEFINEMETHODNAME;
  if(extension > 7) {
    THROWINVALIDFLAGS(_T("Extension=%d. Max is 7"), extension);
  }
  m_bytes = swapBytes(op,m_size);
  if(getFlags() & HAS_BYTE_SIZEBIT) {
    if(op & 1) {
      THROWINVALIDFLAGS(_T("HAS_BYTE_SIZEBIT set for opcode %X (bit 0 already set)"),op);
    }
  }
  if(getFlags() & HAS_IMM_XBIT) {
    if(op & 2) {
      THROWINVALIDFLAGS(_T("HAS_IMM_XBIT set for opcode %X (bit 1 already set)"),op);
    }
    if(getFlags() & (HAS_DIRBIT0|HAS_DIRBIT1)) {
      THROWINVALIDFLAGS(_T("Cannot have both HAS_IMM_XBIT and DIRBIT0/1 set for opcode %X"),op);
    }
  }
  if(getFlags() & HAS_WORDPREFIX) {
    if(!(getFlags() & (WORDPTR_ALLOWED | WORDGPR_ALLOWED | IMM16_ALLOWED))) {
      THROWINVALIDFLAGS(_T("HAS_WORDPREFIX set for opcode %X, but word size operands not allowed"),op);
    }
  }

  if (getFlags() & LASTOP_IMMONLY) {
    if((opCount == 2) && (getFlags() & OP2_REGONLY)) {
      THROWINVALIDFLAGS(_T("Cannot have both LASTOP_IMMONLY and OP2_REGONLY for 2-argument opcode %X"),op);
    }
  }
#ifdef IS64BIT
  if(getFlags() & HAS_REXWBIT) {
    if(!(getFlags() & (QWORDPTR_ALLOWED | QWORDGPR_ALLOWED))) {
      THROWINVALIDFLAGS(_T("HAS_REXWBIT set for opcode %X, but qword size operands not allowed"),op);
    }
  }
#endif // IS64BIT

  if(getFlags() & HAS_DIRBIT1) {
    if(op & 2) {
      THROWINVALIDFLAGS(_T("HAS_DIRBIT1 set for opcode %X (bit 1 already set)"),op);
    }
    if(getFlags() & OP1_REGONLY) {
      THROWINVALIDFLAGS(_T("HAS_DIRBIT1 and OP1_REGONLY cannot both be set"));
    }
    if(getFlags() & OP2_REGONLY) {
      THROWINVALIDFLAGS(_T("HAS_DIRBIT1 and OP2_REGONLY cannot both be set"));
    }
  }

  if(getFlags() & HAS_DIRBIT0) {
    if(op & 1) {
      THROWINVALIDFLAGS(_T("HAS_DIRBIT0 set for opcode %X (bit 0 already set)"),op);
    }
    if(getFlags() & HAS_DIRBIT1) {
      THROWINVALIDFLAGS(_T("Cannot have both HAS_DIRBIT0 and HAS_DIRBIT1 set for opcode %X"),op);
    }
    if(getFlags() & OP1_REGONLY) {
      THROWINVALIDFLAGS(_T("Cannot have both HAS_DIRBIT0 and OP1_REGONLY set"));
    }
    if(getFlags() & HAS_BYTE_SIZEBIT) {
      THROWINVALIDFLAGS(_T("HAS_DIRBIT0 and HAS_BYTE_SIZEBIT cannot both be set for opcode %X"),op);
    }
  }

#define ALL_REGTYPES (REGTYPE_GPR0_ALLOWED|REGTYPE_GPR_ALLOWED|REGTYPE_SEG_ALLOWED|REGTYPE_FPU_ALLOWED|REGTYPE_XMM_ALLOWED)

  if((getFlags() & OP1_REGONLY) && ((getFlags() & ALL_REGTYPES) == 0)) {
    THROWINVALIDFLAGS(_T("OP1_REGONLY is set, but no registertypes allowed"));
  }
  if((getFlags() & LASTOP_IMMONLY) && ((getFlags() & (IMMEDIATEVALUE_ALLOWED | IMM64_ALLOWED)) == 0)) {
    THROWINVALIDFLAGS(_T("LASTOP_IMMONLY is set, but no immediate operands allowed"));
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
    if(!isGPRegisterSizeAllowed(reg.getSize())) {
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

bool OpcodeBase::isGPRegisterSizeAllowed(RegSize size) const {
  switch(size) {
  case REGSIZE_BYTE : return (getFlags() & BYTEGPR_ALLOWED   ) != 0;
  case REGSIZE_WORD : return (getFlags() & WORDGPR_ALLOWED   ) != 0;
  case REGSIZE_DWORD: return (getFlags() & DWORDGPR_ALLOWED  ) != 0;
  case REGSIZE_QWORD: return (getFlags() & QWORDGPR_ALLOWED  ) != 0;
  default           : throwInvalidArgumentException(__TFUNCTION__
                                                   ,_T("size=%s. (not GP-register size)")
                                                   ,::toString(size).cstr()
                                                   );
  }
  return false;
}

bool OpcodeBase::isMemoryOperandSizeAllowed(OperandSize size) const {
  switch(size) {
  case REGSIZE_BYTE   : return (getFlags() & BYTEPTR_ALLOWED   ) != 0;
  case REGSIZE_WORD   : return (getFlags() & WORDPTR_ALLOWED   ) != 0;
  case REGSIZE_DWORD  : return (getFlags() & DWORDPTR_ALLOWED  ) != 0;
  case REGSIZE_QWORD  : return (getFlags() & QWORDPTR_ALLOWED  ) != 0;
  case REGSIZE_TBYTE  : return (getFlags() & TBYTEPTR_ALLOWED  ) != 0;
  case REGSIZE_MMWORD : return (getFlags() & MMWORDPTR_ALLOWED ) != 0;
  case REGSIZE_XMMWORD: return (getFlags() & XMMWORDPTR_ALLOWED) != 0;
  case REGSIZE_VOID   : return (getFlags() & VOIDPTR_ALLOWED   ) != 0;
  default             : throwInvalidArgumentException(__TFUNCTION__,_T("size=%d"), size);
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

bool OpcodeBase::throwInvalidOperandCombination(const InstructionOperand &op1, const InstructionOperand &op2, bool throwOnError) const {
  RAISEERROR(_T("Invalid combination of operands:%s,%s")
                ,op1.toString().cstr()
                ,op2.toString().cstr()
                );
}

bool OpcodeBase::throwInvalidOperandCombination(const InstructionOperand &op1, const InstructionOperand &op2, const InstructionOperand &op3, bool throwOnError) const {
  RAISEERROR(_T("Invalid combination of operands:%s,%s,%s")
                ,op1.toString().cstr()
                ,op2.toString().cstr()
                ,op3.toString().cstr()
                );
}

bool OpcodeBase::throwInvalidOperandType(const InstructionOperand &op, BYTE index, bool throwOnError) const {
  RAISEERROR(_T("%s not a valid %d. operand")
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

#ifdef IS32BIT
#define ERRORSTRING_1OPGPR0 _T("Operand 1 must be AL/AX/EAX. op1=%s")
#else // IS64BIT
#define ERRORSTRING_1OPGPR0 _T("Operand 1 must be AL/AX/EAX/RAX. op1=%s")
#endif // IS64BIT

#define CHECKOP1_GPR0ONLY(op)                                                                   \
{ if((getFlags() & OP1_GPR0ONLY) && !op.isGPR0()) {                                             \
    RAISEERROR(ERRORSTRING_1OPGPR0,op.toString().cstr());                                       \
  }                                                                                             \
}

#define CHECKOP1_REGONLY(op)                                                                    \
{ if(getFlags() & (OP1_GPR0ONLY|OP1_REGONLY)) {                                                 \
    CHECKOP1_GPR0ONLY(op);                                                                      \
    if((getFlags() & OP1_REGONLY) && !op.isRegister()) {                                        \
      RAISEERROR(_T("Operand 1 must be register. op1=%s")                                       \
                ,op.toString().cstr());                                                         \
    }                                                                                           \
  }                                                                                             \
}

#define CHECKOP2_REGONLY(op)                                                                    \
{ if((getFlags() & OP2_REGONLY) && !op.isRegister()) {                                          \
    RAISEERROR(_T("Operand 2 must be register. op2=%s")                                         \
                ,op.toString().cstr());                                                         \
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

bool OpcodeBase::validateImmediateValue(const InstructionOperand &dst, const InstructionOperand &imm, bool throwOnError) const {
  switch(dst.getType()) {
  case REGISTER     : return validateImmediateValue(dst.getRegister()  , imm, throwOnError);
  case MEMORYOPERAND: return validateImmediateValue((MemoryOperand&)dst, imm, throwOnError);
  default           : RAISEERROR(_T("%s is not a valid destination for immediate value %s")
                                ,dst.toString().cstr()
                                ,imm.toString().cstr()
                                )
  }
  return false;
}

bool OpcodeBase::isCompatibleSize(OperandSize size1, OperandSize size2) const {
  return size1 == size2;
}

bool OpcodeBase::validateCompatibleSize(const Register &reg1, const Register &reg2, bool throwOnError) const {
  if(!isCompatibleSize(reg1.getSize(),reg2.getSize())) {
    RAISEERROR(_T("Sizes not compatible:%s,%s"), reg1.toString().cstr(), reg2.toString().cstr());
  }
  return validateIsRexCompatible(reg1,reg2,throwOnError);
}

bool OpcodeBase::validateCompatibleSize(const Register &reg, const InstructionOperand &op, bool throwOnError) const {
  if(op.getType() == REGISTER) {
    return validateCompatibleSize(reg, op.getRegister(), throwOnError);
  } else {
    if(!isCompatibleSize(reg.getSize(),op.getSize())) {
      RAISEERROR(_T("Sizes not compatible:%s,%s"), reg.toString().cstr(), op.toString().cstr());
    }
    if(!validateIsRexCompatible(reg,op, throwOnError)) {
      return false;
    }
  }
  return true;
}

bool OpcodeBase::validateCompatibleSize(const InstructionOperand &op1, const InstructionOperand &op2, bool throwOnError) const {
  if(op1.getType() == REGISTER) {
    if(!validateCompatibleSize(op1.getRegister(),op2, throwOnError)) {
      return false;
    }
  } else if(op2.getType() == REGISTER) {
    if(!validateCompatibleSize(op2.getRegister(),op1, throwOnError)) {
      return false;
    }
  } else {
    return throwInvalidOperandCombination(op1,op2,throwOnError);
  }
  return true;
}

#ifdef IS64BIT
bool OpcodeBase::validateIsRexCompatible(const Register &reg, const InstructionOperand &op, bool throwOnError) const {
  if(!reg.isREXCompatible(op.needREXByte())) {
    RAISEERROR(_T("%s not allowed together with %s (Use %s)")
              ,reg.toString().cstr()
              ,op.toString().cstr()
              ,Register::getREXCompatibleRegisterNames()
             );
  }
  return true;
}

bool OpcodeBase::validateIsRexCompatible1(const Register &reg1, const Register &reg2, bool throwOnError) const {
  if(!reg1.isREXCompatible(reg2.needREXByte())) {
    RAISEERROR(_T("%s not allowed together with %s (Use %s)")
              ,reg1.toString().cstr()
              ,reg2.toString().cstr()
              ,Register::getREXCompatibleRegisterNames()
              );
  }
  return true;
}

bool OpcodeBase::validateIsRexCompatible(const Register &reg1, const Register &reg2, bool throwOnError) const {
  return validateIsRexCompatible1(reg1,reg2,throwOnError) && validateIsRexCompatible1(reg2,reg1,throwOnError);
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
    return throwInvalidOperandType(op,index,throwOnError);
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
    return throwInvalidOperandType(op,index,throwOnError);
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
    return throwInvalidOperandType(op,index,throwOnError);
  default            :
    throwUnknownOperandType(op,index);
  }
  return true;
}

bool OpcodeBase::validateIsImmediateOperand(const InstructionOperand &op, BYTE index, bool throwOnError) const {
  switch(op.getType()) {
  case REGISTER      :
  case MEMORYOPERAND :
    return throwInvalidOperandType(op,index,throwOnError);
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
    return throwInvalidOperandType(op,index,throwOnError);
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
    CHECKOP1_REGONLY(op);
  } else if(index == 2) {
    CHECKOP2_REGONLY(op);
  }

  if(index == getMaxOpCount()) {
    CHECKLASTOP_IMMONLY(op);
  }
  switch(op.getType()) {
  case REGISTER      : return isRegisterAllowed(op.getRegister());
  case MEMORYOPERAND : return isMemoryOperandAllowed((MemoryOperand&)op);
  case IMMEDIATEVALUE: return isImmediateSizeAllowed(op.getSize());
  }
  return throwInvalidOperandType(op,index);
}

bool OpcodeBase::isValidOperand(const InstructionOperand &op, bool throwOnError) const {
  if(!validateOpCount(1, throwOnError)) {
    return false;
  }
  CHECKOP1_REGONLY(op);
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
  CHECKOP1_GPR0ONLY(      reg);
  CHECKOP2_REGONLY(       op );
  CHECKLASTOP_IMMONLY(    op );
  if(!validateRegisterAllowed(reg, throwOnError)) {
    return false;
  }
  switch(op.getType()) {
  case REGISTER       : // reg <- reg
    { if(!validateRegisterAllowed(op.getRegister(), throwOnError)) {
        return false;
      }
      if(!validateCompatibleSize(reg, op, throwOnError)) {
        return false;
      }
      if(reg.getType() == REGTYPE_FPU) {
        if(!reg.isST0() && !op.isST0()) {
          RAISEERROR(_T("st(0) must be one of the registers. (%s,%s)"),reg.toString().cstr(),op.toString().cstr());
        }
      }
    }
    break;
  case MEMORYOPERAND  : // reg <- mem
    if(!validateMemoryOperandAllowed((MemoryOperand&)op, throwOnError)) {
      return false;
    }
    if(!validateCompatibleSize(reg, op, throwOnError)) {
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
  CHECKOP1_REGONLY(       op1);
  CHECKOP2_REGONLY(       op2);
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
        if(!validateCompatibleSize(op1,op2, throwOnError)) {
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
  CHECKOP1_REGONLY(       op1);
  CHECKOP2_REGONLY(       op2);
  CHECKLASTOP_IMMONLY(    op3);
  switch(op1.getType()) {
  case REGISTER       :
    if(!validateRegisterAllowed(op1.getRegister(), throwOnError)) {
      return false;
    }
    if(!validateIsRegisterOrMemoryOperand(op2, 2, throwOnError)) {
      return false;
    }
    if(!validateCompatibleSize(op1,op2, throwOnError)) {
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
  throwUnsupportedOperationException(format(_T("%s %s"), m_mnemonic.cstr(), op.toString().cstr()).cstr());
  return InstructionBuilder(*this);
}

InstructionBase OpcodeBase::operator()(const InstructionOperand &op1, const InstructionOperand &op2) const {
  throwUnsupportedOperationException(format(_T("%s %s,%s")
                                           ,m_mnemonic.cstr()
                                           ,op1.toString().cstr()
                                           ,op2.toString().cstr()
                                           ).cstr()
                                     );
  return InstructionBuilder(*this);
}

InstructionBase OpcodeBase::operator()(const InstructionOperand &op1, const InstructionOperand &op2, const InstructionOperand &op3) const {
  throwUnsupportedOperationException(format(_T("%s %s,%s,%s")
                                           ,m_mnemonic.cstr()
                                           ,op1.toString().cstr()
                                           ,op2.toString().cstr()
                                           ,op3.toString().cstr()
                                           ).cstr()
                                     );
  return InstructionBuilder(*this);
}
