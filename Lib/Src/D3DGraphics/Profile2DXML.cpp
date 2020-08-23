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
  XMLNodePtr clist = doc.createNode(n, _T("profilecurve2d"));
  setValue<Array<ProfileCurve2D>, ProfileCurve2D>(doc, clist, v.m_curveArray);
}

void getValue(XMLDoc &doc, XMLNodePtr n, ProfilePolygon2D &v) {
  getValue(doc, n, _T("closed"), v.m_closed);
  getValue(doc, n, _T("start" ), v.m_start );
  XMLNodePtr clist = doc.getChild(n, _T("profilecurve2d"));
  getValue<Array<ProfileCurve2D>, ProfileCurve2D>(doc, clist, v.m_curveArray);
}

void setValue(XMLDoc &doc, XMLNodePtr n, const Profile2D &v) {
  XMLNodePtr plist = doc.createNode(n, _T("profilepolygon2d"));
  setValue<Array<ProfilePolygon2D>, ProfilePolygon2D>(doc, plist, v.m_polygonArray);
}

void getValue(XMLDoc &doc, XMLNodePtr n, Profile2D &v) {
  XMLNodePtr plist = doc.getChild(n, _T("profilepolygon2d"));
  getValue<Array<ProfilePolygon2D>, ProfilePolygon2D>(doc, plist, v.m_polygonArray);
}

void Profile2D::putDataToDoc(XMLDoc &doc) {
  XMLNodePtr       root = doc.createRoot(_T("profile2d"));
  setValue(doc,    root, *this);
}

void Profile2D::getDataFromDoc(XMLDoc &doc) {
  XMLNodePtr       root = doc.getRoot();
  XMLDoc::checkTag(root, _T("profile2d"));
  getValue(doc,    root, *this);
}

