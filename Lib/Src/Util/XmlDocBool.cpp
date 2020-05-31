#include "pch.h"
#include <XMLDoc.h>

void setValue(XMLDoc &doc, const XMLNodePtr &n, bool value) {
  setValue(doc, n, boolToStr(value));
}

void getValue(XMLDoc &doc, const XMLNodePtr &n, bool &value) {
  String str;
  doc.getNodeText(n, str);
  try {
    value = strToBool(str.cstr());
  } catch(Exception e) {
    int v;
    if(_stscanf(str.cstr(), _T("%d"), &v) == 1) {
      switch(v) {
      case 0: value = false; return;
      case 1: value = true;  return;
      }
    }
    throw e;
  }
}
