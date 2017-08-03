#pragma once

#include "D3Scene.h"

class D3CoordinateSystem : public D3SceneObject {
private:
  D3DXCube3       m_cube;
  D3WireFrameBox *m_box;
  D3LineArrow    *m_xaxis, *m_yaxis, *m_zaxis;
  D3PosDirUpScale m_origin;
  bool            m_boxVisible, m_axesVisible;
  void init();
  void cleanUp();
  void setCube(float minX, float minY, float minZ, float maxX, float maxY, float maxZ);
public:
  D3CoordinateSystem(D3Scene &scene, const D3DXCube3 *cube = NULL);
  D3CoordinateSystem(const D3CoordinateSystem &src);
  D3CoordinateSystem &operator=(const D3CoordinateSystem &src);
  ~D3CoordinateSystem();
  void setCube(const D3DXCube3 &cube);
  const D3DXCube3 getCube() const {
    return m_cube;
  }
  void setBoxVisible(bool visible) {
    m_boxVisible = visible;
  }
  bool getBoxVisible() const {
    return m_boxVisible;
  }
  void setAxesVisible(bool visible) {
    m_axesVisible = visible;
  }
  bool getAxesVisible() const {
    return m_axesVisible;
  }
  D3PosDirUpScale &getPDUS() {
    m_origin = D3Scene::getOrigo();
    return m_origin;
  }
  void draw();
};

