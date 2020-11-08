#pragma once

#include <StringArray.h>
#include <PropertyContainer.h>
#include <SourcePosition.h>
#include <Math/Real.h>
#include "PragmaLib.h"

namespace Expr {

typedef enum {
  EXPR_NORETURNTYPE
 ,EXPR_RETURN_FLOAT
 ,EXPR_RETURN_BOOL
} ExpressionReturnType;


class MachineCode;
class ExpressionNode;
class ExpressionVariable;
class ExpressionSymbolTable;

typedef enum {
  EP_TRIGONOMETRICMODE     // TrigonometricMode
 ,EP_RETURNTYPE            // ExpressionReturnType
 ,EP_MACHINECODE           // bool
} ExpressionProperty;

class Expression : public PropertyContainer {
  friend class ParserTree;
private:
  typedef Real (Expression::*PevalReal)() const;
  typedef bool (Expression::*PevalBool)() const;

  ExpressionSymbolTable     *m_symbolTable;
  ParserTree                *m_tree;
  TrigonometricMode          m_trigonometricMode;
  ExpressionReturnType       m_returnType;
  bool                       m_machineCode;
  bool                       m_ok;
  const MachineCode         *m_code;
  PevalReal                  m_realfp;
  PevalBool                  m_boolfp;
  FILE                      *m_listFile;

  void   initialize(ExpressionReturnType returnType = EXPR_NORETURNTYPE);
  void   uninitialize();
  void   print(const ExpressionNode *n, FILE *f = stdout) const;

  void   genMachineCode();
  void   clearMachineCode();
  // throws Exception - called by evaluate(), when no compile code/parsertree
  Real evalRealError() const;
  // throws Exception - called by evaluateBool(), when no compile code/parsertree
  bool evalBoolError() const;
  Real evalRealFast()  const;
  bool evalBoolFast()  const;
  Real evalRealTree()  const;
  bool evalBoolTree()  const;
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
  explicit Expression(const ExpressionNode *root);
  Expression &operator=(const Expression &src);
  virtual ~Expression();
  Expression getDerived(const String &name, bool optimize = true) const;
  static String getDefaultListFileName();

  bool compile(const String &expr, StringArray &errors, bool machineCode, bool optimize = false, FILE *listFile = nullptr);

  // Error should be an element from StringArray returned by compile().
  // Will return sourcePosition specified in error as "(line,col):errorText"
  // and modify error to be text after "(line,col):"
  // If no leading "(line,col):" an Exception is thrown
  static SourcePosition decodeErrorString(String &error);

  // Error should be an element from StringArray returned by compile().
  // Will return textposition in expr, remove the textposition "(line,column)" from error
  // If no leading "(line,col):" an Exception is thrown
  static UINT decodeErrorString(const String &expr, String &error);

  inline ExpressionReturnType getReturnType() const {
    return m_returnType;
  }
  inline bool isMachineCode() const {
    return m_machineCode;
  }
  inline bool isOk() const {
    return m_ok;
  }
  bool hasSyntaxTree() const;
  inline Real evaluate() const {
    return (this->*m_realfp)();
  }
  inline bool evaluateBool() const {
    return (this->*m_boolfp)();
  }
  void clear();
  void setTrigonometricMode(TrigonometricMode mode);
  inline TrigonometricMode getTrigonometricMode() const {
    return m_trigonometricMode;
  }
  inline ParserTree           *getTree() const {
    return m_tree;
  }
  ExpressionNode              *getRoot() const;
  inline const ExpressionSymbolTable &getSymbolTable() const {
    return *m_symbolTable;
  }
  Expression                  &setValue(   const String &name, const Real &value);
  const ExpressionVariable    *getVariable(const String &name) const;
  Real                        &getValueRef(const ExpressionVariable &var) const;
  Real                        *getValueRef(const String &name) const;
  // Return *this
  Expression                  &expandMarkedNodes();
  // Return *this
  Expression                  &multiplyMarkedNodes();
  bool                         equal(      const Expression &e) const;
  bool                         equalMinus( const Expression &e) const;

  void print(FILE *f = stdout) const;
  String toString() const;
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
