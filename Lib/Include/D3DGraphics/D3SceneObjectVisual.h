#pragma once

#include <MyString.h>
#include "D3MathWorld.h"
#include "Material.h"
#include "D3SceneObject.h"

class D3Scene;
class D3Device;
class D3Ray;
class D3PickedInfo;

class D3SceneObjectVisual : public D3SceneObject {
protected:
  String       m_name;
  bool         m_visible;
  void        *m_userData;
protected:
  D3DXMATRIX   m_world;
public:
  D3SceneObjectVisual(D3Scene &scene, const String &name=_T("Untitled"))
    : D3SceneObject(scene)
    , m_name(name)
    , m_visible(true)
    , m_userData(NULL)
  {
    m_world = D3World();
  }
  virtual ~D3SceneObjectVisual() {
  }
  virtual SceneObjectType getType() const {
    return SOTYPE_VISUALOBJECT;
  }
  virtual void draw() = 0;
  virtual LPD3DXMESH getMesh() const {
    return NULL;
  }
 
  virtual int getMaterialId() const {
    return 0;
  }
  virtual void modifyContextMenu(CMenu &menu) {
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
  const MATERIAL &getMaterial() const;
  inline bool hasMaterial() const {
    return getMaterialId() >= 0;
  }
  void resetWorld() {
    getWorld() = D3World();
  }
  virtual D3DXMATRIX &getWorld() {
    return m_world;
  }
  inline void setPos(const D3DXVECTOR3 &pos) {
    getWorld() = D3World(getWorld()).setPos(pos);
  }
  inline D3DXVECTOR3 getPos() const {
    return D3World(((D3SceneObjectVisual*)this)->getWorld()).getPos();
  }
  inline void setScale(const D3DXVECTOR3 &scale) {
    getWorld() = D3World(getWorld()).setScale(scale);
  }
  inline void setScaleAll(float scale) {
    setScale(D3DXVECTOR3(scale, scale, scale));
  }
  inline D3DXVECTOR3 getScale() const {
    return D3World(((D3SceneObjectVisual*)this)->getWorld()).getScale();
  }
  inline void setOrientation(const D3DXQUATERNION &q) {
    getWorld() = D3World(getWorld()).setOrientation(q);
  }
  inline D3DXQUATERNION getOrientation() const {
    return D3World(((D3SceneObjectVisual*)this)->getWorld()).getOrientation();
  }
  bool intersectsWithRay(const D3Ray &ray, float &dist, D3PickedInfo *info = NULL) const;

  inline const String &getName() const {
    return m_name;
  }
  inline void setName(const String &name) {
    m_name = name;
  }
};

typedef CompactArray<D3SceneObjectVisual*>  D3VisualArray;
