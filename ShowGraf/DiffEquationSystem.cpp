#include "stdafx.h"
#include "DiffEquationSystem.h"

DiffEquationSystem::~DiffEquationSystem() {
  cleanup();
}

void DiffEquationSystem::cleanup() {
  for (size_t i = 0; i < m_exprArray.size(); i++) {
    delete m_exprArray[i];
  }
  m_exprArray.clear();
}

DiffEquationSystem::DiffEquationSystem(const DiffEquationSystem &src) {
  m_equationSystemDescription = src.m_equationSystemDescription;
  if (src.isCompiled()) {
    CompilerErrorList errorList;
    if(!compile(errorList)) errorList.throwFirstError();
  }
}

DiffEquationSystem &DiffEquationSystem::operator=(const DiffEquationSystem &src) {
  const DiffEquationSystemDescription saveDesc = m_equationSystemDescription;
  cleanup();
  m_equationSystemDescription = src.m_equationSystemDescription;
  if(src.isCompiled()) {
    CompilerErrorList errorList;
    if(!compile(errorList)) {
      cleanup();
      m_equationSystemDescription = saveDesc;
      errorList.throwFirstError();
    }
  }
  return *this;
}

bool DiffEquationSystem::compile(CompilerErrorList &errorList) {
  cleanup();
  errorList.clear();
  try {
    for(UINT i = 0; i < m_equationSystemDescription.size(); i++) {
      const DiffEquationDescription &desc = m_equationSystemDescription[i];
      if(!DiffEquationDescription::isValidName(desc.m_name)) {
        errorList.addError(i, false, _T("<%s> is not a valid function name"), desc.m_name.cstr());
      }
    }
    for (UINT i = 1; i < m_equationSystemDescription.size(); i++) {
      const String &name1 = m_equationSystemDescription[i].m_name;
      for(UINT j = 0; j < i; j++) {
        const String &name2 = m_equationSystemDescription[j].m_name;
        if(name1 == name2) {
          errorList.addError(i, false, _T("Cannot have more than 1 definition of %s'"), name1.cstr());
        }
      }
    }
    for(UINT i = 0; i < m_equationSystemDescription.size(); i++) {
      const DiffEquationDescription &desc = m_equationSystemDescription[i];
      ExpressionWithInputVector     *e    = new ExpressionWithInputVector();
      e->compile(desc.m_expr, true);
      if (e->isOk()) {
        m_exprArray.add(e);
      } else {
        errorList.addErrors(i, e->getErrors());
        delete e;
      }
    }
    if(!errorList.isOk()) return false;
    for (UINT i = 0; i < m_exprArray.size(); i++) {
      ExpressionWithInputVector  *e  = m_exprArray[i];
      const ExpressionVariable   *vx = e->getVariable(_T("x"));
      if(vx) {
        e->m_input.add(ExpressionInputIndex(0, &(e->getValueRef(*vx))));
        if (!vx->isInput()) {
          errorList.addError(i, true, _T("Variable 'x' is not input in this equation"));
        }
      }
      for(UINT j = 0; j < m_equationSystemDescription.size(); j++) {
        const String             &namej = m_equationSystemDescription[j].m_name;
        const ExpressionVariable *vj    = e->getVariable(namej);
        if(vj == NULL) continue;
        if (!vj->isInput()) {
          errorList.addError(i, true, _T("Variable '%s' is not input in this equation"), namej.cstr());
        } else {
          e->m_input.add(ExpressionInputIndex(j+1, &(e->getValueRef(*vj)))); // +1 because v[0] is variable "x"
        }
      }
    }
  } catch (...) {
    cleanup();
    throw;
  }
  return errorList.isOk();
}

bool DiffEquationSystem::compile(const DiffEquationSystemDescription &desc, CompilerErrorList &errorList) {
  cleanup();
  m_equationSystemDescription = desc;
  return compile(errorList);
}

void DiffEquationSystem::setDescription(const DiffEquationSystemDescription &desc) {
  CompilerErrorList errorList;
  validate(desc, errorList);
  cleanup();
  m_equationSystemDescription = desc;
}

bool DiffEquationSystem::validate(const DiffEquationSystemDescription &desc, CompilerErrorList &errorList) { // static
  DiffEquationSystem s;
  return s.compile(desc, errorList);
}

bool DiffEquationDescription::isValidName(const String &s) {
  String tmp = s;
  tmp.trim();
  if((tmp != s) || (tmp == _T("x"))) return false;
  LexStringStream stream(s);
  ExpressionLex lex(&stream);
  if(lex.getNextLexeme() != NAME) return false;
  if(lex.getNextLexeme() != EOI ) return false;
  return true;
}

Real ExpressionWithInputVector::evaluate(const Vector &x) {
  for(size_t i = 0; i < m_input.size(); i++) {
    *m_input[i].m_var = x[m_input[i].m_index];
  }
  return Expression::evaluate();
}

Vector DiffEquationSystem::operator()(const Vector &x) {
  const size_t n = getEquationCount();
  Vector dx(x.getDimension());
  dx[0] = 0;
  for(size_t i = 0; i < n; i++) {
    dx[i+1] = m_exprArray[i]->evaluate(x);
  }
  return dx;
}

void CompilerErrorList::vaddError(UINT eqIndex, bool expr, const TCHAR *form, va_list argptr) {
  const String msg = vformat(form, argptr);
  add(format(_T("(%d,%c):%s"), eqIndex, expr?_T('E'):_T('N'), msg.cstr()));
}

void CompilerErrorList::addError(UINT eqIndex, bool expr, const TCHAR *format, ...) {
  va_list argptr;
  va_start(argptr, format);
  vaddError(eqIndex, expr, format, argptr);
  va_end(argptr);
}

void CompilerErrorList::addErrors(UINT eqIndex, const StringArray &errors) { // errors from Expression.compile()
  for(size_t i = 0; i < errors.size(); i++) {
    addError(eqIndex, true, _T("%s"), errors[i].cstr());
  }
}

ErrorPosition::ErrorPosition(const String &error) {
  int eqIndex;
  TCHAR fieldMark;
  Tokenizer tok(error, _T(":"));
  const String eqMark = tok.next();
  if (_stscanf(eqMark.cstr(), _T("(%d,%c)"), &eqIndex, &fieldMark) != 2) {
    m_eqIndex = -1;
  } else {
    m_eqIndex = eqIndex;
    m_inExpr  = (fieldMark == _T('E'));

    if(m_inExpr) {
      const String posStr = tok.next();
      int line, col;
      if(_stscanf(posStr.cstr(), _T("(%d,%d)"), &line,&col) == 2) {
        m_pos.setLocation(line, col);
      } else {
        m_pos.setLocation(0,0);
      }
    } else {
      m_pos.setLocation(0,0);
    }
  }
}
