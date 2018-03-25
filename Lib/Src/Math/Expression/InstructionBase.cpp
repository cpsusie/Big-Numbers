#include "pch.h"
#include "InstructionBuilder.h"

#pragma warning(disable : 4073)
#pragma init_seg(lib)

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

#ifdef IS32BIT
const RegSizeSet Instruction0Arg::s_validOperandSizeSet(REGSIZE_WORD, REGSIZE_END);
#else // IS64BIT
const RegSizeSet Instruction0Arg::s_validOperandSizeSet(REGSIZE_WORD, REGSIZE_QWORD, REGSIZE_END);
#endif // IS64BIT

bool Instruction0Arg::isValidOperandSize(OperandSize size) const {
  return s_validOperandSizeSet.contains(size);
}

Instruction0Arg::Instruction0Arg(const String &mnemonic, const Instruction0Arg &ins, OperandSize size) 
  : InstructionBase(ins)
  , m_mnemonic(toLowerCase(mnemonic))
{
  validateOperandSize(size);
  (*(InstructionBase*)this) = InstructionBuilder(*this).setOperandSize(size);
#ifdef __NEVER__
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
    throwInvalidArgumentException(__TFUNCTION__,_T("%s:size=%s"), getMnemonic().cstr(), ::toString(size).cstr());
  }
  (*(InstructionBase*)this) = ib;
#endif
}

void Instruction0Arg::validateOperandSize(OperandSize size) const {
  if(!isValidOperandSize(size)) {
    throwInvalidArgumentException(__TFUNCTION__
                                 ,_T("%s:%s not a valid size")
                                 ,getMnemonic().cstr()
                                 ,::toString(size).cstr()
                                 );
  }
}

Instruction0ArgB::Instruction0ArgB(const String &mnemonic, const Instruction0Arg &ins, OperandSize size)
: Instruction0Arg(mnemonic, ins)
{
  validateOperandSize(size);
  (*(InstructionBase*)this) = InstructionBuilder(*this, HAS_SIZEBIT).setOperandSize(size);
}

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
