#pragma once

#include <XMLUtil.h>
#include "GraphParameters.h"

void setValue(XMLDoc &doc, XMLNodePtr n, GraphStyle         style     );
void getValue(XMLDoc &doc, XMLNodePtr n, GraphStyle        &style     );
void setValue(XMLDoc &doc, XMLNodePtr n, const RollingAvg  &rollingAvg);
void getValue(XMLDoc &doc, XMLNodePtr n, RollingAvg        &rollingAvg);
