#include "pch.h"
#include <Math/MathXML.h>
#include <MFCUtil/MFCXML.h>
#include <D3DGraphics/Profile3D.h>
#include <D3DGraphics/D3XML.h>

// -------------------------------------------------------------------------------------

void setValue(XMLDoc &doc, XMLNodePtr n, const Point2DTo3DConverter &v) {
  const String rotStr = format(_T("%c"), v.getRotateAxis()), rotAlignsToStr = format(_T("%c"), v.getRotateAxisAlignsTo());
  setValue(doc,n, _T("rotateaxis"        ), rotStr              );
  setValue(doc,n, _T("rotateaxisalignsto"), rotAlignsToStr      );
}

void getValue(XMLDoc &doc, XMLNodePtr n, Point2DTo3DConverter &v) {
  String rotStr, rotAlignsToStr;
  getValue(doc,n, _T("rotateaxis"        ), rotStr             );
  getValue(doc,n, _T("rotateaxisalignsto"), rotAlignsToStr     );
  assert(rotStr.length()         == 1);
  assert(rotAlignsToStr.length() == 1);
  v = Point2DTo3DConverter((char)rotStr[0],(char)rotAlignsToStr[0]);
}

void setValue(XMLDoc &doc, XMLNodePtr n, const ProfileRotationParameters &v) {
  XMLNodePtr   cn = doc.createNode(n, _T("converter"));
  setValue(doc,cn                         , v.m_converter       );
  setValue(doc,n, _T("rad"               ), v.m_rad             );
  setValue(doc,n, _T("edgeCount"         ), v.m_edgeCount       );
  setValue(doc,n, _T("flags"             ), v.m_flags           );
  setValue(doc,n, _T("color"             ), v.m_color      ,true);
}

void getValue(XMLDoc &doc, XMLNodePtr n, ProfileRotationParameters &v) {
  XMLNodePtr   cn = doc.getChild(n, _T("converter"));
  getValue(doc,cn                         , v.m_converter      );
  getValue(doc,n, _T("rad"               ), v.m_rad            );
  getValue(doc,n, _T("edgeCount"         ), v.m_edgeCount      );
  getValue(doc,n, _T("flags"             ), v.m_flags          );
  getValue(doc,n, _T("color"             ), v.m_color     ,true);
}
