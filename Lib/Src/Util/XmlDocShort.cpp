#include "pch.h"
#include "MathUtil.h"
#include <XMLDoc.h>

void setValue(XMLDoc &doc, const XMLNodePtr &n, short s, bool hex) {
  setValue(doc, n, (int)s, hex);
}

void getValue(XMLDoc &doc, const XMLNodePtr &n, short &s, bool hex) {
  int tmp;
  getValue(doc, n, tmp, hex);
  assert(isShort(tmp));
  s = (short)tmp;
}

void setValue(XMLDoc &doc, const XMLNodePtr &n, USHORT s, bool hex) {
  setValue(doc, n, (UINT)s, hex);
}

void getValue(XMLDoc &doc, const XMLNodePtr &n, USHORT &s, bool hex) {
  UINT tmp;
  getValue(doc, n, tmp, hex);
  assert(isUshort(tmp));
  s = (USHORT)tmp;
}
