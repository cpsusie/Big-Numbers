#include "pch.h"
#include <D3DGraphics/D3ToString.h>

// -----------------------------------------------------------------------------------------------------------

String toString(LPD3DXMESH mesh, BYTE flags) {
  const bool multiLines = (flags & (FORMAT_BUFFERDATA | FORMAT_BUFFERDESCNL)) != 0;
  if(mesh == NULL) {
    return multiLines ? _T("Null\n") : _T("Null");
  }
  String result;
  if(flags & FORMAT_VERTEXBUFFER) {
    LPDIRECT3DVERTEXBUFFER vertexBuffer;
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
    LPDIRECT3DINDEXBUFFER  indexBuffer;
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

String toString(LPDIRECT3DVERTEXBUFFER vertexBuffer, BYTE flags) {
  const bool multiLines = (flags & (FORMAT_BUFFERDATA | FORMAT_BUFFERDESCNL)) != 0;
  if(vertexBuffer == NULL) {
    return multiLines ? _T("Null\n") : _T("Null");
  }
  String result;
  D3DVERTEXBUFFER_DESC desc;
  V(vertexBuffer->GetDesc(&desc));
  const int itemSize = FVFToSize(desc.FVF);
  if(flags & FORMAT_BUFFERDESC) {
    if(multiLines) {
      result = format(_T("Description:\n%s"
                         "ItemSize   :%d bytes\n"
                         "ItemCount  :%s\n")
                     ,indentString(toString(desc,true), 2).cstr()
                     ,itemSize
                     ,format1000(desc.Size/itemSize).cstr()
                     );
    } else {
      result = format(_T("%s,ItemSize:%d bytes,ItemCount:%s")
                     ,toString(desc,false).cstr()
                     ,itemSize
                     ,format1000(desc.Size/itemSize).cstr()
                     );
    }
  }
  if(flags & FORMAT_BUFFERDATA) {
    void *bufferItems = NULL;
    V(vertexBuffer->Lock(0, 0, &bufferItems, D3DLOCK_READONLY));

    int index = 0;
    for(size_t bp = 0; bp < desc.Size; bp += itemSize, index++) {
      const String str = vertexToString(((const char*)bufferItems) + bp, desc.FVF, 5);
      result += format(_T("%4d:%s\n"), index, str.cstr());
    }
    V(vertexBuffer->Unlock());
  }
  return result;
}

String toString(LPDIRECT3DINDEXBUFFER indexBuffer, BYTE flags) {
  const bool multiLines = (flags & (FORMAT_BUFFERDATA | FORMAT_BUFFERDESCNL)) != 0;
  if(indexBuffer == NULL) {
    return multiLines ? _T("Null\n") : _T("Null");
  }
  String result;
  D3DINDEXBUFFER_DESC desc;
  V(indexBuffer->GetDesc(&desc));
  const int itemSize  = formatToSize(desc.Format);
  const int itemCount = desc.Size/itemSize;
  if(flags & FORMAT_BUFFERDESC) {
    if(multiLines) {
      result = format(_T("Description:\n%s"
                         "ItemSize   :%d\n"
                         "ItemCount  :%s\n")
                     ,indentString(toString(desc,true), 2).cstr()
                     ,itemSize
                     ,format1000(itemCount).cstr()
                     );
    } else {
      result = format(_T("%s,ItemSize:%d,ItemCount:%s")
                     ,toString(desc,false).cstr()
                     ,itemSize
                     ,format1000(itemCount).cstr()
                     );
    }
  }
  if(flags & FORMAT_BUFFERDATA) {
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
  }
  return result;
}
