#pragma once

#include <ByteArray.h>
#include "ExpressionParser.h"
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
  CompactArray<MemoryReference>   m_refenceArray;
  CompactArray<JumpFixup>         m_jumpFixups;
  // Number of entries in ParserTree::m_valueTable
  size_t                          m_valueCount;
  // Offset in bytes, of esi/rsi from m_valueTable[0], when code is executing. 0 <= m_esiOffset < 128
  char                            m_esiOffset;
#ifdef IS64BIT
  BYTE                            m_stackTop;
#endif // IS64BIT
  void changeShortJumpToNearJump(int addr);
  void adjustReferenceArray(int addr, int n);
public:
  MachineCode();
  MachineCode(const MachineCode &src);
  MachineCode &operator=(const MachineCode &src);
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
  void fixupJumps();
  void fixupMemoryReference(const MemoryReference &ref);
  void emitStackOp(const IntelOpcode &op, int offset);
  inline void emitTableOp(const IntelOpcode &op, const ExpressionNode *n) {
    emitTableOp(op, n->getValueIndex());
  }
  void emitTableOp(const IntelOpcode &op, int index);
  void setValueCount(size_t valueCount);
  inline size_t getValueCount() const {
    return m_valueCount;
  }
  inline char getESIOffset() const {
    return m_esiOffset;
  }
  inline int getESIOffset(size_t valueIndex) const {
    if(valueIndex >= m_valueCount) {
      throwInvalidArgumentException(__TFUNCTION__, _T("valueIndex=%zu. m_valueCount=%zu"), valueIndex, m_valueCount);
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
  void linkReferences();
  ExpressionEntryPoint getEntryPoint() const { return (ExpressionEntryPoint)getData(); }
};

class JumpList {
public:
  CompactIntArray trueJumps;
  CompactIntArray falseJumps;
};

typedef enum {
  EXPR_NORETURNTYPE
 ,EXPR_RETURN_REAL
 ,EXPR_RETURN_BOOL
} ExpressionReturnType;

typedef enum {
  EXPR_EMPTY
 ,EXPR_COMPILED
 ,EXPR_DERIVED
 ,EXPR_CANONICALFORM
 ,EXPR_MAINREDUCTION1
 ,EXPR_MAINREDUCTION2
 ,EXPR_RATIONALPOWERSREDUCTION
 ,EXPR_STANDARDFORM
 ,EXPR_REDUCTIONDONE
} ExpressionState;

class ExpressionTransformer {
public:
  ExpressionTransformer() {
  }
  virtual SNode           transform(SNode n) = 0;
  virtual ExpressionState getState() const = 0;
};

class Expression : public ParserTree {
private:
  DECLARECLASSNAME;
  bool                       m_machineCode;
  ExpressionEntryPoint       m_entryPoint;
  void                      *m_esi;
  MachineCode                m_code;
  ExpressionReturnType       m_returnType;
  mutable ExpressionState    m_state;
  mutable UINT               m_reduceIteration;
  TrigonometricMode          m_trigonometricMode;
#ifdef IS64BIT
  bool                       m_hasCalls;
#endif // IS64BIT

  friend class ExpressionNodeTree;
  friend class ExpressionPainter;
  friend class MainReducer;
  friend class MarkedNodeMultiplier;
  friend class RationalPowersReducer;

  void   parse(const String &expr);
  ExpressionReturnType findReturnType() const;
  void throwInvalidTrigonometricMode();
  static void throwUnknownSymbolException(  const TCHAR *method, SNode                 n);
  static void throwUnknownSymbolException(  const TCHAR *method, const ExpressionNode *n);
  void        throwInvalidSymbolForTreeMode(const TCHAR *method, const ExpressionNode *n) const;

#ifdef _DEBUG
  inline void checkReturnType(const TCHAR *method, ExpressionReturnType expectedReturnType) const {
    if(m_returnType != expectedReturnType) {
      throwException(_T("%s:Returntype=%d. exptected=%d\n"), method, m_returnType, expectedReturnType);
    }
  }
#define CHECKRETURNTYPE(expectedType) checkReturnType(__TFUNCTION__, expectedType)
#else
#define CHECKRETURNTYPE(expectedType)
#endif // _DEBUG

  // Evaluate tree nodes
  Real   evaluateStatementListReal(                        const ExpressionNode *n) const;
  bool   evaluateStatementListBool(                        const ExpressionNode *n) const;
  Real   evaluateRealExpr(                                 const ExpressionNode *n) const;
  bool   evaluateBoolExpr(                                 const ExpressionNode *n) const;
  Real   evaluateProduct(                                  const ExpressionNode *n) const;
  Real   evaluateSum(                                      const ExpressionNode *n) const;
  Real   evaluatePow(                                      const ExpressionNode *n) const;
  Real   evaluateRoot(                                     const ExpressionNode *n) const;
  Real   evaluatePolynomial(                               const ExpressionNode *n) const;
  void   getCoefficients(         CompactArray<Real> &dst, const ExpressionNodeArray &coefficientArray) const;
  void   doAssignment(                                     const ExpressionNode *n) const;
  void   print(                                            const ExpressionNode *n, FILE *f = stdout) const;

  // Evaluate by machinecode
  inline void *getESI() const {
    return (BYTE*)&getValueRef(0) + m_code.getESIOffset();
  }
  Real   fastEvaluateReal();
  bool   fastEvaluateBool();

  void clearMachineCode();
  // Code generation (compile to machinecode)
  void genMachineCode();
  void genProlog();
  void genEpilog();
  void genStatementList(                                   const ExpressionNode *n);
  void genReturnBoolExpression(                            const ExpressionNode *n);
  void genExpression(                                      const ExpressionNode *n, const ExpressionDestination &dst);
  void genCall1Arg(                                        const ExpressionNode *arg                             , BuiltInFunction1    f, const ExpressionDestination &dst);
  void genCall1Arg(                                        const ExpressionNode *arg                             , BuiltInFunctionRef1 f, const ExpressionDestination &dst);
  void genCall2Arg(                                        const ExpressionNode *arg1, const ExpressionNode *arg2, BuiltInFunction2    f, const ExpressionDestination &dst);
  void genCall2Arg(                                        const ExpressionNode *arg1, const ExpressionNode *arg2, BuiltInFunctionRef2 f, const ExpressionDestination &dst);

#ifdef IS64BIT
#ifdef LONGDOUBLE
#define ALLARGS_BYREF
#endif
#endif

  void genCall(      const ExpressionNode *n                               , BuiltInFunctionRef1 f, const ExpressionDestination &dst) {
    genCall1Arg(n->left(), f, dst);
  }
  void genCall(      const ExpressionNode *n                               , BuiltInFunctionRef2 f, const ExpressionDestination &dst) {
    genCall2Arg(n->left(), n->right(), f, dst);
  }
#ifndef ALLARGS_BYREF
  void genCall(      const ExpressionNode *n                               , BuiltInFunction1    f, const ExpressionDestination &dst) {
    genCall1Arg(n->left(), f, dst);
  }
  void genCall(      const ExpressionNode *n                               , BuiltInFunction2    f, const ExpressionDestination &dst) {
    genCall2Arg(n->left(), n->right(), f, dst);
  }
#else // ALLARGS_BYREF
  void genCall(      const ExpressionNode *n                               , BuiltInFunction1    f, const ExpressionDestination &dst) {
    genCall(n, (BuiltInFunctionRef1)f, dst);
  }
  void genCall(      const ExpressionNode *n                               , BuiltInFunction2    f, const ExpressionDestination &dst) {
    genCall(n, (BuiltInFunctionRef2)f, dst);
  }
#endif // ALLARGS_BYREF

  void genPolynomial(                                      const ExpressionNode *n  , const ExpressionDestination &dst);
  void genIf(                                              const ExpressionNode *n  , const ExpressionDestination &dst);
  void genPowMultSequence(UINT y);


#ifdef IS32BIT
  int  genPush(                                            const ExpressionNode *n);
  int  genPushRef(                                         const ExpressionNode *n, int index);
  int  genPushReal(                                        const Real           &x);
  int  genPushReturnAddr();
  int  genPushInt(int n);
  int  genPush(                                            const void           *p, UINT size); // return size
  int  genPushRef(                                         const void           *p);
#else // IS64BIT
  BYTE genSetParameter(                                    const ExpressionNode *n, int index, bool saveOnStack);
  void genSetRefParameter(                                 const ExpressionNode *n, int index);
  BYTE genSetRefParameter(                                 const ExpressionNode *n, int index, bool &savedOnStack);
#endif // IS64BIT

  void genAssignment(                                      const ExpressionNode *n);
  void genIndexedExpression(                               const ExpressionNode *n);
  JumpList genBoolExpression(                              const ExpressionNode *n);

  // Differentiation
  SNode                       D(                           SNode n, const String &name);
  SNode                       DPolynomial(                 SNode n, const String &name);
  SNode                       DStatementList(              SNode n, const String &name);

  // Conversion between Standard-, Canonical- and Numeric tree form
  SNode toSForm(               ExpressionNode *n);
  SNode toSFormSum(            ExpressionNode *n);
  SNode toSFormProduct(        ExpressionNode *n);
  SNode toSFormPow(            ExpressionNode *n);
  SNode toSFormPoly(           ExpressionNode *n);
  SNode toSFormTreeNode(       ExpressionNode *n);
  SNode toSFormFactorArray(    FactorArray    &a, bool changeExponentSign);

  SNode toCForm(               ExpressionNode *n);
  SNode toCFormSum(            ExpressionNode *n);
  SNode toCFormProduct(        ExpressionNode *n);
  SNode toCFormPoly(           ExpressionNode *n);
  SNode toCFormTreeNode(       ExpressionNode *n);
  AddentArray &toCFormSum(     AddentArray &result, ExpressionNode *n, bool positive);
  FactorArray &toCFormProduct( FactorArray &result, ExpressionNode *n, SNode &exponent);
  FactorArray &toCFormPower(   FactorArray &result, ExpressionNode *n, SNode &exponent);
  FactorArray &toCFormRoot(    FactorArray &result, ExpressionNode *n, SNode &exponent);

  SNode toNForm(               ExpressionNode *n);
  SNode toNFormBoolExp(        ExpressionNode *n);
  SNode toNFormRealExp(        ExpressionNode *n);
  SNode toNFormSum(            ExpressionNode *n);
  SNode toNFormProduct(        ExpressionNode *n);
  SNode toNFormPoly(           ExpressionNode *n);
  SNode toNFormTreeNode(       ExpressionNode *n);

  // Reduction
  void  iterateTransformation(ExpressionTransformer &transformer);

  SNode                   reduce(                      SNode             n);
  SNode                   reduceBoolExp(               SNode             n);
  SNode                   reduceRealExp(               SNode             n);
  SNode                   reduceTreeNode(              SNode             n);
  SNode                   reduceSum(                   SNode             n);
  SNode                   reduceProduct(               SNode             n);
  SNode                   reduceModulus(               SNode             n);

  SNode                   reducePower(                 SNode             n);
  SNode                   reduceConstantFactors(       FactorArray            &factors);
  SNode                   reduceRationalPower(         const Rational         &base, const Rational &exponent);
  bool                    reducesToRationalConstant(   SNode             n, Rational *r = NULL);
  bool                    reducesToRational(           SNode             n, Rational *r = NULL);
  SNode                   multiplyParentheses(         SNode             n);
  SNode                   multiplyParenthesesInSum(    SNode             n);
  SNode                   multiplyParenthesesInProduct(SNode             n);
  SNode                   multiplyParenthesesInPoly(   SNode             n);
  SNode                   multiplyTreeNode(            SNode             n);
  SNode                   multiply(                    ExpressionFactor *a, ExpressionNodeSum *s);

  SNode                   factorsToNode(               FactorArray      &a);
  SNode                   addentsToNode(               AddentArray      &a);
  SumElement             *getCommonFactor(             SumElement       &e1, SumElement &e2);
  FactorArray            &getFactors(                  FactorArray &result,   const SNode            n);
  FactorArray            &getFactors(                  FactorArray &result,   const SNode            n,        const SNode exponent);
  FactorArray            &getFactorsInPower(           FactorArray &result,   const SNode            n,        const SNode exponent);
  FactorArray            &multiplyExponents(           FactorArray &result,   FactorArray           &factors,  const SNode exponent);
  SNode                   changeFirstNegativeFactor(   SNode             n);
  SNode                   reduceLn(                    SNode             n);
  SNode                   reduceLog10(                 SNode             n);
  SNode                   getPowerOfE(                 SNode             n);
  SNode                   getPowerOf10(                SNode             n);
  SumElement             *mergeLogarithms(             SumElement             &e1, SumElement &e2);
  SNode                   reduceAsymmetricFunction(    SNode             n);
  SNode                   reduceSymmetricFunction(     SNode             n);
  SNode                   reducePolynomial(            SNode             n);
  ExpressionFactor       *reduceTrigonometricFactors(  ExpressionFactor       &f1, ExpressionFactor &f2);

  bool                    canUseIdiotRule(             SNode             n1, const SNode n2);
  bool                    canUseReverseIdiotRule(      SumElement       *e1,       SumElement *e2, SumElement* &result);
  bool                    isSquareOfSinOrCos(          SNode             n);
  bool                    sameLogarithm(               SNode             n1, const SNode n2);
  static bool             rationalExponentsMultiply(   const Rational         &r1, const Rational &r2);

#ifdef __NEVER__
  const ExpressionNode   *replaceRationalPowers(       const ExpressionNode   *n) const;
  const ExpressionNode   *replaceRationalPower(        const ExpressionNode   *n) const;
  const ExpressionFactor *replaceRationalPower(        const ExpressionFactor *f) const;
  const ExpressionNode   *replaceRationalFactors(      const ExpressionNode   *n) const;
  const ExpressionNode   *substituteName(              const ExpressionNode   *n,  const String &from, const String &to) const;
  const ExpressionNode   *normalizePolynomial(         const ExpressionNode   *n) const;
#endif

  // Properties
  void setState(ExpressionState           newState   );
  void setReduceIteration(UINT            iteration  );
  void setReturnType(ExpressionReturnType returnType );
  void setMachineCode(bool                machinecode);
public:
  Expression(TrigonometricMode mode = RADIANS);
  Expression(const Expression &src);
  Expression &operator=(const Expression &src);
  Expression getDerived(const String &name, bool reduceResult = true) const;
  void   compile(const String &expr, bool machineCode);
  inline ExpressionReturnType getReturnType() const {
    return m_returnType;
  }
  inline bool isMachineCode() const {
    return m_machineCode;
  }
  inline Real evaluate() {
    CHECKRETURNTYPE(EXPR_RETURN_REAL);
    return isMachineCode() ? fastEvaluateReal() : evaluateStatementListReal(getRoot());
  }
  inline bool evaluateBool() {
    CHECKRETURNTYPE(EXPR_RETURN_BOOL);
    return isMachineCode() ? fastEvaluateBool() : evaluateStatementListBool(getRoot());
  }

  void checkIsStandardForm();
  void checkIsCanonicalForm();

  void clear();
  void reduce();

  bool treesEqual(     const ExpressionNode *n1, const ExpressionNode *n2);
  bool treesEqualMinus(SNode n1, SNode n2);

  bool equalMinus(Expression &e) {
    return treesEqualMinus(getRoot(), e.getRoot());
  }

  void setTrigonometricMode(TrigonometricMode mode);

  inline TrigonometricMode getTrigonometricMode() const {
    return m_trigonometricMode;
  }

  inline ExpressionState getState() const {
    return m_state;
  }

  inline UINT getReduceIteration() const {
    return m_reduceIteration;
  }

  // return this
  Expression &toCanonicalForm();
  // return this
  Expression &toStandardForm();
  // return this
  Expression &toNumericForm();
  // return this
  Expression &expandMarkedNodes();
  // return this
  Expression &multiplyMarkedNodes();

  String toString() const;
  void print(FILE *f = stdout) const;
};

class ExpressionDescription {
private:
  const TCHAR *m_syntax;
  const TCHAR *m_description;

  static const ExpressionDescription expressionHelpList[];
  static const int helpListSize;

  ExpressionDescription(const TCHAR *syntax, const TCHAR *description);
public:
  const TCHAR *getSyntax() const {
    return m_syntax;
  }

  const TCHAR *getDescription() const {
    return m_description;
  }

  static const ExpressionDescription *getHelpList() {
    return expressionHelpList;
  }

  static int getHelpListSize() {
    return helpListSize;
  }
};
