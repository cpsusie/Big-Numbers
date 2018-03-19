#include "pch.h"
#include "InstructionBuilder.h"

InstructionBase::InstructionBase(const OpcodeBase &opcode)
  : m_size(opcode.size())
{
  memcpy(m_bytes, opcode.getBytes(),m_size);
}

String InstructionBase::toString() const {
  String result;
  for(const BYTE *p = m_bytes, *end = m_bytes + m_size;;) {
    result += format(_T("%02X"), *(p++));
    if(p < end) {
      result += _T(" ");
    } else {
      break;
    }
  }
  return result;
}

Instruction0Arg::Instruction0Arg(const Instruction0Arg &ins, RegSize size) : InstructionBase(ins) {
  InstructionBuilder ib(*this);
  switch (size) {
  case REGSIZE_WORD :
    ib.wordIns();
    break;
#ifdef IS64BIT
  case REGSIZE_QWORD:
    ib.setRexBits(8);
    break;
#endif // IS64BIT
  default           :
    throwInvalidArgumentException(__TFUNCTION__,_T("size=%s"), ::toString(size).cstr());
  }
  *this = (Instruction0Arg&)ib;
}
