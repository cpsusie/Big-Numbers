#include "pch.h"
#include <Math/Expression/ExpressionNode.h>

namespace Expr {

ExpressionVariable::ExpressionVariable(const String &name, bool isConstant, bool isDefined, bool isLoopVar)
: m_name(name)
{
  m_valueIndex      = -1;
  m_occurrenceCount = 1;
  m_constant        = isConstant;
  m_defined         = isDefined;
  m_loopVar         = isLoopVar;
}

String ExpressionVariable::toString() const {
  return format(_T("Variable:[Name:%-10s (freq:%3d) v-Index:%2d %-9s %-9s %-9s]")
              , format(_T("<%s>"), m_name.cstr()).cstr()
              , m_occurrenceCount
              , m_valueIndex
              , (m_constant ? _T("constant") : EMPTYSTRING)
              , (m_loopVar  ? _T("loop var") : EMPTYSTRING)
              , (m_defined  ? _T("defined" ) : _T("undefined")));
}

}; // namespace Expr
