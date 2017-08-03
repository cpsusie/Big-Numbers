#include "pch.h"
#include <D3DGraphics/MeshArray.h>

MeshArray &MeshArray::operator=(const MeshArray &src) {
  clear((int)src.getCapacity());
  addAll(src);
  return *this;
}

void MeshArray::add(UINT index, const LPD3DXMESH &m, UINT count) {
  __super::add(index, m, count);
  while(count--) {
    LPD3DXMESH mesh = (*this)[index++];
    SAFEADDREF(mesh);
  }
}

bool MeshArray::addAll(const MeshArray &src) {
  size_t index = size();
  if(!__super::addAll(src)) {
    return false;
  } else {
    while(index < size()) {
      LPD3DXMESH mesh = (*this)[index++];
      SAFEADDREF(mesh);
    }
    return true;
  }
}

void MeshArray::remove(UINT index, UINT count) {
  if(count == 0) {
    return;
  }
  for(int i = index, j = count; j--;) {
    LPD3DXMESH mesh = (*this)[i++];
    SAFERELEASE(mesh);
  }
  __super::remove(index, count);
}

void MeshArray::clear(int capacity) {
  for(size_t i = 0; i < size(); i++) {
    LPD3DXMESH mesh = (*this)[i];
    SAFERELEASE(mesh);
  }
  __super::clear(capacity);
}
