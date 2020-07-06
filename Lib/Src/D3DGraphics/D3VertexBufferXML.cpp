#include "pch.h"
#include <D3DGraphics/D3ToString.h>
#include <D3DGraphics/D3XML.h>

void setValue(XMLDoc &doc, XMLNodePtr n, const LPDIRECT3DVERTEXBUFFER &vb) {
  const D3DVERTEXBUFFER_DESC desc = getDesc(vb);
  setValue(doc, n, _T("description"), desc);
  void *bufferItems = NULL;
  V(vb->Lock(0, 0, &bufferItems, D3DLOCK_READONLY));
  try {
    int        index    = 0;
    const UINT itemSize = FVFToSize(desc.FVF);
    StringArray dataLines;
    for(size_t bp = 0; bp < desc.Size; bp += itemSize, index++) {
      dataLines.add(vertexToString(((const char*)bufferItems) + bp, desc.FVF, 5));
    }
    V(vb->Unlock());
    XMLNodePtr dataNode = doc.createNode(n, _T("bufferdata"));
    setValue<StringArray, String>(doc, dataNode, dataLines);
  } catch(...) {
    vb->Unlock();
    throw;
  }
}

void getValue(XMLDoc &doc, XMLNodePtr n, LPDIRECT3DVERTEXBUFFER      &vb) {
  throwException(_T("%s:not implemented"), __TFUNCTION__);
}
