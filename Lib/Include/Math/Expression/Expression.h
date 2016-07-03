#pragma once

#include <ByteArray.h>
#include <Math/MathFunctions.h>
#include "ExpressionParser.h"
#include "OpCodes.h"

typedef void (*BuiltInFunction)();
typedef BuiltInFunction ExpressionEntryPoint;
typedef Real (*BuiltInFunctionRef1)(const Real &x);
typedef Real (*BuiltInFunctionRef2)(const Real &x, const Real &y);
typedef Real (*BuiltInFunction1)(Real x);
typedef Real (*BuiltInFunction2)(Real x, Real y);

//#define TEST_MACHINECODE

#ifndef LONGDOUBLE

#define FLD_REAL_PTR_DS    MEM_ADDR_DS(FLD_QWORD  )
#define FSTP_REAL_PTR_DS   MEM_ADDR_DS(FSTP_QWORD )
#define FSTP_REAL_PTR_ESP  MEM_ADDR_ESP(FSTP_QWORD)

#else

#define FLD_REAL_PTR_DS    MEM_ADDR_DS(FLD_TBYTE  )
#define FSTP_REAL_PTR_DS   MEM_ADDR_DS(FSTP_TBYTE )
#define FSTP_REAL_PTR_ESP  MEM_ADDR_ESP(FSTP_TBYTE)

#endif // LONGDOUBLE

class ExternalReference {
public:
  int             m_addr;
  BuiltInFunction m_f;
  inline ExternalReference() {
  }
  inline ExternalReference(int addr, BuiltInFunction f) : m_addr(addr), m_f(f) {
  }
};

class MachineCode : public ExecutableByteArray {
private:
  DECLARECLASSNAME;
  CompactArray<ExternalReference> m_externals;
#ifdef IS32BIT
  Real                            m_tmpVar[3];
#else
  BYTE                            m_stackTop;
#endif // IS32BIT
public:
  MachineCode();
  MachineCode(const MachineCode &src);
  MachineCode &operator=(const MachineCode &src);
  ~MachineCode();
  int  addBytes(const void *bytes, int count);
  void setBytes(int addr, const void *bytes, int count);
  int  emit(const IntelInstruction &ins);
  void emitCall(BuiltInFunction f);
  void emitFLoad(    const ExpressionNode *n) { emitBinaryOp(FLD_REAL_PTR_DS          , n); }
  void emitFLoad(    const Real           *x) { emitBinaryOp(FLD_REAL_PTR_DS          , x); }
  void emitFAdd(     const Real           *x) { emitBinaryOp(MEM_ADDR_DS(FADD_QWORD)  , x); }
  void emitFStorePop(const Real           *x) { emitBinaryOp(FSTP_REAL_PTR_DS         , x); }
  void emitFStorePop(const ExpressionNode *n) { emitBinaryOp(FSTP_REAL_PTR_DS         , n); }
  void emitFCompare( const ExpressionNode *n) { emitBinaryOp(MEM_ADDR_DS(FCOMP_QWORD) , n); }
  void emitTestAH(  BYTE                  n);
  void emitTestEAX( unsigned long         n);
  void addImmediateAddr(const void *addr);
  int  emitShortJmp(const IntelInstruction &ins);  // return address of fixup address
  void fixupShortJump(int addr, int jmpAddr);
  void fixupShortJumps(const CompactIntArray &jumps, int jmpAddr);
  void fixupCall(const ExternalReference &ref);
  void emitBinaryOp(const IntelInstruction &ins, const ExpressionNode *n) { emitBinaryOp(ins,getValueAddr(n)); }
  void emitBinaryOp(const IntelInstruction &ins, const void *p);
#ifdef IS32BIT
  void emitAddESP(  int                   n);
  void emitSubESP(  int                   n);
  Real &getTmpVar(int index) { return m_tmpVar[index]; }
#else
  void resetStack(BYTE startOffset) { m_stackTop = startOffset; }
  BYTE pushTmp();
  BYTE popTmp();
  void emitAddRSP(  int                   n);
  void emitSubRSP(  int                   n);
#endif // IS32BIT
  const Real *getValueAddr(const ExpressionNode *n) const;
  void linkExternals();
  ExpressionEntryPoint getEntryPoint() const { return (ExpressionEntryPoint)getData(); }

#ifdef TEST_MACHINECODE
  void genTestSequence();
#endif
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
  MachineCode                m_code;
  ExpressionReturnType       m_returnType;
  mutable ExpressionState    m_state;
  mutable unsigned int       m_reduceIteration;
  TrigonometricMode          m_trigonometricMode;

  friend class ExpressionNodeTree;
  friend class ExpressionPainter;
  friend class MainReducer;
  friend class MarkedNodeMultiplier;
  friend class RationalPowersReducer;

  void   parse(const String &expr);
  ExpressionReturnType findReturnType() const;
  void throwInvalidTrigonometricMode();
  static void throwUnknownSymbolException(const TCHAR *method, const SNode           n);
  static void throwUnknownSymbolException(const TCHAR *method, const ExpressionNode *n);

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
  Real   fastEvaluate();
  bool   fastEvaluateBool();

  // Code generation (compile to machinecode)
  void genCode();
  void genProlog();
  void genEpilog();
  void genCode(                                            const ExpressionNode *n);
  void genStatementList(                                   const ExpressionNode *n);
  void genExpression(                                      const ExpressionNode *n);
  void genReturnBoolExpression(                            const ExpressionNode *n);
#ifdef IS32BIT
  int  genPush(                                            const ExpressionNode *n);
  int  genPushRef(                                         const ExpressionNode *n, int index);
  int  genPushReal(                                        const Real           &x);
  int  genPushReturnAddr();
  int  genPushInt(int n);
  int  genPush(                                            const void           *p, unsigned int size); // return size
  int  genPushRef(                                         const void           *p);
#else
  BYTE genSetParameter(                                    const ExpressionNode *n, int index, bool saveOnStack);
  BYTE genSetRefParameter(                                 const ExpressionNode *n, int index, bool &savedOnStack);
  BYTE genSetIntParameter(                                 int n                  , int index, bool saveOnStack);
#endif // IS32BIT
  void genCall(                                            const ExpressionNode *n, BuiltInFunction1    f);
  void genCall(                                            const ExpressionNode *n, BuiltInFunction2    f);
  void genCall(                                            const ExpressionNode *n, BuiltInFunctionRef1 f);
  void genCall(                                            const ExpressionNode *n, BuiltInFunctionRef2 f);
  void genPolynomial(                                      const ExpressionNode *n);
  void genAssignment(                                      const ExpressionNode *n);
  void genIndexedExpression(                               const ExpressionNode *n);
  void genIf(                                              const ExpressionNode *n);
  JumpList genBoolExpression(                              const ExpressionNode *n);

  // Differentiation
  SNode                       D(                           const SNode &n, const String &name) const;
  SNode                       DPolynomial(                 const SNode &n, const String &name) const;
  SNode                       DStatementList(              const SNode &n, const String &name) const;

  // Conversion between Standard-, Canonical- and Numeric tree form
  SNode toSForm(               const ExpressionNode *n) const;
  SNode toSFormSum(            const ExpressionNode *n) const;
  SNode toSFormProduct(        const ExpressionNode *n) const;
  SNode toSFormPow(            const ExpressionNode *n) const;
  SNode toSFormPoly(           const ExpressionNode *n) const;
  SNode toSFormTreeNode(       const ExpressionNode *n) const;
  SNode toSFormFactorArray(    const FactorArray          &a, bool changeExponentSign) const;

  SNode toCForm(               const ExpressionNode *n) const;
  SNode toCFormSum(            const ExpressionNode *n) const;
  SNode toCFormProduct(        const ExpressionNode *n) const;
  SNode toCFormPoly(           const ExpressionNode *n) const;
  SNode toCFormTreeNode(       const ExpressionNode *n) const;
  AddentArray &toCFormSum(     AddentArray &result, const ExpressionNode *n, bool positive) const;
  FactorArray &toCFormProduct( FactorArray &result, const ExpressionNode *n, const SNode &exponent) const;
  FactorArray &toCFormPower(   FactorArray &result, const ExpressionNode *n, const SNode &exponent) const;
  FactorArray &toCFormRoot(    FactorArray &result, const ExpressionNode *n, const SNode &exponent) const;

  SNode toNForm(               const ExpressionNode *n) const;
  SNode toNFormBoolExp(        const ExpressionNode *n) const;
  SNode toNFormRealExp(        const ExpressionNode *n) const;
  SNode toNFormSum(            const ExpressionNode *n) const;
  SNode toNFormProduct(        const ExpressionNode *n) const;
  SNode toNFormPoly(           const ExpressionNode *n) const;
  SNode toNFormTreeNode(       const ExpressionNode *n) const;

  // Reduction
  void  iterateTransformation(ExpressionTransformer &transformer);

  SNode                   reduce(                      const SNode             n) const;
  SNode                   reduceBoolExp(               const SNode             n) const;
  SNode                   reduceRealExp(               const SNode             n) const;
  SNode                   reduceTreeNode(              const SNode             n) const;
  SNode                   reduceSum(                   const SNode             n) const;
  SNode                   reduceProduct(               const SNode             n) const;
  SNode                   reduceModulus(               const SNode             n) const;

  SNode                   reducePower(                 const SNode             n) const;
  SNode                   reduceConstantFactors(       const FactorArray      &factors) const;
  SNode                   reduceRationalPower(         const Rational         &base, const Rational &exponent) const;
  bool                    reducesToRationalConstant(   const SNode             n, Rational *r = NULL) const;
  bool                    reducesToRational(           const SNode             n, Rational *r = NULL) const;
  SNode                   multiplyParentheses(         const SNode             n) const;
  SNode                   multiplyParenthesesInSum(    const SNode             n) const;
  SNode                   multiplyParenthesesInProduct(const SNode             n) const;
  SNode                   multiplyParenthesesInPoly(   const SNode             n) const;
  SNode                   multiplyTreeNode(            const SNode             n) const;
  SNode                   multiply(                    const ExpressionFactor *a, const ExpressionNodeSum *s) const;

  SNode                   factorsToNode(               const FactorArray      &a) const;
  SNode                   addentsToNode(               const AddentArray      &a) const;
  const SumElement       *getCommonFactor(             const SumElement       &e1, const SumElement &e2) const;
  FactorArray            &getFactors(                  FactorArray &result,   const SNode            n) const;
  FactorArray            &getFactors(                  FactorArray &result,   const SNode            n,        const SNode exponent) const;
  FactorArray            &getFactorsInPower(           FactorArray &result,   const SNode            n,        const SNode exponent) const;
  FactorArray            &multiplyExponents(           FactorArray &result,   const FactorArray      &factors, const SNode exponent) const;
  SNode                   changeFirstNegativeFactor(   const SNode             n) const;
  SNode                   reduceLn(                    const SNode             n) const;
  SNode                   reduceLog10(                 const SNode             n) const;
  SNode                   getPowerOfE(                 const SNode             n) const;
  SNode                   getPowerOf10(                const SNode             n) const;
  const SumElement       *mergeLogarithms(             const SumElement       &e1, const SumElement &e2) const;
  SNode                   reduceAsymmetricFunction(    const SNode             n) const;
  SNode                   reduceSymmetricFunction(     const SNode             n) const;
  SNode                   reducePolynomial(            const SNode             n) const;
  const ExpressionFactor *reduceTrigonometricFactors(  const ExpressionFactor &f1, const ExpressionFactor &f2) const;

  bool                    canUseIdiotRule(             const SNode             n1, const SNode n2) const;
  bool                    canUseReverseIdiotRule(      const SumElement       *e1, const SumElement *e2, SumElement* &result) const;
  bool                    isSquareOfSinOrCos(          const SNode             n) const;
  bool                    sameLogarithm(               const SNode             n1, const SNode n2) const;
  static bool             rationalExponentsMultiply(   const Rational         &r1, const Rational &r2) ;

#ifdef __NEVER__
  const ExpressionNode   *replaceRationalPowers(       const ExpressionNode   *n) const;
  const ExpressionNode   *replaceRationalPower(        const ExpressionNode   *n) const;
  const ExpressionFactor *replaceRationalPower(        const ExpressionFactor *f) const;
  const ExpressionNode   *replaceRationalFactors(      const ExpressionNode   *n) const;
  const ExpressionNode   *substituteName(              const ExpressionNode   *n,  const String &from, const String &to) const;
  const ExpressionNode   *normalizePolynomial(         const ExpressionNode   *n) const;
#endif

  inline void checkReturnType(const TCHAR *method, ExpressionReturnType expectedReturnType) const {
    if(m_returnType != expectedReturnType) {
      throwMethodException(s_className, method, _T("Returntype=%d. exptected=%d\n"), m_returnType, expectedReturnType);
    }
  }

  // Properties
  void setState(ExpressionState newState);
  void setReduceIteration(unsigned int iteration);
  void setReturnType(ExpressionReturnType returnType);
public:
  Expression(TrigonometricMode mode = RADIANS);
  Expression(const Expression &src);
  Expression &operator=(const Expression &src);
  Expression getDerived(const String &name, bool reduceResult = true) const;
  void   compile(const String &expr, bool machineCode);
  inline Real evaluate() { 
    checkReturnType(_T("evaluate"), EXPR_RETURN_REAL);
    return m_machineCode ? fastEvaluate()     : evaluateStatementListReal(getRoot());
  }
  inline bool evaluateBool() { 
    checkReturnType(_T("evaluateBool"), EXPR_RETURN_BOOL);
    return m_machineCode ? fastEvaluateBool() : evaluateStatementListBool(getRoot());
  }

  inline ExpressionReturnType getReturnType() const {
    return m_returnType;
  }

  void checkIsStandardForm() const;
  void checkIsCanonicalForm() const;

  static void throwUnknownSymbolException(const TCHAR *className, const TCHAR *method, const ExpressionNode *n);

  void clear();
  void reduce();

  bool treesEqual(     const ExpressionNode *n1, const ExpressionNode *n2) const;
  bool treesEqualMinus(const SNode           n1, const SNode           n2) const;

  bool equalMinus(const Expression &e) const {
    return treesEqualMinus(getRoot(), e.getRoot());
  }

  void setTrigonometricMode(TrigonometricMode mode);
  
  inline TrigonometricMode getTrigonometricMode() const {
    return m_trigonometricMode;
  }

  inline ExpressionState getState() const {
    return m_state;
  }

  inline unsigned int getReduceIteration() const {
    return m_reduceIteration;
  }

  Expression &toCanonicalForm(); // return this
  Expression &toStandardForm();
  Expression &toNumericForm();
  Expression &expandMarkedNodes();
  Expression &multiplyMarkedNodes();

  String toString() const;
  void print(FILE *f = stdout) const;
};

class ExpressionDescription {
private:
  const char *m_syntax;
  const char *m_description;

  static const ExpressionDescription expressionHelpList[];
  static const int helpListSize;

  ExpressionDescription(const char *syntax, const char *description);
public:
  const char *getSyntax() const {
    return m_syntax;
  }

  const char *getDescription() const {
    return m_description;
  }

  static const ExpressionDescription *getHelpList() {
    return expressionHelpList;
  }

  static int getHelpListSize() {
    return helpListSize;
  }
};
