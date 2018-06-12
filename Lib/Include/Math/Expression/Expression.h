#pragma once

#include "ExpressionParser.h"
#include "MachineCode.h"

namespace Expr {

class Expression : public ParserTree {
private:
  typedef Real (Expression::*PevalReal)() const;
  typedef bool (Expression::*PevalBool)() const;

  bool                       m_machineCode;
  const MachineCode         *m_code;
  PevalReal                  m_realfp;
  PevalBool                  m_boolfp;
  FILE                      *m_listFile;
  ExpressionReturnType       m_returnType;

  void   init(ExpressionReturnType returnType = EXPR_NORETURNTYPE);
  void   parse(const String &expr);
  void   print(const ExpressionNode *n, FILE *f = stdout) const;

  void   genMachineCode();
  void   clearMachineCode();
  Real evalRealError() const;
  bool evalBoolError() const;
  inline Real evalRealFast()  const { return m_code->evaluateReal();    }
  inline bool evalBoolFast()  const { return m_code->evaluateBool();    }
  inline Real evalRealTree()  const { return getRoot()->evaluateReal(); }
  inline bool evalBoolTree()  const { return getRoot()->evaluateBool(); }
  inline void setEvalPointers(PevalReal rf, PevalBool bf) {
    m_realfp = rf; m_boolfp = bf;
  }
  void updateEvalPointers();

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

  inline Real evaluate() const {
    return (this->*m_realfp)();
  }
  inline bool evaluateBool() const {
    return (this->*m_boolfp)();
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
