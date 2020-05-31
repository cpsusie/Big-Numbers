#include "pch.h"
#include <XMLUtil.h>
#include <MFCUtil/ColorSpace.h>

void setValue(XMLDoc &doc, XMLNodePtr n, const D3DCOLORVALUE &v) {
  setValue(doc, n, _T("r"      ), v.r        );
  setValue(doc, n, _T("g"      ), v.g        );
  setValue(doc, n, _T("b"      ), v.b        );
  setValue(doc, n, _T("a"      ), v.a        );
}

void getValue(XMLDoc &doc, XMLNodePtr n, D3DCOLORVALUE &v) {
  getValue(doc, n, _T("r"      ), v.r        );
  getValue(doc, n, _T("g"      ), v.g        );
  getValue(doc, n, _T("b"      ), v.b        );
  getValue(doc, n, _T("a"      ), v.a        );
}
