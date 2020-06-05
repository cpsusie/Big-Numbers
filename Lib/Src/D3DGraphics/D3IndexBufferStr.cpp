#include "pch.h"
#include <D3DGraphics/D3ToString.h>

String toString(LPDIRECT3DINDEXBUFFER indexBuffer, BYTE flags) {
  const bool multiLines = (flags & (FORMAT_BUFFERDATA | FORMAT_BUFFERDESCNL)) != 0;
  if(indexBuffer == NULL) {
    return multiLines ? _T("Null\n") : _T("Null");
  }
  String result;
  D3DINDEXBUFFER_DESC desc;
  V(indexBuffer->GetDesc(&desc));
  const UINT itemSize = formatToSize(desc.Format);
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
