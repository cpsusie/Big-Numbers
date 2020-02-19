#include "pch.h"
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/D3SceneObjectVisual.h>

const MATERIAL &D3SceneObjectVisual::getMaterial() const {
  return getScene().getMaterial(getMaterialId());
}
