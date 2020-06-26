#pragma once

#include "D3SceneObjectVisual.h"

template<typename BaseClass> class D3SceneObjectVisualChild : public BaseClass {
public:
  D3SceneObjectVisualChild(D3SceneObjectVisual *parent, const String &name = EMPTYSTRING)
    : BaseClass(parent, name)
  {
  }
  D3SceneObjectVisualChild(D3SceneObjectVisual *parent, LPD3DXMESH mesh = NULL, const String &name = EMPTYSTRING)
    : BaseClass(parent, mesh, name)
  {
  }
  bool                    hasTextureId()              const override {
    return getParent()->hasTextureId();
  }
  int                     getTextureId()              const override {
    return getParent()->getTextureId();
  }
  bool                    hasFillMode()               const override {
    return getParent()->hasFillMode();
  }
  D3DFILLMODE             getFillMode()               const override {
    return getParent()->getFillMode();
  }
  bool                    hasShadeMode()              const override {
    return getParent()->hasShadeMode();
  }
  D3DSHADEMODE            getShadeMode()              const override {
    return getParent()->getShadeMode();
  }
  bool                    getLightingEnable()         const override {
    return getParent()->getLightingEnable();
  }
  int                     getMaterialId()             const override {
    return getParent()->getMaterialId();
  }
  D3DXMATRIX             &getWorld()                        override {
    return getParent()->getWorld();
  }
};
