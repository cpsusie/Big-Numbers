#pragma once

#include <MyString.h>
#include "D3World.h"
#include "D3Material.h"
#include "D3SceneObject.h"

class D3Scene;
class D3SceneObjectVisual;
class D3Ray;
class D3PickedInfo;

typedef CompactArray<D3SceneObjectVisual*> D3VisualArray;

class D3SceneObjectVisual : public D3SceneObject {
protected:
  D3SceneObjectVisual *m_parent;
  D3VisualArray        m_children;
  String               m_name;
  bool                 m_visible;
  void                *m_userData;
  D3DXMATRIX           m_world;
  // Return index of the child in m_children. can be used as parameter to getChild(UINT index)
  UINT addChild(D3SceneObjectVisual *child);
  inline D3SceneObjectVisual *getChild(UINT index) const {
    return m_children[index];
  }
public:
  D3SceneObjectVisual(D3Scene             &scene , const String &name=_T("VisualObject"));
  D3SceneObjectVisual(D3SceneObjectVisual *parent, const String &name=EMPTYSTRING);
  ~D3SceneObjectVisual();
  virtual SceneObjectType getType() const {
    return SOTYPE_VISUALOBJECT;
  }
  D3SceneObjectVisual *getParent() const {
    return m_parent;
  }
  virtual void draw();
  virtual LPD3DXMESH getMesh() const {
    return NULL;
  }
  inline bool hasMesh() const {
    return getMesh() != NULL;
  }

  virtual int getMaterialId() const {
    return -1;
  }
  virtual String toString() const {
    return getName();
  }
  inline void *getUserData() {
    return m_userData;
  }
  inline void setUserData(void *p) {
    m_userData = p;
  }
  inline void setVisible(bool visible) {
    m_visible = visible;
  }
  inline bool isVisible() const {
    return m_visible;
  }
  virtual bool hasFillMode() const {
    return false;
  }
  virtual void setFillMode(D3DFILLMODE fillMode) {
  }
  virtual D3DFILLMODE getFillMode() const {
    return D3DFILL_SOLID;
  }
  virtual bool hasShadeMode() const {
    return false;
  }
  virtual void setShadeMode(D3DSHADEMODE shadeMode) {
  }
  virtual D3DSHADEMODE getShadeMode() const {
    return D3DSHADE_GOURAUD;
  }
  const D3Material &getMaterial() const;
  inline bool hasMaterial() const {
    return getMaterialId() >= 0;
  }
  void resetWorld() {
    m_world = D3World();
  }
  virtual D3DXMATRIX &getWorld() {
    return m_world;
  }
  operator D3World() const {
    return D3World(m_world);
  }
  bool intersectsWithRay(const D3Ray &ray, float &dist, D3PickedInfo *info = NULL) const;

  inline const String &getName() const {
    return m_name;
  }
  inline void setName(const String &name) {
    m_name = name;
  }
};
