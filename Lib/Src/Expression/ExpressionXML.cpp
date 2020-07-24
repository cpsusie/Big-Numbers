#include "pch.h"
#include <Math/Expression/Expr3.h>
#include <Math/Expression/ExpressionXML.h>

namespace Expr {

void setValue(XMLDoc &doc, XMLNodePtr n, const Expr3 &expr) {
  setValue(doc, n, _T("common"          ), expr.getCommonText());
  setValue(doc, n, _T("exprx"           ), expr.getRawText(0)  );
  setValue(doc, n, _T("expry"           ), expr.getRawText(1)  );
  setValue(doc, n, _T("exprz"           ), expr.getRawText(2)  );
}

void getValue(XMLDoc &doc, XMLNodePtr n, Expr3 &expr) {
  String x, y, z, common;
  getValueLF(doc,n, _T("exprx" ), x     );
  getValueLF(doc,n, _T("expry" ), y     );
  getValueLF(doc,n, _T("exprz" ), z     );
  getValueLF(doc,n, _T("common"), common);
  expr = Expr3(  x, y, z        , common);
}

};
