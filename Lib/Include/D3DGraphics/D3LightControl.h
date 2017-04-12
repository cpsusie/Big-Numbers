#pragma once

#include <D3DGraphics/D3Scene.h>

class D3LightControl : public SceneObjectWithMesh {
private:
  static bool   s_renderEffectEnabled;
  const int     m_lightIndex;
  int           m_materialIndex;
  float         m_size;

  LPD3DXEFFECT  m_effect;
  D3DXHANDLE    m_renderWith1LightNoTextureHandle;
  D3DXHANDLE    m_materialDiffuseColorHandle;
  D3DXHANDLE    m_lightDirHandle;
  D3DXHANDLE    m_worldViewProjectionHandle;
  D3DXHANDLE    m_worldHandle;

  void createEffect();
  void createMaterial();
  void setMaterialColors() const;
  void setMaterialColors(D3DMATERIAL &mat) const;
protected:
  static LPD3DXMESH &optimizeMesh(LPD3DXMESH &mesh);
  virtual D3DCOLORVALUE getColor() const;
  D3PosDirUpScale m_pdus;
  void prepareEffect();

  inline D3DCOLORVALUE getDisabledColor() const {
    return D3DXCOLOR(0.1f,0.1f,0.1f,1);
  }

  inline void setSize(float size) {
    m_size = size;
  }
  inline float getSize() const {
    return m_size;
  }
public:
  D3LightControl(D3Scene &scene, int lightIndex);
  D3LightControl(           const D3LightControl &src); // not defined
  D3LightControl &operator=(const D3LightControl &src); // not defined
  ~D3LightControl();

  SceneObjectType getType() const {
    return SOTYPE_LIGHTCONTROL;
  }
  virtual D3DLIGHTTYPE getLightType() const = 0;

  inline int getLightIndex() const {
    return m_lightIndex;
  }
  int getMaterialIndex() const {
    return s_renderEffectEnabled ? -1 : m_materialIndex;
  }
  LIGHT getLightParam() const;

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
