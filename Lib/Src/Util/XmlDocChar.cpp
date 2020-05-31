#include "pch.h"
#include "MathUtil.h"
#include <XMLDoc.h>

void setValue(XMLDoc &doc, const XMLNodePtr &n, char c, bool hex) {
  setValue(doc, n, (int)c, hex);
}

void getValue(XMLDoc &doc, const XMLNodePtr &n, char &c, bool hex) {
  int tmp;
  getValue(doc, n, tmp);
  assert(isChar(tmp));
  c = (char)tmp;
}

void setValue(XMLDoc &doc, const XMLNodePtr &n, BYTE  b, bool hex) {
  setValue(doc, n, (UINT)b, hex);
}

void getValue(XMLDoc &doc, const XMLNodePtr &n, BYTE &b, bool hex) {
  UINT tmp;
  getValue(doc, n, tmp, hex);
  assert(isUchar(tmp));
  b = (BYTE)tmp;
}

