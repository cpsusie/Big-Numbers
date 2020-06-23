#pragma once

#include <D3DGraphics/D3Device.h>
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/D3SceneObjectWithMesh.h>
#include <D3DGraphics/D3SceneObjectAnimatedMesh.h>
#include <D3DGraphics/MeshCreators.h>
#include "resource.h"       // main symbols

class D3AnimatedFunctionSurface : public D3SceneObjectAnimatedMesh {
private:
  int  m_materialId;
  int  m_textureId;
  bool m_hasTextureId;
public:
  D3AnimatedFunctionSurface(D3Scene &scene, const MeshArray &ma)
    : D3SceneObjectAnimatedMesh(scene, ma)
    , m_textureId(-1)
  {
    m_hasTextureId = hasTextureCoordinates();
    m_materialId   = scene.addMaterial(D3Material::createDefaultMaterial());
  }
  D3AnimatedFunctionSurface(D3Scene &scene, const MeshArray &ma, UINT textureId)
    : D3SceneObjectAnimatedMesh(scene, ma)
    , m_textureId(textureId)
    , m_hasTextureId(true)
  {
    m_materialId = -1;
  }
  ~D3AnimatedFunctionSurface() override {
    getScene().removeMaterial(m_materialId);
  }
  void modifyContextMenu(HMENU menu) override {
    appendMenuItem(menu, _T("Edit function"), ID_OBJECT_EDITFUNCTION);
  }
  int getMaterialId() const override {
    return m_materialId;
  }
  bool hasTextureId() const override {
    return m_hasTextureId;
  }
  void setTextureId(int id) override {
    if(hasTextureId()) {
      m_textureId = id;
    }
  }
  int getTextureId() const override {
    return hasTextureId() ? m_textureId : -1;
  }
};

class D3FunctionSurface : public D3SceneObjectWithMesh {
private:
  int  m_materialId;
  int  m_textureId;
  bool m_hasTextureId;
public:
  D3FunctionSurface(D3Scene &scene, LPD3DXMESH mesh)
    : D3SceneObjectWithMesh(scene, mesh)
    , m_textureId(-1)
  {
    m_hasTextureId = hasTextureCoordinates();
    m_materialId   = scene.addMaterial(D3Material::createDefaultMaterial());
  }
  D3FunctionSurface(D3Scene &scene, LPD3DXMESH mesh, UINT textureId)
    : D3SceneObjectWithMesh(scene, mesh), m_textureId(textureId)
    , m_hasTextureId(true)
  {
    m_materialId = -1;
  }
  ~D3FunctionSurface() override {
    if(hasMaterial()) {
      getScene().removeMaterial(m_materialId);
    }
  }
  void modifyContextMenu(HMENU menu) override {
    appendMenuItem(menu, _T("Edit function"), ID_OBJECT_EDITFUNCTION);
  }
  int getMaterialId() const override {
    return m_materialId;
  }
  bool hasTextureId() const override {
    return m_hasTextureId;
  }
  void setTextureId(int id) override {
    if(hasTextureId()) {
      m_textureId = id;
    }
  }
  int getTextureId() const override {
    return hasTextureId() ? m_textureId : -1;
  }
};

template<typename T> D3FunctionSurface *createSurface(D3Scene &s, const T &param) {
  D3FunctionSurface *obj = new D3FunctionSurface(s, createMesh(s.getDevice(), param)); TRACE_NEW(obj);
  if(obj->hasMutableTexture() && param.hasTextureFileName()) {
    LPDIRECT3DTEXTURE texture = s.getDevice().loadTextureFromFile(param.m_textureFileName);
    obj->setTextureId(s.addTexture(texture));
  }
  return obj;
}

template<typename T> D3AnimatedFunctionSurface *createAnimatedSurface(CWnd *parent, D3Scene &s, const T &param) {
  D3AnimatedFunctionSurface *obj = new D3AnimatedFunctionSurface(s, createMeshArray(parent, s.getDevice(), param)); TRACE_NEW(obj);
  if(obj->hasMutableTexture() && param.hasTextureFileName()) {
    LPDIRECT3DTEXTURE texture = s.getDevice().loadTextureFromFile(param.m_textureFileName);
    obj->setTextureId(s.addTexture(texture));
  }
  return obj;
}
