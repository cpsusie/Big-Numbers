#include "pch.h"
#include <Math/MathXML.h>

void setValue(XMLDoc &doc, XMLNodePtr n, TrigonometricMode mode) {
  String str = toLowerCase(toString(mode));
  setValue(doc, n, _T("trigo"), str);
}

void getValue(XMLDoc &doc, XMLNodePtr n, TrigonometricMode &mode) {
  String str;
  getValue(doc, n, _T("trigo"), str);
  mode = _TrigonometricModeName::strToMode(str.cstr());
}
