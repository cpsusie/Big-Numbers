#include "pch.h"
#include <D3DGraphics/D3Device.h>
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/D3SceneObject.h>

D3Device &D3SceneObject::getDevice() const {
  return getScene().getDevice();
}

LPDIRECT3DDEVICE D3SceneObject::getDirectDevice() const {
  return getScene().getDirectDevice();
}
