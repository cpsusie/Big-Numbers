#include "pch.h"
#include <XMLDoc.h>
#include <Math/Expression/Expr3.h>

namespace Expr {

Expr3::Expr3(const String &expr1, const String &expr2, const String &expr3, const String &commonText)
: m_commonText(commonText)
{
  add(expr1);
  add(expr2);
  add(expr3);
}

};
