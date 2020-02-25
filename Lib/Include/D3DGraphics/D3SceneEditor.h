#pragma once

#include <TinyBitSet.h>
#include <MFCUtil/WinTools.h>
#include <MFCUtil/PropertyDialogMap.h>
#include "D3SceneObject.h"
#include "D3Ray.h"
#include "D3PickedInfo.h"
#include "D3SceneObjectPoint.h"
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
#define SE_LIGHTCONTROLS      0x20
#define SE_MOUSEVISIBLE       0x40
#define SE_RENDERALL          (SE_RENDER3D | SE_RENDERINFO)
#define SE_ALL                (SE_ENABLED | SE_PROPCHANGES | SE_RENDERALL | SE_LIGHTCONTROLS)

class D3Camera;
class D3SceneContainer;
class D3SceneObjectVisual;
class D3LightControl;
class D3SceneObjectAnimatedMesh;

class D3SceneEditor : public PropertyChangeListener {
private:
    D3SceneContainer     *m_sceneContainer;
    D3EditorControl       m_currentControl;
    D3Camera             *m_currentCamera;
    int                   m_currentCameraIndex;
    D3SceneObjectVisual  *m_currentObj, *m_coordinateSystem, *m_selectedCube;
    PropertyDialogMap     m_propertyDialogMap;
    PropertyDialog       *m_currentPropertyDialog;
    BYTE                  m_stateFlags;
    CPoint                m_lastMouse;
    D3SceneObjectPoint    m_centerOfRotation;
    D3DXVECTOR3           m_pickedPoint; // in world space
    D3Ray                 m_pickedRay;   // in world space
    D3PickedInfo          m_pickedInfo;
    String                m_paramFileName;

    HWND     getCurrentHwnd() const;
    D3Scene &getScene() const {
      return m_sceneContainer->getScene();
    }
    CameraSet getVisibleCameraSet() const;
    CameraSet getCurrentCameraSet() const;
    inline void render(BYTE flags, CameraSet cameraSet) {
      if(isSet(flags)) {
        m_sceneContainer->render(flags, cameraSet);
      }
    }
    inline void renderInfo() {
      render(SE_RENDERINFO, CameraSet());
    }
    inline void renderCurrent(BYTE flags) {
      render(flags, getCurrentCameraSet());
    }
    inline void renderVisible(BYTE flags) {
      render(flags, getVisibleCameraSet());
    }
    int               findCameraIndex(CPoint p) const;
    // if index >= 0, set m_currentCamera = scene.getCameraArray()[index], else = NULL, and set m_currentCameraIndex = index
    void              selectCamera(int index);
    void              rotateCurrentVisualFrwBckw(  float angle1 , float angle2);
    void              rotateCurrentVisualLeftRight(float angle) ;
    void              adjustCurrentVisualScale(int component, float factor);

    void              moveCurrentObjXY(CPoint pt);
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
    void selectPropertyDialog(PropertyDialog *dlg) {
      m_currentPropertyDialog = dlg;
    }
    void unselectPropertyDialog() {
      m_currentPropertyDialog = NULL;
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
    void OnSaveSceneParameters();
    void OnLoadSceneParameters();
    void OnControlObjMoveRotate();
    void OnSplitWindow(bool vertical);
    void OnControlObjPos();
    void OnControlObjScale();

    void setLightControlsVisible(bool visible);
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

    void OnContextMenuBackground(  CPoint point);
    void OnContextMenuObj(         CPoint point);
    void OnContextMenuVisualObj(   CPoint point);
    void OnContextMenuLightControl(CPoint point);
public:
    void OnControlCameraWalk();
    void OnControlCameraProjection();
    void OnCameraResetPosition();
    void OnCameraResetOrientation();
    void OnCameraResetProjection();
    void OnCameraResetAll();
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
    void OnObjectSetCenterOfRotation();
    void OnObjectResetCenterOfRotation();
    void OnLightAdjustColors();
    void OnLightAdjustSpotAngles();
    void OnLightControlSpotAt();
    void OnLightControlHide();
    void OnAddLightDirectional();
    void OnAddLightPoint();
    void OnAddLightSpot();
    void OnEditAmbientLight();
    void OnEditBackgroundColor();
    void OnFillmodePoint();
    void OnFillmodeWireframe();
    void OnFillmodeSolid();
    void OnShadingFlat();
    void OnShadingGouraud();
    void OnShadingPhong();
    void setCoordinateSystemVisible(bool visible);
    void setSelectedCubeVisible(    bool visible);
    void setLightEnabled(bool enabled);
    void OnLightRemove();
    // point in window-coordinates
    void OnLButtonDown(  UINT nFlags, CPoint point);
    // point in window-coordinates
    void OnLButtonUp(    UINT nFlags, CPoint point);
    // point in window-coordinates
    void OnMouseMove(    UINT nFlags, CPoint point);
    void OnLButtonDblClk(UINT nFlags, CPoint point);
    // pt in window-coordinates
    BOOL OnMouseWheel(   UINT nFlags, short zDelta, CPoint pt);
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
    inline D3Camera            *getCurrentCamera() const {
      return m_currentCamera;
    }
    // return one of { SOTYPE_NULL, SOTYPE_VISUALOBJECT, SOTYPE_LIGHTCONTROL }
    SceneObjectType             getCurrentControlObjType() const;
    void                        setCurrentObj(D3SceneObjectVisual *obj);

    inline D3SceneObjectVisual *getCurrentObj() const {
      return m_currentObj;
    }
    SceneObjectType             getCurrentObjType() const;
    // Return NULL, if m_currentVisual->type not in {SOTYPE_VISUALOBJECT, SOTYPE_ANIMATEDOBJECT, }
    D3SceneObjectVisual        *getCurrentVisual() const;
    // return NULL, if m_currentVisual->type not SOTYPE_ANIMATEDOBJECT
    D3SceneObjectAnimatedMesh  *getCurrentAnimatedObj() const;
    inline const D3DXVECTOR3   &getPickedPoint() const {
      return m_pickedPoint;
    }
    inline bool hasCurrentObj() const {
      return getCurrentObj() != NULL;
    }
    inline bool hasCurrentVisual() const {
      return getCurrentVisual() != NULL;
    }
    inline bool hasCurrentAnimatedObj() const {
      return getCurrentAnimatedObj() != NULL;
    }
    // p in screen-coordinates
    inline const D3Ray &getPickedRay() const {
      return m_pickedRay;
    }
    inline const D3PickedInfo &getPickedInfo() const {
      return m_pickedInfo;
    }
    bool isCoordinateSystemVisible() const;
    bool isSelectedCubeVisible()     const;
    BOOL PreTranslateMessage(MSG *pMsg);
    String toString() const;
};
