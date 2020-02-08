#pragma once

#include <MFCUtil/CoordinateSystem/AxisType.h>
#include <MFCUtil/CoordinateSystem/DataRange.h>
#include "D3Scene.h"

class D3SceneObjectCoordinateSystem : public D3SceneObject {
private:
  D3SceneObjectLineArrow     m_axis;
  D3PosDirUpScale            m_origin;
  int                        m_axisMaterialId[3];
  D3SceneObjectCoordinateSystem(           const D3SceneObjectCoordinateSystem &src); // not implemented
  D3SceneObjectCoordinateSystem &operator=(const D3SceneObjectCoordinateSystem &src); // not implemented
public:
  D3SceneObjectCoordinateSystem(D3Scene &scene);
  ~D3SceneObjectCoordinateSystem();
  D3PosDirUpScale &getPDUS() {
    return m_origin = getScene().getOrigoPDUS();
  }
  void draw();
};
