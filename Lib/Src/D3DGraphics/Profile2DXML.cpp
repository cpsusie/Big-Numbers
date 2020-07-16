#include "pch.h"
#include <Math/MathXML.h>
#include <MFCUtil/MFCXML.h>
#include <MFCUtil/PolygonCurve2D.h>
#include <D3DGraphics/Profile2D.h>
#include <D3DGraphics/D3XML.h>

void setValue(XMLDoc &doc, XMLNodePtr n, const ProfileCurve2D &v) {
  PolygonCurve2D pc = v;
  setValue(doc, n, pc);
}

void getValue(XMLDoc &doc, XMLNodePtr n, ProfileCurve2D &v) {
  PolygonCurve2D pc;
  getValue(doc, n, pc);
  v = pc;
}

void setValue(XMLDoc &doc, XMLNodePtr n, const ProfilePolygon2D &v) {
  setValue(doc, n,_T("closed"), v.m_closed);
  setValue(doc, n,_T("start" ), v.m_start );
  XMLNodePtr clist = doc.createNode(n, _T("profilecurve"));
  setValue<Array<ProfileCurve2D>, ProfileCurve2D>(doc, clist, v.m_curveArray);
}

void getValue(XMLDoc &doc, XMLNodePtr n, ProfilePolygon2D &v) {
  getValue(doc, n, _T("closed"), v.m_closed);
  getValue(doc, n, _T("start" ), v.m_start );
  XMLNodePtr clist = doc.getChild(n, _T("profilecurve"));
  getValue<Array<ProfileCurve2D>, ProfileCurve2D>(doc, clist, v.m_curveArray);
}

void setValue(XMLDoc &doc, XMLNodePtr n, const Profile2D &v) {
  XMLNodePtr pn = doc.createNode(n, _T("profile"));
  setValue(doc, pn, _T("name"), v.m_name);
  XMLNodePtr plist = doc.createNode(pn, _T("profilepolygon"));
  setValue<Array<ProfilePolygon2D>, ProfilePolygon2D>(doc, plist, v.m_polygonArray);
}

void getValue(XMLDoc &doc, XMLNodePtr n, Profile2D &v) {
  XMLNodePtr pn = doc.getChild(n, _T("profile"));
  getValue(doc, pn, _T("name"), v.m_name);
  XMLNodePtr plist = doc.getChild(pn, _T("profilepolygon"));
  getValue<Array<ProfilePolygon2D>, ProfilePolygon2D>(doc, plist, v.m_polygonArray);
}

void setValue(XMLDoc &doc, XMLNodePtr n, const ProfileRotationParameters &v) {
  const String rotStr = format(_T("%c"), v.m_rotateAxis), rotAlignsToStr = format(_T("%c"), v.m_rotateAxisAlignsTo);
  setValue(doc,n, _T("rotateaxis"        ), rotStr              );
  setValue(doc,n, _T("rotateaxisalignsto"), rotAlignsToStr      );
  setValue(doc,n, _T("rad"               ), v.m_rad             );
  setValue(doc,n, _T("edgeCount"         ), v.m_edgeCount       );
  setValue(doc,n, _T("flags"             ), v.m_flags           );
  setValue(doc,n, _T("color"             ), v.m_color      ,true);
}

void getValue(XMLDoc &doc, XMLNodePtr n, ProfileRotationParameters &v) {
  String rotStr, rotAlignsToStr;
  getValue(doc,n, _T("rotateaxis"        ), rotStr             );
  getValue(doc,n, _T("rotateaxisalignsto"), rotAlignsToStr     );
  getValue(doc,n, _T("rad"               ), v.m_rad            );
  getValue(doc,n, _T("edgeCount"         ), v.m_edgeCount      );
  getValue(doc,n, _T("flags"             ), v.m_flags          );
  getValue(doc,n, _T("color"             ), v.m_color     ,true);

  assert(rotStr.length()         == 1);
  assert(rotAlignsToStr.length() == 1);
  v.m_rotateAxis         = (char)rotStr[0];
  v.m_rotateAxisAlignsTo = (char)rotAlignsToStr[0];
}
