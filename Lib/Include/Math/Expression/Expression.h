#pragma once

#include "ExpressionParser.h"

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
  const void                *m_code; // actually MachineCode*
  FILE                      *m_listFile;
  ExpressionReturnType       m_returnType;
  mutable ExpressionState    m_state;
  mutable UINT               m_reduceIteration;
  TrigonometricMode          m_trigonometricMode;

  friend class ExpressionNodeTree;
  friend class ExpressionPainter;
  friend class MainReducer;
  friend class MarkedNodeMultiplier;
  friend class RationalPowersReducer;

  void   init(TrigonometricMode    trigonometricMode
             ,ExpressionReturnType returnType      = EXPR_NORETURNTYPE
             ,ExpressionState      state           = EXPR_EMPTY
             ,UINT                 reduceIteration = 0);
  void   parse(const String &expr);
  ExpressionReturnType findReturnType() const;

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

  void   openListFile();
  void   closeListFile();
  void   genMachineCode();
  void   clearMachineCode();
  Real   fastEvaluateReal();
  bool   fastEvaluateBool();
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
  static String getDefaultListFileName();

  void compile(const String &expr, bool machineCode, FILE *listFile = NULL);
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
