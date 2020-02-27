#include "pch.h"
#include <D3DGraphics/D3Device.h>
#include <D3DGraphics/D3ToString.h>
#include <D3DGraphics/D3SceneObjectWithVertexBuffer.h>

D3SceneObjectWithVertexBuffer::D3SceneObjectWithVertexBuffer(D3Scene &scene, const String &name)
: D3SceneObjectVisual(scene, name)
{
  m_vertexBuffer   = NULL;
  m_primitiveCount = 0;
}

D3SceneObjectWithVertexBuffer::D3SceneObjectWithVertexBuffer(D3SceneObjectVisual *parent, const String &name)
: D3SceneObjectVisual(parent, name)
{
  m_vertexBuffer   = NULL;
  m_primitiveCount = 0;
}

D3SceneObjectWithVertexBuffer::~D3SceneObjectWithVertexBuffer() {
  releaseVertexBuffer();
}

D3SceneObjectWithVertexBuffer &D3SceneObjectWithVertexBuffer::lockVertexArray(void **a, UINT nbytes) {
  V(m_vertexBuffer->Lock(0, nbytes, a, 0));
  return *this;
}

D3SceneObjectWithVertexBuffer &D3SceneObjectWithVertexBuffer::unlockVertexArray() {
  if(hasVertexBuffer()) {
    V(m_vertexBuffer->Unlock());
  }
  return *this;
}

D3SceneObjectWithVertexBuffer &D3SceneObjectWithVertexBuffer::releaseVertexBuffer() {
  SAFERELEASE(m_vertexBuffer);
  return *this;
}

D3Device &D3SceneObjectWithVertexBuffer::setStreamSource() {
  D3Device &device = getDevice();
  device.setStreamSource(m_vertexBuffer, m_vertexSize, m_fvf);
  return device;
}

D3Cube D3SceneObjectWithVertexBuffer::getBoundingBox() const {
  return ::getBoundingBox(m_vertexBuffer) + D3World(*this).getPos();
}

String D3SceneObjectWithVertexBuffer::toString() const {
  return format(_T("%s\nVertexBuffer:\n%s")
               ,__super::toString().cstr()
               ,indentString(::toString(m_vertexBuffer), 2).cstr()
  );
}
