#include "pch.h"
#include <D3DGraphics/D3Device.h>
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/D3SceneObjectVisual.h>
#include <D3DGraphics/D3SceneObjectVisualNormals.h>

D3SceneObjectVisual::D3SceneObjectVisual(D3Scene &scene, const String &name)
  : D3SceneObject(scene)
  , m_parent(NULL)
  , m_name(name)
  , m_userData(NULL) {
  m_world = D3World();
}
D3SceneObjectVisual::D3SceneObjectVisual(D3SceneObjectVisual *parent, const String &name)
  : D3SceneObject(parent->getScene())
  , m_parent(parent)
  , m_name(name.length() ? name : parent->getName())
  , m_userData(NULL) {
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
    throwInvalidArgumentException(__TFUNCTION__, _T("child(name=%s).parent(%s) != this(%s)")
                                 , child->getName().cstr()
                                 , child->m_parent->getName().cstr()
                                 , getName().cstr()
                                 );
  }
  if(isAncestor(child)) { // prevent circular references, which would cause infinite recursion several places
    throwInvalidArgumentException(__TFUNCTION__, _T("child(name=%s) is ancestor to this(%s)")
                                 , child->getName().cstr()
                                 , getName().cstr()
                                 );
  }
  const UINT index = (UINT)m_children.size();
  m_children.add(child);
  return index;
}

void D3SceneObjectVisual::removeChild(UINT index) {
  D3SceneObjectVisual *child = m_children[index];
  m_children.remove(index);
  SAFEDELETE(child);
}

int D3SceneObjectVisual::findChildByType(SceneObjectType type) const {
  const size_t n = m_children.size();
  for(size_t i = 0; i < n; i++) {
    if(m_children[i]->getType() == type) {
      return (int)i;
    }
  }
  return -1;
}

bool D3SceneObjectVisual::isDescendant(D3SceneObjectVisual * const visual, bool recursive) const {
  if(recursive) {
    for(const D3SceneObjectVisual *child : m_children) {
      if((child == visual) || child->isDescendant(visual, true)) {
        return true;
      }
    }
    return false;
  } else {
    return m_children.getFirstIndex(visual) >= 0;
  }
}

bool D3SceneObjectVisual::isAncestor(D3SceneObjectVisual * const visual) const {
  for(D3SceneObjectVisual *p = getParent(); p; p = p->getParent()) {
    if(p == visual) {
      return true;
    }
  }
  return false;
}

LPDIRECT3DTEXTURE D3SceneObjectVisual::getTexture() const {
  return getScene().getTexture(getTextureId()).getImage();
}

D3Device &D3SceneObjectVisual::setDeviceTextureIfExist() const {
  D3Device &device     = getDevice();
  const int textureId  = getTextureId();
  if((textureId >= 0) && hasTextureCoordinates()) {
    getDevice().setSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR)
               .setSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR)
               .setTexture(     0, getTexture())
               .setAlphaBlendEnable(false)
               .setZEnable(         D3DZB_TRUE);
  }
  return device;
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

DWORD D3SceneObjectVisual::getFVF() const {
  if(hasMesh()) {
    return getMesh()->GetFVF();
  } else {
    return getVertexBufferDesc().FVF;
  }
}

D3DVERTEXBUFFER_DESC D3SceneObjectVisual::getVertexBufferDesc() const {
  if(hasMesh()) {
    return getvDesc(getMesh());
  } else if(hasVertexBuffer()) {
    return getDesc(getVertexBuffer());
  } else {
    D3DVERTEXBUFFER_DESC desc;
    memset(&desc, 0, sizeof(desc));
    return desc;
  }
}

D3DINDEXBUFFER_DESC D3SceneObjectVisual::getIndexBufferDesc() const {
  if(hasMesh()) {
    return getiDesc(getMesh());
  } else if(hasIndexBuffer()) {
    return getDesc(getIndexBuffer());
  } else {
    D3DINDEXBUFFER_DESC desc;
    memset(&desc, 0, sizeof(desc));
    return desc;
  }
}

bool D3SceneObjectVisual::hasNormals() const {
  return hasVertexNormals(getFVF());
}

bool D3SceneObjectVisual::hasTextureCoordinates() const {
  return hasTextureVertices(getFVF());
}

bool D3SceneObjectVisual::isNormalsVisible() const {
  int index;
  return hasNormals() && ((index = findChildByType(SOTYPE_NORMALSOBJECT)) >= 0) && getChild(index)->isVisible();
}

void D3SceneObjectVisual::setNormalsVisible(bool visible) {
  if(visible != isNormalsVisible()) {
    if(visible) {
      addChild(new D3SceneObjectVisualNormals(this));
    } else {
      const int index = findChildByType(SOTYPE_NORMALSOBJECT);
      removeChild(index);
    }
  }
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
