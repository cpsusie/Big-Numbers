#include "pch.h"
#include <Math/Expression/ExpressionFunction3D.h>
#include <D3DGraphics/ExprIsoSurfaceParameters.h>
#include <D3DGraphics/MeshCreators.h>

/* ---------------- Implicit surface polygonizer supportfunctions ------------------- */

using namespace Expr;

LPD3DXMESH createMesh(AbstractMeshFactory &amf, const ExprIsoSurfaceParameters &param) {
  if(param.m_includeTime) {
    throwInvalidArgumentException(__TFUNCTION__, _T("param.includeTime=false"));
  }
  ExpressionFunction3D f(param.m_expr, RADIANS, param.m_machineCode);
  return ::createMesh(amf, param, f);
}

MeshArray createMeshArray(CWnd *wnd, AbstractMeshFactory &amf, const ExprIsoSurfaceParameters &param) {
  if(!param.m_includeTime) {
    throwInvalidArgumentException(__TFUNCTION__, _T("param.includeTime=false"));
  }
  ExpressionFunction3D f(param.m_expr, RADIANS, param.m_machineCode);
  return ::createMeshArray(wnd, amf, param, f);
}

