#pragma once

#include <D3DGraphics/D3Scene.h>

#define USE_RENDEREFFECT

class D3LightControl : public SceneObjectWithMesh {
private:

  const int       m_lightIndex;
  float           m_size;

#ifdef USE_RENDEREFFECT
  LPD3DXEFFECT  m_effect;
  D3DXHANDLE    m_renderWith1LightNoTextureHandle;
  D3DXHANDLE    m_materialDiffuseColorHandle;
  D3DXHANDLE    m_lightDirHandle;
  D3DXHANDLE    m_worldViewProjectionHandle;
  D3DXHANDLE    m_worldHandle;

  void createEffect();
#endif

protected:
  static LPD3DXMESH &optimizeMesh(LPD3DXMESH &mesh);
  virtual D3DCOLORVALUE getColor() const;
  D3PosDirUpScale m_pdus;
  D3DMATERIAL getMaterial() const;

#ifdef USE_RENDEREFFECT
  void prepareEffect();
#endif

  inline D3DCOLORVALUE getDisabledColor() const {
    return D3DXCOLOR(0.1f,0.1f,0.1f,1);
  }

  void setSize(float size) {
    m_size = size;
  }
  float getSize() const {
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

  int getLightIndex() const {
    return m_lightIndex;
  }
  LIGHT getLightParam() const;

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
