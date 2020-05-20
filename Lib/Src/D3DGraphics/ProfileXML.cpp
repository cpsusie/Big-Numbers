#include "pch.h"
#include <MFCUtil/PolygonCurve.h>
#include <MFCUtil/2DXML.h>
#include <D3DGraphics/Profile.h>
#include <D3DGraphics/D3XML.h>

void setValue(XMLDoc &doc, XMLNodePtr n, const ProfileCurve &v) {
  PolygonCurve pc = v;
  setValue(doc, n, pc);
}

void getValue(XMLDoc &doc, XMLNodePtr n, ProfileCurve &v) {
  PolygonCurve pc;
  getValue(doc, n, pc);
  v = pc;
}

void setValue(XMLDoc &doc, XMLNodePtr n, const ProfilePolygon &v) {
  doc.setValue(n,_T("closed" ), v.m_closed);
  setValue(doc,n,_T("start"), v.m_start );
  XMLNodePtr clist = doc.createNode(n, _T("profilecurve"));
  setValue<Array<ProfileCurve>, ProfileCurve>(doc, clist, v.m_curveArray);
}

void getValue(XMLDoc &doc, XMLNodePtr n, ProfilePolygon &v) {
  doc.getValue(n, _T("closed"), v.m_closed);
  getValue(doc, n, _T("start"), v.m_start);
  XMLNodePtr clist = doc.getChild(n, _T("profilecurve"));
  getValue<Array<ProfileCurve>, ProfileCurve>(doc, clist, v.m_curveArray);
}

void setValue(XMLDoc &doc, XMLNodePtr n, const Profile &v) {
  XMLNodePtr pn = doc.createNode(n, _T("profile"));
  doc.setValue(pn, _T("name"), v.m_name);
  XMLNodePtr plist = doc.createNode(pn, _T("profilepolygon"));
  setValue<Array<ProfilePolygon>, ProfilePolygon>(doc, plist, v.m_polygonArray);
}

void getValue(XMLDoc &doc, XMLNodePtr n, Profile &v) {
  XMLNodePtr pn = doc.getChild(n, _T("profile"));
  doc.getValue(pn, _T("name"), v.m_name);
  XMLNodePtr plist = doc.getChild(pn, _T("profilepolygon"));
  getValue<Array<ProfilePolygon>, ProfilePolygon>(doc, plist, v.m_polygonArray);
}
