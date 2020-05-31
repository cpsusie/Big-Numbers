#include "pch.h"
#include <BitSet.h>
#include <Tokenizer.h>
#include <XMLUtil.h>

void setValue(XMLDoc &doc, XMLNodePtr n, const BitSet &set) {
  setValue(doc, n, _T("capacity"), set.getCapacity());
  setValue(doc, n, _T("set"     ), set.toString()   );
}

void getValue(XMLDoc &doc, XMLNodePtr n, BitSet &set) {
  UINT64 capacity;
  getValue(doc, n, _T("capacity"), capacity);
  CHECKUINT64ISVALIDSIZET(capacity);
  set.clear().setCapacity((size_t)capacity);
  String s;
  getValue(doc, n, _T("set"     ), s);
  String delim = _T("(,)");
  for(Tokenizer tok(s, delim); tok.hasNext();) {
    set.add(tok.getSizet());
  }
}
