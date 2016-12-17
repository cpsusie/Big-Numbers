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
    compile();
  }
}

DiffEquationSystem &DiffEquationSystem::operator=(const DiffEquationSystem &src) {
  cleanup();
  m_equationSystemDescription = src.m_equationSystemDescription;
  if(src.isCompiled()) {
    compile();
  }
  return *this;
}

void DiffEquationSystem::compile() {
  cleanup();
  try {
    for(UINT i = 0; i < m_equationSystemDescription.size(); i++) {
      const DiffEquationDescription &desc = m_equationSystemDescription[i];
      if(!DiffEquationDescription::isValidName(desc.m_name)) {
        throwException(_T("<%s> is not a valid function name"), desc.m_name.cstr());
      }
    }
    for (UINT i = 1; i < m_equationSystemDescription.size(); i++) {
      const String &name1 = m_equationSystemDescription[i].m_name;
      for(UINT j = 0; j < i; j++) {
        const String &name2 = m_equationSystemDescription[j].m_name;
        if(name1 == name2) {
          throwException(_T("Cannot have more than 1 definition of %s'"), name1.cstr());
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
        const String msg = e->getErrors()[0];
        delete e;
        throwException(msg);
      }
    }
    for (UINT i = 0; i < m_exprArray.size(); i++) {
      ExpressionWithInputVector  *e  = m_exprArray[i];
      const ExpressionVariable   *vx = e->getVariable(_T("x"));
      if(vx) {
        e->m_input.add(ExpressionInputIndex(0, &(e->getValueRef(*vx))));
        if (!vx->isInput()) {
          throwException(_T("Variable 'x' is not input in equation %d"), (int)i);
        }
      }
      for(UINT j = 0; j < m_equationSystemDescription.size(); j++) {
        const String             &namej = m_equationSystemDescription[j].m_name;
        const ExpressionVariable *vj    = e->getVariable(namej);
        if(vj == NULL) continue;
        if (!vj->isInput()) {
          throwException(_T("Variable '%s' is not input in equation %d"), namej.cstr(), (int)i);
        }
        e->m_input.add(ExpressionInputIndex(j+1, &(e->getValueRef(*vj)))); // +1 becaus v[0] is variable "x"
      }
    }
  } catch (...) {
    cleanup();
    throw;
  }
}

void DiffEquationSystem::compile(const DiffEquationSystemDescription &desd) {
  cleanup();
  m_equationSystemDescription = desd;
  compile();
}

void DiffEquationSystem::setDescription(const DiffEquationSystemDescription &desd) {
  validate(desd);
  cleanup();
  m_equationSystemDescription = desd;
}

void DiffEquationSystem::validate(const DiffEquationSystemDescription &desd) { // static
  DiffEquationSystem s;
  s.compile(desd);
}

bool DiffEquationDescription::isValidName(const String &s) {
  String tmp = s;
  tmp.trim();
  if(tmp != s) return false;
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
