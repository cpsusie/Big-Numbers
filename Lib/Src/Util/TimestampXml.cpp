#include "pch.h"
#include <Date.h>
#include <XmlUtil.h>

void setValue(XMLDoc &doc, const XMLNodePtr &n, const Timestamp &t) {
  doc.setNodeValue(n, t);
}

void getValue(XMLDoc &doc, const XMLNodePtr &n, Timestamp &t) {
  doc.getNodeValue(n, t);
}
