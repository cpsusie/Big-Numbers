#pragma once

#include <TinyBitSet.h>
#include <MFCUtil/WinTools.h>
#include <MFCUtil/PropertyDialogMap.h>
#include "D3SceneObject.h"
#include "D3Ray.h"
#include "D3PickedInfo.h"
#include "D3SceneObjectPoint.h"
#include "D3SceneObjectCoordinateSystem.h"
#include "D3SceneContainer.h"

typedef enum {
  CONTROL_IDLE
 ,CONTROL_CAMERA_WALK
 ,CONTROL_CAMERA_PROJECTION
 ,CONTROL_OBJECT_POS
 ,CONTROL_OBJECT_SCALE
 ,CONTROL_ANIMATION_SPEED
 ,CONTROL_MATERIAL
 ,CONTROL_LIGHT
 ,CONTROL_SPOTLIGHTPOINT
 ,CONTROL_SPOTLIGHTANGLES
 ,CONTROL_LIGHTCOLOR
 ,CONTROL_BACKGROUNDCOLOR
 ,CONTROL_AMBIENTLIGHTCOLOR
} D3EditorControl;

#define SE_INITDONE           0x01
#define SE_ENABLED            0x02
#define SE_PROPCHANGES        0x04
#define SE_RENDER3D           0x08
#define SE_RENDERINFO         0x10
#define SE_RENDERNOW          0x20
#define SE_LIGHTCONTROLS      0x40
#define SE_MOUSEVISIBLE       0x80
#define SE_RENDERALL          (SE_RENDER3D | SE_RENDERINFO)
#define SE_ALL                (SE_ENABLED | SE_PROPCHANGES | SE_RENDERALL | SE_LIGHTCONTROLS)

class D3Camera;
class D3SceneContainer;
class D3SceneObjectVisual;
class D3LightControl;
class D3SceneObjectAnimatedMesh;

class D3EditorPickedInfo {
public:
  D3PickedInfo m_info;
  D3DXVECTOR3  m_worldPoint; // in world space
  float        m_dist;
  D3EditorPickedInfo() {
    clear();
  }
  void clear();
  String toString() const;
};

class D3SceneEditor : public PropertyChangeListener {
private:
    D3SceneContainer              *m_sceneContainer;
    D3EditorControl                m_currentControl;
    D3Camera                      *m_selectedCamera;
    int                            m_selectedCameraIndex;
    D3SceneObjectVisual           *m_currentObj;
    D3SceneObjectCoordinateSystem *m_coordinateSystem;
    PropertyDialogMap              m_propertyDialogMap;
    PropertyDialog                *m_currentPropertyDialog;
    BYTE                           m_stateFlags;
    CPoint                         m_lastMouse;
    D3SceneObjectPoint             m_centerOfRotation;
    D3Ray                          m_pickedRay;   // in world space
    D3EditorPickedInfo             m_pickedInfo;
    String                         m_paramFileName;

    HWND     getCurrentHwnd() const;
    D3Scene &getScene() const {
      return m_sceneContainer->getScene();
    }
    CameraSet getActiveCameraSet() const;
    CameraSet getSelectedCameraSet() const;
    inline void render(BYTE flags, CameraSet cameraSet) {
      if(isSet(flags)) {
        m_sceneContainer->render(flags, cameraSet);
      }
    }
    inline void renderInfo() {
      render(SE_RENDERINFO, CameraSet());
    }
    inline void renderSelected(BYTE flags) {
      render(flags, getSelectedCameraSet());
    }
    inline void renderActive(BYTE flags) {
      render(flags, getActiveCameraSet());
    }
    int               findCameraIndex(CPoint p) const;
    // if index >= 0, set m_currentCamera = scene.getCameraArray()[index], else = NULL, and set m_currentCameraIndex = index
    // return boolean value of (m_selectedCamera != NULL) after adjustment
    bool              selectCAM(int index);
    bool              selectCAM(CPoint p);
    bool              isSameCAM(CPoint p) const;
    bool              hasCAM() const {
      return m_selectedCamera != NULL;
    }

    void              rotateCurrentVisualFrwBckw(  float angle1 , float angle2);
    void              rotateCurrentVisualLeftRight(float angle) ;
    void              adjustCurrentVisualScale(int component, float factor);

    // pt in window-coordinates
    void              moveCurrentObjXY(CPoint pt);
    // pt in window-coordinates
    void              moveCurrentObjXZ(CPoint pt);
    // Assume getCurrentObjType() in { SOTYPE_VISUALOBJECT, SOTYPE_LIGHTCONTROL, SOTYPE_ANIMATEDOBJECT }
    D3DXVECTOR3       getCurrentObjPos();
    // Assume getCurrentObjType() in { SOTYPE_VISUALOBJECT, SOTYPE_LIGHTCONTROL, SOTYPE_ANIMATEDOBJECT }
    void              setCurrentObjPos(   const D3DXVECTOR3 &pos);
    // Assume getCurrentVisual() != NULL (currentSceneObject.type in { SOTYPE_ANIMATEDOBJECT, SOTYPE_VISUALOBJECT }
    void              setCurrentVisualWorld(       const D3DXMATRIX &world);
    // return pointer to getCurrentObj->getWorld() if getCcurrentVisual() != NULL, else NULL
    const D3DXMATRIX *getCurrentVisualWorld() const;
    void              setCurrentVisualOrientation(const D3DXQUATERNION &q    );
    void              setCurrentVisualScale(      const D3DXVECTOR3    &scale);
    void walkWithCamera(       float  dist   , float  angle);
    void sidewalkWithCamera(   float  upDist , float rightDist);
    void moveCamera(           const D3DXVECTOR3 &dir, float dist);
    void rotateCameraUpDown(   float angle);
    void rotateCameraLeftRight(float angle);
    void selectPropertyDialog(PropertyDialog *dlg, D3EditorControl control) {
      m_currentPropertyDialog = dlg;
      setCurrentControl(control);
    }
    void unselectPropertyDialog() {
      m_currentPropertyDialog = NULL;
      setCurrentControl(CONTROL_IDLE);
    }
    inline D3SceneEditor &setFlags(BYTE flags) {
      m_stateFlags |= flags;
      return *this;
    }
    inline D3SceneEditor &clrFlags(BYTE flags) {
      m_stateFlags &= ~flags;
      return *this;
    }
    inline bool isSet(BYTE flags) const {
      return (m_stateFlags & flags) != 0;
    }
    void setMouseVisible(bool visible);

    inline bool isMouseVisible() const {
      return isSet(SE_MOUSEVISIBLE);
    }
    CMenu &loadMenu(CMenu &menu, int id);
    void showContextMenu(CMenu &menu, CPoint point);

    // set m_currentControl = CONTROL_IDLE, m_currentCamera = NULL, m_currentVisual = NULL
    void resetCurrentControl();
    void setCurrentControl(D3EditorControl control);

          D3LightControl *getCurrentLightControl();
    const D3LightControl *getCurrentLightControl() const;
    void setLightControlRenderEffect(bool enabled);
    void setSpecularEnable(          bool enabled);
    void OnSceneSaveParameters();
    void OnSceneLoadParameters();
    void OnControlObjMoveRotate();
    void OnControlObjPos();
    void OnControlObjScale();

    void setAllLightControlsVisible(bool visible);
    void addLight(D3DLIGHTTYPE type);
    void setSpotToPointAt(CPoint point);

    void OnMouseMoveCameraWalk(           UINT nFlags, CPoint pt);               // pt in window-coordinates
    void OnMouseMoveObjPos(               UINT nFlags, CPoint pt);               // pt in window-coordinates
    void OnMouseWheelObjPos(              UINT nFlags, short zDelta, CPoint pt); // pt in window-coordinates
    void OnMouseWheelObjScale(            UINT nFlags, short zDelta, CPoint pt); // pt in window-coordinates
    void OnMouseWheelAnimationSpeed(      UINT nFlags, short zDelta, CPoint pt); // pt in window-coordinates
    void OnMouseWheelCameraWalk(          UINT nFlags, short zDelta, CPoint pt); // pt in window-coordinates
    void OnMouseWheelCameraProjection(    UINT nFlags, short zDelta, CPoint pt); // pt in window-coordinates

    void OnMouseMoveLight(                UINT nFlags, CPoint pt);               // pt in window-coordinates
    void OnMouseMoveLightPoint(           UINT nFlags, CPoint pt);               // pt in window-coordinates
    void OnMouseMoveLightSpot(            UINT nFlags, CPoint pt);               // pt in window-coordinates
    void OnMouseWheelLight(               UINT nFlags, short zDelta, CPoint pt); // pt in window-coordinates
    void OnMouseWheelLightPoint(          UINT nFlags, short zDelta, CPoint pt); // pt in window-coordinates
    void OnMouseWheelLightDirectional(    UINT nFlags, short zDelta, CPoint pt); // pt in window-coordinates
    void OnMouseWheelLightSpot(           UINT nFlags, short zDelta, CPoint pt); // pt in window-coordinates
    void OnMouseWheelLightSpotAngle(      UINT nFlags, short zDelta, CPoint pt); // pt in window-coordinates

    // point in screen-coordinates
    void OnContextMenuBackground(  CPoint point);
    // point in screen-coordinates
    void OnContextMenuObj(         CPoint point);
    // point in screen-coordinates
    void OnContextMenuVisualObj(   CPoint point);
    // point in screen-coordinates
    void OnContextMenuLightControl(CPoint point);
public:
    void OnControlCameraWalk();
    void OnControlCameraProjection();
    void OnCameraResetPosition();
    void OnCameraResetOrientation();
    void OnCameraResetProjection();
    void OnCameraResetAll();
    void OnCameraSplit(bool vertical);
    void OnCameraRemove();
private:
    void SetRightHanded(bool rightHanded);
    void OnObjectStartAnimation();
    void OnObjectStartBckAnimation();
    void OnObjectStartAltAnimation();
    void OnObjectReverseAnimation();
    void OnObjectStopAnimation();
    void OnObjectControlSpeed();
    void OnObjectCreateCube();
    void OnObjectEditMaterial();
    void OnObjectResetPosition();
    void OnObjectResetScale();
    void OnObjectResetOrientation();
    void OnObjectResetAll();
    void OnObjectRemove();
    void OnObjectFillmodePoint();
    void OnObjectFillmodeWireframe();
    void OnObjectFillmodeSolid();
    void OnObjectShadingFlat();
    void OnObjectShadingGouraud();
    void OnObjectShadingPhong();
    void OnObjectSetCenterOfRotation();
    void OnObjectResetCenterOfRotation();
    void OnLightAdjustColors();
    void OnLightAdjustSpotAngles();
    void OnLightControlSpotAt();
    void OnLightControlHide();
    void OnLightAddDirectional();
    void OnLightAddPoint();
    void OnLightAddSpot();
    void OnLightRemove();
    void OnSceneEditAmbientLight();
    void OnCameraEditBackgroundColor();
    void setCoordinateSystemVisible(bool visible);
    void setLightEnabled(bool enabled);
    // point in window-coordinates
    void OnLButtonDown(  UINT nFlags, CPoint point);
    // point in window-coordinates
    void OnLButtonUp(    UINT nFlags, CPoint point);
    // point in window-coordinates
    void OnMouseMove(    UINT nFlags, CPoint point);
    void OnLButtonDblClk(UINT nFlags, CPoint point);
    // pt in window-coordinates
    BOOL OnMouseWheel(   UINT nFlags, short zDelta, CPoint pt);
    // point in screen-coordinates
    void OnContextMenu(  HWND pwnd, CPoint point);
    String stateFlagsToString() const;
    String getSelectedString() const;
#ifdef _DEBUG
    void checkInvariant(const TCHAR *method, int line) const;
#define CHECKINVARIANT() checkInvariant(__TFUNCTION__,__LINE__)
#else
#define CHECKINVARIANT()
#endif // _DEBUG

public:
    D3SceneEditor();
    ~D3SceneEditor();
    void init(D3SceneContainer *sceneContainer);
    void close();
    void handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue);
    inline bool isInitDone() const {
      return isSet(SE_INITDONE);
    }
    // flags is any combination of SE_*
    D3SceneEditor &setEnabled(bool enabled, BYTE flags = SE_ALL);
    inline bool isEnabled() const {
      return isSet(SE_ENABLED);
    }
    inline D3EditorControl      getCurrentControl() const {
      return m_currentControl;
    }
    inline D3Camera            *getSelectedCAM() const {
      return m_selectedCamera;
    }
    // return one of { SOTYPE_NULL, SOTYPE_VISUALOBJECT, SOTYPE_LIGHTCONTROL }
    SceneObjectType             getCurrentControlObjType() const;
    void                        setCurrentObj(D3SceneObjectVisual *obj);

    inline D3SceneObjectVisual *getCurrentObj() const {
      return m_currentObj;
    }
    inline bool                 hasObj() const {
      return getCurrentObj() != NULL;
    }
    SceneObjectType             getCurrentObjType() const;
    // Return NULL, if m_currentVisual->type not in {SOTYPE_VISUALOBJECT, SOTYPE_ANIMATEDOBJECT, }
    D3SceneObjectVisual        *getCurrentVisual() const;
    // return NULL, if m_currentVisual->type not SOTYPE_ANIMATEDOBJECT
    D3SceneObjectAnimatedMesh  *getCurrentAnimatedObj() const;
    inline bool hasCurrentObj() const {
      return getCurrentObj() != NULL;
    }
    inline bool hasCurrentVisual() const {
      return getCurrentVisual() != NULL;
    }
    inline bool hasCurrentAnimatedObj() const {
      return getCurrentAnimatedObj() != NULL;
    }
    inline const D3Ray &getPickedRay() const {
      return m_pickedRay;
    }

    // Assume hasCAM(). Point in window-coordinates (m_hwnd) of m_selectedCamera;
    D3SceneObjectVisual *getPickedVisual(const CPoint &point, long mask, D3Ray &ray, D3EditorPickedInfo &info) const;

    inline const D3EditorPickedInfo &getPickedInfo() const {
      return m_pickedInfo;
    }
    bool isCoordinateSystemVisible() const;
    BOOL PreTranslateMessage(MSG *pMsg);
    String toString() const;
};
