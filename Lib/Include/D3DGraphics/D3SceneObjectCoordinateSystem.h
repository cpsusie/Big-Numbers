#pragma once

#include <MFCUtil/CoordinateSystem/AxisType.h>
#include "D3DXCube.h"
#include "D3SceneObjectVisual.h"

class D3Scene;

class D3SceneObjectCoordinateSystem : public D3SceneObjectVisual {
private:
  friend class D3CoordinateSystemFrameObject;
  friend class D3SCoordSystemLineArrow;
  D3CoordinateSystemFrameObject *m_frameObject;
  D3SCoordSystemLineArrow       *m_axis[3];
  D3DXCube3                      m_cube;
  D3SceneObjectCoordinateSystem(           const D3SceneObjectCoordinateSystem &src); // not implemented
  D3SceneObjectCoordinateSystem &operator=(const D3SceneObjectCoordinateSystem &src); // not implemented
public:
  D3SceneObjectCoordinateSystem(D3Scene &scene, const D3DXCube3 *cube=NULL);
  ~D3SceneObjectCoordinateSystem();
  void setRange(const D3DXCube3 &cube);
  inline const D3DXCube3 &getRange() const {
    return m_cube;
  }
  LPD3DXMESH getMesh() const;
  void draw();
};
