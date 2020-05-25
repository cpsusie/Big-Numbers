#include "pch.h"
#include <MFCUtil/MFCXML.h>

void setValue(XMLDoc &doc, XMLNodePtr n, const CPoint      &p) {
  doc.setValue(n, _T("x"), p.x);
  doc.setValue(n, _T("y"), p.y);
}
void getValue(XMLDoc &doc, XMLNodePtr n, CPoint            &p) {
  doc.getValue(n, _T("x"), p.x);
  doc.getValue(n, _T("y"), p.y);
}

void setValue(XMLDoc &doc, XMLNodePtr n, const CSize       &s) {
  doc.setValue(n, _T("cx"), s.cx);
  doc.setValue(n, _T("cy"), s.cy);
}

void getValue(XMLDoc &doc, XMLNodePtr n, CSize             &s) {
  doc.getValue(n, _T("cx"), s.cx);
  doc.getValue(n, _T("cy"), s.cy);
}
