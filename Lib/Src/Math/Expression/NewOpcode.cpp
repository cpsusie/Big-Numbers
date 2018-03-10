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

String MemoryRef::toString() const {
  String result;
  if(m_reg   ) result = m_reg->getName();
  if(m_addreg) {
    if(result.length() > 0) result += _T("+");
    if(hasShift()) result += format(_T("%d*"),1<<getShift());
    result += m_addreg->getName();
  }
  if(m_offset) {
    if(result.length() > 0) {
      result += format(_T("%+d"), m_offset);
    } else {
      result = format(_T("%08xh"), m_offset);
      if(!iswdigit(result[0])) {
        result.insert(0,'0');
      }
    }
  }
  return result;
}

MemoryRef operator+(const IndexRegister &reg, int offset) {
  return MemoryRef(&reg,NULL,0,offset);
}

MemoryRef operator-(const IndexRegister &reg, int offset) {
  return MemoryRef(&reg,NULL,0,-offset);
}

MemoryRef operator+(const MemoryRef &mr, int offset) {
  if(mr.getOffset() != 0) {
    throwInvalidArgumentException(__TFUNCTION__,_T("Illegal index:%s+%d"),mr.toString().cstr(),offset);
  }
  return MemoryRef(mr.getReg(),mr.getAddreg(),mr.getShift(),offset);
}

MemoryRef operator-(const MemoryRef &mr, int offset) {
  if(mr.getOffset() != 0) {
    throwInvalidArgumentException(__TFUNCTION__,_T("Illegal index:%s-%d"),mr.toString().cstr(),offset);
  }
  return MemoryRef(mr.getReg(),mr.getAddreg(),mr.getShift(),-offset);
}

MemoryRef operator+(const IndexRegister &reg, const MemoryRef &mr) {
  if((mr.getReg() != NULL) || (mr.getAddreg() == NULL) || (mr.getOffset() != 0)) {
    throwInvalidArgumentException(__TFUNCTION__,_T("Illegal index:%s+%s"),reg.getName().cstr(),mr.toString().cstr());
  }
  return MemoryRef(&reg,mr.getAddreg(),mr.getShift());
}

MemoryRef operator+(const IndexRegister &reg, const IndexRegister &addreg) {
  return MemoryRef(&reg,&addreg,0);
}

MemoryRef operator*(BYTE a, const IndexRegister &reg) {
  if((reg.getIndex()&7) == 4) {
    throwInvalidArgumentException(__TFUNCTION__,_T("Invalid indexregister:%s"), reg.getName().cstr());
  }
  return MemoryRef(NULL,&reg,findShift(a));
}

String MemoryOperand::toString() const {
  return (m_segReg)
        ? format(_T("%s ptr %s:[%s]"), getOpSizeName(getSize()), m_segReg->getName().cstr(), m_mr.toString().cstr())
        : format(_T("%s ptr[%s]"), getOpSizeName(getSize()), m_mr.toString().cstr());
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
    return result.setMemoryReference((MemoryOperand&)op);
  case IMMEDIATEVALUE :
    validateImmediateValueAllowed();
    break;
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
  assert((bits&0xf0)==0);
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

InstructionBase &InstructionBase::addrStack(int offset) {
  if(offset == 0) return addrStack0();
  if(ImmediateOperand::isByte(offset)) return addrStack1((char)offset);
  return addrStack4(offset);
}

InstructionBase &InstructionBase::addrPtr0(const IndexRegister &reg) {
  const BYTE regIndex = reg.getIndex();
  assert(((regIndex&7)!=4) && ((regIndex&7)!=5));
  SETREXBITONHIGHREG(reg,0);
  return add(regIndex&7);
}

// ptr[reg+offset], (reg&7) != 4, offset=[-128;127]
InstructionBase &InstructionBase::addrPtr1(const IndexRegister &reg, char offset) {
  const BYTE regIndex = reg.getIndex();
  assert((regIndex&7)!=4);
  SETREXBITONHIGHREG(reg,0);
  return add(0x40 | (regIndex&7)).add(offset);
}

// ptr[reg+offset], (reg&7) != 4, offset=[INT_MIN;INT_MAX]
InstructionBase &InstructionBase::addrPtr4(const IndexRegister &reg, int offset) {
  const BYTE regIndex = reg.getIndex();
  assert((regIndex&7)!=4);
  SETREXBITONHIGHREG(reg,0);
  return add(0x80 | (regIndex&7)).add(offset, 4);
}

// ptr[reg+(addReg<<shift)], (reg&7) != 5, (addReg&7) != 4, shift<=3
InstructionBase &InstructionBase::addrShiftAddReg0(const IndexRegister &reg, BYTE shift, const IndexRegister &addReg) {
  const BYTE regIndex = reg.getIndex(), addRegIndex = addReg.getIndex();
  assert(((regIndex&7)!=5) && ((addRegIndex&7)!=4) && (shift<=3));
  SETREXBITSONHIGHREG2(reg,addReg);
  return add(0x04).add((shift << 6) | ((addRegIndex&7) << 3) | (regIndex&7));
}

// ptr[reg+(addReg<<shift)+offset], (addReg&7) != 4, shift<=3, offset=[-128;127]
InstructionBase &InstructionBase::addrShiftAddReg1(const IndexRegister &reg, BYTE shift, const IndexRegister &addReg, char offset) {
  const BYTE regIndex = reg.getIndex(), addRegIndex = addReg.getIndex();
  assert(((addRegIndex&7)!=4) && (shift<=3));
  SETREXBITSONHIGHREG2(reg,addReg);
  return add(0x44).add((shift << 6) | ((addRegIndex&7) << 3) | (regIndex&7)).add(offset);
}

// ptr[reg+(addReg<<shift)+offset], (addReg&7) != 4, shift<=3, offset=[INT_MIN;INT_MAX]
InstructionBase &InstructionBase::addrShiftAddReg4(const IndexRegister &reg, BYTE shift, const IndexRegister &addReg, int offset) {
  const BYTE regIndex = reg.getIndex(), addRegIndex = addReg.getIndex();
  assert(((addRegIndex&7)!=4) && (shift<=3));
  SETREXBITSONHIGHREG2(reg,addReg);
  return add(0x84).add((shift << 6) | ((addRegIndex&7) << 3) | (regIndex&7)).add(offset, 4);
}

  // ptr[(reg<<shift)+offset], (reg&7) != 4, shift<=3, offset=[INT_MIN;INT_MAX]
InstructionBase &InstructionBase::addrShiftPtr4(const IndexRegister &reg, BYTE shift, int offset) {
  const BYTE regIndex = reg.getIndex();
  assert(((regIndex&7)!=4) && (shift<=3));
  SETREXBITONHIGHREG(reg,1);
  return add(0x04).add(0x05 | (shift << 6) | ((regIndex&7) << 3)).add(offset, 4);
}

// ------------------------------------------------------------------------------------------
InstructionBase &InstructionBase::addrPtr(const IndexRegister &reg, int offset) {
  const BYTE regIndex = reg.getIndex();
  switch(regIndex&7) {
  case 4 :
    return addrStack(offset);
  default:
    if((offset == 0) && ((regIndex&7)!=5)) return addrPtr0(reg);
    if(ImmediateOperand::isByte(offset)) return addrPtr1(reg,(char)offset);
    return addrPtr4(reg,offset);
  }
}

InstructionBase &InstructionBase::addrShiftAddReg(const IndexRegister &reg, const IndexRegister &addReg, BYTE shift, int offset) {
  DEFINEMETHODNAME;
  const BYTE regIndex = reg.getIndex(), addRegIndex = addReg.getIndex();
  if((addRegIndex&7)==4) {
    throwInvalidArgumentException(method, _T("Invalid index register:%s"), reg.getName().cstr());
  }
  if(shift > 3) {
    throwInvalidArgumentException(method, _T("shift=%d. Valid range=[0;3]"),shift);
  }
  if((offset == 0) && ((regIndex&7) != 5)) return addrShiftAddReg0(reg,shift,addReg);
  if(ImmediateOperand::isByte(offset))     return addrShiftAddReg1(reg,shift,addReg,(char)offset);
  return addrShiftAddReg4(reg,shift,addReg,offset);
}

InstructionBase &InstructionBase::prefixSegReg(const SegmentRegister &reg) {
  static const BYTE segRegPrefix[] = { 0x26,0x2e,0x36,0x3e,0x64,0x65 }; // es,cs,ss,ds,fs,gs
  assert(reg.getIndex() < ARRAYSIZE(segRegPrefix));
  prefix(segRegPrefix[reg.getIndex()]);
  return *this;
}

InstructionBase &InstructionBase::setRegister(const Register &reg) {
  SETREXBITONHIGHREG(reg,0);
  return add(0xc0 | (reg.getIndex()&7));
}

InstructionBase &InstructionBase::setMemoryReference(const MemoryOperand &mop) {
  const MemoryRef &mr = *mop.getMemoryReference();
  if(mop.hasSegmentRegister()) {
    prefixSegReg(mop.getSegmentRegister());
  }
  if(mr.isImmediateAddr()) {
    addrImmDword(mr.getOffset());
  } else if(mr.getAddreg()) {
    addrShiftAddReg(*mr.getReg(), *mr.getAddreg(), mr.getShift(), mr.getOffset());
  } else if(mr.hasShift()) {
    addrShiftPtr4(*mr.getAddreg(), mr.getShift(), mr.getOffset());
  } else {
    addrPtr(*mr.getReg(), mr.getOffset());
  }
  return *this;
}
