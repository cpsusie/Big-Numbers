#pragma once

class MeshArray : public CompactArray<LPD3DXMESH> {
public:
  MeshArray() {
  }
  explicit MeshArray(UINT capacity) : CompactArray<LPD3DXMESH>(capacity) {
  }
  MeshArray::MeshArray(const MeshArray &src) : CompactArray<LPD3DXMESH>(src.getCapacity()) {
    addAll(src);
  }
  MeshArray &operator=(const MeshArray &src);
  ~MeshArray() override {
    clear();
  }
  void add(const LPD3DXMESH &m) {
    add((UINT)size(), m);
  }
  void add(UINT index, const LPD3DXMESH &m, UINT count = 1);
  bool addAll(const MeshArray &src);
  void remove(UINT index, UINT count = 1);
  void removeLast() {
    remove((UINT)size()-1);
  }
  CompactArray<DWORD> getFVFArray() const;
  DWORD getFVFUnion() const;
  bool  hasNormals() const;
  bool  hasTextureCoordinates() const;
  void  clear(int capacity=0);
};
