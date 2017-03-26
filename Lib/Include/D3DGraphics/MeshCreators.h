#pragma once

#include <Math/Point2D.h>
#include "Function2DSurface.h"
#include "ParametricSurface.h"
#include "IsoSurface.h"
#include "D3DeviceFactory.h"
#include "MeshArray.h"

LPD3DXMESH     createMeshFrom2DFunction(   DIRECT3DDEVICE device, Function2D &f, const DoubleInterval &xInterval, const DoubleInterval &yInterval, UINT nx, UINT ny, bool doubleSided);
LPD3DXMESH     createMesh(                 DIRECT3DDEVICE device, const Function2DSurfaceParameters &param);
MeshArray      createMeshArray(CWnd *wnd,  DIRECT3DDEVICE device, const Function2DSurfaceParameters &param);
LPD3DXMESH     createMesh(                 DIRECT3DDEVICE device, const ParametricSurfaceParameters &param);
MeshArray      createMeshArray(CWnd *wnd,  DIRECT3DDEVICE device, const ParametricSurfaceParameters &param);
LPD3DXMESH     createMesh(                 DIRECT3DDEVICE device, const IsoSurfaceParameters        &param);
MeshArray      createMeshArray(CWnd *wnd,  DIRECT3DDEVICE device, const IsoSurfaceParameters        &param);
LPD3DXMESH     createSphereMesh(           DIRECT3DDEVICE device, double                            radius);

D3SceneObject *createIsoSurfaceDebugObject(D3Scene &scene,        const IsoSurfaceParameters        &param);
