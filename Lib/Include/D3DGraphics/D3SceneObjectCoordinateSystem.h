#pragma once

#include <MFCUtil/CoordinateSystem/AxisType.h>
#include <MFCUtil/CoordinateSystem/DataRange.h>
#include "D3Scene.h"

class D3SceneObjectCoordinateSystem : public D3SceneObject {
private:
  D3DXCube3                  m_cube;
  D3SceneObjectWireFrameBox *m_box;
  D3SceneObjectLineArrow    *m_axis[3];
  D3PosDirUpScale            m_origin;
  AxisAttribute              m_axisAttr[3];
  bool                       m_boxVisible, m_axesVisible;
  void init();
  void cleanUp();
  void setCube(float minX, float minY, float minZ, float maxX, float maxY, float maxZ);
public:
  D3SceneObjectCoordinateSystem(D3Scene &scene, const D3DXCube3 *cube = NULL);
  D3SceneObjectCoordinateSystem(const D3SceneObjectCoordinateSystem &src);
  D3SceneObjectCoordinateSystem &operator=(const D3SceneObjectCoordinateSystem &src);
  ~D3SceneObjectCoordinateSystem();
  void setCube(const D3DXCube3 &cube);
  inline const D3DXCube3 &getCube() const {
    return m_cube;
  }
  inline void setBoxVisible(bool visible) {
    m_boxVisible = visible;
  }
  inline bool getBoxVisible() const {
    return m_boxVisible;
  }
  inline void setAxesVisible(bool visible) {
    m_axesVisible = visible;
  }
  inline bool getAxesVisible() const {
    return m_axesVisible;
  }
  D3PosDirUpScale &getPDUS() {
    m_origin = D3Scene::getOrigo();
    return m_origin;
  }
  static DoubleInterval getDefaultInterval(AxisType type);

  void draw();
};
