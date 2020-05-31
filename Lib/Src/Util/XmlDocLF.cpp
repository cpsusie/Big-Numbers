#include "pch.h"
#include <XMLDoc.h>

void getValueLF(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, String &v) {
  XMLNodePtr n = doc.getChild(parent, tag);
  getValueLF(doc, n, v);
}

void getValueLF(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, String &v, const String &defaultValue) {
  XMLNodePtr n = doc.findChild(parent, tag);
  if(n) {
    getValueLF(doc, n, v);
  } else {
    v = defaultValue;
  }
}
