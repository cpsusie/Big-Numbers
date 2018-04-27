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

inline String labelToString(CodeLabel label) {
  return format(_T("L%d"), label);
}

class CodeLabelPair {
public:
  const CodeLabel m_falseLabel;
  const CodeLabel m_trueLabel;
  inline CodeLabelPair(CodeLabel falseLabel, CodeLabel trueLabel)
    : m_falseLabel(falseLabel), m_trueLabel(trueLabel)
  {
  }
  inline String toString() const {
    return format(_T("(falseLabel:%-3s, trueLabel:%-3s)")
                 ,labelToString(m_falseLabel).cstr()
                 ,labelToString(m_trueLabel).cstr());
  }
};

inline CodeLabelPair operator!(const CodeLabelPair &clp) {
  return CodeLabelPair(clp.m_trueLabel, clp.m_falseLabel);
}

class JumpFixup {
public:
  const OpcodeBase *m_op;
  bool              m_isShortJump;    // is jump-instruction short/near (IP-rel8/rel32)
  int               m_instructionPos; // index of first Byte of jmp-instruction in CodeArray
  int               m_jmpTo;          // index of BYTE to jump to
  CodeLabel         m_jmpLabel;
  BYTE              m_instructionSize;
  bool              m_fixed;
  JumpFixup()
    :m_op(NULL)
    ,m_isShortJump(false)
    ,m_instructionPos(-1)
    ,m_jmpTo(-1)
    ,m_jmpLabel(-1)
    ,m_instructionSize(0)
    ,m_fixed(false)

  {}

  JumpFixup(const OpcodeBase &op, int pos, CodeLabel label, int jmpTo = 0)
    :m_op(&op)
    ,m_isShortJump(true)
    ,m_instructionPos(pos)
    ,m_jmpTo(jmpTo)
    ,m_jmpLabel(label)
    ,m_instructionSize(0)
    ,m_fixed(false)
  {
  }
  InstructionBase makeInstruction() const;
  String toString() const;
};

class JumpList : public CodeLabelPair {
public:
  CompactIntArray m_trueJumps;
  CompactIntArray m_falseJumps;
  inline JumpList(const CodeLabelPair &clp) : CodeLabelPair(clp) {
  }
  inline JumpList(CodeLabel falseLabel, CodeLabel trueLabel) : CodeLabelPair(falseLabel, trueLabel) {
  }
  inline bool hasFalseJumps() const {
    return !m_falseJumps.isEmpty();
  }
  inline bool hasTrueJumps() const {
    return !m_trueJumps.isEmpty();
  }
  inline bool hasJumps(bool b) const {
    return b ? hasTrueJumps() : hasFalseJumps();
  }
  inline bool hasJumps() const {
    return hasFalseJumps() || hasTrueJumps();
  }
  inline CompactIntArray &getJumps(bool b) {
    return b ? m_trueJumps : m_falseJumps;
  }
  inline const CompactIntArray &getJumps(bool b) const {
    return b ? m_trueJumps : m_falseJumps;
  }
  inline JumpList &operator^=(const JumpList &jl) {
    getJumps(false).addAll(jl.getJumps(true ));
    getJumps(true ).addAll(jl.getJumps(false));
    return *this;
  }
  inline String toString() const {
    return __super::toString() + _T(": <")
         + m_falseJumps.toStringBasicType() + _T(",")
         + m_trueJumps.toStringBasicType()  + _T(">");
  }
};

class FunctionCall {
private:
  const TCHAR *m_paramStr;
public:
  const BuiltInFunction m_fp;
  const String          m_name;
  inline FunctionCall(const BuiltInFunction fp, const String &name, const TCHAR *paramStr)
    : m_fp(fp)
    , m_name(name)
    , m_paramStr(paramStr)
  {
  }
  inline FunctionCall(BuiltInFunction1 fp, const String &name)
    : m_fp((BuiltInFunction)fp)
    , m_name(name)
    , m_paramStr(_T("(real x)"))
  {
  }
  inline FunctionCall(BuiltInFunction2 fp, const String &name)
    : m_fp((BuiltInFunction)fp)
    , m_name(name)
    , m_paramStr(_T("(real x, real y)"))
  {
  }
  inline FunctionCall(BuiltInFunctionRef1 fp, const String &name)
    : m_fp((BuiltInFunction)fp)
    , m_name(name)
    , m_paramStr(_T("(real &x)"))
  {
  }
  inline FunctionCall(BuiltInFunctionRef2 fp, const String &name)
    : m_fp((BuiltInFunction)fp)
    , m_name(name)
    , m_paramStr(_T("(real &x, real &y)"))
  {
  }
  inline String toString() const {
    return format(_T("%-20s (%p)"), format(_T("%s%s"),m_name.cstr(),m_paramStr).cstr(), m_fp);
  }
};

class MachineCode;

class FunctionCallInfo : public FunctionCall {
public:
  int              m_pos;              // index of address in Machinecode
  BYTE             m_instructionSize;
  inline FunctionCallInfo(const FunctionCall &fc, int pos, BYTE insSize)
    : FunctionCall(fc)
    , m_pos(pos)
    , m_instructionSize(insSize)
  {
  }
  InstructionBase makeInstruction(const MachineCode *code) const;
  String toString() const {
    return format(_T("%4d %-40s  (size:%d)")
                 ,m_pos, __super::toString().cstr()
                 ,m_instructionSize
                 );
  }
};
