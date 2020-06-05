#include "pch.h"
#include <D3DGraphics/D3ToString.h>

String toString(LPDIRECT3DVERTEXBUFFER vertexBuffer, BYTE flags) {
  const bool multiLines = (flags & (FORMAT_BUFFERDATA | FORMAT_BUFFERDESCNL)) != 0;
  if(vertexBuffer == NULL) {
    return multiLines ? _T("Null\n") : _T("Null");
  }
  String result;
  D3DVERTEXBUFFER_DESC desc;
  V(vertexBuffer->GetDesc(&desc));
  const UINT itemSize = FVFToSize(desc.FVF);
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
