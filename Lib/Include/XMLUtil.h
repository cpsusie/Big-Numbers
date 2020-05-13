#pragma once

#include "XmlDoc.h"
#include "MathUtil.h"
#include "NumberInterval.h"

inline void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, BYTE  b) {
  doc.setValue(parent, tag, (UINT)b);
}

inline void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, BYTE &b) {
  UINT tmp;
  doc.getValue(parent, tag, tmp);
  assert(isUchar(tmp));
  b = (BYTE)tmp;
}


template<typename T> void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const NumberInterval<T> &interval) {
  XMLNodePtr n = doc.createNode(parent, tag);
  doc.setValue(n, _T("from"), interval.getFrom());
  doc.setValue(n, _T("to")  , interval.getTo());
}

template<typename T> void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, NumberInterval<T> &interval) {
  XMLNodePtr n = doc.getChild(parent, tag);
  T from, to;
  doc.getValue(n, _T("from"), from);
  doc.getValue(n, _T("to")  , to  );
  interval.setFrom(from);
  interval.setTo(to);
}

void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const BitSet &set);
void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag,       BitSet &set);
