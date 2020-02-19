#include "pch.h"
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/D3SceneObject.h>

const MATERIAL &D3SceneObject::getMaterial() const {
  return getScene().getMaterial(getMaterialId());
}
