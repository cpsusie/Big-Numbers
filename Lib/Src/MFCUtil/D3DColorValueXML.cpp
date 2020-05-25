#include "pch.h"
#include <XMLUtil.h>
#include <MFCUtil/ColorSpace.h>

void setValue(XMLDoc &doc, XMLNodePtr n, const D3DCOLORVALUE &v) {
  doc.setValue(n, _T("r"      ), v.r        );
  doc.setValue(n, _T("g"      ), v.g        );
  doc.setValue(n, _T("b"      ), v.b        );
  doc.setValue(n, _T("a"      ), v.a        );
}

void getValue(XMLDoc &doc, XMLNodePtr n, D3DCOLORVALUE &v) {
  doc.getValue(n, _T("r"      ), v.r        );
  doc.getValue(n, _T("g"      ), v.g        );
  doc.getValue(n, _T("b"      ), v.b        );
  doc.getValue(n, _T("a"      ), v.a        );
}
