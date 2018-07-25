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
  updateEvalPointers();
}

void Expression::clear() {
  releaseAll();
  setReturnType(EXPR_NORETURNTYPE);
  setMachineCode(false);
  setState(PS_EMPTY);
  setReduceIteration(0);
}

void Expression::compile(const String &expr, bool machineCode, bool optimize, FILE *listFile) {
  parse(expr);
  if(!isOk()) {
    return;
  }
  setReturnType(getRoot()->getReturnType());
  if(optimize) {
    reduce();
  }
  try {
    if(machineCode && (listFile!=NULL)) {
      m_listFile = listFile;
      ListFile::printComment(m_listFile, _T("%s\n\n"), expr.cstr());
      ListFile::printComment(m_listFile, _T("%s\n\n"), getSymbolTable().toString().cstr());
    }

    setMachineCode(machineCode);

    if(m_listFile) {
      ListFile::printComment(m_listFile, _T("----------------------------------------------------\n"));
      m_listFile = NULL;
    }
  } catch(Exception e) {
    addError(_T("%s"), e.what());
    m_listFile = NULL;
  } catch(...) {
    m_listFile = NULL;
    throw;
  }
}

void Expression::parse(const String &expr) {
  clear();
  setOk(true);
  setTreeForm(TREEFORM_STANDARD);
  LexStringStream    stream(expr);
  ExpressionLex      lex(&stream);
  ExpressionParser   parser(*this, &lex);
  lex.setParser(&parser);
  parser.parse();
  if(isOk()) {
    buildSymbolTable();
    setState(PS_COMPILED);
  }
}


void Expression::setReturnType(ExpressionReturnType returnType) {
  setProperty(EP_RETURNTYPE, m_returnType, returnType);
  updateEvalPointers();
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
  updateEvalPointers();
}

void Expression::genMachineCode() {
  clearMachineCode();
  m_code = CodeGenerator(this, m_listFile).getCode();
}

void Expression::clearMachineCode() {
  SAFEDELETE(m_code);
}

void Expression::updateEvalPointers() {
  switch(getReturnType()) {
  case EXPR_NORETURNTYPE:
    setEvalPointers(&Expression::evalRealError, &Expression::evalBoolError);
    break;
  case EXPR_RETURN_REAL :
    setEvalPointers(isMachineCode()?&Expression::evalRealFast
                                   :isEmpty()?&Expression::evalRealError:&Expression::evalRealTree
                                   ,&Expression::evalBoolError);
    break;
  case EXPR_RETURN_BOOL :
    setEvalPointers(&Expression::evalRealError
                   ,isMachineCode()?&Expression::evalBoolFast
                                   :isEmpty()?&Expression::evalBoolError:&Expression::evalBoolTree);
    break;
  }
}

Real Expression::evalRealError() const {
  throwException(_T("Cannot evaluate real. Returntype is %s")
                ,ExpressionNode::getReturnTypeName(m_returnType).cstr()
                );
  return 0;
}

bool Expression::evalBoolError() const {
  throwException(_T("Cannot evaluate bool. Returntype is %s")
                ,ExpressionNode::getReturnTypeName(m_returnType).cstr()
                );
  return false;
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
