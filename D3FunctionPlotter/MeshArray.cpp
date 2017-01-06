#include "stdafx.h"

MeshArray &MeshArray::operator=(const MeshArray &src) {
  clear((int)src.getCapacity());
  addAll(src);
  return *this;
}

void MeshArray::add(UINT index, const LPD3DXMESH &m, UINT count) {
  CompactArray<LPD3DXMESH>::add(index, m, count);
  while(count--) {
    int ref = (*this)[index++]->AddRef();
  }
}

bool MeshArray::addAll(const MeshArray &src) {
  size_t index = size();
  if(!CompactArray<LPD3DXMESH>::addAll(src)) {
    return false;
  } else {
    while(index < size()) {
      int ref = (*this)[index++]->AddRef();
    }
    return true;
  }
}

void MeshArray::remove(UINT index, UINT count) {
  if(count == 0) {
    return;
  }
  for(int i = index, j = count; j--;) {
    int ref = (*this)[i++]->Release();
  }
  CompactArray<LPD3DXMESH>::remove(index, count);
}

void MeshArray::clear(int capacity) {
  for(size_t i = 0; i < size(); i++) {
    int ref = (*this)[i]->Release();
  }
  CompactArray<LPD3DXMESH>::clear(capacity);
}
