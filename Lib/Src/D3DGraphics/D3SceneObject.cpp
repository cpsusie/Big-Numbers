#include "pch.h"
#include <D3DGraphics/D3Device.h>
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/D3SceneObject.h>

D3Device &D3SceneObject::getDevice() {
  return getScene().getDevice();
}
LPDIRECT3DDEVICE D3SceneObject::getDirectDevice() {
  return getDevice();
}
