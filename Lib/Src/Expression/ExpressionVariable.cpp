#include "pch.h"
#include <Math/Expression/ExpressionNode.h>

namespace Expr {

ExpressionVariable::ExpressionVariable(const String &name, bool constant, bool defined, bool loopVar)
: m_name(      name    )
, m_constant(  constant)
, m_loopVar(   loopVar )
, m_defined(   defined )
, m_valueIndex(-1      )
{
}

String ExpressionVariable::toString(bool fillers) const {
  const TCHAR *formatStr = fillers
                         ? _T("Variable:[Name:%-10s V.Index:%2d%-9s%-9s%-10s]")
                         : _T("Variable:[Name:%s V.Index:%d%s%s%s]");
  return format(formatStr
               ,format(_T("<%s>"), m_name.cstr()).cstr()
               ,m_valueIndex
               ,(m_constant ? _T(" constant") : EMPTYSTRING)
               ,(m_loopVar  ? _T(" loop var") : EMPTYSTRING)
               ,(m_defined  ? _T(" defined") : _T(" undefined")));
}

}; // namespace Expr
