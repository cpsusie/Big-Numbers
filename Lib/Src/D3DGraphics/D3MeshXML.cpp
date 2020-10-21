#include "pch.h"
#include <D3DGraphics/D3ToString.h>
#include <D3DGraphics/D3XML.h>

void setValue(XMLDoc &doc, XMLNodePtr n, const LPD3DXMESH &m) {
  LPDIRECT3DVERTEXBUFFER vb = nullptr;
  try {
    V(m->GetVertexBuffer(&vb)); TRACE_REFCOUNT(vb);
    setValue(doc, n, _T("vertexbuffer"), vb);
    SAFERELEASE(vb);
  } catch(...) {
    SAFERELEASE(vb);
    throw;
  }
  LPDIRECT3DINDEXBUFFER ib = nullptr;
  try {
    V(m->GetIndexBuffer(&ib)); TRACE_REFCOUNT(ib);
    setValue(doc, n, _T("indexbuffer"), ib);
    SAFERELEASE(ib);
  } catch(...) {
    SAFERELEASE(ib);
    throw;
  }
}

void getValue(XMLDoc &doc, XMLNodePtr n, LPD3DXMESH &m) {
  throwException(_T("%s:not implemented"), __TFUNCTION__);
}
