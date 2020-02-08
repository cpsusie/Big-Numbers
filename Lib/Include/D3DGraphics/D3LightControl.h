#pragma once

#include <D3DGraphics/D3Scene.h>

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
  static LPD3DXMESH &optimizeMesh(LPD3DXMESH &mesh);
  void createMaterial();
  D3PosDirUpScale m_pdus;
  void prepareEffect();

  static D3DCOLORVALUE getMaterialColor(const LIGHT &l);
  static inline D3DCOLORVALUE getDisabledMaterialColor() {
    return D3DXCOLOR(0.1f,0.1f,0.1f,1);
  }
  static D3DMATERIAL   createMaterialFromLight(const LIGHT &l);
  inline D3DCOLORVALUE getMaterialColor() const {
    return getMaterialColor(getLight());
  }

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
  virtual D3DLIGHTTYPE getLightType() const = 0;

  inline int getLightIndex() const {
    return m_lightIndex;
  }
  int getMaterialId() const {
    return s_renderEffectEnabled ? -1 : m_materialId;
  }
  static bool isDifferentMaterial(const LIGHT &l1, const LIGHT &l2);
  LIGHT getLight() const;
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

  LPD3DXMESH createArrowMesh();
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
  D3PosDirUpScale &getPDUS();
};

class D3LightControlPoint : public D3LightControl {
public:
  D3LightControlPoint(D3Scene &scene, int lightIndex);
  D3DLIGHTTYPE getLightType() const {
    return D3DLIGHT_POINT;
  }
  D3PosDirUpScale &getPDUS();
};

class D3LightControlSpot : public D3LightControl {
private:
  LPD3DXMESH createSpotMesh();
public:
  D3LightControlSpot(D3Scene &scene, int lightIndex);

  D3DLIGHTTYPE getLightType() const {
    return D3DLIGHT_SPOT;
  }
  D3PosDirUpScale &getPDUS();
};
