#pragma once

#include <OpCode.h>
#include <CompactHashMap.h>
#include <Math/MathLib.h>
#include <Math/Expression/MachineCode.h>
#include "ListFile.h"

namespace Expr {

#ifdef IS64BIT
#ifndef LONGDOUBLE
#define USEXMMREG 1
#endif // LONGDOUBLE
#endif

#ifdef IS64BIT
typedef enum {
  RESULT_IN_FPU
 ,RESULT_IN_ADDRRDI
 ,RESULT_ON_STACK
 ,RESULT_IN_VALUETABLE
#ifdef USEXMMREG
 ,RESULT_IN_XMM  // XMM-register cannot be used for 80-bit floating points
#endif // USEXMMREG
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
#ifdef USEXMMREG
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
#ifdef USEXMMREG
  inline const XMMRegister &getXMMReg() const {
    assert(m_type == RESULT_IN_XMM);
    return *m_register;
  }
#endif
};

#define DST_FPU                 ,ExpressionDestination(RESULT_IN_FPU       , -1     )
#define DST_ADDRRDI             ,ExpressionDestination(RESULT_IN_ADDRRDI   , -1     )
#define DST_ONSTACK(offs1)      ,ExpressionDestination(RESULT_ON_STACK     , offs1  )
#define DST_INVALUETABLE(index) ,ExpressionDestination(RESULT_IN_VALUETABLE, index  )
#ifdef USEXMMREG
#define DST_XMM(xmmReg)         ,ExpressionDestination(xmmReg )
#endif
#define DCL_DSTPARAM            ,const ExpressionDestination &dst
#define DST_PARAM               ,dst

#else // IS32BIT

#define DCL_DSTPARAM
#define DST_PARAM
#define DST_FPU
#define DST_ADDRRDI

#endif // IS32BIT

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

class LabelGenerator {
private:
  CodeLabel m_next;
public:
  inline LabelGenerator() : m_next(0) {
  }
  inline CodeLabel nextLabel() {
    return m_next++;
  }
  inline CodeLabelPair nextLabelPair() {
    return CodeLabelPair(nextLabel(), nextLabel());
  }
};

class JumpFixup {
public:
  const OpcodeBase *m_op;
  bool              m_isShortJump;    // is jump-instruction short/near (IP-rel8/rel32)
  UINT              m_instructionPos; // index of first Byte of jmp-instruction in CodeArray
  UINT              m_jmpTo;          // index of BYTE to jump to
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
  {
  }

  JumpFixup(const OpcodeBase &op, UINT pos, CodeLabel label, UINT jmpTo, BYTE instructionSize)
    :m_op(&op)
    ,m_isShortJump(true)
    ,m_instructionPos(pos)
    ,m_jmpTo(jmpTo)
    ,m_jmpLabel(label)
    ,m_instructionSize(instructionSize)
    ,m_fixed(false)
  {
  }
  InstructionBase makeInstruction() const;
  String toString() const;
};

class JumpList : public CodeLabelPair {
public:
  CompactUintArray m_trueJumps;
  CompactUintArray m_falseJumps;
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
  inline CompactUintArray &getJumps(bool b) {
    return b ? m_trueJumps : m_falseJumps;
  }
  inline const CompactUintArray &getJumps(bool b) const {
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

class FunctionCallInfo : public FunctionCall {
public:
  UINT              m_instructionPos;      // index of address in Machinecode
  BYTE              m_instructionSize;
  inline FunctionCallInfo(const FunctionCall &fc, UINT pos, BYTE insSize)
    : FunctionCall(fc)
    , m_instructionPos(pos)
    , m_instructionSize(insSize)
  {
  }
#ifdef IS32BIT
  InstructionBase makeInstruction(const MachineCode &code) const;
#endif
  String toString() const;
};

#ifndef LONGDOUBLE
#define RealPtr QWORDPtr
#else  // LONGDOUBLE
#define RealPtr TBYTEPtr
#endif // LONGDOUBLE

#ifdef IS32BIT
#define TABLEREF_REG ESI
#define STACK_REG    ESP
#else  // IS64BIT
#define TABLEREF_REG RSI
#define STACK_REG    RSP
#endif // IS64BIT

class FPUSlot {
private:
  int  m_valueIndex;
public:
  inline FPUSlot() {
    setEmpty();
  }
  inline bool isEmpty() const {
    return m_valueIndex == -1;
  }
  inline bool isMixed() const {
    return m_valueIndex == -2;
  }
  inline void setEmpty() {
    m_valueIndex = -1;
  }
  inline void setMixed() {
    m_valueIndex = -2;
  }
  inline void setValueIndex(BYTE index) {
    m_valueIndex = index;
  }
  String toString() const;
};

class FPUContent {
private:
  BYTE    m_topSlot;
  FPUSlot m_slot[8];
public:
  inline FPUContent() {
    m_topSlot = 8;
  }
  inline UINT getHeight() const {
    return 8 - m_topSlot;
  }
  inline bool isEmpty() const {
    return getHeight() == 0;
  }
  inline FPUSlot &ST(BYTE index) {
    assert(index < getHeight());
    return m_slot[m_topSlot+index-1];
  }
  inline const FPUSlot &ST(BYTE index) const {
    assert(index < getHeight());
    return m_slot[m_topSlot+index-1];
  }
  inline void push(BYTE valueIndex) {
    assert(getHeight() < 8);
    m_topSlot--;
    ST(0).setValueIndex(valueIndex);
  }
  inline void pushMixed() {
    push(0);
    setMixed(0);
  }
  inline void setMixed(BYTE index) { // index=0 is ST(0)., etc
    ST(index).setMixed();
  }
  void pop(BYTE count = 1) {
    assert(count <= getHeight());
    m_topSlot += count;
  }
  void fxch(const FPURegister &reg) {
    const UINT index = reg.getIndex();
    assert(index <= getHeight());
    const FPUSlot slot0 = ST(0); ST(0) = ST(index); ST(index) = slot0;
  }
  String toString() const;
};

class InstructionInfo {
  const UINT m_pos           : 25;
  const UINT m_size          : 4;   // [1..15];
  const INT  m_FPUStackDelta : 3;   // [-2..1];
public:
  InstructionInfo(UINT pos, const InstructionBase &ins)
    : m_pos(pos)
    , m_size(ins.size())
    , m_FPUStackDelta(ins.getFPUStackDelta())
  {
  }
  inline UINT getPos() const {
    return m_pos;
  }
  inline UINT size() const {
    return m_size;
  }
  inline int getFPUStackDelta() const {
    return m_FPUStackDelta;
  }
};

class CodeGeneration {
private:
  MachineCode                      &m_code;
  CompactArray<JumpFixup>           m_jumpFixups;
  Array<FunctionCallInfo>           m_callTable;
  ValueAddressCalculation           m_addressTable;
  FPUContent                        m_FPUContent;
  ListFile                          m_listFile;
  bool                              m_listEnabled;

#ifdef IS64BIT
  BYTE                              m_stackTop;
  int                               m_functionTableStart; // Offset in bytes from code[0] to first function-reference
  typedef CompactKeyType<BuiltInFunction> FunctionKey;
  CompactHashMap<FunctionKey, UINT> m_fpMap;              // Value is index into m_uniqueFunctionCall, not offset
  Array<FunctionCall>               m_uniqueFunctionCall;
  UINT getFunctionRefIndex(const FunctionCall &fc);
#endif // IS64BIT

  void changeShortJumpToNearJump(JumpFixup &jf);
  void finalJumpFixup();
  void insertZeroes(UINT pos, UINT count);
#ifdef IS32BIT
  void linkFunctionCall(const FunctionCallInfo &call);
#endif // IS32BIT
  void linkFunctionCalls();
  inline bool hasCalls() const {
    return !m_callTable.isEmpty();
  }
  InstructionInfo insertIns(UINT pos, const InstructionBase &ins);
  inline InstructionInfo emitIns(const InstructionBase &ins) {
    return insertIns(size(), ins);
  }
  void listFixupTable() const;
  void listCallTable() const;
  inline bool enableListing(bool enable) {
    const bool old = m_listEnabled;
    m_listEnabled = enable;
    return old;
  }
public:
  CodeGeneration(MachineCode *code, const CompactRealArray &valueTable, const StringArray &nameCommentArray, FILE *listFile);
  inline UINT size() const {
    return (UINT)m_code.size();
  }
  void list(const TCHAR *format,...) const;
  inline void listLabel(CodeLabel label) {
    if(listEnabled()) m_listFile.add(size(), label);
  }
  inline bool listEnabled() const {
    return m_listEnabled && m_listFile.isOpen();
  }

  // Returns bytes added/inserted
  InstructionInfo insert(   UINT pos, const Opcode0Arg    &opcode);
  InstructionInfo insert(   UINT pos, const OpcodeBase    &opcode, const InstructionOperand &arg);
  InstructionInfo insert(   UINT pos, const OpcodeBase    &opcode, const InstructionOperand &arg1, const InstructionOperand &arg2);
  InstructionInfo insert(   UINT pos, const StringPrefix  &prefix, const StringInstruction  &strins);
  InstructionInfo insertLEA(UINT pos, const IndexRegister &dst   , const MemoryOperand      &mem);
  // Returns index into m_jumpFixups. not index in code-array
  UINT insertJump(UINT pos, const OpcodeBase    &opcode, CodeLabel lbl);

  // Return index of instruction in byte array
  inline InstructionInfo emit(   const Opcode0Arg &opcode) {
    return insert(size(), opcode          );
  }
  inline InstructionInfo emit(   const OpcodeBase &opcode, const InstructionOperand &arg) {
    return insert(size(), opcode, arg      );
  }
  inline InstructionInfo emit(   const OpcodeBase &opcode, const InstructionOperand &arg1, const InstructionOperand &arg2) {
    return insert(size(), opcode, arg1, arg2);
  }
  inline InstructionInfo emit(   const StringPrefix &prefix, const StringInstruction &strins) {
    return insert(size(), prefix, strins  );
  }
  inline InstructionInfo emitLEAReal(const IndexRegister &dst, const MemoryRef &ref) {
    return insertLEA(size(), dst, RealPtr(ref));
  }
  // Returns index into m_jumpFixups. not index in code-array
  inline UINT emitJump(const OpcodeBase &opcode, CodeLabel lbl) {
    return insertJump(size(), opcode, lbl);
  }

#ifdef IS32BIT
  UINT emitCall(const FunctionCall &fc);
#else // IS64BIT
private:
  UINT emitCall(const FunctionCall &fc);
public:
  UINT emitCall(const FunctionCall &fc, const ExpressionDestination &dst);
#endif // IS64BIT

  inline InstructionInfo emitFSTP(const MemoryRef &mem) {
    return emit(FSTP, RealPtr(mem));
  }
  inline InstructionInfo emitFLD( const MemoryRef &mem) {
    return emit(FLD, RealPtr(mem));
  }
  inline MemoryRef getTableRef(int index) const {
    return TABLEREF_REG + m_addressTable.getESIOffset(index);
  }
  inline MemoryRef getStackRef(int offset) const {
    return STACK_REG + offset;
  }
#ifdef USEXMMREG
  inline InstructionInfo emitXMMToMem(const XMMRegister &reg, const MemoryRef &mem) {
    return emit(MOVSD1,MMWORDPtr(mem), reg);
  }
  inline InstructionInfo emitMemToXMM(const XMMRegister &reg, const MemoryRef &mem) {
    return emit(MOVSD1,reg,MMWORDPtr(mem));
  }
#endif // USEXMMREG

  inline void emitAddReg(const GPRegister &reg, int value) {
    if(value == 0) return;
    emit(ADD,reg,value);
  }
  inline void emitSubReg(const GPRegister &reg, int value) {
    if(value == 0) return;
    emit(SUB,reg,value);
  }
  inline void emitAddStack(int n) {
    emitAddReg(STACK_REG,n);
  }
  inline void emitSubStack(int n) {
    emitSubReg(STACK_REG,n);
  }
  void fixupJumps(const JumpList &list, bool b);
  // if jmpTo==-1, then use m_code->size()
  CodeGeneration &fixupJump(UINT index, int jmpTo = -1);
#ifdef IS64BIT
  inline void resetStack(BYTE startOffset) {
    m_stackTop = startOffset;
  }
  inline BYTE pushTmp() {
    const BYTE offset = m_stackTop;
    m_stackTop += sizeof(Real);
    return offset;
  }
  inline BYTE popTmp() {
    m_stackTop -= sizeof(Real);
    return m_stackTop;
  }
#endif // IS64BIT

  void finalize();
};

}; // namespace Expr
