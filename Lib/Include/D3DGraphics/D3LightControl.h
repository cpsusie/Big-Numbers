#pragma once

#include <D3DGraphics/D3SceneObjectWithMesh.h>

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
  static D3DMATERIAL   createMaterialFromLight(const D3Light &l);
  D3DCOLORVALUE getMaterialColor() const;

  inline void setSize(float size) {
    m_size = size;
  }
  inline float getSize() const {
    return m_size;
  }
public:
  D3LightControl(D3Scene &scene, int lightIndex);
  ~D3LightControl();

  SceneObjectType getType() const {
    return SOTYPE_LIGHTCONTROL;
  }
  bool isVisible() const;
  virtual D3DLIGHTTYPE getLightType() const = 0;

  inline int getLightIndex() const {
    return m_lightIndex;
  }
  LPD3DXMESH getMesh() const;

  int getMaterialId() const {
    return s_renderEffectEnabled ? -1 : m_materialId;
  }
  static bool isDifferentMaterial(const D3Light &l1, const D3Light &l2);
  D3Light getLight() const;
  void updateMaterial() const;
  static inline void enableRenderEffect(bool enabled) {
    s_renderEffectEnabled = enabled;
  }
  static inline bool isRenderEffectEnabled() {
    return s_renderEffectEnabled;
  }
  void draw();
};

class D3LightControlDirectional : public D3LightControl {
private:
  float m_sphereRadius;
public:
  D3LightControlDirectional(D3Scene &scene, int lightIndex);
  D3DLIGHTTYPE getLightType() const {
    return D3DLIGHT_DIRECTIONAL;
  }
  inline float getSphereRadius() const {
    return m_sphereRadius;
  }
  inline void setSphereRadius(float radius) {
    m_sphereRadius = radius;
  }
  D3DXMATRIX &getWorld();
};

class D3LightControlPoint : public D3LightControl {
public:
  D3LightControlPoint(D3Scene &scene, int lightIndex);
  D3DLIGHTTYPE getLightType() const {
    return D3DLIGHT_POINT;
  }
  D3DXMATRIX &getWorld();
};

class D3LightControlSpot : public D3LightControl {
public:
  D3LightControlSpot(D3Scene &scene, int lightIndex);
  D3DLIGHTTYPE getLightType() const {
    return D3DLIGHT_SPOT;
  }
  D3DXMATRIX &getWorld();
};
