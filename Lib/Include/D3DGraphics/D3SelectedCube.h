#pragma once

#include "D3SceneObjectSolidBox.h"
#include "D3Cube.h"

class D3SelectedCube : public D3SceneObjectSolidBox {
private:
  D3SelectedCube(const D3SelectedCube &src);            // not implemented
  D3SelectedCube&operator=(const D3SelectedCube &src);  // not implemented
public:
  D3SelectedCube(D3Scene &scene, const D3Cube &cube = D3Cube::getStdCube());
  void setCube(float minX, float minY, float minZ, float maxX, float maxY, float maxZ);
  void setCube(const D3Cube &cube);
  inline D3Cube getCube() const {
    return getBoundingBox();
  }
};
