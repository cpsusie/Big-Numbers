#pragma once

#include <Math/Point2D.h>
#include <Math/Point3D.h>
#include <MFCUtil/DirectXDeviceFactory.h>
#include "MeshArray.h"

class AbstractMeshFactory;
class Function2DSurfaceParameters;
class ExprFunction2DSurfaceParameters;
class ParametricSurfaceParameters;
class IsoSurfaceParameters;
class ExprIsoSurfaceParameters;

LPD3DXMESH     createMeshFrom2DFunction(  AbstractMeshFactory &amf, Function2D &f, const DoubleInterval &xInterval, const DoubleInterval &yInterval, UINT nx, UINT ny, bool doubleSided);
LPD3DXMESH     createMesh(                AbstractMeshFactory &amf, const Function2DSurfaceParameters     &param, Function2D                           &f);
MeshArray      createMeshArray(CWnd *wnd, AbstractMeshFactory &amf, const Function2DSurfaceParameters     &param, FunctionWithTimeTemplate<Function2D> &f);
LPD3DXMESH     createMesh(                AbstractMeshFactory &amf, const ExprFunction2DSurfaceParameters &param);
MeshArray      createMeshArray(CWnd *wnd, AbstractMeshFactory &amf, const ExprFunction2DSurfaceParameters &param);
LPD3DXMESH     createMesh(                AbstractMeshFactory &amf, const ParametricSurfaceParameters     &param);
MeshArray      createMeshArray(CWnd *wnd, AbstractMeshFactory &amf, const ParametricSurfaceParameters     &param);
LPD3DXMESH     createMesh(                AbstractMeshFactory &amf, const IsoSurfaceParameters            &param, Function3D                           &f);
MeshArray      createMeshArray(CWnd *wnd, AbstractMeshFactory &amf, const IsoSurfaceParameters            &param, FunctionWithTimeTemplate<Function3D> &f);
LPD3DXMESH     createMesh(                AbstractMeshFactory &amf, const ExprIsoSurfaceParameters        &param);
MeshArray      createMeshArray(CWnd *wnd, AbstractMeshFactory &amf, const ExprIsoSurfaceParameters        &param);
LPD3DXMESH     createSphereMesh(          AbstractMeshFactory &amf, double radius);
