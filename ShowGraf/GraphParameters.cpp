#include "stdafx.h"
#include "GraphXML.h"
#include "GraphParameters.h"

GraphParameters::GraphParameters(const String &name, COLORREF color, const RollingAvg &rollingAvg, GraphStyle style) {
  setName(name);
  m_color       = color;
  m_rollingAvg  = rollingAvg;
  m_style       = style;
};

void GraphParameters::putDataToDoc(  XMLDoc &doc) {
  XMLNodePtr root = doc.getRoot();
  setValue(doc, root                   , m_style      );
  setValue(doc, root, _T("color"      ), m_color, true);
  setValue(doc, root, _T("rollingavg" ), m_rollingAvg );
}

void GraphParameters::getDataFromDoc(XMLDoc &doc) {
  try {
    XMLNodePtr root = doc.getRoot();
    getValue(doc, root                  , m_style      );
    getValue(doc, root, _T("color"     ), m_color, true);
    getValue(doc, root, _T("rollingavg"), m_rollingAvg );
  } catch(...) {
    getDataFromDocOld(doc);
  }
}

void GraphParameters::getDataFromDocOld(XMLDoc &doc) {
  XMLNodePtr root = doc.getRoot();
  getValue(doc, root, m_style);
  getValue(doc, root, _T("color"      ), m_color, true   );
  UINT queueSize;
  getValue(doc, root, _T("rollsize"   ), queueSize       );
  m_rollingAvg = RollingAvg::isValidQueueSize(queueSize) ? RollingAvg(true, queueSize) : RollingAvg::s_default;
}
