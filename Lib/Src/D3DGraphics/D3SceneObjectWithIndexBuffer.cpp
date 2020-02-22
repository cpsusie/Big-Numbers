#include "pch.h"
#include <D3DGraphics/D3Device.h>
#include <D3DGraphics/D3ToString.h>
#include <D3DGraphics/D3SceneObjectWithIndexBuffer.h>

D3SceneObjectWithIndexBuffer::D3SceneObjectWithIndexBuffer(D3Scene &scene, const String &name)
  : D3SceneObjectWithVertexBuffer(scene, name)
{
  m_indexBuffer = NULL;
}

D3SceneObjectWithIndexBuffer::D3SceneObjectWithIndexBuffer(D3SceneObjectVisual *parent, const String &name)
: D3SceneObjectWithVertexBuffer(parent, name)
{
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

D3SceneObjectWithIndexBuffer &D3SceneObjectWithIndexBuffer::unlockIndexBuffer() {
  V(m_indexBuffer->Unlock());
  return *this;
}

D3SceneObjectWithIndexBuffer &D3SceneObjectWithIndexBuffer::releaseIndexBuffer() {
  SAFERELEASE(m_indexBuffer)
  return *this;
}


D3Device &D3SceneObjectWithIndexBuffer::setIndices(D3Device &device) {
  return device.setIndices(m_indexBuffer);
}

#define GETLOCKEDSHORTBUFFER(count) (USHORT*)allocateIndexBuffer(false, count)
#define GETLOCKEDLONGBUFFER( count) (ULONG* )allocateIndexBuffer(true , count)

String D3SceneObjectWithIndexBuffer::toString() const {
  return format(_T("%s\nIndexBuffer:\n%s")
               ,__super::toString().cstr()
               ,indentString(::toString(m_indexBuffer), 2).cstr());
}
