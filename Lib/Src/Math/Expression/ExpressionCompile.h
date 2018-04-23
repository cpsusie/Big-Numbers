#pragma once
#include "CodeGeneration.h"

#ifndef LONGDOUBLE
#define RealPtr QWORDPtr
#define FADD_Real8Ptr( arg)  FADD( QWORDPtr(arg))
#define FSUB_Real8Ptr( arg)  FSUB( QWORDPtr(arg))
#define FSUBR_Real8Ptr( arg) FSUBR(QWORDPtr(arg))
#define FMUL_Real8Ptr( arg)  FMUL( QWORDPtr(arg))
#define FDIV_Real8Ptr( arg)  FDIV( QWORDPtr(arg))
#define FDIVR_Real8Ptr(arg)  FDIVR(QWORDPtr(arg))
#define FCOMP_Real8Ptr(arg)  FCOMP(QWORDPtr(arg))
#else  // LONGDOUBLE
#define RealPtr TBYTEPtr
#endif // LONGDOUBLE

#define FLD_REAL( arg)      FLD( RealPtr(arg))
#define FSTP_REAL(arg)      FSTP(RealPtr(arg))

#ifdef IS32BIT
#define TABLEREF_REG ESI
#define STACK_REG    ESP
#else  // IS64BIT
#define TABLEREF_REG RSI
#define STACK_REG    RSP
#endif // IS64BIT

#define EMITTABLEOP(memPtrOp, index ) emit(memPtrOp(TABLEREF_REG + getESIOffset(index)))
#define EMITSTACKOP(memPtrOp, offset) emit(memPtrOp(STACK_REG    + (offset)))


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
#ifdef IS64BIT
  BYTE                            m_stackTop;
  BYTE                           *m_referenceFunction;
#endif // IS64BIT
  void changeShortJumpToNearJump(JumpFixup &jf);
  void fixupJumps();
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
public:

#ifdef TRACE_CALLS
  bool m_callsGenerated;
#endif
  MachineCode(const CompactRealArray &valueTable);
  ~MachineCode();
  void clear();
  int  addBytes(const void *bytes, int count);
  int  emit(const InstructionBase &ins);
  void emitCall(BuiltInFunction f);
  void emitCall(BuiltInFunction f, const ExpressionDestination &dst);
  void emitFLoad(   const ExpressionNode *n);
  inline void emitFPopVal(int index) {
    EMITTABLEOP(FSTP_REAL, index);
  }
  inline void emitFPopVal( const ExpressionNode *n) {
    emitFPopVal(n->getValueIndex());
  }
#ifndef LONGDOUBLE
  void emitFAddVal( const ExpressionNode *n) { EMITTABLEOP(FADD_Real8Ptr , n->getValueIndex()); }
  void emitFSubVal( const ExpressionNode *n) { EMITTABLEOP(FSUB_Real8Ptr , n->getValueIndex()); }
  void emitFSubRVal(const ExpressionNode *n) { EMITTABLEOP(FSUBR_Real8Ptr, n->getValueIndex()); }
  void emitFMulVal( const ExpressionNode *n) { EMITTABLEOP(FMUL_Real8Ptr , n->getValueIndex()); }
  void emitFDivVal( const ExpressionNode *n) { EMITTABLEOP(FDIV_Real8Ptr , n->getValueIndex()); }
  void emitFDivRVal(const ExpressionNode *n) { EMITTABLEOP(FDIVR_Real8Ptr, n->getValueIndex()); }
  void emitFCompVal(const ExpressionNode *n) { EMITTABLEOP(FCOMP_Real8Ptr, n->getValueIndex()); }
#endif
#ifdef IS64BIT
#ifndef LONGDOUBLE
  void emitXMMToVal(const XMMRegister &reg, int index) {
    emit(MOVSD1(MMWORDPtr(TABLEREF_REG + getESIOffset(index)), reg));
  }
  void emitValToXMM(const XMMRegister &reg, int index) {
    emit(MOVSD1(reg, MMWORDPtr(TABLEREF_REG + getESIOffset(index))));
  }
  void emitXMMToStack(const XMMRegister &reg, int offset) {
    emit(MOVSD1(MMWORDPtr(STACK_REG + offset), reg));
  }
  void emitStackToXMM(const XMMRegister &reg, int offset) {
    emit(MOVSD1(reg, MMWORDPtr(STACK_REG + offset)));
  }
#endif // !LONGDOUBLE
  bool emitFLoad(   const ExpressionNode *n, const ExpressionDestination &dst);
#endif // IS64BIT

  // Return index in m_jumpFixups of new jump-instruction
  int emitJmp(const OpcodeBase &op);
  inline void fixupJump(int index, int jmpTo) {
    m_jumpFixups[index].m_jmpTo = jmpTo;
  }
  void fixupJumps(const CompactIntArray &jumps, int jmpTo);
  void clearJumpTable() {
    m_jumpFixups.clear();
  }

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
  void emitSubR64(  const GPRegister &r64, int  value);
  const BYTE *getReferenceFunction() const {
    return m_referenceFunction;
  }
#endif // IS64BIT

  void finalize();
  void dump(const String &fname, const String &title) const;

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
#ifdef IS64BIT
  bool                    m_hasCalls;
#endif // IS64BIT

  inline TrigonometricMode getTrigonometricMode() const {
    return m_trigonometricMode;
  }

  inline int getESIOffset(size_t valueIndex) const {
    return m_code->getESIOffset(valueIndex);
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
  void     emitCall(            BuiltInFunction f, const ExpressionDestination &dst);
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
