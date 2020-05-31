#include "pch.h"
#include <Date.h>
#include <XmlUtil.h>

void setValue(XMLDoc &doc, const XMLNodePtr &n, const Date &d) {
  doc.setNodeValue(n, d);
}

void getValue(XMLDoc &doc, const XMLNodePtr &n, Date &d) {
  doc.getNodeValue(n, d);
}
