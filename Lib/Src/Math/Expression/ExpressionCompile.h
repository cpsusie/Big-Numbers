#pragma once
#include "CodeGeneration.h"

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

#ifdef IS64BIT
extern "C" {
  void callRealResultExpression(ExpressionEntryPoint ep, const void *rsiValue, Real &result);
  int  callIntResultExpression( ExpressionEntryPoint ep, const void *rsiValue);
};
#endif // IS64BIT

class MachineCode : public ExecutableByteArray {
private:
  DECLARECLASSNAME;
  ExpressionEntryPoint            m_entryPoint;
  void                           *m_esi;
  Array<JumpFixup>                m_jumpFixups;
  // Reference to first element in ParserTree::m_valueTable
  const CompactRealArray         &m_valueTable;
  // Offset in bytes, of esi/rsi from m_valueTable[0], when code is executing. 0 <= m_esiOffset < 128
  char                            m_esiOffset;
  FILE                           *m_listFile;
  int                             m_lastCodeSize;
  StringArray                     m_valueStr;     // for comments in listfile
  String                          m_insStr;
  const TCHAR                    *m_listComment;
#ifdef IS64BIT
  BYTE                            m_stackTop;
  BYTE                           *m_referenceFunction;
#endif // IS64BIT
  void changeShortJumpToNearJump(JumpFixup &jf);
  void finalJumpFixup();
  inline int getESIOffset(size_t valueIndex) const {
    if(valueIndex >= getValueCount()) {
      throwInvalidArgumentException(__TFUNCTION__, _T("valueIndex=%zu. #values=%zu"), valueIndex, getValueCount());
    }
    return (int)valueIndex * sizeof(Real) - m_esiOffset;
  }
  inline size_t esiOffsetToIndex(int offset) const {
    return (offset + m_esiOffset) / sizeof(Real);
  }

#ifdef IS32BIT
  CompactArray<FunctionCall>      m_callArray;
  void clearFunctionCalls() {
    m_callArray.clear();
  }
  void linkFunctionCalls();
  void linkFunctionCall(const FunctionCall &call);
  void adjustFunctionCalls(int pos, int bytesAdded);
#endif // IS32BIT
  MachineCode(const MachineCode &src);             // not implemented
  MachineCode &operator=(const MachineCode &src);  // not implemented
  int emitIns(const InstructionBase &ins);
  void initValueStr(const ExpressionVariableArray &variables);
  // return NULL if not comment found
  const TCHAR *findListComment(const InstructionOperand &op) const;
public:

#ifdef TRACE_CALLS
  bool m_callsGenerated;
#endif
  MachineCode(ParserTree &m_tree, FILE *listFile = NULL);
  ~MachineCode();
  void clear();
  int  addBytes(const void *bytes, int count);
  int  emit(const Opcode0Arg &opCode);
  int  emitJmpWithLabel(const OpcodeBase &opCode, CodeLabel label);
  int  emit(const OpcodeBase &opCode, const InstructionOperand &op);
  int  emit(const OpcodeBase &opCode, const InstructionOperand &op1, const InstructionOperand &op2);
  int  emit(const StringPrefix &prefix, const StringInstruction &strins);
  void emitCall(BuiltInFunction f);
  void emitCall(BuiltInFunction f, const ExpressionDestination &dst);
  inline void emitFSTP(const MemoryRef &mem) {
    emit(FSTP, RealPtr(mem));
  }
  inline void emitFLD(const MemoryRef &mem) {
    emit(FLD, RealPtr(mem));
  }
  void emitFLD(const ExpressionNode *n);
  inline MemoryRef getTableRef(int index) const {
    return TABLEREF_REG + getESIOffset(index);
  }
  inline MemoryRef getStackRef(int offset) const {
    return STACK_REG + offset;
  }
#ifndef LONGDOUBLE
  inline void emitFPUOpVal(const OpcodeBase &op, const ExpressionNode *n) {
    emit(op, RealPtr(getTableRef(n->getValueIndex())));
  }
#endif

#ifdef IS64BIT
  bool emitFLoad(const ExpressionNode *n, const ExpressionDestination &dst);
#ifndef LONGDOUBLE
  inline void emitXMMToMem(const XMMRegister &reg, const MemoryRef &mem) {
    emit(MOVSD1,MMWORDPtr(mem), reg);
  }
  inline void emitMemToXMM(const XMMRegister &reg, const MemoryRef &mem) {
    emit(MOVSD1,reg,MMWORDPtr(mem));
  }
#endif // !LONGDOUBLE
#endif // IS64BIT

  // if offset==0, emit(MOV dst,src); else emit(LEA dst,RealPtr(src+offset));
  void emitLoadAddr(const IndexRegister &dst, const MemoryRef &ref);
  // Return index in m_jumpFixups of new jump-instruction
  int emitJmp(const OpcodeBase &op, CodeLabel label);
  inline void fixupJump(int index, int jmpTo) {
    m_jumpFixups[index].m_jmpTo = jmpTo;
  }
  void fixupJumps(const JumpList &list, bool b);
  void clearJumpTable() {
    m_jumpFixups.clear();
  }
  void setValueCount(size_t valueCount);
  inline size_t getValueCount() const {
    return m_valueTable.size();
  }
  inline void emitAddReg(const GPRegister &reg, int  value) {
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
  inline const BYTE *getReferenceFunction() const {
    return m_referenceFunction;
  }
#endif // IS64BIT

  void finalize();
  void list(const TCHAR *format,...);
  void listIns(const TCHAR *format,...);
  inline void listLabel(CodeLabel label) {
    if(isListFileOpen()) list(_T("%s:\n"), labelToString(label).cstr());
  }
  inline bool isListFileOpen() const {
    return m_listFile != NULL;
  }
  inline Real evaluateReal() const {
    Real result;
#ifdef IS32BIT
    const ExpressionEntryPoint ep    = m_entryPoint;
    const void                *daddr = m_esi;
    __asm {
      push esi
      mov  esi, daddr
      call ep
      pop  esi
      fstp result;
    }
#else // IS64BIT
    callRealResultExpression(m_entryPoint, m_esi, result);
#endif // IS64BIT
    return result;
  }

  inline bool evaluateBool() const {
#ifdef IS32BIT
    const ExpressionEntryPoint ep    = m_entryPoint;
    const void                *daddr = m_esi;
    int result;
    __asm {
      push esi
      mov  esi, daddr
      call ep
      pop  esi
      mov result, eax
    }
    return result ? true : false;
#else  // IS64BIT
    return callIntResultExpression(m_entryPoint, m_esi) ? true : false;
#endif // IS64BIT
  }
};

class CodeGenerator {
private:
  ParserTree             &m_tree;
  const TrigonometricMode m_trigonometricMode;
  MachineCode            *m_code;
  int                     m_nextLbl;
#ifdef IS64BIT
  bool                    m_hasCalls;
#endif // IS64BIT

  inline TrigonometricMode getTrigonometricMode() const {
    return m_trigonometricMode;
  }

  inline MemoryRef getTableRef(const ExpressionNode *n) {
    return m_code->getTableRef(n->getValueIndex());
  }

  // Code generation (compile to machinecode)
  void genMachineCode();
  void genProlog();
  void genEpilog();
  void genStatementList(       const ExpressionNode *n);
  void genReturnBoolExpression(const ExpressionNode *n);
  void genExpression(          const ExpressionNode *n, const ExpressionDestination &dst);
  void genCall1Arg(            const ExpressionNode *arg                             , BuiltInFunction1    f, const ExpressionDestination &dst);
  void genCall1Arg(            const ExpressionNode *arg                             , BuiltInFunctionRef1 f, const ExpressionDestination &dst);
  void genCall2Arg(            const ExpressionNode *arg1, const ExpressionNode *arg2, BuiltInFunction2    f, const ExpressionDestination &dst);
  void genCall2Arg(            const ExpressionNode *arg1, const ExpressionNode *arg2, BuiltInFunctionRef2 f, const ExpressionDestination &dst);

#ifdef IS64BIT
#ifdef LONGDOUBLE
#define ALLARGS_BYREF
#endif
#endif

  void genCall(const ExpressionNode *n, BuiltInFunctionRef1 f, const ExpressionDestination &dst) {
    genCall1Arg(n->left(), f, dst);
  }
  void genCall(const ExpressionNode *n, BuiltInFunctionRef2 f, const ExpressionDestination &dst) {
    genCall2Arg(n->left(), n->right(), f, dst);
  }
#ifndef ALLARGS_BYREF
  void genCall(const ExpressionNode *n, BuiltInFunction1 f, const ExpressionDestination &dst) {
    genCall1Arg(n->left(), f, dst);
  }
  void genCall(const ExpressionNode *n, BuiltInFunction2 f, const ExpressionDestination &dst) {
    genCall2Arg(n->left(), n->right(), f, dst);
  }
#else // ALLARGS_BYREF
  void genCall(const ExpressionNode *n, BuiltInFunction1 f, const ExpressionDestination &dst) {
    genCall(n, (BuiltInFunctionRef1)f, dst);
  }
  void genCall(const ExpressionNode *n, BuiltInFunction2 f, const ExpressionDestination &dst) {
    genCall(n, (BuiltInFunctionRef2)f, dst);
  }
#endif // ALLARGS_BYREF

  void     genPolynomial(       const ExpressionNode *n, const ExpressionDestination &dst);
  void     genIf(               const ExpressionNode *n, const ExpressionDestination &dst);
  void     genPowMultSequence(  UINT y);


#ifdef IS32BIT
  int      genPush(             const ExpressionNode *n);
  int      genPushRef(          const ExpressionNode *n, int index);
  int      genPushReal(         const Real           &x);
  int      genPushReturnAddr();
  int      genPushInt(int n);
  int      genPush(             const void           *p, UINT size); // return size
  int      genPushRef(          const void           *p);
#else // IS64BIT
  void     genSetParameter(     const ExpressionNode *n, int index);
  void     genSetRefParameter(  const ExpressionNode *n, int index);
  BYTE     genSetRefParameter(  const ExpressionNode *n, int index, bool &savedOnStack);
  void     emitCall(            BuiltInFunction f, const ExpressionDestination &dst);
#endif // IS64BIT

  void     genAssignment(       const ExpressionNode *n);
  void     genIndexedExpression(const ExpressionNode *n);
  void     genBoolExpression(   const ExpressionNode *n, JumpList &jumpList);
  inline CodeLabel nextLabel() {
    return m_nextLbl++;
  }
  inline CodeLabelPair getLabelPair() {
    return CodeLabelPair(nextLabel(), nextLabel());
  }
  void throwInvalidTrigonometricMode();

public:
  CodeGenerator(ParserTree *tree, TrigonometricMode trigonometricMode, FILE *listFile = NULL);
  const MachineCode *getCode() const {
    return m_code;
  }
};
