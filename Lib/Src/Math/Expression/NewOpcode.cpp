#include "pch.h"
#include <Math/Expression/NewOpCode.h>

// ---------------------------------- IntelOpcode ----------------------------

#ifdef IS32BIT
#define SETREXBITS(          bits      )
#define SETREXBITONHIGHREG(  reg,bit   )
#define SETREXBITSONHIGHREG2(reg,addReg)
#else // IS64BIT
#define SETREXBITS(          bits      ) { if(bits) setRexBits(bits); }
#define SETREXBITONHIGHREG(  reg,bit   )                                          \
{ if((reg.getIndex()) > 7) setRexBits(1<<(bit));                                  \
}
#define SETREXBITSONHIGHREG2(reg,addReg)                                          \
{ const BYTE _rexBits = (((reg.getIndex())>>3)&1) | (((addReg.getIndex())>>2)&2); \
  SETREXBITS(_rexBits)                                                            \
}
#endif // IS64BIT

static char findShift(BYTE a) {
  static const char shift[] = { -1, 0, 1, -1, 2, -1, -1, -1, 3 };
  if((a >= ARRAYSIZE(shift)) || (shift[a] < 0)) {
    throwInvalidArgumentException(__TFUNCTION__, _T("a=%d. must be 1,2,4,8"));
  }
  return shift[a];
}

String IndexRegisterCombination::toString() const {
  String result;
  if(m_reg   ) result = m_reg->getName();
  if(m_addreg) {
    if(m_reg) result += _T("+");
    if(hasShift()) result += format(_T("%d*"),1<<getShift());
    result += m_addreg->getName();
  }
  if(m_offset) {
    result += format(_T("%+d"), m_offset);
  }
  return result;
}

IndexRegisterCombination operator+(const IndexRegister &reg, int offset) {
  return IndexRegisterCombination(&reg,NULL,-1,offset);
}

IndexRegisterCombination operator-(const IndexRegister &reg, int offset) {
  return IndexRegisterCombination(&reg,NULL,-1,-offset);
}

IndexRegisterCombination operator+(const IndexRegisterCombination &irc, int offset) {
  if(irc.getOffset() != 0) {
    throwInvalidArgumentException(__TFUNCTION__,_T("Illegal index:%s+%d"),irc.toString().cstr(),offset);
  }
  return IndexRegisterCombination(irc.getReg(),irc.getAddreg(),irc.getShift(),offset);
}

IndexRegisterCombination operator-(const IndexRegisterCombination &irc, int offset) {
  if(irc.getOffset() != 0) {
    throwInvalidArgumentException(__TFUNCTION__,_T("Illegal index:%s-%d"),irc.toString().cstr(),offset);
  }
  return IndexRegisterCombination(irc.getReg(),irc.getAddreg(),irc.getShift(),-offset);
}

IndexRegisterCombination operator+(const IndexRegister &reg, const IndexRegisterCombination &irc) {
  if((irc.getReg() != NULL) || (irc.getAddreg() == NULL) || (irc.getOffset() != 0)) {
    throwInvalidArgumentException(__TFUNCTION__,_T("Illegal index:%s+%s"),reg.getName().cstr(),irc.toString().cstr());
  }
  return IndexRegisterCombination(&reg,irc.getAddreg(),irc.getShift());
}

IndexRegisterCombination operator+(const IndexRegister &reg, const IndexRegister &addreg) {
  return IndexRegisterCombination(&reg,&addreg,0);
}

IndexRegisterCombination operator*(BYTE a, const IndexRegister &reg) {
  if((reg.getIndex()&7) == 4) {
    throwInvalidArgumentException(__TFUNCTION__,_T("Invalid indexregister:%s"), reg.getName().cstr());
  }
  return IndexRegisterCombination(NULL,&reg,findShift(a));
}

OperandSize ImmediateOperand::findMinSize(int    v) {
  if(isByte(v)) return REGSIZE_BYTE;
  if(isWord(v)) return REGSIZE_WORD;
  return REGSIZE_DWORD;
}
OperandSize ImmediateOperand::findMinSize(UINT   v) {
  if(isByte(v)) return REGSIZE_BYTE;
  if(isWord(v)) return REGSIZE_WORD;
  return REGSIZE_DWORD;
}
OperandSize ImmediateOperand::findMinSize(INT64  v) {
  if(isByte( v)) return REGSIZE_BYTE;
  if(isWord( v)) return REGSIZE_WORD;
  if(isDword(v)) return REGSIZE_DWORD;
  return REGSIZE_QWORD;
}
OperandSize ImmediateOperand::findMinSize(UINT64 v) {
  if(isByte( v)) return REGSIZE_BYTE;
  if(isWord( v)) return REGSIZE_WORD;
  if(isDword(v)) return REGSIZE_DWORD;
  return REGSIZE_QWORD;
}

void ImmediateOperand::setValue(int    v) {
  switch(getSize()) {
  case REGSIZE_BYTE : m_v8  = (BYTE )v; break;
  case REGSIZE_WORD : m_v16 = (WORD )v; break;
  case REGSIZE_DWORD: m_v32 = (DWORD)v; break;
  default           : throwInvalidSize(__TFUNCTION__);
  }
}

void ImmediateOperand::setValue(UINT   v) {
  switch(getSize()) {
  case REGSIZE_BYTE : m_v8  = (BYTE )v; break;
  case REGSIZE_WORD : m_v16 = (WORD )v; break;
  case REGSIZE_DWORD: m_v32 = (DWORD)v; break;
  default           : throwInvalidSize(__TFUNCTION__);
  }
}

void ImmediateOperand::setValue(INT64  v) {
  switch(getSize()) {
  case REGSIZE_BYTE : m_v8  = (BYTE )v; break;
  case REGSIZE_WORD : m_v16 = (WORD )v; break;
  case REGSIZE_DWORD: m_v32 = (DWORD)v; break;
  case REGSIZE_QWORD: m_v64 = v       ; break;
  default           : throwInvalidSize(__TFUNCTION__);
  }
}

void ImmediateOperand::setValue(UINT64 v) {
  switch(getSize()) {
  case REGSIZE_BYTE : m_v8  = (BYTE )v; break;
  case REGSIZE_WORD : m_v16 = (WORD )v; break;
  case REGSIZE_DWORD: m_v32 = (DWORD)v; break;
  case REGSIZE_QWORD: m_v64 = v       ; break;
  default           : throwInvalidSize(__TFUNCTION__);
  }
}

String ImmediateOperand::toString() const {
  switch(getSize()) {
  case REGSIZE_BYTE : return format(_T("%#04x"   ),m_v8 );
  case REGSIZE_WORD : return format(_T("%#06x"   ),m_v16);
  case REGSIZE_DWORD: return format(_T("%#010x"  ),m_v32);
  case REGSIZE_QWORD: return format(_T("%#18I64x"),m_v64);
  default           : throwInvalidSize(__TFUNCTION__);
  }
  return EMPTYSTRING;
}

void ImmediateOperand::throwInvalidSize(const TCHAR *method) const {
  throwException(_T("%s:Invalid size for immediate value:%d"), method, getSize());
}

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
  if(!hasRegisterMode()) {
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

void OpcodeBase::validateOperandSize(RegSize size) const {
  if(!isOperandSizeAllowed(size)) {
    throwInvalidArgumentException(__TFUNCTION__, _T("Size %s not allowed for this opcode"), getOpSizeName(size));
  }
}

InstructionBase OpcodeBase::operator()(const InstructionOperand &op) const {
  DEFINEMETHODNAME;
  validateOpCount(1);
  InstructionBase result(getBytes(),size());
  switch(op.getType()) {
  case REGISTER       :
    validateRegisterAllowed(op.getRegister());
    return result.setRegister(op.getRegister());
  case MEMREFERENCE   :
    validateMemoryReferenceAllowed();
    validateOperandSize(op.getSize());
    return result.setMemoryReference(*op.getMemoryReference());
  case IMMEDIATEVALUE :
;
  }
  return result;
}

InstructionBase &InstructionBase::insertByte(BYTE index, BYTE b) {
  assert((m_size < MAX_INSTRUCTIONSIZE) && (index<m_size));
  BYTE *bp = m_bytes + index;
  for(BYTE *dst = m_bytes + m_size++, *src = dst-1; dst > bp;) {
    *(dst--) = *(src--);
  }
  *bp = b;
  return *this;
}

#ifdef IS64BIT
InstructionBase &InstructionBase::setRexBits(BYTE bits) {
  assert(((bits&0xf0)==0) && hasRexMode());
  if(m_hasRexByte) {
    m_bytes[m_rexByteIndex] |= bits; // just add new bits to rex-byte
    return *this;
  }
  m_hasRexByte = true;
  return insertByte(m_rexByteIndex, 0x40|bits);
}
#endif

InstructionBase &InstructionBase::add(INT64 bytesToAdd, BYTE count) {
  assert(m_size+count <= MAX_INSTRUCTIONSIZE);
  if(count == 1) {
    m_bytes[m_size++] = (BYTE)bytesToAdd;
  } else {
    for(BYTE *dst = m_bytes+m_size, *src = (BYTE*)&bytesToAdd, *end = src+count; src<end;) {
      *(dst++) = *(src++);
    }
    m_size += count;
  }
  return *this;
}

InstructionBase &InstructionBase::memAddrEsp(int offset) {
  if(offset == 0) return memAddrEsp0();
  if(ImmediateOperand::isByte(offset)) return memAddrEsp1((char)offset);
  return memAddrEsp4(offset);
}

InstructionBase &InstructionBase::memAddrPtr0(const IndexRegister &reg) {
  const BYTE regIndex = reg.getIndex();
  assert(((regIndex&7)!=4) && ((regIndex&7)!=5));
  SETREXBITONHIGHREG(reg,0);
  return add(regIndex&7);
}

// ptr[reg+offset], (reg&7) != 4, offset=[-128;127]
InstructionBase &InstructionBase::memAddrPtr1(const IndexRegister &reg, char offset) {
  const BYTE regIndex = reg.getIndex();
  assert((regIndex&7)!=4);
  SETREXBITONHIGHREG(reg,0);
  return add(0x40 | (regIndex&7)).add(offset);
}

// ptr[reg+offset], (reg&7) != 4, offset=[INT_MIN;INT_MAX]
InstructionBase &InstructionBase::memAddrPtr4(const IndexRegister &reg, int offset) {
  const BYTE regIndex = reg.getIndex();
  assert((regIndex&7)!=4);
  SETREXBITONHIGHREG(reg,0);
  return add(0x80 | (regIndex&7)).add(offset, 4);
}

// ptr[reg+(addReg<<p2)], (reg&7) != 5, (addReg&7) != 4, p2<=3
InstructionBase &InstructionBase::memAddrMp2AddReg0(const IndexRegister &reg, BYTE p2, const IndexRegister &addReg) {
  const BYTE regIndex = reg.getIndex(), addRegIndex = addReg.getIndex();
  assert(((regIndex&7)!=5) && ((addRegIndex&7)!=4) && (p2<=3));
  SETREXBITSONHIGHREG2(reg,addReg);
  return add(0x04).add((p2 << 6) | ((addRegIndex&7) << 3) | (regIndex&7));
}

// ptr[reg+(addReg<<p2)+offset], (addReg&7) != 4, p2<=3, offset=[-128;127]
InstructionBase &InstructionBase::memAddrMp2AddReg1(const IndexRegister &reg, BYTE p2, const IndexRegister &addReg, char offset) {
  const BYTE regIndex = reg.getIndex(), addRegIndex = addReg.getIndex();
  assert(((addRegIndex&7)!=4) && (p2<=3));
  SETREXBITSONHIGHREG2(reg,addReg);
  return add(0x44).add((p2 << 6) | ((addRegIndex&7) << 3) | (regIndex&7)).add(offset);
}

// ptr[reg+(addReg<<p2)+offset], (addReg&7) != 4, p2<=3, offset=[INT_MIN;INT_MAX]
InstructionBase &InstructionBase::memAddrMp2AddReg4(const IndexRegister &reg, BYTE p2, const IndexRegister &addReg, int offset) {
  const BYTE regIndex = reg.getIndex(), addRegIndex = addReg.getIndex();
  assert(((addRegIndex&7)!=4) && (p2<=3));
  SETREXBITSONHIGHREG2(reg,addReg);
  return add(0x84).add((p2 << 6) | ((addRegIndex&7) << 3) | (regIndex&7)).add(offset, 4);
}

  // ptr[(reg<<p2)+offset], (reg&7) != 4, p2<=3, offset=[INT_MIN;INT_MAX]
InstructionBase &InstructionBase::memAddrMp2Ptr4(const IndexRegister &reg, BYTE p2, int offset) {
  const BYTE regIndex = reg.getIndex();
  assert(((regIndex&7)!=4) && (p2<=3));
  SETREXBITONHIGHREG(reg,1);
  return add(0x04).add(0x05 | (p2 << 6) | ((regIndex&7) << 3)).add(offset, 4);
}

// ------------------------------------------------------------------------------------------
InstructionBase &InstructionBase::memAddrPtr(const IndexRegister &reg, int offset) {
  const BYTE regIndex = reg.getIndex();
  switch(regIndex&7) {
  case 4 :
    return memAddrEsp(offset);
  default:
    if(offset == 0 && ((regIndex&7)!=5)) return memAddrPtr0(reg);
    if(ImmediateOperand::isByte(offset)) return memAddrPtr1(reg,(char)offset);
    return memAddrPtr4(reg,offset);
  }
}

InstructionBase &InstructionBase::memAddrMp2AddReg(const IndexRegister &reg, const IndexRegister &addReg, BYTE p2, int offset) {
  DEFINEMETHODNAME;
  const BYTE regIndex = reg.getIndex(), addRegIndex = addReg.getIndex();
  if((addRegIndex&7)==4) {
    throwInvalidArgumentException(method, _T("Invalid index register:%s"), reg.getName().cstr());
  }
  if(p2 > 3) {
    throwInvalidArgumentException(method, _T("p2=%d. Valid range=[0;3]"),p2);
  }
  if((offset == 0) && ((regIndex&7) != 5)) return memAddrMp2AddReg0(reg,p2,addReg);
  if(ImmediateOperand::isByte(offset))     return memAddrMp2AddReg1(reg,p2,addReg,(char)offset);
  return memAddrMp2AddReg4(reg,p2,addReg,offset);
}

InstructionBase &InstructionBase::setRegister(const Register &reg) {
  SETREXBITONHIGHREG(reg,0);
  return add(0xc0 | (reg.getIndex()&7));
}

InstructionBase &InstructionBase::setMemoryReference(const IndexRegisterCombination &irc) {
  if(irc.getAddreg()) {
    memAddrMp2AddReg(*irc.getReg(), *irc.getAddreg(), irc.getShift(), irc.getOffset());
  } else if(irc.hasShift()) {
    memAddrMp2Ptr4(*irc.getAddreg(), irc.getShift(), irc.getOffset());
  } else {
    memAddrPtr(*irc.getReg(), irc.getOffset());
  }
  return *this;
}
