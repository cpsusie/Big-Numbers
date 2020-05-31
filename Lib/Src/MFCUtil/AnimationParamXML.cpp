#include "pch.h"
#include <XMLUtil.h>
#include <MFCUtil/AnimationParameters.h>

void setValue(XMLDoc &doc, XMLNodePtr n, const AnimationParameters &d) {
  setValue(doc,  n, _T("includetime"       ), d.m_includeTime     );
  if(d.m_includeTime) {
    setValue(doc, n, _T("timeinterval"     ), d.m_timeInterval    );
    setValue(doc, n, _T("framecount"       ), d.m_frameCount      );
  }
}

void getValue(XMLDoc &doc, XMLNodePtr n, AnimationParameters &d) {
  d.reset();
  getValue(doc,  n, _T("includetime"       ), d.m_includeTime     );
  if(d.m_includeTime) {
    getValue(doc, n, _T("timeinterval"     ), d.m_timeInterval    );
    getValue(doc, n, _T("framecount"       ), d.m_frameCount      );
  }
}
