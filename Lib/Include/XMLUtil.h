#pragma once

#include "XmlDoc.h"
#include "NumberInterval.h"

void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const DoubleInterval &interval);
void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag,       DoubleInterval &interval);
