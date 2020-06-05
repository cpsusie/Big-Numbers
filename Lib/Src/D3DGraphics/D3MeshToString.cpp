#include "pch.h"
#include <D3DGraphics/D3ToString.h>

String toString(LPD3DXMESH mesh, BYTE flags) {
  const bool multiLines = (flags & (FORMAT_BUFFERDATA | FORMAT_BUFFERDESCNL)) != 0;
  if(mesh == NULL) {
    return multiLines ? _T("Null\n") : _T("Null");
  }
  String result;
  if(flags & FORMAT_VERTEXBUFFER) {
    LPDIRECT3DVERTEXBUFFER vertexBuffer = NULL;
    try {
      V(mesh->GetVertexBuffer(&vertexBuffer)); TRACE_REFCOUNT(vertexBuffer);
      if(multiLines) result += _T("VertexBuffer:\n");
      const String str = toString(vertexBuffer, flags);
      result += multiLines ? indentString(str, 2) : str;
      SAFERELEASE(vertexBuffer);
    } catch(...) {
      SAFERELEASE(vertexBuffer);
      throw;
    }
  }
  if(flags & FORMAT_INDEXBUFFER) {
    LPDIRECT3DINDEXBUFFER indexBuffer = NULL;
    try {
      V(mesh->GetIndexBuffer(&indexBuffer)); TRACE_REFCOUNT(indexBuffer);
      if((result.length() > 0) && !multiLines) result += " ";
      if(multiLines) result += _T("IndexBuffer:\n");
      const String str = toString(indexBuffer, flags);
      result += multiLines ? indentString(str, 2) : str;
      SAFERELEASE(indexBuffer);
    } catch(...) {
      SAFERELEASE(indexBuffer);
      throw;
    }
  }
  return result;
}
