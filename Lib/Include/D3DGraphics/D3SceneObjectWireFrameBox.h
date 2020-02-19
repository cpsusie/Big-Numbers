#pragma once

#include "D3SceneObjectLineArray.h"

class D3SceneObjectWireFrameBox : public D3SceneObjectLineArray {
private:
  void init(const Vertex &p1, const Vertex &p2);
public:
  inline D3SceneObjectWireFrameBox(D3Scene &scene, const D3DXCube3 &cube)
    : D3SceneObjectLineArray(scene)
  {
    init(cube.getMin(), cube.getMax());
  }
  inline D3SceneObjectWireFrameBox(D3Scene &scene, const Vertex &p1, const Vertex &p2)
    : D3SceneObjectLineArray(scene)
  {
    init(p1, p2);
  }
};
