#pragma once

#include "D3SceneObjectWithVertexBuffer.h"

class D3SceneObjectWithIndexBuffer : public D3SceneObjectWithVertexBuffer {
private:
  D3SceneObjectWithIndexBuffer &releaseIndexBuffer();
  LPDIRECT3DINDEXBUFFER m_indexBuffer;
  static bool use32BitIndices(size_t vertexCount);
protected:
  void                         *allocateIndexArray(bool int32, UINT count);
  D3SceneObjectWithIndexBuffer &lockIndexArray(    void **a);
  D3SceneObjectWithIndexBuffer &unlockIndexArray();

  template<typename VertexType> void initBuffers(const CompactArray<VertexType> &varray, const CompactUIntArray &iarray) {
    const size_t vertexCount = varray.size();
    const size_t indexCount  = iarray.size();
    const bool   useInt32    = use32BitIndices(vertexCount);

    VertexType *vertices = allocateVertexArray<VertexType>((UINT)vertexCount);
    memcpy(vertices, varray.getBuffer(), vertexCount * sizeof(VertexType));
    unlockVertexArray();
    void *indexArray = allocateIndexArray(useInt32, (UINT)indexCount);
    if(useInt32) {
      memcpy(indexArray, iarray.getBuffer(), indexCount * sizeof(UINT));
    } else {
      const UINT *srcp = iarray.getBuffer(), *endp = srcp + iarray.size();
      for(USHORT *dstp = (USHORT*)indexArray; srcp < endp; srcp++) {
        *(dstp++) = (USHORT)(*srcp);
      }
    }
    unlockIndexArray();
  }

  D3SceneObjectWithIndexBuffer(D3Scene &scene, const String &name = _T("ObjectWithIndexBuffer"))
    : D3SceneObjectWithVertexBuffer(scene, name)
    , m_indexBuffer(nullptr)
  {
  }
  D3SceneObjectWithIndexBuffer(D3SceneObjectVisual *parent, const String &name = _T("ObjectWithIndexBuffer"))
    : D3SceneObjectWithVertexBuffer(parent, name)
    , m_indexBuffer(nullptr)
  {
  }
  template<typename VertexType> D3SceneObjectWithIndexBuffer(D3Scene &scene, const CompactArray<VertexType> &varray, const CompactUIntArray &iarray, const String &name=_T("ObjectWithIndexBuffer"))
    : D3SceneObjectWithVertexBuffer(scene, name)
    , m_indexBuffer(nullptr)
  {
    initBuffers(varray, iarray);
  }
  template<typename VertexType> D3SceneObjectWithIndexBuffer(D3SceneObjectVisual *parent, const CompactArray<VertexType> &varray, const CompactUIntArray &iarray, const String &name=_T("ObjectWithIndexBuffer"))
    : D3SceneObjectWithVertexBuffer(parent, name)
    , m_indexBuffer(nullptr)
  {
    initBuffers(varray, iarray);
  }

  ~D3SceneObjectWithIndexBuffer() override {
    releaseIndexBuffer();
  }

  D3Device &setDeviceIndices();
public:
  LPDIRECT3DINDEXBUFFER getIndexBuffer() const override {
    return m_indexBuffer;
  }
  String toString() const;
};
