#pragma once

#include <MFCUtil/CoordinateSystem/AxisType.h>
#include "D3Cube.h"
#include "D3SceneObjectVisual.h"

class D3Scene;

class D3SceneObjectCoordinateSystem : public D3SceneObjectVisual {
private:
  D3Cube m_cube;
  bool   m_visible;
  D3SceneObjectCoordinateSystem(           const D3SceneObjectCoordinateSystem &src); // not implemented
  D3SceneObjectCoordinateSystem &operator=(const D3SceneObjectCoordinateSystem &src); // not implemented
public:
  D3SceneObjectCoordinateSystem(D3Scene &scene, const D3Cube *cube=NULL);
  void setVisible(bool visible) {
    m_visible = visible;
  }
  bool isVisible() const override {
    return m_visible;
  }
  D3DXMATRIX &getWorld() override;
  void setRange(const D3Cube &cube);
  inline const D3Cube &getRange() const {
    return m_cube;
  }
};
