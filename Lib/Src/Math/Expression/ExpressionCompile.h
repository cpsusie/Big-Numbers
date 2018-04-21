#pragma once
#include "CodeGeneration.h"
#include "OpCode.h"

#ifndef LONGDOUBLE
#define FLD_REAL           FLD_QWORD
#define FSTP_REAL          FSTP_QWORD
#else // LONGDOUBLE
#define FLD_REAL           FLD_TBYTE
#define FSTP_REAL          FSTP_TBYTE
#endif // LONGDOUBLE

#define FSTP_REAL_PTR_ESP  MEM_ADDR_PTR(FSTP_REAL,ESP,0)

class MachineCode : public ExecutableByteArray {
private:
  DECLARECLASSNAME;
  ExpressionEntryPoint            m_entryPoint;
  void                           *m_esi;
  CompactArray<MemoryReference>   m_refenceArray;
  CompactArray<JumpFixup>         m_jumpFixups;
  // Reference to first element in ParserTree::m_valueTable
  const CompactRealArray         &m_valueTable;
  // Offset in bytes, of esi/rsi from m_valueTable[0], when code is executing. 0 <= m_esiOffset < 128
  char                            m_esiOffset;
#ifdef IS64BIT
  BYTE                            m_stackTop;
#endif // IS64BIT
  void changeShortJumpToNearJump(int addr);
  void fixupJumps();
  void fixupMemoryReference(const MemoryReference &ref);
  void adjustReferenceArray(int addr, int n);
  void linkReferences();
  MachineCode(const MachineCode &src);             // not implemented
  MachineCode &operator=(const MachineCode &src);  // not implemented
public:
  MachineCode(const CompactRealArray &valueTable);
  ~MachineCode();
  void clear();
  int  addBytes(const void *bytes, int count);
  int  emit(const IntelInstruction &ins);
  void emitCall(BuiltInFunction f, const ExpressionDestination &dst);
  void emitFLoad(       const ExpressionNode *n);
  void emitFStorePop(   int               index) { emitTableOp(FSTP_REAL   , index); }
  void emitFStorePop(   const ExpressionNode *n) { emitTableOp(FSTP_REAL   , n    ); }
#ifndef LONGDOUBLE
  void emitFComparePop( const ExpressionNode *n) { emitTableOp(FCOMP_QWORD , n    ); }
#endif
#ifdef IS64BIT
#ifndef LONGDOUBLE
  void emitXMM0ToAddr(  int               index) { emitTableOp(MOVSD_MMWORD_XMM(XMM0), index); }
#endif
  bool emitFLoad(       const ExpressionNode *n, const ExpressionDestination &dst);
#endif
  // return address of fixup address
  int  emitShortJmp(const IntelInstruction &ins);
  inline void fixupShortJump(int addr, int jmpAddr) {
    m_jumpFixups.add(JumpFixup(addr, jmpAddr));
  }
  void fixupShortJumps(const CompactIntArray &jumps, int jmpAddr);

  void emitStackOp(const IntelOpcode &op, int offset);
  inline void emitTableOp(const IntelOpcode &op, const ExpressionNode *n) {
    emitTableOp(op, n->getValueIndex());
  }
  void emitTableOp(const IntelOpcode &op, int index);
  void setValueCount(size_t valueCount);
  inline size_t getValueCount() const {
    return m_valueTable.size();
  }
  inline int getESIOffset(size_t valueIndex) const {
    if(valueIndex >= getValueCount()) {
      throwInvalidArgumentException(__TFUNCTION__, _T("valueIndex=%zu. #values=%zu"), valueIndex, getValueCount());
    }
    return (int)valueIndex * sizeof(Real) - m_esiOffset;
  }
#ifdef IS32BIT
  void emitAddESP(  int             n);
  void emitSubESP(  int             n);
#else // IS64BIT
  void resetStack(BYTE startOffset) { m_stackTop = startOffset; }
  BYTE pushTmp();
  BYTE popTmp();
  void emitAddRSP(  int               n);
  void emitSubRSP(  int               n);
  void emitAddR64(  const GPRegister &r64, int  value);
#endif // IS32BIT
  void   finalize();
  Real   evaluateReal() const;
  bool   evaluateBool() const;
};

class CodeGenerator {
private:
  ParserTree                     &m_tree;
  const TrigonometricMode         m_trigonometricMode;
  MachineCode                    *m_code;
#ifdef IS64BIT
  bool                            m_hasCalls;
#endif // IS64BIT

  inline TrigonometricMode getTrigonometricMode() const {
    return m_trigonometricMode;
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
  BYTE     genSetParameter(     const ExpressionNode *n, int index, bool saveOnStack);
  void     genSetRefParameter(  const ExpressionNode *n, int index);
  BYTE     genSetRefParameter(  const ExpressionNode *n, int index, bool &savedOnStack);
#endif // IS64BIT

  void     genAssignment(       const ExpressionNode *n);
  void     genIndexedExpression(const ExpressionNode *n);
  JumpList genBoolExpression(   const ExpressionNode *n);

  void throwInvalidTrigonometricMode();

public:
  CodeGenerator(ParserTree *tree, TrigonometricMode trigonometricMode);
  const MachineCode *getCode() const {
    return m_code;
  }
};
