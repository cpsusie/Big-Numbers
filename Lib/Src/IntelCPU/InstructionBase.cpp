#include "pch.h"

#pragma warning(disable : 4073)
#pragma init_seg(lib)

InstructionBase::InstructionBase(const OpcodeBase &opcode)
  : m_size(opcode.size())
  , m_FPUStackDelta(opcode.getFPUStackDelta())
{
  memcpy(m_bytes, opcode.getBytes(),m_size);
}

InstructionBase::InstructionBase(const Opcode0Arg &opcode)
  : m_size(opcode.size())
  , m_FPUStackDelta(opcode.getFPUStackDelta())
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

InstructionBase StringPrefix::operator()(const StringInstruction &ins) const {
  return InstructionBuilder(*this).add(ins);
}
