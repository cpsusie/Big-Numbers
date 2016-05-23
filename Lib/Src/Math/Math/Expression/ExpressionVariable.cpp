#include "pch.h"
#include <Math/Expression/ExpressionNode.h>

ExpressionVariable::ExpressionVariable(const String &name, const Real &value, bool isConstant, bool isDefined, bool isLoopVar) {
  m_name     = name;
  m_value    = value;
  m_constant = isConstant;
  m_defined  = isDefined;
  m_loopVar  = isLoopVar;
}

void ExpressionVariable::setValue(const Real &value) {
  if(m_constant) {
    throwException(_T("Cannot change value of %s"), getName().cstr());
  }
  m_value = value;
}

String ExpressionVariable::toString() const {
  return format(_T("Variable %s = %s %s %s %s")
              , m_name.cstr()
              ,::toString(m_value).cstr()
              , (m_constant ? _T(" constant") : _T(""))
              , (m_loopVar  ? _T(" loop variable") : _T(""))
              , (m_defined  ? _T(" defined") : _T(" undefined")));
}

