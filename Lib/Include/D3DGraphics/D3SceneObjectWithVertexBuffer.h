#pragma once

#include "D3SceneObjectVisual.h"
#include "D3DXCube.h"
#include "D3Scene.h"

class D3Device;

class D3SceneObjectWithVertexBuffer : public D3SceneObjectVisual {
private:
  D3SceneObjectWithVertexBuffer &releaseVertexBuffer();
protected:
  int                    m_primitiveCount;
  DWORD                  m_fvf;
  int                    m_vertexSize;
  LPDIRECT3DVERTEXBUFFER m_vertexBuffer;
  template<typename VertexType> VertexType *allocateVertexArray(UINT count) {
    releaseVertexBuffer();
    UINT bufferSize;
    m_vertexBuffer = getDevice().allocateVertexBuffer<VertexType>(count, &bufferSize);
    m_vertexSize = sizeof(VertexType);
    m_fvf = VertexType::FVF_Flags;
    assert(bufferSize == m_vertexSize * count);
    VertexType *bufferItems = NULL;
    lockVertexArray((void**)&bufferItems, bufferSize);
    return bufferItems;
  }

  D3SceneObjectWithVertexBuffer &lockVertexArray(void **a, UINT nbytes);
  D3SceneObjectWithVertexBuffer &unlockVertexArray();
  D3Device &setStreamSource();

public:
  D3SceneObjectWithVertexBuffer(D3Scene &scene);
  ~D3SceneObjectWithVertexBuffer();

  inline LPDIRECT3DVERTEXBUFFER &getVertexBuffer() {
    return m_vertexBuffer;
  }
  inline bool hasVertexBuffer() const {
    return m_vertexBuffer != NULL;
  }
  D3DXCube3 getBoundingBox() const;
  String toString() const;
};
