#include "pch.h"
#include <XMLUtil.h>

void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const BitSet &set) {
  XMLNodePtr n = doc.createNode(parent, tag);
  doc.setValue(n, _T("capacity"), set.getCapacity());
  doc.setValue(n, _T("set"     ), set.toString()   );
}

void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, BitSet &set) {
  XMLNodePtr n = doc.getChild(parent, tag);
  UINT64 capacity;
  doc.getValue(n, _T("capacity"), capacity);
  CHECKUINT64ISVALIDSIZET(capacity);
  set.clear().setCapacity(capacity);
  String s;
  doc.getValue(n, _T("set"     ), s);
  String delim = _T("(,)");
  for(Tokenizer tok(s, delim); tok.hasNext();) {
    set.add(tok.getSizet());
  }
}