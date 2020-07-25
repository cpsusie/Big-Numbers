#include "pch.h"
#include <Math/Expression/ExpressionFunctionR1R3.h>
#include <D3DGraphics/ExprParametricR1R3SurfaceParameters.h>
#include <D3DGraphics/MeshCreators.h>

using namespace Expr;

LPD3DXMESH createMesh(AbstractMeshFactory &amf, const ExprParametricR1R3SurfaceParameters &param) {
  checkIsAnimation(__TFUNCTION__, param, false);
  ExpressionFunctionR1R3 f(param.m_expr, RADIANS, param.m_machineCode);
  return createMesh(amf, param, f);
}

MeshArray createMeshArray(CWnd *wnd, AbstractMeshFactory &amf, const ExprParametricR1R3SurfaceParameters &param) {
  checkIsAnimation(__TFUNCTION__, param, true);
  ExpressionFunctionR1R3 f(param.m_expr, RADIANS, param.m_machineCode);
  return createMeshArray(wnd, amf, param, f);
}
