#pragma once

#include "D3SceneObjectWithVertexBuffer.h"

class D3SceneObjectWithIndexBuffer : public D3SceneObjectWithVertexBuffer {
private:
  D3SceneObjectWithIndexBuffer &releaseIndexBuffer();
protected:
  LPDIRECT3DINDEXBUFFER m_indexBuffer;
  void *allocateIndexBuffer(bool int32, int count);
  D3SceneObjectWithIndexBuffer &unlockIndexBuffer();

  D3Device &setIndices(D3Device &device);

public:
  D3SceneObjectWithIndexBuffer(D3Scene &scene);
  ~D3SceneObjectWithIndexBuffer();
  inline bool hasIndexBuffer() const {
    return m_indexBuffer != NULL;
  }
  String toString() const;
};

