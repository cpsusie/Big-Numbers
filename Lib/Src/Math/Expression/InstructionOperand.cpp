#include "pch.h"
#include <Math/Expression/NewOpCode.h>

String toString(OperandType type) {
  switch(type) {
  case REGISTER      : return _T("REGISTER"      );
  case IMMEDIATEVALUE: return _T("IMMEDIATEVALUE");
  case MEMREFERENCE  : return _T("MEMREFERENCE"  );
  default            : return format(_T("Unknown operandType:%d"), type);
  }
}

OperandSize InstructionOperand::findMinSize(int    v) {
  if(isByte(v)) return REGSIZE_BYTE;
  if(isWord(v)) return REGSIZE_WORD;
  return REGSIZE_DWORD;
}
OperandSize InstructionOperand::findMinSize(UINT   v) {
  if(isByte(v)) return REGSIZE_BYTE;
  if(isWord(v)) return REGSIZE_WORD;
  return REGSIZE_DWORD;
}
OperandSize InstructionOperand::findMinSize(INT64  v) {
  if(isByte( v)) return REGSIZE_BYTE;
  if(isWord( v)) return REGSIZE_WORD;
  if(isDword(v)) return REGSIZE_DWORD;
  return REGSIZE_QWORD;
}
OperandSize InstructionOperand::findMinSize(UINT64 v) {
  if(isByte( v)) return REGSIZE_BYTE;
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

void InstructionOperand::validateType(const TCHAR *method, OperandType expectedType) const {
  if(getType() != expectedType) {
    throwException(_T("%s:Invalid type (=%s). Expected %s")
                  ,method
                  ,::toString(getType()).cstr()
                  ,::toString(expectedType).cstr()
                  );
  }
}

void InstructionOperand::validateSize(const TCHAR *method, OperandSize expectedSize) const {
  const OperandSize size = getSize();
  switch(expectedSize) {
  case REGSIZE_BYTE :
    if(size == REGSIZE_BYTE) return;
    break;
  case REGSIZE_WORD :
    if((size == REGSIZE_BYTE) || (size == REGSIZE_WORD)) return;
    break;
  case REGSIZE_DWORD:
    if((size == REGSIZE_BYTE) || (size == REGSIZE_WORD) || (size == REGSIZE_DWORD)) return;
    break;
  case REGSIZE_QWORD:
    return;
  default           :
    throwInvalidArgumentException(method, _T("ExpectedSize=%s"), ::toString(expectedSize).cstr());
  }
  throwException(_T("%s:Operandsize=%s. Cannot convert to %s")
                ,method
                ,::toString(size        ).cstr()
                ,::toString(expectedSize).cstr());
}

#define VALIDATEISIMMVALUE() validateType(method,IMMEDIATEVALUE)
#define VALIDATESIZE(size)   validateSize(method,size)

char   InstructionOperand::getImmInt8()   const {
  DEFINEMETHODNAME;
  VALIDATEISIMMVALUE();
  VALIDATESIZE(REGSIZE_BYTE);
  return m_v8;
}
BYTE   InstructionOperand::getImmUint8()  const {
  DEFINEMETHODNAME;
  VALIDATEISIMMVALUE();
  VALIDATESIZE(REGSIZE_BYTE);
  return m_v8;
}
short  InstructionOperand::getImmInt16()  const {
  DEFINEMETHODNAME;
  VALIDATEISIMMVALUE();
  VALIDATESIZE(REGSIZE_WORD);
  switch(getSize()) {
  case REGSIZE_BYTE: return (USHORT)m_v8;
  case REGSIZE_WORD: return (USHORT)m_v16;
  default          : NODEFAULT;
  }
  return 0;
}
USHORT InstructionOperand::getImmUint16() const {
  DEFINEMETHODNAME;
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
  DEFINEMETHODNAME;
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
UINT   InstructionOperand::getImmUint32() const {
  DEFINEMETHODNAME;
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
  DEFINEMETHODNAME;
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
UINT64 InstructionOperand::getImmUInt64() const {
  DEFINEMETHODNAME;
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
    case REGSIZE_BYTE : return format(_T("%#04x"   ),m_v8 );
    case REGSIZE_WORD : return format(_T("%#06x"   ),m_v16);
    case REGSIZE_DWORD: return format(_T("%#010x"  ),m_v32);
    case REGSIZE_QWORD: return format(_T("%#18I64x"),m_v64);
    default           : throwUnknownSize(__TFUNCTION__);
    }
  default:
    throwUnsupportedOperationException(__TFUNCTION__);
  }
  return EMPTYSTRING;
}

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
        ? format(_T("%s ptr %s:[%s]"), ::toString(getSize()).cstr(), m_segReg->getName().cstr(), m_mr.toString().cstr())
        : format(_T("%s ptr[%s]")    , ::toString(getSize()).cstr(), m_mr.toString().cstr());
}
