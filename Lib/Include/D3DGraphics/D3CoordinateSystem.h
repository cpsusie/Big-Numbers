#pragma once

#include "D3Scene.h"

class D3CoordinateSystem : public D3SceneObject {
private:
  D3LineArray *m_box;
  D3LineArrow *m_xaxes, *m_yaxes, *m_zaxes;
  D3PosDirUpScale m_origo;
public:
  D3CoordinateSystem(D3Scene &scene);
  ~D3CoordinateSystem();
  D3PosDirUpScale &getPDUS() {
    m_origo = D3Scene::getOrigo();
    return m_origo;
  }
  void draw();
};

