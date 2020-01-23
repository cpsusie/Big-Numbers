#include "pch.h"
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/D3ToString.h>

// ------------------------------------------------ D3SceneObjectWithVertexBuffer ---------------------------------------------------

D3SceneObjectWithVertexBuffer::D3SceneObjectWithVertexBuffer(D3Scene &scene) : D3SceneObject(scene) {
  m_vertexBuffer   = NULL;
  m_primitiveCount = 0;
}

D3SceneObjectWithVertexBuffer::~D3SceneObjectWithVertexBuffer() {
  releaseVertexBuffer();
}

void D3SceneObjectWithVertexBuffer::lockVertexArray(void **a, UINT nbytes) {
  V(m_vertexBuffer->Lock(0, nbytes, a, 0));
}

void D3SceneObjectWithVertexBuffer::unlockVertexArray() {
  if(hasVertexBuffer()) {
    V(m_vertexBuffer->Unlock());
  }
}

void D3SceneObjectWithVertexBuffer::releaseVertexBuffer() {
  SAFERELEASE(m_vertexBuffer);
}

D3DXCube3 D3SceneObjectWithVertexBuffer::getBoundingBox() const {
  return ::getBoundingBox(m_vertexBuffer) + getPos();
}

String D3SceneObjectWithVertexBuffer::toString() const {
  return format(_T("%s\nVertexBuffer:\n%s")
               ,__super::toString().cstr()
               ,indentString(::toString(m_vertexBuffer), 2).cstr()
  );
}
