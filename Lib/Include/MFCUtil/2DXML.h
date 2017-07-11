#pragma once

#include <XMLDoc.h>
#include <Math/Rectangle2D.h>

void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const CPoint      &p);
void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag,       CPoint      &p);
void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const CSize       &s);
void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag,       CSize       &s);
void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const Point2D     &p);
void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag,       Point2D     &p);
void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const Rectangle2D &r);
void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag,       Rectangle2D &r);
