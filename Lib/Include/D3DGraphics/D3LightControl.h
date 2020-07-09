#pragma once

#include "D3SceneObjectWithMesh.h"

class D3Light;

class D3LightControl : public D3SceneObjectWithMesh {
private:
  static bool   s_renderEffectEnabled;
  const int     m_lightIndex;
  int           m_materialId;
  float         m_size;

  LPD3DXEFFECT  m_effect;
  D3DXHANDLE    m_renderWith1LightNoTextureHandle;
  D3DXHANDLE    m_materialDiffuseColorHandle;
  D3DXHANDLE    m_lightDirHandle;
  D3DXHANDLE    m_worldViewProjectionHandle;
  D3DXHANDLE    m_worldHandle;


  D3LightControl(           const D3LightControl &src); // Not defined. Class not cloneable
  D3LightControl &operator=(const D3LightControl &src); // Not defined. Class not cloneable

  void createEffect();
protected:
  void createMaterial();
  void prepareEffect();

  static D3DCOLORVALUE getMaterialColor(const D3Light &l);
  static inline D3DCOLORVALUE getDisabledMaterialColor() {
    return D3DXCOLOR(0.1f,0.1f,0.1f,1);
  }
  static D3MATERIAL    createMaterialFromLight(const D3Light &l);
  D3DCOLORVALUE        getMaterialColor() const;

  inline void setSize(float size) {
    m_size = size;
  }
  inline float getSize() const {
    return m_size;
  }
public:
  D3LightControl(D3Scene &scene, int lightIndex);
  ~D3LightControl()                override;

  SceneObjectType getType()  const override {
    return SOTYPE_LIGHTCONTROL;
  }
  bool isVisible()           const override;
  void draw()                      override;
  LPD3DXMESH getMesh()       const override;
  int getMaterialId()        const override {
    return s_renderEffectEnabled ? -1 : m_materialId;
  }
  virtual D3DLIGHTTYPE getLightType() const = 0;

  inline int getLightIndex() const {
    return m_lightIndex;
  }

  static bool isDifferentMaterial(const D3Light &l1, const D3Light &l2);
  D3Light getLight()         const;
  void updateMaterial()      const;
  static inline void enableRenderEffect(bool enabled) {
    s_renderEffectEnabled = enabled;
  }
  static inline bool isRenderEffectEnabled() {
    return s_renderEffectEnabled;
  }
};

class D3LightControlDirectional : public D3LightControl {
private:
  float m_sphereRadius;
public:
  D3LightControlDirectional(D3Scene &scene, int lightIndex);
  D3DLIGHTTYPE getLightType()    const override {
    return D3DLIGHT_DIRECTIONAL;
  }
  D3DXMATRIX &getWorld()               override;
  inline float getSphereRadius() const {
    return m_sphereRadius;
  }
  inline void setSphereRadius(float radius) {
    m_sphereRadius = radius;
  }
};

class D3LightControlPoint : public D3LightControl {
public:
  D3LightControlPoint(D3Scene &scene, int lightIndex);
  D3DLIGHTTYPE getLightType()    const override {
    return D3DLIGHT_POINT;
  }
  D3DXMATRIX &getWorld()                override;
};

class D3LightControlSpot : public D3LightControl {
public:
  D3LightControlSpot(D3Scene &scene, int lightIndex);
  D3DLIGHTTYPE getLightType()     const override {
    return D3DLIGHT_SPOT;
  }
  D3DXMATRIX &getWorld()                override;
};
