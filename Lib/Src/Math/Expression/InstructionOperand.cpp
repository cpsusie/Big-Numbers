#include "pch.h"
#include <Math/Expression/NewOpCode.h>

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
  bool neg;
  if(v >= 0) {
    neg = false;
  } else {
    v        = -v;
    neg      = true;
    showSign = true;
  }
  String result = format(_T("%X"), v);
  if(!iswdigit(result[0])) {
    result.insert(0,'0');
  }
  if(v >= 10) result += _T('h');
  if(showSign) {
    result.insert(0, neg ? '-' : '+');
  }
  return result;
}

// Convert int64-value to disassembler format
String formatHexValue(INT64 v, bool showSign) {
  bool neg;
  if(v >= 0) {
    neg = false;
  } else {
    v        = -v;
    neg      = true;
    showSign = true;
  }
  String result = format(_T("%I64X"), v);
  if(!iswdigit(result[0])) {
    result.insert(0,'0');
  }
  if(v >= 10) result += _T('h');
  if(showSign) {
    result.insert(0, neg ? '-' : '+');
  }
  return result;
}

String getImmSizeErrorString(const String &dst, INT64 immv) {
  return format(_T("Immediate value %s doesn't fit in %s"),formatHexValue(immv,false).cstr(), dst.cstr());
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
    case REGSIZE_BYTE : 
    case REGSIZE_WORD : 
    case REGSIZE_DWORD: return formatHexValue(getImmInt32(),false);
    case REGSIZE_QWORD: return formatHexValue(getImmInt64(),false);
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
  String result;
  if(hasBase()) {
    result += m_base->getName();
  }
  if(hasInx()) {
    if(result.length() > 0) result += _T("+");
    result += m_inx->getName();
    if(hasShift()) result += format(_T("*%d"),1<<getShift());
  }
  if(hasOffset() || result.isEmpty()) {
    result += formatHexValue(m_offset, !result.isEmpty());
  }
  return result;
}

MemoryRef operator+(const IndexRegister &base, int offset) {
  return MemoryRef(&base,NULL,0,offset);
}

MemoryRef operator-(const IndexRegister &base, int offset) {
  return MemoryRef(&base,NULL,0,-offset);
}

MemoryRef operator+(const MemoryRef &mr, int offset) {
  if(mr.hasOffset()) {
    throwInvalidArgumentException(__TFUNCTION__,_T("Illegal index:%s+%d"),mr.toString().cstr(),offset);
  }
  return offset ? MemoryRef(mr.getBase(),mr.getInx(),mr.getShift(),offset) : mr;
}

MemoryRef operator-(const MemoryRef &mr, int offset) {
  if(mr.hasOffset()) {
    throwInvalidArgumentException(__TFUNCTION__,_T("Illegal index:%s-%d"),mr.toString().cstr(),offset);
  }
  return offset ? MemoryRef(mr.getBase(),mr.getInx(),mr.getShift(),-offset) : mr;
}

MemoryRef operator+(const IndexRegister &base, const MemoryRef &mr) {
  if(mr.hasBase() || (!mr.hasInx() || mr.hasOffset())) {
    throwInvalidArgumentException(__TFUNCTION__,_T("Illegal index:%s+%s"),base.getName().cstr(),mr.toString().cstr());
  }
  return MemoryRef(&base,mr.getInx(),mr.getShift());
}

MemoryRef operator+(const IndexRegister &base, const IndexRegister &inx) {
  return MemoryRef(&base,&inx,0);
}

MemoryRef operator*(BYTE a, const IndexRegister &inx) {
  if(!inx.isValidIndexRegister()) {
    throwInvalidArgumentException(__TFUNCTION__,_T("Invalid indexregister:%s"), inx.getName().cstr());
  }
  return MemoryRef(NULL,&inx,findShift(a));
}

MemoryRef operator*(const IndexRegister &inx, BYTE a) {
  if(!inx.isValidIndexRegister()) {
    throwInvalidArgumentException(__TFUNCTION__,_T("Invalid indexregister:%s"), inx.getName().cstr());
  }
  return MemoryRef(NULL,&inx,findShift(a));
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
