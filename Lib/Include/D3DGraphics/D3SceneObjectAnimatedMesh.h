#pragma once

#include "AbstractAnimator.h"
#include "D3SceneObjectVisual.h"
#include "MeshArray.h"

class D3SceneObjectAnimatedMesh : public D3SceneObjectVisual, public AbstractAnimator {
private:
  MeshArray     m_meshArray;
  D3DFILLMODE   m_fillMode;
  D3DSHADEMODE  m_shadeMode;
  UINT          m_lastRenderedIndex;
  void init();
public:
  D3SceneObjectAnimatedMesh(D3Scene             &scene , const MeshArray &meshArray, const String &name = _T("Animated Mesh"));
  D3SceneObjectAnimatedMesh(D3SceneObjectVisual *parent, const MeshArray &meshArray, const String &name = _T("Animated Mesh"));
  ~D3SceneObjectAnimatedMesh()                                override;
  void            handleTimeout(Timer &timer)                 override;
  SceneObjectType getType()                             const override {
    return SOTYPE_ANIMATEDOBJECT;
  }
  void            draw()                                      override;
  LPD3DXMESH      getMesh()                             const override;
  const MeshArray             &getMeshArray()           const {
    return m_meshArray;
  }
  bool            hasFillMode()                         const override {
    return true;
  }
  void            setFillMode(D3DFILLMODE fillMode)           override {
    m_fillMode = fillMode;
  }
  D3DFILLMODE      getFillMode()                        const override {
    return m_fillMode;
  }
  bool             hasShadeMode()                       const override {
    return true;
  }
  void             setShadeMode(D3DSHADEMODE shadeMode)       override {
    m_shadeMode = shadeMode;
  }
  D3DSHADEMODE     getShadeMode()                       const override {
    return m_shadeMode;
  }
};
