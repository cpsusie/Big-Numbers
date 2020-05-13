#include "pch.h"
#include <Math/Expression/ExpressionFunctionR2R1.h>
#include <D3DGraphics/ExprFunctionR2R1SurfaceParameters.h>
#include <D3DGraphics/MeshCreators.h>

using namespace Expr;

LPD3DXMESH createMesh(AbstractMeshFactory &amf, const ExprFunctionR2R1SurfaceParameters &param) {
  checkIsAnimation(__TFUNCTION__, param, false);
  ExpressionFunctionR2R1 f(param.m_expr, RADIANS, param.m_machineCode);
  return createMesh(amf, param, f);
}

MeshArray createMeshArray(CWnd *wnd, AbstractMeshFactory &amf, const ExprFunctionR2R1SurfaceParameters &param) {
  checkIsAnimation(__TFUNCTION__, param, true);
  ExpressionFunctionR2R1 f(param.m_expr, RADIANS, param.m_machineCode);
  return createMeshArray(wnd, amf, param, f);
}
