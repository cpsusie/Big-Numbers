#include "pch.h"
#include <D3DGraphics/D3Device.h>
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/D3ToString.h>
#include <D3DGraphics/D3SceneObjectVisual.h>

D3SceneObjectVisual::D3SceneObjectVisual(D3Scene &scene, const String &name)
  : D3SceneObject(scene)
  , m_parent(NULL)
  , m_name(name)
  , m_userData(NULL)
{
  m_world = D3World();
}
D3SceneObjectVisual::D3SceneObjectVisual(D3SceneObjectVisual *parent, const String &name)
  : D3SceneObject(parent->getScene())
  , m_parent(parent)
  , m_name(name.length() ? name : parent->getName())
  , m_userData(NULL)
{
  m_world = D3World();
}

D3SceneObjectVisual::~D3SceneObjectVisual() {
  for(Iterator<D3SceneObjectVisual*> it = m_children.getIterator(); it.hasNext();) {
    D3SceneObjectVisual *child = it.next();
    SAFEDELETE(child);
  }
  m_children.clear();
}

UINT D3SceneObjectVisual::addChild(D3SceneObjectVisual *child) {
  if(child->m_parent != this) {
    throwInvalidArgumentException(__TFUNCTION__,_T("child(name=%s).parent(%s) != this (%s)")
                                 ,child->getName().cstr()
                                 ,child->m_parent->getName().cstr()
                                 ,getName().cstr()
                                 );
  }
  const UINT index = (UINT)m_children.size();
  m_children.add(child);
  return index;
}

const D3Material &D3SceneObjectVisual::getMaterial() const {
  return getScene().getMaterial(getMaterialId());
}

D3Device &D3SceneObjectVisual::setDeviceMaterialIfExist() const {
  D3Device &device     = getDevice();
  const int materialId = getMaterialId();
  if(materialId >= 0) {
    device.setMaterial(getScene().getMaterial(materialId));
  }
  return device;
}

void D3SceneObjectVisual::draw() {
  for(Iterator<D3SceneObjectVisual*> it = m_children.getIterator(); it.hasNext();) {
    D3SceneObjectVisual *child = it.next();
    if(child->isVisible()) {
      child->draw();
    }
  }
}

String D3SceneObjectVisual::getInfoString() const {
  String result = getName();
  if(hasFillMode()) {
    result += _T(" ");
    result += ::toString(getFillMode());
  }
  if(hasShadeMode()) {
    result += _T(" ");
    result += ::toString(getShadeMode());
  }
  if(result.length()) {
    result += _T("\n");
    result += D3World(getWorld()).toString();
  }
  return result.trimRight();
}
