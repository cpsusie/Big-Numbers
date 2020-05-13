#include "pch.h"
#include <Math/Expression/ExpressionFunctionR2R3.h>
#include <D3DGraphics/ExprParametricR2R3SurfaceParameters.h>
#include <D3DGraphics/MeshCreators.h>

using namespace Expr;

LPD3DXMESH createMesh(AbstractMeshFactory &amf, const ExprParametricR2R3SurfaceParameters &param) {
  checkIsAnimation(__TFUNCTION__, param, false);
  ExpressionFunctionR2R3 f(param.m_expr, RADIANS, param.m_machineCode);
  return createMesh(amf, param, f);
}

MeshArray createMeshArray(CWnd *wnd, AbstractMeshFactory &amf, const ExprParametricR2R3SurfaceParameters &param) {
  checkIsAnimation(__TFUNCTION__, param, true);
  ExpressionFunctionR2R3 f(param.m_expr, RADIANS, param.m_machineCode);
  return createMeshArray(wnd, amf, param, f);
}
