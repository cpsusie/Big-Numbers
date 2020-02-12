#pragma once

#include <MFCUtil/CoordinateSystem/AxisType.h>
#include "D3DXCube.h"
#include "D3Scene.h"

class D3SceneObjectCoordinateSystem : public D3SceneObject {
private:
  friend class D3CoordinateSystemFrameObject;
  D3PosDirUpScale                m_pdus;
  D3CoordinateSystemFrameObject *m_frameObject;
  D3DXCube3                      m_cube;
  D3SceneObjectLineArrow         m_axis;
  int                            m_axisMaterialId[3];
  D3SceneObjectCoordinateSystem(           const D3SceneObjectCoordinateSystem &src); // not implemented
  D3SceneObjectCoordinateSystem &operator=(const D3SceneObjectCoordinateSystem &src); // not implemented
public:
  D3SceneObjectCoordinateSystem(D3Scene &scene, const D3DXCube3 *cube=NULL);
  ~D3SceneObjectCoordinateSystem();
  void setRange(const D3DXCube3 &cube);
  inline const D3DXCube3 &getRange() const {
    return m_cube;
  }
  D3PosDirUpScale &getPDUS() {
    return m_pdus.resetPos();
  }
  LPD3DXMESH getMesh() const;
  void draw();
};
