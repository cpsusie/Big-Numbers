#include "pch.h"
#include <Math/Expression/Expression.h>
#include "ExpressionCompile.h"

namespace Expr {

Expression::Expression(TrigonometricMode mode) : ParserTree(mode) {
  init();
}

Expression::Expression(const Expression &src) : ParserTree(src) {
  init(src.getReturnType());
  buildSymbolTable(&src.getSymbolTable().getAllVariables());
  setMachineCode(src.isMachineCode());
}

Expression &Expression::operator=(const Expression &src) {
  if(&src == this) {
    return *this;
  }

  clear();
  ParserTree::operator=(src);
  setTrigonometricMode(src.getTrigonometricMode());
  buildSymbolTable(   &src.getSymbolTable().getAllVariables());
  setMachineCode(      src.isMachineCode());
  setReturnType(       src.getReturnType());

  return *this;
}

void Expression::init(ExpressionReturnType returnType) {
  m_returnType        = returnType;
  m_machineCode       = false;
  m_code              = NULL;
  m_listFile          = NULL;
}

void Expression::clear() {
  releaseAll();
  setReturnType(EXPR_NORETURNTYPE);
  setMachineCode(false);
  setState(PS_EMPTY);
  setReduceIteration(0);
}

void Expression::compile(const String &expr, bool machineCode, FILE *listFile) {
  parse(expr);
  if(!isOk()) {
    return;
  }
  setReturnType(getRoot()->getReturnType());
  try {
    if(machineCode && (listFile!=NULL)) {
      m_listFile = listFile;
      _ftprintf(m_listFile, _T("%s\n\n"), expr.cstr());
      _ftprintf(m_listFile, _T("%s\n\n"), getSymbolTable().toString().cstr());
    }

    setMachineCode(machineCode);

    if(m_listFile) {
      _ftprintf(m_listFile, _T("----------------------------------------------------\n"));
      m_listFile = NULL;
    }
  } catch (...) {
    m_listFile = NULL;
    throw;
  }
}

void Expression::parse(const String &expr) {
  clear();
  setOk(true);
  LexStringStream    stream(expr);
  ExpressionLex      lex(&stream);
  ExpressionParser   parser(*this, &lex);
  lex.setParser(&parser);
  parser.parse();
  if(isOk()) {
    buildSymbolTable();
    setTreeForm(TREEFORM_STANDARD);
    setState(PS_COMPILED);
  }
}


void Expression::setReturnType(ExpressionReturnType returnType) {
  setProperty(EP_RETURNTYPE, m_returnType, returnType);
}

void Expression::setMachineCode(bool machinecode) {
  if(machinecode != isMachineCode()) {
    if(machinecode) {
      genMachineCode();
    } else {
      clearMachineCode();
    }
    setProperty(EP_MACHINECODE, m_machineCode, machinecode);
  }
}

void Expression::genMachineCode() {
  clearMachineCode();
  m_code = CodeGenerator(this, m_listFile).getCode();
}

void Expression::clearMachineCode() {
  SAFEDELETE(m_code);
}

void Expression::setTrigonometricMode(TrigonometricMode mode) {
  const TrigonometricMode oldMode = getTrigonometricMode();
  if(mode != oldMode) {
    __super::setTrigonometricMode(mode);
    if(isMachineCode()) {
      genMachineCode();
    }
    notifyPropertyChanged(EP_TRIGONOMETRICMODE, &oldMode, &mode);
  }
}

void Expression::setTreeForm(ParserTreeForm form) {
  if(form == getTreeForm()) return;
  __super::setTreeForm(form);
}

String Expression::toString() const {
  const ExpressionNode *root = getRoot();
  return root ? root->toString() : EMPTYSTRING;
}

void Expression::print(const ExpressionNode *n, FILE *f) const {
  _ftprintf(f,_T("%s"), n->toString().cstr());
}

void Expression::print(FILE *f) const {
  _ftprintf(f,_T("%s"),toString().cstr());
}

String Expression::getDefaultListFileName() { // static
#ifdef IS32BIT
#define PFSTR "x86\\"
#else // IS64BIT
#define PFSTR "x64\\"
#endif // IS64BIT
#ifdef _DEBUG
#define CONFSTR "Debug\\"
#else
#define CONFSTR "Release\\"
#endif // _DEBUG
#ifdef LONGDOUBLE
#define REALSTR "TB\\"
#else
#define REALSTR "QW\\"
#endif // LONGDOUBLE
  return _T("c:\\temp\\ExprList\\" PFSTR CONFSTR REALSTR "expr.lst");
}

}; // namespace Expr
