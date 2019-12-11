#pragma once

#include "XmlDoc.h"
#include "NumberInterval.h"

template<typename T> void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const NumberInterval<T> &interval) {
  XMLNodePtr n = doc.createNode(parent, tag);
  doc.setValue(n, _T("from"), interval.getFrom());
  doc.setValue(n, _T("to")  , interval.getTo());
}

template<typename T> void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, NumberInterval<T> &interval) {
  XMLNodePtr n = PersistentData::getChild(doc, parent, tag);
  T from, to;
  doc.getValue(n, _T("from"), from);
  doc.getValue(n, _T("to")  , to  );
  interval.setFrom(from);
  interval.setTo(to);
}
