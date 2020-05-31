#include "pch.h"
#include <MFCUtil/MFCXML.h>

void setValue(XMLDoc &doc, XMLNodePtr n, const CPoint      &p) {
  setValue(doc, n, _T("x"), p.x);
  setValue(doc, n, _T("y"), p.y);
}
void getValue(XMLDoc &doc, XMLNodePtr n, CPoint            &p) {
  getValue(doc, n, _T("x"), p.x);
  getValue(doc, n, _T("y"), p.y);
}

void setValue(XMLDoc &doc, XMLNodePtr n, const CSize       &s) {
  setValue(doc, n, _T("cx"), s.cx);
  setValue(doc, n, _T("cy"), s.cy);
}

void getValue(XMLDoc &doc, XMLNodePtr n, CSize             &s) {
  getValue(doc, n, _T("cx"), s.cx);
  getValue(doc, n, _T("cy"), s.cy);
}
