#pragma once

#include "ExpressionParser.h"
#include "MachineCode.h"

namespace Expr {

class Expression : public ParserTree {
private:
  bool                       m_machineCode;
  const MachineCode         *m_code;
  FILE                      *m_listFile;
  ExpressionReturnType       m_returnType;

  void   init(ExpressionReturnType returnType = EXPR_NORETURNTYPE);
  void   parse(const String &expr);

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
  Expression getDerived(const String &name, bool optimize = true) const;
  static String getDefaultListFileName();

  void compile(const String &expr, bool machineCode, bool optimize = false, FILE *listFile = NULL);
  inline ExpressionReturnType getReturnType() const {
    return m_returnType;
  }
  inline bool isMachineCode() const {
    return m_machineCode;
  }
  inline Real evaluate() {
    CHECKRETURNTYPE(EXPR_RETURN_REAL);
    return isMachineCode() ? fastEvaluateReal() : getRoot()->evaluateReal();
  }
  inline bool evaluateBool() {
    CHECKRETURNTYPE(EXPR_RETURN_BOOL);
    return isMachineCode() ? fastEvaluateBool() : getRoot()->evaluateBool();
  }

  void clear();

  void setTrigonometricMode(TrigonometricMode mode);
  void setTreeForm(ParserTreeForm form);

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

}; // namespace Expr
