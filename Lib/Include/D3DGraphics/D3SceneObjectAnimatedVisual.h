#pragma once

#include "AbstractAnimator.h"
#include "D3SceneObjectVisual.h"
#include "MeshArray.h"

class D3SceneObjectAnimatedVisual : public D3SceneObjectVisual, public AbstractAnimator {
private:
  UINT m_lastRenderedIndex;
  void init();
public:
  D3SceneObjectAnimatedVisual(D3Scene             &scene , const MeshArray &meshArray, const String &name = _T("Animated Visual"));
  D3SceneObjectAnimatedVisual(D3SceneObjectVisual *parent, const MeshArray &meshArray, const String &name = _T("Animated Visual"));
  ~D3SceneObjectAnimatedVisual()                             override;
  void            handleTimeout(Timer &timer)                override;
  SceneObjectType getType()                            const override {
    return SOTYPE_ANIMATEDOBJECT;
  }
  void            draw()                                     override;
  LPD3DXMESH      getMesh()                            const override;
  bool            hasTextureCoordinates()              const override;
  bool            hasTextureId()                       const override;
  void            setTextureId(int id)                       override;
  int             getTextureId()                       const override;
  bool            hasNormals()                         const override;
  bool            isNormalsVisible()                   const override;
  void            setNormalsVisible(bool visible)            override;
  bool            hasFillMode()                        const override;
  bool            hasShadeMode()                       const override;
  void            setFillMode( D3DFILLMODE  fillMode )       override;
  D3DFILLMODE     getFillMode()                        const override;
  void            setShadeMode(D3DSHADEMODE shadeMode)       override;
  D3DSHADEMODE    getShadeMode()                       const override;
  int             getMaterialId()                      const override;
  D3DXMATRIX     &getWorld()                                 override;
};
