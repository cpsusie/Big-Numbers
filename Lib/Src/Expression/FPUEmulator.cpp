#include "pch.h"
#include "FPUEmulator.h"

namespace Expr {

FPUOpcodeHashMap FPUEmulator::s_FPUOpcodeMap;

FPUOpcodeKey FPUEmulator::execute(const OpcodeBase &opcode) {
  m_changed = false;
  FPUOpcodeKey codeKey;
  if(!isFPUOpcode(opcode,codeKey)) return codeKey;
  return execute(codeKey, opcode.getFPUStackDelta());
}

FPUOpcodeKey FPUEmulator::execute(const OpcodeBase &opcode, const InstructionOperand &arg) {
  m_changed = false;
  FPUOpcodeKey codeKey;
  if(!isFPUOpcode(opcode,codeKey)) return codeKey;
  if(arg.isRegister()) {
    return execute(codeKey, opcode(arg).getFPUStackDelta(), -1, arg.getRegister().getIndex());
  } else {
    return execute(codeKey, opcode(arg).getFPUStackDelta(), m_container?m_container->getValueIndex(arg):-1);
  }
}

FPUOpcodeKey FPUEmulator::execute(const OpcodeBase &opcode, const InstructionOperand &arg1, const InstructionOperand &arg2) {
  m_changed = false;
  FPUOpcodeKey codeKey;
  if(!isFPUOpcode(opcode,codeKey)) return codeKey;
  assert(arg1.isRegister());
  assert(arg2.isRegister());
  return execute(codeKey, opcode(arg1,arg2).getFPUStackDelta(), arg1.getRegister().getIndex(), arg2.getRegister().getIndex());
}

FPUOpcodeKey FPUEmulator::execute(FPUOpcodeKey codeKey, char stackDelta, int memIndex, int reg1, int reg2) {
  switch(stackDelta) {
  case -2:
    pop(2); // FCOMPP, FUCOMPP
    break;
  case -1:
    switch(codeKey) {
    case _FSTP   :
      if(reg1 > 0) {
        ST(reg1) = ST(0);
      }
      pop();
      break;
    case _FADD   :
    case _FMUL   :
    case _FSUB   :
    case _FDIV   :
    case _FSUBR  :
    case _FDIVR  :
      pop(1);
      setMixed(0);
      break;
    case _FADDP  :
    case _FMULP  :
    case _FSUBP  : 
    case _FDIVP  :  
    case _FSUBRP :   
    case _FDIVRP :
      setMixed(reg1);
      pop();
      break;
    default:
      pop();
      break;
    }
    break;
  case  0:
    switch(codeKey) {
    case _FST    :
      break;
    case _FADD   :
    case _FMUL   :
    case _FSUB   :
    case _FDIV   :
    case _FSUBR  :
    case _FDIVR  :
      if(memIndex >= 0) {
        setMixed(0);
      } else if(reg1 >= 0) {
        setMixed(reg1);
      }
      break;
    case _FCOM   :
    case _FCOMI  :
    case _FUCOM  :
    case _FUCOMI :
    case _FIST   :
      break;
    case _FIADD  :
    case _FIMUL  :
    case _FISUB  :
    case _FIDIV  :
    case _FISUBR :
    case _FIDIVR :
      setMixed(0); // dst always ST(0)
      break;
    case _FICOM  :
      break;
    case _FCMOVB :
    case _FCMOVAE:
    case _FCMOVE :
    case _FCMOVNE:
    case _FCMOVBE:
    case _FCMOVA :
    case _FCMOVU :
    case _FCMOVNU:
      setMixed(0); // dst always ST0
      break;
    case _FFREE  :
      setEmpty(reg1);
      break;
    case _FXCH   :
      swapWith0(reg1);
      break;
    case _FCHS   :
    case _FABS   :
    case _FSQRT  :
    case _FRNDINT:
    case _FSCALE :
    case _FSIN   :
    case _FCOS   :
    case _FPREM1 :
    case _FPREM  :
      setMixed(0);
      break;
    case _FDECSTP:
      fdecstp();
      break;
    case _FINCSTP:  
      fincstp();
      break;
    }
    break;
  case  1:
    switch(codeKey) {
    case _FLD:
      if(reg1 >= 0) {
        push(ST(reg1).getValueIndex());
      } else if(memIndex >= 0) {
        push(memIndex);
      } else {
        pushMixed();
      }
      break;
    default:
      pushMixed();
      break;
    }
    break;
  }
  if(m_changed && m_container && m_container->wantFPUComment()) {
    m_container->putFPUComment(toString());
  }
  return codeKey;
}

int FPUEmulator::findRegisterWithValueIndex(UINT valueIndex) const {
  const UINT h = getHeight();
  for(BYTE index = 0; index < h; index++) {
    if(ST(index).getValueIndex() == valueIndex) {
      return index;
    }
  }
  return -1;
}

String FPUEmulator::toString() const {
  const UINT h = getHeight();
  if(h == 0) return _T("FPU empty");
  String result;
  for(UINT i = 0; i < h; i++) {
    result += format(_T("ST(%u):%-6s"),i,ST(i).toString().cstr());
  }
  return result;
}

String FPUSlot::toString() const {
  if(isEmpty()) return EMPTYSTRING;
  if(isMixed()) return _T("mixed");
  return format(_T("v[%2d]"), m_valueIndex);
}

}; // namespace Expr
