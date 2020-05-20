#include "pch.h"
#include <XMLUtil.h>

void setValue(XMLDoc &doc, XMLNodePtr n, const BitSet &set) {
  doc.setValue(n, _T("capacity"), set.getCapacity());
  doc.setValue(n, _T("set"     ), set.toString()   );
}

void getValue(XMLDoc &doc, XMLNodePtr n, BitSet &set) {
  UINT64 capacity;
  doc.getValue(n, _T("capacity"), capacity);
  CHECKUINT64ISVALIDSIZET(capacity);
  set.clear().setCapacity((size_t)capacity);
  String s;
  doc.getValue(n, _T("set"     ), s);
  String delim = _T("(,)");
  for(Tokenizer tok(s, delim); tok.hasNext();) {
    set.add(tok.getSizet());
  }
}
