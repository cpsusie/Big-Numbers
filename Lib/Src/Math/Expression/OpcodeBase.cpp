#include "pch.h"
#include <Math/Expression/NewOpCode.h>
#include "RexByte.h"

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

OpcodeBase::OpcodeBase(UINT op, BYTE extension, BYTE opCount, UINT flags)
  : m_size(     findSize(op))
  , m_extension(extension   )
  , m_opCount(  opCount     )
  , m_flags(    flags       )
{
  assert(extension <= 7);
  m_bytes = swapBytes(op,m_size);
}

bool OpcodeBase::isRegisterTypeAllowed(RegType type) const {
  switch(type) {
  case REGTYPE_GPR : return (getFlags() & REGTYPE_GPR_ALLOWED) != 0;
  case REGTYPE_FPU : return (getFlags() & REGTYPE_FPU_ALLOWED) != 0;
  case REGTYPE_XMM : return (getFlags() & REGTYPE_XMM_ALLOWED) != 0;
  }
  return false;
}

bool OpcodeBase::isRegisterSizeAllowed(RegSize size) const {
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

bool OpcodeBase::isMemoryOperandSizeAllowed(OperandSize size) const {
  switch(size) {
  case REGSIZE_BYTE  : return (getFlags() & BYTEPTR_ALLOWED ) != 0;
  case REGSIZE_WORD  : return (getFlags() & WORDPTR_ALLOWED ) != 0;
  case REGSIZE_DWORD : return (getFlags() & DWORDPTR_ALLOWED) != 0;
  case REGSIZE_QWORD : return (getFlags() & QWORDPTR_ALLOWED) != 0;
  case REGSIZE_TBYTE : return (getFlags() & TBYTEPTR_ALLOWED) != 0;
  case REGSIZE_OWORD : return (getFlags() & OWORDPTR_ALLOWED) != 0;
  }
  return false;
}

void OpcodeBase::validateOpCount(int count) const {
  if(getOpCount() != count) {
    throwInvalidArgumentException(__TFUNCTION__, _T("%d operand(s) specified. Expected %d"), count, getOpCount());
  }
}

void OpcodeBase::validateRegisterAllowed(const Register &reg) const {
  if(!isRegisterAllowed(reg)) {
    throwInvalidArgumentException(__TFUNCTION__, _T("%s not allowed for this opcode"), reg.getName().cstr());
  }
}

void OpcodeBase::validateMemoryOperandAllowed(const MemoryOperand &memop) const {
  if(!isMemoryOperandAllowed(memop)) {
    throwInvalidArgumentException(__TFUNCTION__, _T("%s not allowed for this opcode"), memop.toString().cstr());
  }
}

void OpcodeBase::validateImmediateValueAllowed() const {
  if(!isImmediateValueAllowed()) {
    throwInvalidArgumentException(__TFUNCTION__, _T("Immediate value not allowed this opcode"));
  }
}

void OpcodeBase::validateSameSize(const InstructionOperand &op1, const InstructionOperand &op2) const {
  if(op1.getSize() != op2.getSize()) {
    throwInvalidArgumentException(__TFUNCTION__, _T("Different size:%s,%s"), op1.toString().cstr(), op2.toString().cstr());
  }

#ifdef IS64BIT
  if(op1.getType() == REGISTER) {
    validateIsRexCompatible(op1.getRegister(),op2);
  }
  if(op2.getType() == REGISTER) {
    validateIsRexCompatible(op2.getRegister(),op1);
  }
#endif // IS64BIT
}

#ifdef IS64BIT
void OpcodeBase::validateIsRexCompatible(const Register &reg, const InstructionOperand &op) const {
  if(!reg.isREXCompatible(op.needREXByte())) {
    throwInvalidArgumentException(__TFUNCTION__
                                 ,_T("%s not allowed together with %s (Use %s)")
                                 ,reg.toString().cstr()
                                 ,op.toString().cstr()
                                 ,Register::getREXCompatibleRegisterNames(op.needREXByte())
                                 );
  }
}
#endif // IS64BIT

