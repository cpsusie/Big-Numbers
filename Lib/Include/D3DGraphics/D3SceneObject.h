#pragma once

class D3Scene;
class D3Device;

typedef enum {
  SOTYPE_NULL
 ,SOTYPE_CAMERA
 ,SOTYPE_VISUALOBJECT
 ,SOTYPE_LIGHTCONTROL
 ,SOTYPE_ANIMATEDOBJECT
} SceneObjectType;

class D3SceneObject {
private:
  D3Scene &m_scene;
public:
  D3SceneObject(D3Scene &scene)
    : m_scene(scene)
  {
  }
  virtual ~D3SceneObject() {
  }
  inline D3Scene &getScene() const {
    return m_scene;
  }
  D3Device        &getDevice();
  LPDIRECT3DDEVICE getDirectDevice();
  virtual SceneObjectType getType() const = 0;
};
