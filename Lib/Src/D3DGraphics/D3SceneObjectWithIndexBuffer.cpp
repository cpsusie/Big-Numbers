#include "pch.h"
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/D3ToString.h>

DECLARE_THISFILE;

// ------------------------------------------------ D3SceneObjectWithIndexBuffer ---------------------------------------------------

D3SceneObjectWithIndexBuffer::D3SceneObjectWithIndexBuffer(D3Scene &scene) : D3SceneObjectWithVertexBuffer(scene) {
  m_indexBuffer = NULL;
}

D3SceneObjectWithIndexBuffer::~D3SceneObjectWithIndexBuffer() {
  releaseIndexBuffer();
}

void *D3SceneObjectWithIndexBuffer::allocateIndexBuffer(bool int32, int count) {
  releaseIndexBuffer();
  void *bufferItems = NULL;
  m_indexBuffer = getScene().allocateIndexBuffer(int32, count);
  V(m_indexBuffer->Lock(0, 0, &bufferItems, 0));
  return bufferItems;
}

void D3SceneObjectWithIndexBuffer::unlockIndexBuffer() {
  V(m_indexBuffer->Unlock());
}

void D3SceneObjectWithIndexBuffer::releaseIndexBuffer() {
  SAFERELEASE(m_indexBuffer)
}

#define GETLOCKEDSHORTBUFFER(count) (USHORT*)allocateIndexBuffer(false, count)
#define GETLOCKEDLONGBUFFER( count) (ULONG* )allocateIndexBuffer(true , count)

String D3SceneObjectWithIndexBuffer::toString() const {
  return format(_T("%s\nIndexBuffer:\n%s")
               ,__super::toString().cstr()
               ,indentString(::toString(m_indexBuffer), 2).cstr());
}

