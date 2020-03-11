#pragma once

#include <Math/Point2D.h>
#include <MFCUtil/DirectXDeviceFactory.h>
#include "Function2DSurface.h"
#include "ParametricSurface.h"
#include "IsoSurface.h"
#include "D3AbstractMeshFactory.h"
#include "MeshArray.h"

LPD3DXMESH     createMeshFrom2DFunction(   AbstractMeshFactory &amf, Function2D &f, const DoubleInterval &xInterval, const DoubleInterval &yInterval, UINT nx, UINT ny, bool doubleSided);
LPD3DXMESH     createMesh(                 AbstractMeshFactory &amf, const Function2DSurfaceParameters &param);
MeshArray      createMeshArray(CWnd *wnd,  AbstractMeshFactory &amf, const Function2DSurfaceParameters &param);
LPD3DXMESH     createMesh(                 AbstractMeshFactory &amf, const ParametricSurfaceParameters &param);
MeshArray      createMeshArray(CWnd *wnd,  AbstractMeshFactory &amf, const ParametricSurfaceParameters &param);
LPD3DXMESH     createMesh(                 AbstractMeshFactory &amf, const IsoSurfaceParameters        &param);
MeshArray      createMeshArray(CWnd *wnd,  AbstractMeshFactory &amf, const IsoSurfaceParameters        &param);
LPD3DXMESH     createSphereMesh(           AbstractMeshFactory &amf, double                            radius);
