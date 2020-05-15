#pragma once

#include <D3DGraphics/D3Device.h>
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/D3SceneObjectWithMesh.h>
#include <D3DGraphics/D3SceneObjectAnimatedMesh.h>
#include <D3DGraphics/MeshCreators.h>
#include "resource.h"       // main symbols

class D3AnimatedFunctionSurface : public D3SceneObjectAnimatedMesh {
private:
  int m_materialId;
public:
  D3AnimatedFunctionSurface(D3Scene &scene, const MeshArray &ma) : D3SceneObjectAnimatedMesh(scene, ma) {
    m_materialId = scene.addMaterial(D3Material::createDefaultMaterial());
  }
  ~D3AnimatedFunctionSurface() {
    getScene().removeMaterial(m_materialId);
  }
  void modifyContextMenu(HMENU menu) {
    appendMenuItem(menu, _T("Edit function"), ID_OBJECT_EDITFUNCTION);
  }
  int getMaterialId() const {
    return m_materialId;
  }
};

class D3FunctionSurface : public D3SceneObjectWithMesh {
private:
  int m_materialId;
public:
  D3FunctionSurface(D3Scene &scene, LPD3DXMESH mesh) : D3SceneObjectWithMesh(scene, mesh) {
    m_materialId = scene.addMaterial(D3Material::createDefaultMaterial());
  }
  ~D3FunctionSurface() {
    getScene().removeMaterial(m_materialId);
  }
  void modifyContextMenu(HMENU menu) {
    appendMenuItem(menu, _T("Edit function"), ID_OBJECT_EDITFUNCTION);
  }
  int getMaterialId() const {
    return m_materialId;
  }
};

template<typename T> D3FunctionSurface *createSurface(D3Scene &s, const T param) {
  D3FunctionSurface *obj = new D3FunctionSurface(s, createMesh(s.getDevice(), param));
  TRACE_NEW(obj);
  return obj;
}

template<typename T> D3AnimatedFunctionSurface *createAnimatedSurface(CWnd *parent, D3Scene &s, const T param) {
  D3AnimatedFunctionSurface *obj = new D3AnimatedFunctionSurface(s, createMeshArray(parent, s.getDevice(), param));
  TRACE_NEW(obj);
  return obj;
}
