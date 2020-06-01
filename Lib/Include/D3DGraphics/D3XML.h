#pragma once

#include <XMLUtil.h>
#include "D3Material.h"
#include "D3Light.h"
#include "D3Math.h"

void setValue(XMLDoc &doc, XMLNodePtr n, const D3DXVECTOR3    &v);
void getValue(XMLDoc &doc, XMLNodePtr n,       D3DXVECTOR3    &v);
void setValue(XMLDoc &doc, XMLNodePtr n, const D3DVECTOR      &v);
void getValue(XMLDoc &doc, XMLNodePtr n,       D3DVECTOR      &v);
void setValue(XMLDoc &doc, XMLNodePtr n, const D3DXQUATERNION &q);
void getValue(XMLDoc &doc, XMLNodePtr n,       D3DXQUATERNION &q);

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

void setValue(XMLDoc &doc, XMLNodePtr n, const D3World       &w);
void getValue(XMLDoc &doc, XMLNodePtr n,       D3World       &w);

void setValue(XMLDoc &doc, XMLNodePtr n, const D3Camera      &c);
void getValue(XMLDoc &doc, XMLNodePtr n,       D3Camera      &c);

void setValue(XMLDoc &doc, XMLNodePtr n, const D3CameraArray &a);
void getValue(XMLDoc &doc, XMLNodePtr n,       D3CameraArray &a);

void setValue(XMLDoc &doc, XMLNodePtr n, const D3Scene       &s);
void getValue(XMLDoc &doc, XMLNodePtr n,       D3Scene       &s);

class ProfileCurve;
class ProfilePolygon;
class Profile;
class ProfileRotationParameters;

void setValue(XMLDoc &doc, XMLNodePtr n, const ProfileCurve   &v);
void getValue(XMLDoc &doc, XMLNodePtr n, ProfileCurve         &v);

void setValue(XMLDoc &doc, XMLNodePtr n, const ProfilePolygon &v);
void getValue(XMLDoc &doc, XMLNodePtr n, ProfilePolygon       &v);

void setValue(XMLDoc &doc, XMLNodePtr n, const Profile        &v);
void getValue(XMLDoc &doc, XMLNodePtr n, Profile              &v);

void setValue(XMLDoc &doc, XMLNodePtr n, const ProfileRotationParameters &v);
void getValue(XMLDoc &doc, XMLNodePtr n,       ProfileRotationParameters &v);
