#pragma once

#include <XMLUtil.h>
#include <Math/Cube3D.h>
#include "D3Material.h"
#include "D3Light.h"
#include "D3Math.h"

template<typename T> void setValue(XMLDoc &doc, XMLNodePtr n, const Point3DTemplate<T> &p) {
  doc.setValue(n, _T("x"), p.x);
  doc.setValue(n, _T("y"), p.y);
  doc.setValue(n, _T("z"), p.z);
}

template<typename T> void getValue(XMLDoc &doc, XMLNodePtr n, Point3DTemplate<T> &p) {
  doc.getValue(n, _T("x"), p.x);
  doc.getValue(n, _T("y"), p.y);
  doc.getValue(n, _T("z"), p.z);
}

void setValue(XMLDoc &doc, XMLNodePtr n, const D3DXVECTOR3    &v);
void getValue(XMLDoc &doc, XMLNodePtr n,       D3DXVECTOR3    &v);
void setValue(XMLDoc &doc, XMLNodePtr n, const D3DVECTOR      &v);
void getValue(XMLDoc &doc, XMLNodePtr n,       D3DVECTOR      &v);
void setValue(XMLDoc &doc, XMLNodePtr n, const D3DXQUATERNION &q);
void getValue(XMLDoc &doc, XMLNodePtr n,       D3DXQUATERNION &q);

template<typename T> void setValue(XMLDoc &doc, XMLNodePtr n, const Cube3DTemplate<T> &cube) {
  setValue(doc, n, _T("lbn"), cube.LBN());
  setValue(doc, n, _T("rtf"), cube.RTF());
}

template<typename T> void getValue(XMLDoc &doc, XMLNodePtr n, Cube3DTemplate<T> &cube) {
  Point3DTemplate<T> lbn, rtf;
  getValue(doc, n, _T("lbn"), lbn);
  getValue(doc, n, _T("rtf"), rtf);
  cube = Cube3DTemplate<T>(lbn, rtf);
}

void setValue(XMLDoc &doc, XMLNodePtr n, const D3DCOLORVALUE &v);
void getValue(XMLDoc &doc, XMLNodePtr n, D3DCOLORVALUE       &v);

void setValue(XMLDoc &doc, XMLNodePtr n, const D3DMATERIAL   &v);
void getValue(XMLDoc &doc, XMLNodePtr n, D3DMATERIAL         &v);
void setValue(XMLDoc &doc, XMLNodePtr n, const MaterialMap   &map);
void getValue(XMLDoc &doc, XMLNodePtr n, MaterialMap         &map);

void setValue(XMLDoc &doc, XMLNodePtr n, const D3DLIGHT      &v);
void getValue(XMLDoc &doc, XMLNodePtr n, D3DLIGHT            &v);
void setValue(XMLDoc &doc, XMLNodePtr n, const D3Light       &v);
void getValue(XMLDoc &doc, XMLNodePtr n, D3Light             &v);
void setValue(XMLDoc &doc, XMLNodePtr n, const LightArray    &a);
void getValue(XMLDoc &doc, XMLNodePtr n, LightArray          &a);

class D3World;
class D3Camera;
class D3CameraArray;
class D3Scene;
class MeshAnimationData;

void setValue(XMLDoc &doc, XMLNodePtr n, const D3World       &w);
void getValue(XMLDoc &doc, XMLNodePtr n,       D3World       &w);

void setValue(XMLDoc &doc, XMLNodePtr n, const D3Camera      &c);
void getValue(XMLDoc &doc, XMLNodePtr n,       D3Camera      &c);

void setValue(XMLDoc &doc, XMLNodePtr n, const D3CameraArray &a);
void getValue(XMLDoc &doc, XMLNodePtr n,       D3CameraArray &a);

void setValue(XMLDoc &doc, XMLNodePtr n, const D3Scene       &s);
void getValue(XMLDoc &doc, XMLNodePtr n,       D3Scene       &s);

void setValue(XMLDoc &doc, XMLNodePtr n, const MeshAnimationData &d);
void getValue(XMLDoc &doc, XMLNodePtr n,       MeshAnimationData &d);

class ProfileCurve;
class ProfilePolygon;
class Profile;

void setValue(XMLDoc &doc, XMLNodePtr n, const ProfileCurve   &v);
void getValue(XMLDoc &doc, XMLNodePtr n, ProfileCurve         &v);

void setValue(XMLDoc &doc, XMLNodePtr n, const ProfilePolygon &v);
void getValue(XMLDoc &doc, XMLNodePtr n, ProfilePolygon       &v);

void setValue(XMLDoc &doc, XMLNodePtr n, const Profile        &v);
void getValue(XMLDoc &doc, XMLNodePtr n, Profile              &v);
