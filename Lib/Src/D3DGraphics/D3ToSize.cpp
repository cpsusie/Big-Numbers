#include "pch.h"

UINT FVFToSize(DWORD fvf) {
  return D3DXGetFVFVertexSize(fvf);
}

UINT formatToSize(D3DFORMAT f) {
  switch(f) {
  case D3DFMT_INDEX16: return sizeof(short);
  case D3DFMT_INDEX32: return sizeof(long );
  default            : throwInvalidArgumentException(__TFUNCTION__, _T("f=%d"), f);
                       return 1;
  }
}
