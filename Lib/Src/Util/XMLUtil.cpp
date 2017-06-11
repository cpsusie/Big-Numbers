#include "pch.h"
#include <PersistentData.h>
#include <XmlUtil.h>

void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const DoubleInterval &interval) {
  XMLNodePtr n = doc.createNode(parent, tag);
  doc.setValue(n, _T("from"), interval.getFrom());
  doc.setValue(n, _T("to")  , interval.getTo());
}

void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, DoubleInterval &interval) {
  XMLNodePtr n = PersistentData::getChild(doc, parent, tag);
  double from, to;
  doc.getValue(n, _T("from"), from);
  doc.getValue(n, _T("to")  , to  );
  interval.setFrom(from);
  interval.setTo(to);
}
