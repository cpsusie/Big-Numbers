#pragma once
#include <Math/MathLib.h>
#include <Math/Expression/ParserTree.h>
#include <ExecutableByteArray.h>
#include "OpCode.h"

typedef void (*BuiltInFunction)();
typedef BuiltInFunction ExpressionEntryPoint;
typedef Real (*BuiltInFunctionRef1)(const Real &x);
typedef Real (*BuiltInFunctionRef2)(const Real &x, const Real &y);
typedef Real (*BuiltInFunction1)(Real x);
typedef Real (*BuiltInFunction2)(Real x, Real y);

#ifndef LONGDOUBLE
#define FLD_REAL           FLD_QWORD
#define FSTP_REAL          FSTP_QWORD
#else // LONGDOUBLE
#define FLD_REAL           FLD_TBYTE
#define FSTP_REAL          FSTP_TBYTE
#endif // LONGDOUBLE

#define FSTP_REAL_PTR_ESP  MEM_ADDR_PTR(FSTP_REAL,ESP,0)

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

class MemoryReference {
public:
  // index of address in Machinecode
  int             m_byteIndex;
  // 4/8 byte absolute address (depending on x86/x64 mode)
  const BYTE     *m_memAddr;
  inline MemoryReference() {
  }
  inline MemoryReference(int byteIndex, const BYTE *memAddr) : m_byteIndex(byteIndex), m_memAddr(memAddr) {
  }
};

class JumpFixup {
public:
  bool m_isShortJump;
  int  m_addr;
  int  m_jmpAddr;
  JumpFixup() {
  }
  JumpFixup(int addr, int jmpAddr) : m_isShortJump(true), m_addr(addr), m_jmpAddr(jmpAddr) {
  }
};

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

class JumpList {
public:
  CompactIntArray trueJumps;
  CompactIntArray falseJumps;
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
