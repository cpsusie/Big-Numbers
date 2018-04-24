#pragma once

#include <OpCode.h>
#include <ExecutableByteArray.h>
#include <Math/MathLib.h>
#include <Math/Expression/ParserTree.h>

typedef void (*BuiltInFunction)();
typedef BuiltInFunction ExpressionEntryPoint;
typedef Real (*BuiltInFunctionRef1)(const Real &x);
typedef Real (*BuiltInFunctionRef2)(const Real &x, const Real &y);
typedef Real (*BuiltInFunction1)(Real x);
typedef Real (*BuiltInFunction2)(Real x, Real y);


#ifdef IS64BIT
typedef enum {
  RESULT_IN_FPU
 ,RESULT_IN_ADDRRDI
 ,RESULT_ON_STACK
 ,RESULT_IN_VALUETABLE
#ifndef LONGDOUBLE
 ,RESULT_IN_XMM  // XMM-register cannot be used for 80-bit floating points
#endif
} ExpressionDestinationType;

class ExpressionDestination {
private:
  const ExpressionDestinationType m_type;
  union {
    const int                       m_offset;
    const XMMRegister              *m_register;
  };
public:
  ExpressionDestination(ExpressionDestinationType type, int offset)
    : m_type(type), m_offset(offset)
  {
  }
#ifndef LONGDOUBLE
  ExpressionDestination(const XMMRegister &xmmReg)
    : m_type(RESULT_IN_XMM)
    , m_register(&xmmReg)
  {
  }
#endif
  inline ExpressionDestinationType getType() const {
    return m_type;
  }
  inline BYTE getStackOffset() const {
    assert(m_type == RESULT_ON_STACK);
    return (BYTE)m_offset;
  }
  inline int getTableIndex() const {
    assert(m_type == RESULT_IN_VALUETABLE);
    return m_offset;
  }
#ifndef LONGDOUBLE
  inline const XMMRegister &getXMMReg() const {
    assert(m_type == RESULT_IN_XMM);
    return *m_register;
  }
#endif
};

#define DST_FPU                 ExpressionDestination(RESULT_IN_FPU       , -1     )
#define DST_ADDRRDI             ExpressionDestination(RESULT_IN_ADDRRDI   , -1     )
#define DST_ONSTACK(offs1)      ExpressionDestination(RESULT_ON_STACK     , offs1  )
#define DST_INVALUETABLE(index) ExpressionDestination(RESULT_IN_VALUETABLE, index  )
#ifndef LONGDOUBLE
#define DST_XMM(xmmReg)         ExpressionDestination(xmmReg )
#endif
#else

typedef int ExpressionDestination;

#define DST_FPU                 0
#define DST_ADDRRDI             0
#define DST_ONSTACK(     offs1) 0
#define DST_INVALUETABLE(offs4) 0
#ifndef LONGDOUBLE
#define DST_XMM(xmmReg)         0
#endif

#endif // IS64BIT

typedef int CodeLabel;
class CodeLabelPair {
public:
  const CodeLabel m_falseLabel;
  const CodeLabel m_trueLabel;
  inline CodeLabelPair(CodeLabel falseLabel, CodeLabel trueLabel)
    : m_falseLabel(falseLabel), m_trueLabel(trueLabel)
  {
  }
};

inline String labelToString(CodeLabel label) {
  return format(_T("L%d"), label);
}

class JumpFixup {
public:
  const OpcodeBase &m_op;
  bool              m_isShortJump;    // is jump-instruction short/near (IP-rel8/rel32)
  int               m_instructionPos; // index of first Byte of jmp-instruction in CodeArray
  int               m_jmpTo;          // index of BYTE to jump to
  BYTE              m_instructionSize;
  JumpFixup(const OpcodeBase &op, int pos, int jmpTo = 0)
    : m_op(op)
    , m_isShortJump(true)
    , m_instructionPos(pos)
    , m_jmpTo(jmpTo)
    , m_instructionSize(0)
  {
  }
  InstructionBase makeInstruction() const;
};

class JumpList : public CodeLabelPair {
public:
  CompactIntArray m_trueJumps;
  CompactIntArray m_falseJumps;
  JumpList(const CodeLabelPair &clp) : CodeLabelPair(clp) {
  }
  JumpList(CodeLabel falseLabel, CodeLabel trueLabel) : CodeLabelPair(falseLabel, trueLabel) {
  }
};

#ifdef IS32BIT
class MachineCode;

class FunctionCall {
public:
  int              m_pos;              // index of address in Machinecode
  BYTE             m_instructionSize;
  BuiltInFunction  m_func;             // 4 ip-rel
  inline FunctionCall() : m_pos(0), m_instructionSize(0), m_func(NULL) {
  }
  inline FunctionCall(int pos, BYTE insSize, const BuiltInFunction f)
    : m_pos(pos)
    , m_instructionSize(insSize)
    , m_func(f)
  {
  }
  InstructionBase makeInstruction(const MachineCode *code) const;
};
#endif // IS32BIT
