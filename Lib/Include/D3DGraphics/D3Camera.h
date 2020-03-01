#pragma once

#include <PropertyContainer.h>
#include <MFCUtil/ColorSpace.h>
#include "D3World.h"
#include "D3Ray.h"
#include "D3SceneObject.h"

class D3Scene;
class D3SceneObjectVisual;
class D3LightControl;
class D3PickedInfo;

typedef enum {
  CAM_VIEW                      // D3DXMATRIX
 ,CAM_PROJECTION                // D3DXMATRIX
 ,CAM_BACKGROUNDCOLOR           // D3DCOLOR
 ,CAM_WINDOW                    // HWND
 ,CAM_LIGHTCONTROLSVISIBLE      // BitSet
} D3CameraProperty;


class D3Camera : public D3SceneObject, public PropertyContainer {
private:
  HWND                    m_hwnd;
  bool                    m_rightHanded;
  D3DCOLOR                m_backgroundColor;
  mutable BitSet          m_visibleLightControlSet;
  float                   m_viewAngle;
  float                   m_nearViewPlane;
  D3World                 m_world;
  D3DXMATRIX              m_projMatrix, m_viewMatrix;

  // notify listeners with properyId=CAM_PROJECTION
  D3Camera      &setProjMatrix();
  D3DXMATRIX    &createProjMatrix(D3DXMATRIX &m) const;
  // notify listeners with properyId=CAM_VIEW
  D3Camera      &setViewMatrix();
  D3DXMATRIX    &createViewMatrix(D3DXMATRIX &m) const;
  D3Camera      &initWorldAndProjection();
  D3Camera      &initWorld();
  D3Camera      &initProjection();
  D3Camera(           const D3Camera &src); // not implemented
  D3Camera &operator=(const D3Camera &src); // not implemented
  D3Camera(const D3Camera *src, HWND hwnd);
public:
  D3Camera(D3Scene &scene, HWND hwnd);
  D3Camera *clone(HWND hwnd) const;
  ~D3Camera();
  SceneObjectType getType() const {
    return SOTYPE_CAMERA;
  }
  void OnSize();

  D3Camera &setHwnd(HWND hwnd);
  inline HWND getHwnd() const {
    return m_hwnd;
  }

  // p in screen-coordinates
  inline CPoint screenToWin(CPoint p) const {
    ScreenToClient(getHwnd(), &p);
    return p;
  }

  // p in screen coordinate
  bool ptInRect(CPoint p) const;
  inline CSize getWinSize() const {
    return getClientRect(getHwnd()).Size();
  }
  inline bool isActive() const {
    return IsWindowVisible(getHwnd());
  }
  inline bool isLightControlVisible(UINT lightIndex) const {
    return getLightControlsVisible().contains(lightIndex);
  }
  const BitSet   &getLightControlsVisible() const;
  // set visibility for many lightcontrols. notify only onnce (if changed)
  D3Camera       &setLightControlsVisible(const BitSet &set);
  // set property m_visibleLightControlSet with notifificationid=CAM_LIGHTCONTROLSVISIBLE
  D3LightControl *setLightControlVisible(UINT lightIndex, bool visible);
  // set property m_backgroundColor with notifificationid=CAM_BACKGROUNDCOLOR)
  inline D3Camera &setBackgroundColor(D3DCOLOR color) {
    setProperty(CAM_BACKGROUNDCOLOR, m_backgroundColor, color);
    return *this;
  }
  inline D3PCOLOR getBackgroundColor() const {
    return m_backgroundColor;
  }

  D3Camera &setViewAngle(float angle);
  inline float   getViewAngle() const {
    return m_viewAngle;
  }
  D3Camera &setNearViewPlane(float zn);
  inline float getNearViewPlane() const {
    return m_nearViewPlane;
  }

  D3Camera &setRightHanded(bool rightHanded);
  inline bool getRightHanded() const {
    return m_rightHanded;
  }
  const D3World &getD3World() const {
    return m_world;
  }
  // notify listeners with properyId=CAM_VIEW
  D3Camera &setD3World(const D3World &world);
  inline const D3DXVECTOR3 &getPos() const {
    return m_world.getPos();
  }
  inline D3DXVECTOR3 getDir() const {
    return m_world.getDir();
  }
  inline D3DXVECTOR3 getUp() const {
    return m_world.getUp();
  }
  inline D3DXVECTOR3 getRight() const {
    return m_world.getRight();
  }
  inline D3PCOLOR getDefaultBackgroundColor() const {
    return D3DCOLOR_XRGB(192, 192, 192);
  }
  // default viewangle in radians (=radians(45))
  static inline float getDefaultViewAngle() {
    return radians(45);
  }
  // default new view Plane (=0.1)
  static inline float getDefaultNearViewPlane() {
    return 0.1f;
  }
  static inline D3DXVECTOR3 getDefaultPos() {
    return D3DXVECTOR3(0, -5, 0);
  }
  static inline D3DXVECTOR3 getDefaultLookAt() {
    return D3DXORIGIN;
  }
  static inline D3DXVECTOR3 getDefaultUp() {
    return D3DXVECTOR3(0, 0, 1);
  }
  D3Camera &resetBackgroundColor();
  D3Camera &resetPos();
  D3Camera &resetOrientation();
  D3Camera &resetProjection();
  D3Camera &resetAll();

  D3Camera &setPos(        const D3DXVECTOR3     &pos);
  D3Camera &setOrientation(const D3DXQUATERNION  &q  );
  D3Camera &setLookAt(     const D3DXVECTOR3     &pos, const D3DXVECTOR3 &lookAt, const D3DXVECTOR3 &up);
  D3Camera &setLookAt(     const D3DXVECTOR3     &point);

  inline const D3DXMATRIX &getProjMatrix() const {
    return m_projMatrix;
  }
  inline const D3DXMATRIX &getViewMatrix() const {
    return m_viewMatrix;
  }
  // point in window-coordinates (m_hwnd)
  // Return Ray in world-space
  D3Ray getPickedRay(const CPoint &point) const;
  // Point in window-coordinates (m_hwnd)
  D3SceneObjectVisual *getPickedVisual(const CPoint &p, long mask = -1, D3DXVECTOR3 *hitPoint = NULL, D3Ray *ray = NULL, float *dist = NULL, D3PickedInfo *info = NULL) const;
  // call m_scene.render(*this)
  void render();
  String toString() const;
};
