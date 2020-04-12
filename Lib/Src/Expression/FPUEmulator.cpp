#include "pch.h"
#include <IntelCPU/OpCode.h>
#include "Math/Expression/ExpressionSymbolTable.h"
#include "FPUEmulator.h"

namespace Expr {

FPUOpcodeHashMap FPUEmulator::s_FPUOpcodeMap(555);
FPUOpcodeKey FPUOpcodeHashMap::getOpcodeKey(const OpcodeBase &opcode) const {
  const FPUOpcodeKey *v = get(opcode.getMnemonic().cstr());
  return v ? *v : _NOTFPU;
}

FPUOpcodeKey FPUEmulator::execute(const OpcodeBase &opcode) {
  FPUOpcodeKey codeKey;
  if(!isFPUOpcode(opcode,codeKey)) return codeKey;
  return execute(codeKey, opcode.getFPUStackDelta());
}

FPUOpcodeKey FPUEmulator::execute(const OpcodeBase &opcode, const InstructionOperand &arg) {
  FPUOpcodeKey codeKey;
  if(!isFPUOpcode(opcode,codeKey)) return codeKey;
  if(arg.isRegister()) {
    return execute(codeKey, opcode(arg).getFPUStackDelta(), -1, arg.getRegister().getIndex());
  } else {
    return execute(codeKey, opcode(arg).getFPUStackDelta(), m_container?m_container->getValueIndex(arg):-1);
  }
}

FPUOpcodeKey FPUEmulator::execute(const OpcodeBase &opcode, const InstructionOperand &arg1, const InstructionOperand &arg2) {
  FPUOpcodeKey codeKey;
  if(!isFPUOpcode(opcode,codeKey)) return codeKey;
  assert(arg1.isRegister());
  assert(arg2.isRegister());
  return execute(codeKey, opcode(arg1,arg2).getFPUStackDelta(), arg1.getRegister().getIndex(), arg2.getRegister().getIndex());
}

FPUOpcodeKey FPUEmulator::execute(FPUOpcodeKey codeKey, char stackDelta, int memIndex, int reg1, int reg2) {
  m_state.resetChanged();
  m_state.execute(codeKey,stackDelta,memIndex,reg1,reg2);
  if(m_state.isChanged() && m_container && m_container->wantFPUComment()) {
    m_container->putFPUComment(toString());
  }
  return codeKey;
}

void FPUState::execute(FPUOpcodeKey codeKey, char stackDelta, int memIndex, int reg1, int reg2) {
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
    case _CALL   : // only activated in 32bit !LONGDOUBLE
      pushMixed();
      break;
    }
    break;
  case  1:
    switch(codeKey) {
    case _FLD:
      if(reg1 >= 0) {
        push(ST(reg1));
      } else if(memIndex >= 0) {
        if(ExpressionSymbolTable::isTempVarIndex(memIndex)) {
          push(TEMPVAR);
        } else {
          push(memIndex);
        }
      } else {
        pushMixed();
      }
      break;
    case _FLDZ:
      push(CONST_0);
      break;
    case _FLD1:
      push(CONST_1);
      break;
    case _FLDPI:
      push(CONST_PI);
      break;
    default:
      pushMixed();
      break;
    }
    break;
  }
}

int FPUState::findRegisterWithValueIndex(UINT valueIndex) const {
  const UINT h = getHeight();
  for(BYTE index = 0; index < h; index++) {
    const FPUSlot &slot = ST(index);
    if(slot.isIndex() && (slot.getValueIndex() == valueIndex)) {
      return index;
    }
  }
  return -1;
}

bool FPUState::operator==(const FPUState &state) const {
  return (m_bottom == state.m_bottom)
      && (m_height == state.m_height);
  // maybe check labelled registers too....need more thinking
}

String FPUState::toString() const {
  const UINT h = getHeight();
  if(h == 0) return _T("FPU empty");
  String result;
  for(UINT i = 0; i < h; i++) {
    result += format(_T("ST(%u):%-6s"),i,ST(i).toString().cstr());
  }
  return result;
}

String FPUSlot::toString() const {
  switch(m_value) {
  case SLOT_EMPTY: return EMPTYSTRING;
  case SLOT_MIXED: return _T("mixed");
  case TEMPVAR   : return _T("$temp");
  case CONST_0   : return _T("0.000");
  case CONST_1   : return _T("1.000");
  case CONST_PI  : return _T("pi"   );
  default        :
    if(!isIndex()) {
      return format(_T("Invalid content:%d"), m_value);
    }
    return format(_T("v[%2d]"), m_value);
  }
}

}; // namespace Expr
