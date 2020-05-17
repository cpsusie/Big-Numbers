#pragma once

#include "D3SceneObjectVisual.h"
#include "D3Cube.h"
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
  D3SceneObjectWithVertexBuffer(D3Scene             &scene , const String &name = _T("ObjectWithVertexBuffer"));
  D3SceneObjectWithVertexBuffer(D3SceneObjectVisual *parent, const String &name = _T("ObjectWithVertexBuffer"));

public:
  ~D3SceneObjectWithVertexBuffer();
  LPDIRECT3DVERTEXBUFFER getVertexBuffer() const {
    return m_vertexBuffer;
  }
  D3DVERTEXBUFFER_DESC getDesc() const;
  D3Cube getBoundingBox() const;
  String toString() const;
  String getInfoString() const;
};
