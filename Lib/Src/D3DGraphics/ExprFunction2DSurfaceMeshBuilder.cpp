#include "pch.h"
#include <Math/Expression/ExpressionFunction2d.h>
#include <D3DGraphics/ExprFunction2DSurfaceParameters.h>
#include <D3DGraphics/MeshCreators.h>

using namespace Expr;

LPD3DXMESH createMesh(AbstractMeshFactory &amf, const ExprFunction2DSurfaceParameters &param) {
  if(param.m_includeTime) {
    throwInvalidArgumentException(__TFUNCTION__, _T("param.includeTime=true"));
  }
  ExpressionFunction2D f(param.m_expr, RADIANS, param.m_machineCode);
  return createMesh(amf, param, f);
}

MeshArray createMeshArray(CWnd *wnd, AbstractMeshFactory &amf, const ExprFunction2DSurfaceParameters &param) {
  if(!param.m_includeTime) {
    throwInvalidArgumentException(__TFUNCTION__, _T("param.includeTime=false"));
  }
  ExpressionFunction2D f(param.m_expr, RADIANS, param.m_machineCode);
  return createMeshArray(wnd, amf, param, f);
}
