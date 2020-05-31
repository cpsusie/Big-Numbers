#include "pch.h"
#include <Date.h>
#include <XmlUtil.h>

void setValue(XMLDoc &doc, const XMLNodePtr &n, const Time &t) {
  doc.setNodeValue(n, t);
}

void setValue(XMLDoc &doc, const XMLNodePtr &n, Time &t) {
  doc.setNodeValue(n, t);
}
