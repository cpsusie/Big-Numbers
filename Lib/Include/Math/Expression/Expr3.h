#pragma once

#include <StringArray.h>

namespace Expr {

class Expr3 {
private:
  String      m_commonText;
  StringArray m_expr;
public:
  inline Expr3() {
  }
  Expr3(const String &expr1, const String &expr2, const String &expr3, const String &commonText=EMPTYSTRING);

  inline const String &getCommonText() const {
    return m_commonText;
  }
  inline String getRawText(UINT index) const {
    return (index < m_expr.size()) ? m_expr[index] : EMPTYSTRING;
  }
  inline String getExprText(UINT index) const {
    return getCommonText() + _T(" ") + getRawText(index);
  }
};

};
