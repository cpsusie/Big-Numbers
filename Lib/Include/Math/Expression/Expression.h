#pragma once

#include "ExpressionParser.h"
#include "MachineCode.h"

typedef enum {
  EXPR_NORETURNTYPE
 ,EXPR_RETURN_REAL
 ,EXPR_RETURN_BOOL
} ExpressionReturnType;

class ExpressionTransformer {
public:
  ExpressionTransformer() {
  }
  virtual SNode           transform(SNode n) = 0;
  virtual ParserTreeState getState() const = 0;
};

class Expression : public ParserTree {
private:
  bool                       m_machineCode;
  const MachineCode         *m_code;
  FILE                      *m_listFile;
  TrigonometricMode          m_trigonometricMode;
  ExpressionReturnType       m_returnType;

  friend class ExpressionNodeTree;
  friend class ExpressionPainter;
  friend class MainReducer;
  friend class RationalPowersReducer;

  void   init(TrigonometricMode    trigonometricMode
             ,ExpressionReturnType returnType      = EXPR_NORETURNTYPE);
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

  void   print(const ExpressionNode *n, FILE *f = stdout) const;

  void   genMachineCode();
  void   clearMachineCode();
  inline Real fastEvaluateReal() {
    return m_code->evaluateReal();
  }
  inline bool fastEvaluateBool() {
    return m_code->evaluateBool();
  }

#ifdef __NEVER__
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
#endif // __NEVER__

  // Reduction
  void  iterateTransformation(ExpressionTransformer &transformer);

#ifdef __NEVER__
  const ExpressionNode   *replaceRationalPowers(       const ExpressionNode   *n) const;
  const ExpressionNode   *replaceRationalPower(        const ExpressionNode   *n) const;
  const ExpressionFactor *replaceRationalPower(        const ExpressionFactor *f) const;
  const ExpressionNode   *replaceRationalFactors(      const ExpressionNode   *n) const;
  const ExpressionNode   *substituteName(              const ExpressionNode   *n,  const String &from, const String &to) const;
  const ExpressionNode   *normalizePolynomial(         const ExpressionNode   *n) const;
#endif __NEVER__

  // Properties
  void setReturnType(ExpressionReturnType returnType );
  void setMachineCode(bool                machinecode);
public:
  Expression(TrigonometricMode mode = RADIANS);
  Expression(const Expression &src);
  Expression &operator=(const Expression &src);
  ~Expression() {
    clear();
  }
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
    return isMachineCode() ? fastEvaluateReal() : getRoot()->evaluateStatementListReal();
  }
  inline bool evaluateBool() {
    CHECKRETURNTYPE(EXPR_RETURN_BOOL);
    return isMachineCode() ? fastEvaluateBool() : getRoot()->evaluateStatementListBool();
  }

  void checkIsStandardForm();
  void checkIsCanonicalForm();

  void clear();
  void reduce();

  bool equalMinus(Expression &e) {
    return ::equalMinus(getRoot(), e.getRoot());
  }

  void setTrigonometricMode(TrigonometricMode mode);
  TrigonometricMode getTrigonometricMode() const {
    return m_trigonometricMode;
  }

  // return this
  Expression &toCanonicalForm();
  // return this
  Expression &toStandardForm();
  // return this
  Expression &toNumericForm();
  // return this
  inline Expression &expandMarkedNodes() {
    __super::expandMarkedNodes();
    return *this;
  }
  // return this
  inline Expression &multiplyMarkedNodes() {
    __super::multiplyMarkedNodes();
    return *this;
  }

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
