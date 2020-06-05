#include "pch.h"
#include <D3DGraphics/D3Device.h>
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/D3ToString.h>
#include <D3DGraphics/D3SceneObjectVisual.h>
#include <D3DGraphics/D3SceneObjectVisualNormals.h>

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

LPDIRECT3DTEXTURE D3SceneObjectVisual::getTexture() const {
  return getScene().getTexture(getTextureId());
}

D3Device &D3SceneObjectVisual::setDeviceTextureIfExist() const {
  D3Device &device     = getDevice();
  const int textureId  = getTextureId();
  if((textureId >= 0) && hasTextureCoordinates()) {
    getDevice().setSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR)
               .setSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR)
               .setTexture(     0, getScene().getTexture(textureId))
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

#define NORMALFVFFLAGS  (D3DFVF_XYZ | D3DFVF_NORMAL)
#define TEXTUREFVFFLAGS (D3DFVF_XYZ | D3DFVF_TEX1)

DWORD D3SceneObjectVisual::getFVF() const {
  return getVertexBufferDesc().FVF;
}

D3DVERTEXBUFFER_DESC D3SceneObjectVisual::getVertexBufferDesc() const {
  D3DVERTEXBUFFER_DESC desc;
  if(hasMesh()) {
    LPD3DXMESH mesh = getMesh();
    LPDIRECT3DVERTEXBUFFER vb;
    V(mesh->GetVertexBuffer(&vb)); TRACE_CREATE(vb);
    V(vb->GetDesc(&desc));
    SAFERELEASE(vb);
  } else if(hasVertexBuffer()) {
    LPDIRECT3DVERTEXBUFFER vb = getVertexBuffer();
    V(vb->GetDesc(&desc));
  } else {
    memset(&desc, 0, sizeof(desc));
  }
  return desc;
}

bool D3SceneObjectVisual::hasNormals() const {
  return (getFVF() & NORMALFVFFLAGS) == NORMALFVFFLAGS;
}

bool D3SceneObjectVisual::hasTextureCoordinates() const {
  return (getFVF() & TEXTUREFVFFLAGS) == TEXTUREFVFFLAGS;
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
