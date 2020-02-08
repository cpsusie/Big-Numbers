#pragma once

#include <TinyBitSet.h>
#include <MFCUtil/WinTools.h>
#include <MFCUtil/PropertyDialogMap.h>
#include <D3DGraphics/Light.h>
#include <D3DGraphics/Material.h>
#include <D3DGraphics/D3LightControl.h>

typedef enum {
  CONTROL_IDLE
 ,CONTROL_CAMERA_WALK
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
} CurrentObjectControl;


#define SE_INITDONE           0x01
#define SE_ENABLED            0x02
#define SE_PROPCHANGES        0x04
#define SE_RENDER3D           0x08
#define SE_RENDERINFO         0x10
#define SE_LIGHTCONTROLS      0x20
#define SE_MOUSEVISIBLE       0x40
#define SE_RENDERALL          (SE_RENDER3D | SE_RENDERINFO)
#define SE_ALL                (SE_ENABLED | SE_PROPCHANGES | SE_RENDERALL | SE_LIGHTCONTROLS)

class D3SceneContainer {
public:
  virtual D3Scene &getScene()               = 0;
  virtual CWnd    *getMessageWindow()       = 0;
  virtual CWnd    *get3DWindow()            = 0;
  // renderFlags is any combination of RENDER_3D,RENDER_INFO
  virtual void     render(BYTE renderFlags) = 0;
  virtual void     modifyContextMenu(CMenu &menu) {
  }
};

class CenterOfRotation {
public:
  D3SceneObject *m_obj; // which object does m_pos refer to
  D3DXVECTOR3    m_pos; // relative to mesh (0,0,0)
  CenterOfRotation() {
    reset();
  }
  inline void reset() {
    set(NULL,D3DXORIGIN);
  }
  inline void set(D3SceneObject *obj, const D3DXVECTOR3 &pos) {
    m_obj = obj;
    m_pos = pos;
  }
};

class D3SceneEditor : public PropertyChangeListener {
private:
    D3SceneContainer                 *m_sceneContainer;
    CurrentObjectControl              m_currentControl;
    D3SceneObject                    *m_currentObj, *m_coordinateSystem, *m_selectedCube;
    PropertyDialogMap                 m_propertyDialogMap;
    PropertyContainer                *m_currentEditor;
    BYTE                              m_stateFlags;
    CPoint                            m_lastMouse;
    CenterOfRotation                  m_centerOfRotation;
    D3DXVECTOR3                       m_pickedPoint; // in world space
    D3Ray                             m_pickedRay;   // in world space
    D3PickedInfo                      m_pickedInfo;
    String                            m_paramFileName;

    inline CWnd *getMessageWindow() const {
      return m_sceneContainer->getMessageWindow();
    }
    inline CWnd *get3DWindow() const {
      return m_sceneContainer->get3DWindow();
    }
    D3Scene &getScene() const {
      return m_sceneContainer->getScene();
    }
    inline void render(BYTE flags) {
      if(isSet(flags)) {
        m_sceneContainer->render(flags);
      }
    }

    void rotateCurrentVisualFrwBckw(  float angle1 , float angle2);
    void rotateCurrentVisualLeftRight(float angle) ;
    void adjustCurrentVisualScale(int component, float factor);

    void moveCurrentObjXY(CPoint pt);
    void moveCurrentObjXZ(CPoint pt);
    // Assume getCurrentObjType() in { SOTYPE_VISUALOBJECT, SOTYPE_LIGHTCONTROL, SOTYPE_ANIMATEDOBJECT }
    D3DXVECTOR3     getCurrentObjPos();
    // Assume getCurrentObjType() in { SOTYPE_VISUALOBJECT, SOTYPE_LIGHTCONTROL, SOTYPE_ANIMATEDOBJECT }
    void            setCurrentObjPos(   const D3DXVECTOR3 &pos);
    // Assume getCurrentVisual() != NULL (currentSceneObject.type in { SOTYPE_ANIMATEDOBJECT, SOTYPE_VISUALOBJECT }
    void            setCurrentVisualPDUS(       const D3PosDirUpScale &pdus);
    // return pointer to getCurrentVisual->getPDUS() if getCcurrentVisual() != NULL, else NULL
    const D3PosDirUpScale *getCurrentVisualPDUS() const;
    void            setCurrentVisualOrientation(const D3DXVECTOR3 &dir, const D3DXVECTOR3 &up);
    void            setCurrentVisualScale(      const D3DXVECTOR3 &pos);
    void            resetCenterOfRotation();
    void            setCenterOfRotation();
    inline D3DXVECTOR3  getCenterOfRotation() const {
      return (getCurrentVisual() == m_centerOfRotation.m_obj) ? m_centerOfRotation.m_pos : D3DXORIGIN;
    }
    void walkWithCamera(       float  dist   , float  angle);
    void sidewalkWithCamera(   float  upDist , float rightDist);
    void moveCamera(           const D3DXVECTOR3 &dir, float dist);
    void rotateCameraUpDown(   float angle);
    void rotateCameraLeftRight(float angle);

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

    // set m_currentControl = CONTROL_IDLE and m_currentObj = NULL
    void resetCurrentControl();
    void setCurrentControl(CurrentObjectControl control);

          D3LightControl *getCurrentLightControl();
    const D3LightControl *getCurrentLightControl() const;
    void setLightControlRenderEffect(bool enabled);
    void setSpecularEnable(          bool enabled);
    void OnSaveSceneParameters();
    void OnLoadSceneParameters();
    void OnControlObjMoveRotate();
    void OnControlObjPos();
    void OnControlObjScale();

    void setLightControlsVisible(bool visible);
    void addLight(D3DLIGHTTYPE type);
    void setSpotToPointAt(CPoint point);
    void OnMouseMoveCameraWalk(           UINT nFlags, CPoint pt);
    void OnMouseMoveObjPos(               UINT nFlags, CPoint pt);
    void OnMouseWheelObjPos(              UINT nFlags, short zDelta, CPoint pt);
    void OnMouseWheelObjScale(            UINT nFlags, short zDelta, CPoint pt);
    void OnMouseWheelAnimationSpeed(      UINT nFlags, short zDelta, CPoint pt);
    void OnMouseWheelCameraWalk(          UINT nFlags, short zDelta, CPoint pt);

    void OnMouseMoveLight(                UINT nFlags, CPoint pt);
    void OnMouseMoveLightPoint(           UINT nFlags, CPoint pt);
    void OnMouseMoveLightSpot(            UINT nFlags, CPoint pt);
    void OnMouseWheelLight(               UINT nFlags, short zDelta, CPoint pt);
    void OnMouseWheelLightPoint(          UINT nFlags, short zDelta, CPoint pt);
    void OnMouseWheelLightDirectional(    UINT nFlags, short zDelta, CPoint pt);
    void OnMouseWheelLightSpot(           UINT nFlags, short zDelta, CPoint pt);
    void OnMouseWheelLightSpotAngle(      UINT nFlags, short zDelta, CPoint pt);

    void OnContextMenuBackground(  CPoint point);
    void OnContextMenuObj(         CPoint point);
    void OnContextMenuVisualObj(   CPoint point);
    void OnContextMenuLightControl(CPoint point);
public:
    void OnControlCameraWalk();
    void OnResetCamera();
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
    void OnObjectRemove();
    void OnObjectSetCenterOfRotation();
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
    void OnLButtonDown(  UINT nFlags, CPoint point);
    void OnLButtonUp(    UINT nFlags, CPoint point);
    void OnMouseMove(    UINT nFlags, CPoint point);
    void OnLButtonDblClk(UINT nFlags, CPoint point);
    BOOL OnMouseWheel(   UINT nFlags, short zDelta, CPoint pt);
    void OnContextMenu(  CWnd *pwnd, CPoint point);
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
    inline CurrentObjectControl getCurrentControl() const {
      return m_currentControl;
    }
    // return one of { SOTYPE_NULL, SOTYPE_VISUALOBJECT, SOTYPE_LIGHTCONTROL }
    SceneObjectType             getCurrentControlObjType() const;
    void                        setCurrentObj(D3SceneObject *obj);

    inline D3SceneObject       *getCurrentObj() const {
      return m_currentObj;
    }
    inline SceneObjectType      getCurrentObjType() const {
      return m_currentObj ? m_currentObj->getType() : SOTYPE_NULL;
    }
    // Return NULL, if m_currentObj->type not in {SOTYPE_VISUALOBJECT, SOTYPE_ANIMATEDOBJECT, }
    D3SceneObject              *getCurrentVisual() const;
    // return NULL, if m_currentObj->type not SOTYPE_ANIMATEDOBJECT
    D3AnimatedSurface          *getCurrentAnimatedObj() const;
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
    inline CPoint screenPTo3DP(CPoint p) const {
      get3DWindow()->ScreenToClient(&p);
      return p;
    }
    // p in screen-coordinates
    inline bool ptIn3DWindow(const CPoint &p) const {
      CRect r;
      GetWindowRect(*get3DWindow(), &r);
      return r.PtInRect(p);
    }
    inline const D3Ray &getPickedRay() const {
      return m_pickedRay;
    }
    inline const D3PickedInfo &getPickedInfo() const {
      return m_pickedInfo;
    }
    inline bool isCoordinateSystemVisible() const {
      return m_coordinateSystem && m_coordinateSystem->isVisible();
    }
    inline bool isSelectedCubeVisible() const {
      return m_selectedCube && m_selectedCube->isVisible();
    }
    BOOL PreTranslateMessage(MSG *pMsg);
    String toString() const;
};
