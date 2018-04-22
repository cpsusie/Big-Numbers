#include "pch.h"
#include <OpCode.h>

String toString(OperandType type) {
  switch(type) {
  case REGISTER      : return _T("REGISTER"      );
  case MEMORYOPERAND : return _T("MEMORYOPERAND" );
  case IMMEDIATEVALUE: return _T("IMMEDIATEVALUE");
  default            : return format(_T("Unknown operandType:%d"), type);
  }
}

// Convert int32-value to disassembler format
String formatHexValue(int v, bool showSign) {
  bool   neg;
  String result;
  if(v >= 0) {
    neg      = false;
    result   = formatHexValue((UINT)v);
  } else {
    neg      = true;
    showSign = true;
    result = formatHexValue((UINT)(-v));
  }
  if(showSign) {
    result.insert(0, neg ? '-' : '+');
  }
  return result;
}

// Convert int64-value to disassembler format
String formatHexValue(INT64 v, bool showSign) {
  bool   neg;
  String result;
  if(v >= 0) {
    neg      = false;
    result   = formatHexValue((UINT64)v);
  } else {
    neg      = true;
    showSign = true;
    result   = formatHexValue((UINT64)(-v));
  }
  if(showSign) {
    result.insert(0, neg ? '-' : '+');
  }
  return result;
}

String formatHexValue(UINT v) {
  String result = format(_T("%X"), v);
  if(!iswdigit(result[0])) {
    result.insert(0,'0');
  }
  if(v >= 10) result += _T('h');
  return result;
}

String formatHexValue(UINT64 v) {
  String result = format(_T("%I64X"), v);
  if(!iswdigit(result[0])) {
    result.insert(0,'0');
  }
  if(v >= 10) result += _T('h');
  return result;
}

String getImmSizeErrorString(const String &dst, INT64 immv) {
  return format(_T("Immediate value %s doesn't fit in %s"),formatHexValue(immv,false).cstr(), dst.cstr());
}

OperandSize InstructionOperand::findMinSize(int    v) {
  if(::isByte(v)) return REGSIZE_BYTE;
  if(isWord(v)) return REGSIZE_WORD;
  return REGSIZE_DWORD;
}
OperandSize InstructionOperand::findMinSize(UINT   v) {
  if(::isByte(v)) return REGSIZE_BYTE;
  if(isWord(v)) return REGSIZE_WORD;
  return REGSIZE_DWORD;
}
OperandSize InstructionOperand::findMinSize(INT64  v) {
  if(::isByte( v)) return REGSIZE_BYTE;
  if(isWord( v)) return REGSIZE_WORD;
  if(isDword(v)) return REGSIZE_DWORD;
  return REGSIZE_QWORD;
}
OperandSize InstructionOperand::findMinSize(UINT64 v) {
  if(::isByte( v)) return REGSIZE_BYTE;
  if(isWord( v)) return REGSIZE_WORD;
  if(isDword(v)) return REGSIZE_DWORD;
  return REGSIZE_QWORD;
}
void InstructionOperand::setValue(int    v) {
  switch(getSize()) {
  case REGSIZE_BYTE : m_v8  = (BYTE )v; break;
  case REGSIZE_WORD : m_v16 = (WORD )v; break;
  case REGSIZE_DWORD: m_v32 = (DWORD)v; break;
  default           : throwUnknownSize(__TFUNCTION__);
  }
}
void InstructionOperand::setValue(UINT   v) {
  switch(getSize()) {
  case REGSIZE_BYTE : m_v8  = (BYTE )v; break;
  case REGSIZE_WORD : m_v16 = (WORD )v; break;
  case REGSIZE_DWORD: m_v32 = (DWORD)v; break;
  default           : throwUnknownSize(__TFUNCTION__);
  }
}
void InstructionOperand::setValue(INT64  v) {
  switch(getSize()) {
  case REGSIZE_BYTE : m_v8  = (BYTE )v; break;
  case REGSIZE_WORD : m_v16 = (WORD )v; break;
  case REGSIZE_DWORD: m_v32 = (DWORD)v; break;
  case REGSIZE_QWORD: m_v64 = v       ; break;
  default           : throwUnknownSize(__TFUNCTION__);
  }
}
void InstructionOperand::setValue(UINT64 v) {
  switch(getSize()) {
  case REGSIZE_BYTE : m_v8  = (BYTE )v; break;
  case REGSIZE_WORD : m_v16 = (WORD )v; break;
  case REGSIZE_DWORD: m_v32 = (DWORD)v; break;
  case REGSIZE_QWORD: m_v64 = v       ; break;
  default           : throwUnknownSize(__TFUNCTION__);
  }
}

void InstructionOperand::throwUnknownSize(const TCHAR *method) const {
  throwException(_T("%s:Unknown size for immediate value:%d"), method, getSize());
}

void InstructionOperand::throwTypeError(const TCHAR *method, OperandType expectedType) const {
  throwException(_T("%s:Invalid type (=%s). Expected %s")
                ,method
                ,::toString(getType()).cstr()
                ,::toString(expectedType).cstr()
                );
}

void InstructionOperand::throwSizeError(const TCHAR *method, OperandSize expectedSize) const {
  throwException(_T("%s:Operandsize=%s. Cannot convert to %s")
                ,method
                ,::toString(getSize()).cstr()
                ,::toString(expectedSize).cstr());
}

#define VALIDATEISIMMVALUE() if(getType() != IMMEDIATEVALUE) throwTypeError(__TFUNCTION__,IMMEDIATEVALUE)
#define VALIDATESIZE(size)   if(!Register::sizeContainsSrcSize(size, getSize())) throwSizeError(__TFUNCTION__,size)

char   InstructionOperand::getImmInt8()   const {
  VALIDATEISIMMVALUE();
  VALIDATESIZE(REGSIZE_BYTE);
  return (char)m_v8;
}

BYTE   InstructionOperand::getImmUint8()  const {
  VALIDATEISIMMVALUE();
  VALIDATESIZE(REGSIZE_BYTE);
  return m_v8;
}

short  InstructionOperand::getImmInt16()  const {
  VALIDATEISIMMVALUE();
  VALIDATESIZE(REGSIZE_WORD);
  switch(getSize()) {
  case REGSIZE_BYTE: return (short)(char)m_v8;
  case REGSIZE_WORD: return (short)m_v16;
  default          : NODEFAULT;
  }
  return 0;
}

USHORT InstructionOperand::getImmUint16() const {
  VALIDATEISIMMVALUE();
  VALIDATESIZE(REGSIZE_WORD);
  switch(getSize()) {
  case REGSIZE_BYTE: return (USHORT)m_v8;
  case REGSIZE_WORD: return (USHORT)m_v16;
  default          : NODEFAULT;
  }
  return 0;
}

int    InstructionOperand::getImmInt32()  const {
  VALIDATEISIMMVALUE();
  VALIDATESIZE(REGSIZE_DWORD);
  switch(getSize()) {
  case REGSIZE_BYTE : return (int)(char)m_v8;
  case REGSIZE_WORD : return (int)(short)m_v16;
  case REGSIZE_DWORD: return (int)m_v32;
  default           : NODEFAULT;
  }
  return 0;
}

UINT   InstructionOperand::getImmUint32() const {
  VALIDATEISIMMVALUE();
  VALIDATESIZE(REGSIZE_DWORD);
  switch(getSize()) {
  case REGSIZE_BYTE : return (UINT)m_v8;
  case REGSIZE_WORD : return (UINT)m_v16;
  case REGSIZE_DWORD: return (UINT)m_v32;
  default           : NODEFAULT;
  }
  return 0;
}

INT64  InstructionOperand::getImmInt64()  const {
  VALIDATEISIMMVALUE();
  switch(getSize()) {
  case REGSIZE_BYTE : return (INT64)(char)m_v8;
  case REGSIZE_WORD : return (INT64)(short)m_v16;
  case REGSIZE_DWORD: return (INT64)(int)m_v32;
  case REGSIZE_QWORD: return (INT64)m_v64;
  default           : NODEFAULT;
  }
  return 0;
}

UINT64 InstructionOperand::getImmUint64() const {
  VALIDATEISIMMVALUE();
  switch(getSize()) {
  case REGSIZE_BYTE : return (UINT64)m_v8;
  case REGSIZE_WORD : return (UINT64)m_v16;
  case REGSIZE_DWORD: return (UINT64)m_v32;
  case REGSIZE_QWORD: return (UINT64)m_v64;
  default           : NODEFAULT;
  }
  return 0;
}

String InstructionOperand::toString() const {
  switch(getType()) {
  case REGISTER:
    return m_reg ? m_reg->getName() : _T("Unknown operand");
  case IMMEDIATEVALUE:
    switch(getSize()) {
    case REGSIZE_BYTE : 
    case REGSIZE_WORD : 
    case REGSIZE_DWORD: return formatHexValue(getImmUint32());
    case REGSIZE_QWORD: return formatHexValue(getImmUint64());
    default           : throwUnknownSize(__TFUNCTION__);
    }
  default:
    throwUnsupportedOperationException(__TFUNCTION__);
  }
  return EMPTYSTRING;
}

const OperandTypeSet InstructionOperand::R(    REGISTER,                             -1);
const OperandTypeSet InstructionOperand::M(             MEMORYOPERAND,               -1);
const OperandTypeSet InstructionOperand::IMM(                         IMMEDIATEVALUE,-1);
const OperandTypeSet InstructionOperand::RM(   REGISTER,MEMORYOPERAND,               -1);
const OperandTypeSet InstructionOperand::S(    REGISTER,              IMMEDIATEVALUE,-1);
const OperandTypeSet InstructionOperand::ALL(  REGISTER,MEMORYOPERAND,IMMEDIATEVALUE,-1);
const OperandTypeSet InstructionOperand::EMPTY(                                      -1);

const char MemoryRef::s_shift[9] = { -1, 0, 1, -1, 2, -1, -1, -1, 3 };

void MemoryRef::sortBaseInx() {
  if(!hasShift() && hasInx()) {
    if(hasBase()) {
      if((!hasOffset() && ((m_base->getIndex()&7)==5))
      || (m_base->isValidIndexRegister() && (m_base->getIndex() > m_inx->getIndex())
          && (hasOffset() || ((m_inx->getIndex()&7)!=5))
         )
        ) {
        const IndexRegister *tmp = m_inx; m_inx = m_base; m_base = tmp;
        SETDEBUGSTR();
      }
    } else { // set base = inx; inx = NULL
      m_base = m_inx; m_inx = NULL;
      SETDEBUGSTR();
    }
  }
}

String MemoryRef::toString() const {
  if(isDisplaceOnly()) {
    return formatHexValue((size_t)getAddr());
  }
  String result;
  if(hasBase()) {
    result += m_base->getName();
  }
  if(hasInx()) {
    if(result.length() > 0) result += _T("+");
    result += m_inx->getName();
    if(hasShift()) result += format(_T("*%d"),1<<getShift());
  }
  if(hasOffset()) {
    result += formatHexValue(getOffset(),true);
  }
  return result;
}

void MemoryRef::throwInvalidIndexScale(const TCHAR *method, BYTE a) { // static
  throwInvalidArgumentException(method, _T("a=%d. Must be 1,2,4,8"));
}

void MemoryRef::throwInvalidIndexRegister(const TCHAR *method, const Register &reg) { // static
  throwInvalidArgumentException(method,_T("Invalid indexregister:%s"), reg.getName().cstr());
}

void MemoryRef::throwInvalidIndex(const TCHAR *method, char op, const String &str) const {
  throwInvalidArgumentException(method,_T("Illegal index:%s%c%s"),toString().cstr(),op,str.cstr());
}

String MemoryOperand::toString() const {
  switch (getSize()) {
  case REGSIZE_VOID:
    return (m_segReg)
          ? format(_T("%s:[%s]"), m_segReg->getName().cstr(), m_mr.toString().cstr())
          : format(_T("[%s]")   , m_mr.toString().cstr());
  default          :
    return (m_segReg)
          ? format(_T("%s ptr %s:[%s]"), ::toString(getSize()).cstr(), m_segReg->getName().cstr(), m_mr.toString().cstr())
          : format(_T("%s ptr[%s]")    , ::toString(getSize()).cstr(), m_mr.toString().cstr());
  }
}

int MemoryRef::offsetCmp(const MemoryRef &mr1, const MemoryRef &mr2) { // static
  int c;
  if(c = boolCmp(mr1.hasOffset(), mr2.hasOffset())) return c;
  if(!mr1.hasOffset()) return 0;
  if(c = boolCmp(isByte(mr2.getOffset()), isByte(mr1.getOffset()))) return c;
  return sign(mr1.getOffset() - mr2.getOffset());
}

int MemoryRef::addrCmp(const MemoryRef &mr1, const MemoryRef &mr2) { // static
  int c;
  if(c = boolCmp(mr1.hasOffset(), mr2.hasOffset())) return c;
  if(!mr1.hasOffset()) return 0;
  if(c = boolCmp(isByte(mr2.getAddr()), isByte(mr1.getAddr()))) return c;
  if(c = boolCmp(isDword(mr2.getAddr()), isDword(mr1.getAddr()))) return c;
  return sign(mr1.getAddr() - mr2.getAddr());
}

int MemoryRef::memRefCmp(const MemoryRef &mr1, const MemoryRef &mr2) { // static
  int c;
  if(c = boolCmp(mr2.isDisplaceOnly(),mr1.isDisplaceOnly())) return c;
  if(mr1.isDisplaceOnly()) {
    return addrCmp(mr1,mr2);
  } else {
    if(c = boolCmp(mr1.hasInx(),mr2.hasInx())) return c;
    if(!mr1.hasInx()) {
      if(c = offsetCmp(mr1,mr2)) return c;
    }
    if(c = boolCmp(mr1.hasBase(), mr2.hasBase())) return c;
    if(mr1.hasBase()) { // && mr2.hasBase()
      if(c = registerCmp(*mr1.getBase(), *mr2.getBase())) return c;
    }
    if(mr1.hasInx()) { // && mr2.hasInx()
      if(c = offsetCmp(mr1,mr2)) return c;
      if(c = (int)mr1.getShift() - (int)mr2.getShift()) return c;
      if(c = registerCmp(*mr1.getInx(),*mr2.getInx())) return c;
    }
  }
  return offsetCmp(mr1,mr2);
}

int MemoryOperand::memOpCmp(const MemoryOperand &mem1, const MemoryOperand &mem2) { // static
  int c = regSizeCmp(mem1.getSize(),mem2.getSize());
  if(c) return c;
  if(c = MemoryRef::memRefCmp(mem1.getMemoryReference(), mem2.getMemoryReference())) return c;
  if(c = boolCmp(mem1.hasSegmentRegister(),mem2.hasSegmentRegister())) return c;
  if(!mem1.hasSegmentRegister()) return 0; // && !mem2.hasSegmentRegister()
  return registerCmp(*mem1.getSegmentRegister(), *mem2.getSegmentRegister());
}

int InstructionOperand::insOpCmp(const InstructionOperand &op1, const InstructionOperand &op2) { // static
  int c = (int)op1.getType() - (int)op2.getType();
  if(c) return c;
  switch(op1.getType()) { // op1 and op2 has same type
  case REGISTER      : return registerCmp(op1.getRegister(), op2.getRegister());
  case MEMORYOPERAND : return MemoryOperand::memOpCmp((MemoryOperand&)op1, (MemoryOperand&)op2);
  case IMMEDIATEVALUE:
    if(c = regSizeCmp(op1.getSize(), op2.getSize())) return c;
    return sign(op1.getImmInt64() - op2.getImmInt64());
  }
  throwInvalidArgumentException(__TFUNCTION__,_T("op1.getType()=%s"), ::toString(op1.getType()).cstr());
  return 0;
}
