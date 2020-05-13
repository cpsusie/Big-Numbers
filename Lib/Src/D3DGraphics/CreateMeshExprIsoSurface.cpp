#include "pch.h"
#include <Math/Expression/ExpressionFunctionR3R1.h>
#include <D3DGraphics/ExprIsoSurfaceParameters.h>
#include <D3DGraphics/MeshCreators.h>

/* ---------------- Implicit surface polygonizer supportfunctions ------------------- */

using namespace Expr;

LPD3DXMESH createMesh(AbstractMeshFactory &amf, const ExprIsoSurfaceParameters &param) {
  checkIsAnimation(__TFUNCTION__, param, false);
  ExpressionFunctionR3R1 f(param.m_expr, RADIANS, param.m_machineCode);
  return createMesh(amf, param, f);
}

MeshArray createMeshArray(CWnd *wnd, AbstractMeshFactory &amf, const ExprIsoSurfaceParameters &param) {
  checkIsAnimation(__TFUNCTION__, param, true);
  ExpressionFunctionR3R1 f(param.m_expr, RADIANS, param.m_machineCode);
  return createMeshArray(wnd, amf, param, f);
}

