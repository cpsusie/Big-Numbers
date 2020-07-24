#pragma once

#include <StringArray.h>

namespace Expr {

class Expr3 : private StringArray {
private:
  String m_commonText;
public:
  inline Expr3() {
  }
  Expr3(const String &expr1, const String &expr2, const String &expr3, const String &commonText=EMPTYSTRING);

  inline String getExprText(UINT i) const {
    return m_commonText + _T(" ") + getRawText(i);
  }
  const String &getCommonText() const {
    return m_commonText;
  }
  inline String getRawText(UINT i) const {
    return (i<size()) ? (*this)[i] : EMPTYSTRING;
  }
};

};
