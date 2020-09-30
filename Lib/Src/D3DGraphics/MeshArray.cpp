#include "pch.h"
#include <D3DGraphics/MeshArray.h>

MeshArray &MeshArray::operator=(const MeshArray &src) {
  clear((int)src.getCapacity());
  addAll(src);
  return *this;
}

void MeshArray::insert(UINT index, const LPD3DXMESH &m, UINT count) {
  __super::insert(index, m, count);
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

CompactArray<DWORD> MeshArray::getFVFArray() const {
  CompactArray<DWORD> result(size());
  for(UINT i = 0; i < size(); i++) {
    LPD3DXMESH mesh = (*this)[i];
    result.add(mesh->GetFVF());
  }
  return result;
}

DWORD MeshArray::getFVFUnion() const {
  CompactArray<DWORD> a = getFVFArray();
  DWORD fvf = 0;
  for(DWORD f : a) {
    fvf |= f;
  }
  return fvf;
}

bool MeshArray::hasNormals() const {
  return hasVertexNormals(getFVFUnion());
}

bool MeshArray::hasTextureCoordinates() const {
  return hasTextureVertices(getFVFUnion());
}

void MeshArray::clear(int capacity) {
  for(size_t i = 0; i < size(); i++) {
    LPD3DXMESH mesh = (*this)[i];
    SAFERELEASE(mesh);
  }
  __super::clear(capacity);
}
