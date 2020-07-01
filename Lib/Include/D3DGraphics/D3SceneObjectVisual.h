#pragma once

#include <MyString.h>
#include <MFCUtil/AbstractMouseHandler.h>
#include "D3World.h"
#include "D3Material.h"
#include "D3SceneObject.h"
class D3Scene;
class D3SceneObjectVisual;
class D3Ray;
class D3PickedInfo;

typedef CompactArray<D3SceneObjectVisual*> D3VisualArray;

class D3SceneObjectVisual : public D3SceneObject, public AbstractMouseHandler {
private:
  D3SceneObjectVisual *m_parent;
  D3VisualArray        m_children;

protected:
  String               m_name;
  void                *m_userData;
  D3DXMATRIX           m_world;
  // Return index of the child in m_children. can be used as parameter to getChild(UINT index)
  UINT addChild(D3SceneObjectVisual *child);
  void removeChild(UINT index);
  inline D3SceneObjectVisual *getParent() const {
    return m_parent;
  }
  inline D3SceneObjectVisual *getChild(UINT index) const {
    return m_children[index];
  }
  inline UINT getChildCount() const {
    return (UINT)m_children.size();
  }

  // return index of first child with the specified type, or -1, if none exist
  // not recursive search
  int  findChildByType(SceneObjectType type) const;
  bool isDescendant(D3SceneObjectVisual * const visual, bool recursive) const;
  bool isAncestor(  D3SceneObjectVisual * const visual) const;

public:
  D3SceneObjectVisual(D3Scene             &scene , const String &name=_T("VisualObject"));
  D3SceneObjectVisual(D3SceneObjectVisual *parent, const String &name=EMPTYSTRING);
  ~D3SceneObjectVisual();
  SceneObjectType     getType()                               const override {
    return SOTYPE_VISUALOBJECT;
  }
  bool                            isVisible()                 const override {
    return true;
  }
  virtual void                    draw();
  virtual LPD3DXMESH              getMesh()                   const {
    return NULL;
  }
  virtual LPDIRECT3DVERTEXBUFFER  getVertexBuffer()           const {
    return NULL;
  }
  virtual LPDIRECT3DINDEXBUFFER   getIndexBuffer()            const {
    return NULL;
  }
  inline bool                     hasMesh()                   const {
    return getMesh() != NULL;
  }
  inline bool                     hasVertexBuffer()           const {
    return getVertexBuffer() != NULL;
  }
  inline bool                     hasIndexBuffer()            const {
    return getIndexBuffer() != NULL;
  }
  D3DVERTEXBUFFER_DESC            getVertexBufferDesc()       const;
  D3DINDEXBUFFER_DESC             getIndexBufferDesc()        const;

  DWORD                           getFVF()                    const;
  virtual bool                    hasTextureCoordinates()     const;
  virtual bool                    hasTextureId()              const {
    return false;
  }
  virtual void                    setTextureId(int id) {
  }
  virtual int                     getTextureId()              const {
    return -1;
  }
  LPDIRECT3DTEXTURE               getTexture()                const;
  inline bool                     hasTexture()                const {
    return getTextureId() >= 0;
  }
  inline bool                     hasMutableTexture()         const {
    return hasTextureId() && hasTextureCoordinates();
  }
  D3Device                       &setDeviceTextureIfExist()   const;

  virtual bool                    hasNormals()                const;
  virtual bool                    isNormalsVisible()          const;
  virtual void                    setNormalsVisible(bool visible);
  virtual bool                    hasFillMode()               const {
    return false;
  }
  virtual void                    setFillMode(D3DFILLMODE fillMode) {
  }
  virtual D3DFILLMODE             getFillMode()               const {
    return D3DFILL_SOLID;
  }
  virtual bool                    hasShadeMode()              const {
    return false;
  }
  virtual void                    setShadeMode(D3DSHADEMODE shadeMode) {
  }
  virtual D3DSHADEMODE            getShadeMode()              const {
    return D3DSHADE_GOURAUD;
  }
  virtual bool                    getLightingEnable()         const {
    return true;
  }
  virtual int                     getMaterialId()             const {
    return -1;
  }
  const D3Material               &getMaterial()               const;
  inline bool                     hasMaterial()               const {
    return getMaterialId() >= 0;
  }
  D3Device                       &setDeviceMaterialIfExist()  const;

  void                            resetWorld() {
    getWorld() = D3World();
  }
  virtual D3DXMATRIX             &getWorld() {
    return m_world;
  }
  const D3DXMATRIX               &getWorld()                  const { // non-virtual
    return ((D3SceneObjectVisual*)this)->getWorld();
  }
  operator D3World()                                          const {
    return D3World(getWorld());
  }
  bool intersectsWithRay(const D3Ray &ray, float &dist, D3PickedInfo *info = NULL) const;

  inline const String            &getName()                   const {
    return m_name;
  }
  inline void                     setName(const String &name) {
    m_name = name;
  }
  inline void                    *getUserData() {
    return m_userData;
  }
  inline void                     setUserData(void *p) {
    m_userData = p;
  }
  virtual String                  toString() const {
    return getName();
  }
  virtual String                  getInfoString() const;
};
