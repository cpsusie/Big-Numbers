#include "pch.h"
#include <D3DGraphics/D3ToString.h>

// -----------------------------------------------------------------------------------------------------------

String toString(LPD3DXMESH mesh) {
  if(mesh == NULL) return _T("null");
  LPDIRECT3DVERTEXBUFFER vertexBuffer;
  LPDIRECT3DINDEXBUFFER  indexBuffer;

  V(mesh->GetVertexBuffer(&vertexBuffer)); TRACE_REFCOUNT(vertexBuffer);
  V(mesh->GetIndexBuffer( &indexBuffer )); TRACE_REFCOUNT(indexBuffer );
  String result = _T("VertexBuffer:\n");
  result += indentString(toString(vertexBuffer), 2);
  result += _T("IndexBuffer:\n");
  result += indentString(toString(indexBuffer), 2);

  SAFERELEASE(indexBuffer);
  SAFERELEASE(vertexBuffer);
  return result;
}

String toString(LPDIRECT3DVERTEXBUFFER vertexBuffer) {
  if(vertexBuffer == NULL) return _T("null");
  D3DVERTEXBUFFER_DESC desc;
  V(vertexBuffer->GetDesc(&desc));
  const int itemSize = FVFToSize(desc.FVF);
  String result = format(_T("Description:\n%s"
                            "ItemSize   :%d\n"
                            "ItemCount  :%d\n")
                        ,indentString(toString(desc), 2).cstr()
                        ,itemSize
                        ,desc.Size/itemSize
                        );
  void *bufferItems = NULL;
  V(vertexBuffer->Lock(0, 0, &bufferItems, D3DLOCK_READONLY));

  int index = 0;
  for(size_t bp = 0; bp < desc.Size; bp += itemSize, index++) {
    const String str = vertexToString(((const char*)bufferItems) + bp, desc.FVF, 5);
    result += format(_T("%4d:%s\n"), index, str.cstr());
  }
  V(vertexBuffer->Unlock());
  return result;
}

String toString(LPDIRECT3DINDEXBUFFER indexBuffer) {
  if(indexBuffer == NULL) return _T("null");
  D3DINDEXBUFFER_DESC desc;
  V(indexBuffer->GetDesc(&desc));
  const int itemSize  = formatToSize(desc.Format);
  const int itemCount = desc.Size/itemSize;
  String result = format(_T("Description:\n%s"
                            "ItemSize   :%d\n"
                            "ItemCount  :%d\n")
                         ,indentString(toString(desc), 2).cstr()
                         ,itemSize
                         ,itemCount
                        );
  void *bufferItems = NULL;
  V(indexBuffer->Lock(0, 0, &bufferItems, D3DLOCK_READONLY));

  int index = 0;
  if(itemCount%3 == 0) {
    if(itemSize == sizeof(USHORT)) {
      for(const USHORT *a = (const USHORT*)bufferItems; index < itemCount; index += 3) {
        result += format(_T("%5d:(%5u, %5u, %5u)\n"), index/3, a[index], a[index+1], a[index+2]);
      }
    } else {
      for(const ULONG *a = (const ULONG*)bufferItems; index < itemCount; index += 3) {
        result += format(_T("%6u:(%7lu, %7lu, %7lu)\n"), index/3, a[index], a[index+1], a[index+2]);
      }
    }
  } else {
    if(itemSize == sizeof(USHORT)) {
      for(const USHORT *a = (const USHORT*)bufferItems; index < itemCount; index++) {
        const int column = index%20;
        if(column==0) result += format(_T("%5d:"), index);
        result += format(_T("%5u%s"), a[index], ((column == 19) || (index == itemCount-1)) ? _T("\n") : _T(" "));
      }
    } else {
      for(const ULONG *a = (const ULONG*)bufferItems; index < itemCount; index++) {
        const int column = index%20;
        if(column==0) result += format(_T("%6d:"), index);
        result += format(_T("%7lu%s"), a[index], ((column == 19) || (index == itemCount-1)) ? _T("\n") : _T(" "));
      }
    }
  }
  V(indexBuffer->Unlock());
  return result;
}
