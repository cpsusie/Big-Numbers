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

Instruction0Arg::Instruction0Arg(const Instruction0Arg &ins, OperandSize size) : InstructionBase(ins) {
  InstructionBuilder ib(*this);
  switch(size) {
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

Instruction0ArgB::Instruction0ArgB(const Instruction0Arg &ins, OperandSize size) : Instruction0Arg(ins) {
  if(!isValidOperandSize(size)) {
    throwInvalidArgumentException(__TFUNCTION__
                                 ,_T("%s not a valid size")
                                 ,::toString(size).cstr()
                                 );
  }
  *this = (Instruction0ArgB&)(InstructionBase&)InstructionBuilder(*this).setOperandSize(size);
}

#pragma warning(disable : 4073)
#pragma init_seg(lib)

#ifdef IS32BIT
const RegSizeSet Instruction0ArgB::s_validOperandSizeSet(REGSIZE_BYTE, REGSIZE_WORD, REGSIZE_DWORD, REGSIZE_END);
#else // IS64BIT
const RegSizeSet Instruction0ArgB::s_validOperandSizeSet(REGSIZE_BYTE, REGSIZE_WORD, REGSIZE_DWORD, REGSIZE_QWORD, REGSIZE_END);
#endif // IS64BIT

bool Instruction0ArgB::isValidOperandSize(OperandSize size) const {
  return s_validOperandSizeSet.contains(size);
}

InstructionBase StringPrefix::operator()(const StringInstruction &ins) const {
  return InstructionBuilder(*this).add(ins.getBytes(), ins.size());
}
