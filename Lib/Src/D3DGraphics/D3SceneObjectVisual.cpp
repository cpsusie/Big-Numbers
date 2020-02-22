#include "pch.h"
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/D3SceneObjectVisual.h>

D3SceneObjectVisual::D3SceneObjectVisual(D3Scene &scene, const String &name)
  : D3SceneObject(scene)
  , m_parent(NULL)
  , m_name(name)
  , m_visible(true)
  , m_userData(NULL)
{
  m_world = D3World();
}
D3SceneObjectVisual::D3SceneObjectVisual(D3SceneObjectVisual *parent, const String &name)
  : D3SceneObject(parent->getScene())
  , m_parent(parent)
  , m_name(name.length() ? name : parent->getName())
  , m_visible(true)
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

D3SceneObjectVisual &D3SceneObjectVisual::addChild(D3SceneObjectVisual *child) {
  if(child->m_parent != this) {
    throwInvalidArgumentException(__TFUNCTION__,_T("child(name=%s).parent(%s) != this (%s)")
                                 ,child->getName().cstr()
                                 ,child->m_parent->getName().cstr()
                                 ,getName().cstr()
                                 );
  }
  m_children.add(child);
  return *this;
}

const D3Material &D3SceneObjectVisual::getMaterial() const {
  return getScene().getMaterial(getMaterialId());
}

void D3SceneObjectVisual::draw() {
  for(Iterator<D3SceneObjectVisual*> it = m_children.getIterator(); it.hasNext();) {
    it.next()->draw();
  }
}
