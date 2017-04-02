#pragma once

#include "D3Scene.h"

class D3CoordinateSystem : public D3SceneObject {
private:
  D3LineArray *m_box;
  D3LineArrow *m_xaxes, *m_yaxes, *m_zaxes;
public:
  D3CoordinateSystem(D3Scene &scene);
  ~D3CoordinateSystem();
  D3PosDirUpScale getPDUS() {
    return D3Scene::getOrigo();
  }
  void draw();
};

