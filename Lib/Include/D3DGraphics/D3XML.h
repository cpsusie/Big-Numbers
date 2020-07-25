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

void setValue(XMLDoc &doc, XMLNodePtr n, const D3DMATERIAL    &v);
void getValue(XMLDoc &doc, XMLNodePtr n,       D3DMATERIAL    &v);
void setValue(XMLDoc &doc, XMLNodePtr n, const D3MATERIAL     &v);
void getValue(XMLDoc &doc, XMLNodePtr n,       D3MATERIAL     &v);
void setValue(XMLDoc &doc, XMLNodePtr n, const MaterialMap    &map);
void getValue(XMLDoc &doc, XMLNodePtr n,       MaterialMap    &map);

void setValue(XMLDoc &doc, XMLNodePtr n, const D3DLIGHT       &v);
void getValue(XMLDoc &doc, XMLNodePtr n,       D3DLIGHT       &v);
void setValue(XMLDoc &doc, XMLNodePtr n, const D3Light        &v);
void getValue(XMLDoc &doc, XMLNodePtr n,       D3Light        &v);
void setValue(XMLDoc &doc, XMLNodePtr n, const LightArray     &a);
void getValue(XMLDoc &doc, XMLNodePtr n,       LightArray     &a);

class D3World;
class D3Camera;
class D3CameraArray;
class D3SceneObjectVisual;
class D3Scene;

void setValue(XMLDoc &doc, XMLNodePtr n, const D3World             &w);
void getValue(XMLDoc &doc, XMLNodePtr n,       D3World             &w);

void setValue(XMLDoc &doc, XMLNodePtr n, const D3Camera            &c);
void getValue(XMLDoc &doc, XMLNodePtr n,       D3Camera            &c);

void setValue(XMLDoc &doc, XMLNodePtr n, const D3CameraArray       &a);
void getValue(XMLDoc &doc, XMLNodePtr n,       D3CameraArray       &a);

void setValue(XMLDoc &doc, XMLNodePtr n, const D3SceneObjectVisual &v);
void getValue(XMLDoc &doc, XMLNodePtr n,       D3SceneObjectVisual &v);

inline void setValue(XMLDoc &doc, XMLNodePtr n, const D3SceneObjectVisual *v) {
  setValue(doc, n, *v);
}
inline void getValue(XMLDoc &doc, XMLNodePtr n,       D3SceneObjectVisual *v) {
  getValue(doc, n, *v);
}

void setValue(XMLDoc &doc, XMLNodePtr n, const D3Scene             &s);
void getValue(XMLDoc &doc, XMLNodePtr n,       D3Scene             &s);

class ProfileCurve2D;
class ProfilePolygon2D;
class Profile2D;
class Point2DTo3DConverter;
class ProfileRotationParameters;

void setValue(XMLDoc &doc, XMLNodePtr n, const ProfileCurve2D            &v);
void getValue(XMLDoc &doc, XMLNodePtr n,       ProfileCurve2D            &v);

void setValue(XMLDoc &doc, XMLNodePtr n, const ProfilePolygon2D          &v);
void getValue(XMLDoc &doc, XMLNodePtr n,       ProfilePolygon2D          &v);

void setValue(XMLDoc &doc, XMLNodePtr n, const Profile2D                 &v);
void getValue(XMLDoc &doc, XMLNodePtr n,       Profile2D                 &v);

void setValue(XMLDoc &doc, XMLNodePtr n, const Point2DTo3DConverter      &v);
void getValue(XMLDoc &doc, XMLNodePtr n,       Point2DTo3DConverter      &v);

void setValue(XMLDoc &doc, XMLNodePtr n, const ProfileRotationParameters &v);
void getValue(XMLDoc &doc, XMLNodePtr n,       ProfileRotationParameters &v);

class D3SurfaceCommonParameters;

void setValue(XMLDoc &doc, XMLNodePtr n, const D3SurfaceCommonParameters &v);
void getValue(XMLDoc &doc, XMLNodePtr n,       D3SurfaceCommonParameters &v);

struct _D3DVERTEXBUFFER_DESC;
struct _D3DINDEXBUFFER_DESC;

void setValue(XMLDoc &doc, XMLNodePtr n, const struct _D3DVERTEXBUFFER_DESC &d);
void getValue(XMLDoc &doc, XMLNodePtr n,       struct _D3DVERTEXBUFFER_DESC &d);


void setValue(XMLDoc &doc, XMLNodePtr n, const struct _D3DINDEXBUFFER_DESC  &d);
void getValue(XMLDoc &doc, XMLNodePtr n,       struct _D3DINDEXBUFFER_DESC  &d);

void setValue(XMLDoc &doc, XMLNodePtr n, const LPDIRECT3DVERTEXBUFFER      &vb);
void getValue(XMLDoc &doc, XMLNodePtr n,       LPDIRECT3DVERTEXBUFFER      &vb);

void setValue(XMLDoc &doc, XMLNodePtr n, const LPDIRECT3DINDEXBUFFER       &ib);
void getValue(XMLDoc &doc, XMLNodePtr n,       LPDIRECT3DINDEXBUFFER       &ib);

void setValue(XMLDoc &doc, XMLNodePtr n, const LPD3DXMESH                   &m);
void getValue(XMLDoc &doc, XMLNodePtr n,       LPD3DXMESH                   &m);

class FunctionR2R1SurfaceParameters;
class IsoSurfaceParameters;
class ParametricR1R3SurfaceParameters;
class ParametricR2R3SurfaceParameters;

class ExprFunctionR2R1SurfaceParameters;
class ExprIsoSurfaceParameters;
class ExprParametricR1R3SurfaceParameters;
class ExprParametricR2R3SurfaceParameters;

void setValue(XMLDoc &doc, XMLNodePtr n, const FunctionR2R1SurfaceParameters       &v);
void getValue(XMLDoc &doc, XMLNodePtr n,       FunctionR2R1SurfaceParameters       &v);

void setValue(XMLDoc &doc, XMLNodePtr n, const IsoSurfaceParameters                &v);
void getValue(XMLDoc &doc, XMLNodePtr n,       IsoSurfaceParameters                &v);

void setValue(XMLDoc &doc, XMLNodePtr n, const ParametricR1R3SurfaceParameters     &v);
void getValue(XMLDoc &doc, XMLNodePtr n,       ParametricR1R3SurfaceParameters     &v);

void setValue(XMLDoc &doc, XMLNodePtr n, const ParametricR2R3SurfaceParameters     &v);
void getValue(XMLDoc &doc, XMLNodePtr n,       ParametricR2R3SurfaceParameters     &v);

void setValue(XMLDoc &doc, XMLNodePtr n, const ExprFunctionR2R1SurfaceParameters   &v);
void getValue(XMLDoc &doc, XMLNodePtr n,       ExprFunctionR2R1SurfaceParameters   &v);

void setValue(XMLDoc &doc, XMLNodePtr n, const ExprIsoSurfaceParameters            &v);
void getValue(XMLDoc &doc, XMLNodePtr n,       ExprIsoSurfaceParameters            &v);

void setValue(XMLDoc &doc, XMLNodePtr n, const ExprParametricR1R3SurfaceParameters &v);
void getValue(XMLDoc &doc, XMLNodePtr n,       ExprParametricR1R3SurfaceParameters &v);

void setValue(XMLDoc &doc, XMLNodePtr n, const ExprParametricR2R3SurfaceParameters &v);
void getValue(XMLDoc &doc, XMLNodePtr n,       ExprParametricR2R3SurfaceParameters &v);
