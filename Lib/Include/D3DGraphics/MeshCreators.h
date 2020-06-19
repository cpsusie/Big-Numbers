#pragma once

#include <Math/Point2D3D.h>
#include <MFCUtil/DirectXDeviceFactory.h>
#include "MeshArray.h"

class AbstractMeshFactory;
class FunctionR2R1SurfaceParameters;
class ExprFunctionR2R1SurfaceParameters;
class ParametricR2R3SurfaceParameters;
class ExprParametricR2R3SurfaceParameters;
class IsoSurfaceParameters;
class ExprIsoSurfaceParameters;

// fvf should be any combination of D3DFVF_XYZ, D3DFVF_NORMAL, D3DFVF_TEX1. D3DFVF_XYZ always set
LPD3DXMESH     createMeshFrom2DFunction(  AbstractMeshFactory &amf, FunctionR2R1 &f, const DoubleInterval &xInterval, const DoubleInterval &yInterval, UINT nx, UINT ny, bool doubleSided, DWORD fvf = D3DFVF_XYZ | D3DFVF_NORMAL);
LPD3DXMESH     createMesh(                AbstractMeshFactory &amf, const FunctionR2R1SurfaceParameters       &param, FunctionR2R1                           &f);
MeshArray      createMeshArray(CWnd *wnd, AbstractMeshFactory &amf, const FunctionR2R1SurfaceParameters       &param, FunctionWithTimeTemplate<FunctionR2R1> &f);
LPD3DXMESH     createMesh(                AbstractMeshFactory &amf, const ExprFunctionR2R1SurfaceParameters   &param);
MeshArray      createMeshArray(CWnd *wnd, AbstractMeshFactory &amf, const ExprFunctionR2R1SurfaceParameters   &param);
LPD3DXMESH     createMesh(                AbstractMeshFactory &amf, const ParametricR2R3SurfaceParameters     &param, FunctionR2R3                           &f);
MeshArray      createMeshArray(CWnd *wnd, AbstractMeshFactory &amf, const ParametricR2R3SurfaceParameters     &param, FunctionWithTimeTemplate<FunctionR2R3> &f);
LPD3DXMESH     createMesh(                AbstractMeshFactory &amf, const ExprParametricR2R3SurfaceParameters &param);
MeshArray      createMeshArray(CWnd *wnd, AbstractMeshFactory &amf, const ExprParametricR2R3SurfaceParameters &param);
LPD3DXMESH     createMesh(                AbstractMeshFactory &amf, const IsoSurfaceParameters                &param, FunctionR3R1                           &f);
MeshArray      createMeshArray(CWnd *wnd, AbstractMeshFactory &amf, const IsoSurfaceParameters                &param, FunctionWithTimeTemplate<FunctionR3R1> &f);
LPD3DXMESH     createMesh(                AbstractMeshFactory &amf, const ExprIsoSurfaceParameters            &param);
MeshArray      createMeshArray(CWnd *wnd, AbstractMeshFactory &amf, const ExprIsoSurfaceParameters            &param);
LPD3DXMESH     createSphereMesh(          AbstractMeshFactory &amf, double radius);

template<typename T> void checkIsAnimation(const TCHAR *method, const T &param, bool mustBeAnimation) {
  if(param.isAnimated() != mustBeAnimation) {
    if(mustBeAnimation) {
      throwInvalidArgumentException(method, _T("param is not animation"));
    } else {
      throwInvalidArgumentException(method, _T("param is animation"));
    }
  }
}
