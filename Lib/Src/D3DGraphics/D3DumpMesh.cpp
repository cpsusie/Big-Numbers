#include "pch.h"
#include <D3DGraphics/D3Scene.h>

DECLARE_THISFILE;

// -----------------------------------------------------------------------------------------------------------

void dumpMesh(LPD3DXMESH mesh, const String &fileName) {
  LPDIRECT3DVERTEXBUFFER vertexBuffer;
  LPDIRECT3DINDEXBUFFER  indexBuffer;

  V(mesh->GetVertexBuffer(&vertexBuffer));
  V(mesh->GetIndexBuffer( &indexBuffer ));

  const String name = (fileName.length() > 0) ? fileName : _T("c:\\temp\\meshDump.txt");

  FILE *f = MKFOPEN(name, "w");

  fprintf(f, "VertexBuffer:\n");
  dumpVertexBuffer(vertexBuffer, f);
  fprintf(f, "InbdexBuffer:\n");
  dumpIndexBuffer(indexBuffer, f);

  fclose(f);

  indexBuffer->Release();
  vertexBuffer->Release();
}

void dumpVertexBuffer(LPDIRECT3DVERTEXBUFFER vertexBuffer, FILE *f) {
  D3DVERTEXBUFFER_DESC desc;
  V(vertexBuffer->GetDesc(&desc));
  const int itemSize = FVFToSize(desc.FVF);
  _ftprintf(f, _T("Description:\n%s"
                  "ItemSize :%d\n"
                  "ItemCount:%d\n")
               ,toString(desc).cstr()
               ,itemSize
               ,desc.Size/itemSize
           );
  void *bufferItems = NULL;
  V(vertexBuffer->Lock(0, 0, &bufferItems, D3DLOCK_READONLY));

  int index = 0;
  for(size_t bp = 0; bp < desc.Size; bp += itemSize, index++) {
    const String str = vertexToString(((const char*)bufferItems) + bp, desc.FVF, 5);
    _ftprintf(f, _T("%4d:%s\n"), index, str.cstr());
  }
  V(vertexBuffer->Unlock());
}

void dumpIndexBuffer(LPDIRECT3DINDEXBUFFER indexBuffer, FILE *f) {
  D3DINDEXBUFFER_DESC desc;
  V(indexBuffer->GetDesc(&desc));
  const int itemSize  = formatToSize(desc.Format);
  const int itemCount = desc.Size/itemSize;
  _ftprintf(f, _T("Description:\n%s"
                  "ItemSize :%d\n"
                  "ItemCount:%d\n")
             ,toString(desc).cstr()
             ,itemSize
             ,itemCount
           );
  void *bufferItems = NULL;
  V(indexBuffer->Lock(0, 0, &bufferItems, D3DLOCK_READONLY));

  int index = 0;
  if(itemCount % 3 == 0) {
    if(itemSize == sizeof(unsigned short)) {
      for(const unsigned short *a = (const unsigned short*)bufferItems; index < itemCount; index += 3) {
        fprintf(f, "%5u, %5u, %5u\n", a[index], a[index+1], a[index+2]);
      }
    } else {
      for(const unsigned long *a = (const unsigned long*)bufferItems; index < itemCount; index += 3) {
        fprintf(f, "%7lu, %7lu, %7lu\n", a[index], a[index+1], a[index+2]);
      }
    }
  } else {
    if(itemSize == sizeof(unsigned short)) {
      for(const unsigned short *a = (const unsigned short*)bufferItems; index < itemCount; index++) {
        fprintf(f, "%5u%s", a[index], ((index % 20 == 19) || (index == itemCount-1)) ? "\n" : " ");
      }
    } else {
      for(const unsigned long *a = (const unsigned long*)bufferItems; index < itemCount; index++) {
        fprintf(f, "%7lu%s", a[index], ((index % 20 == 19) || (index == itemCount-1)) ? "\n" : " ");
      }
    }
  }

  V(indexBuffer->Unlock());
}

