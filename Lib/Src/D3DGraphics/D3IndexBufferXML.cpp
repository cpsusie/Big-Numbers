#include "pch.h"
#include <D3DGraphics/D3ToString.h>
#include <D3DGraphics/D3XML.h>

void setValue(XMLDoc &doc, XMLNodePtr n, const LPDIRECT3DINDEXBUFFER &ib) {
  D3DINDEXBUFFER_DESC desc;
  V(ib->GetDesc(&desc));
  setValue(doc, n, _T("description"), desc);

  const UINT itemSize  = formatToSize(desc.Format);
  const int  itemCount = desc.Size/itemSize;

  void *bufferItems = NULL;
  V(ib->Lock(0, 0, &bufferItems, D3DLOCK_READONLY));
  try {
    StringArray dataLines;
    int index = 0;
    if(itemCount%3 == 0) {
      if(itemSize == sizeof(USHORT)) {
        for(const USHORT *a = (const USHORT*)bufferItems; index < itemCount; index += 3) {
          dataLines.add(format(_T("%5u,%5u,%5u"), a[index], a[index+1], a[index+2]));
        }
      } else {
        for(const ULONG *a = (const ULONG*)bufferItems; index < itemCount; index += 3) {
          dataLines.add(format(_T("%7lu,%7lu,%7lu"), a[index], a[index+1], a[index+2]));
        }
      }
    } else {
      String line;
      if(itemSize == sizeof(USHORT)) {
        for(const USHORT *a = (const USHORT*)bufferItems; index < itemCount;) {
          line += format(_T("%5u"), a[index++]);
          if((index%20 == 0) || (index == itemCount)) {
            dataLines.add(line);
            line = EMPTYSTRING;
          } else {
            line += ',';
          }
        }
      } else {
        for(const ULONG *a = (const ULONG*)bufferItems; index < itemCount;) {
          line += format(_T("%7u"), a[index++]);
          if((index%20 == 0) || (index == itemCount)) {
            dataLines.add(line);
            line = EMPTYSTRING;
          } else {
            line += ',';
          }
        }
      }
    }
    V(ib->Unlock());
    XMLNodePtr dataNode = doc.createNode(n, _T("bufferdata"));
    setValue<StringArray, String>(doc, dataNode, dataLines);
  } catch(...) {
    ib->Unlock();
    throw;
  }
}

void getValue(XMLDoc &doc, XMLNodePtr n, LPDIRECT3DINDEXBUFFER       &ib) {
  throwException(_T("%s:not implemented"), __TFUNCTION__);
}
