#pragma once

#include <MFCUtil/WinTools.h>
#include <D3DGraphics/D3LightControl.h>
#include <D3DGraphics/PropertyDlgThread.h>

typedef enum {
  CONTROL_IDLE
 ,CONTROL_CAMERA_WALK
 ,CONTROL_OBJECT_POS
 ,CONTROL_OBJECT_SCALE
 ,CONTROL_OBJECT_POS_KEEPFOCUS
 ,CONTROL_OBJECT_SCALE_KEEPFOCUS
 ,CONTROL_CAMERA_KEEPFOCUS
 ,CONTROL_LIGHT
 ,CONTROL_SPOTLIGHTPOINT
 ,CONTROL_SPOTANGLES
 ,CONTROL_ANIMATION_SPEED
} CurrentObjectControl;

#define RENDER_3D   0x1
#define RENDER_INFO 0x2
#define RENDER_ALL  (RENDER_3D|RENDER_INFO)

class D3SceneContainer {
public:
  virtual D3Scene &getScene()               = 0;
  virtual CWnd    *getMessageWindow()       = 0;
  virtual CWnd    *get3DWindow()            = 0;
  virtual void     render(BYTE renderFlags) = 0; // any combination of RENDER_3D,RENDER_INFO
};

class SceneEditor : public PropertyChangeListener {
private:
    CPropertyDlgThread         *m_lightDlgThread, *m_materialDlgThread;
    D3SceneContainer           *m_sceneContainer;
    CurrentObjectControl        m_currentControl;
    CPoint                      m_lastMouse;
    bool                        m_editorEnabled;
    D3SceneObject              *m_selectedSceneObject;
    PropertyContainer          *m_currentEditor;
    bool                        m_mouseVisible;
    D3DXVECTOR3                 m_focusPoint;
    D3DXVECTOR3                 m_pickedPoint;
    D3Ray                       m_pickedRay;
    String                      m_paramFileName;

    inline CWnd *getMessageWindow() const {
      return m_sceneContainer->getMessageWindow();
    }
    inline CWnd *get3DWindow() const {
      return m_sceneContainer->get3DWindow();
    }
    D3Scene &getScene() const {
      return m_sceneContainer->getScene();
    }
    inline CPoint screenPToMessageP(CPoint p) const {
      getMessageWindow()->ScreenToClient(&p);
      return p;
    }
    inline CPoint screenPTo3DP(CPoint p) const {
      get3DWindow()->ScreenToClient(&p);
      return p;
    }
    inline void render(BYTE flags) {
      m_sceneContainer->render(flags);
    }

    inline bool hasFocusPoint() const {
      return m_currentControl == CONTROL_OBJECT_POS_KEEPFOCUS
          || m_currentControl == CONTROL_OBJECT_SCALE_KEEPFOCUS
          || m_currentControl == CONTROL_CAMERA_KEEPFOCUS
          ;
    }
    void rotateObjectFrwBckw(  double angle1 , double angle2);
    void rotateObjectLeftRight(double angle) ;
    void adjustScale(          int component, double factor);

    void moveSceneObjectXY(CPoint pt, SceneObjectType type, int lightIndex = -1);
    void moveSceneObjectXZ(CPoint pt, SceneObjectType type, int lightIndex = -1);

    D3DXVECTOR3 getSceneObjectPos(SceneObjectType type, int lightIndex);
    void        setSceneObjectPos(const D3DXVECTOR3 &pos, SceneObjectType type, int lightIndex);

    void walkWithCamera(       double dist   , double angle);
    void sidewalkWithCamera(   double upDist , double rightDist);
    void rotateCameraUpDown(   double angle) ;
    void rotateCameraLeftRight(double angle);

    void setMouseVisible(bool visible);
    CMenu &loadMenu(CMenu &menu, int id);
    void showContextMenu(CMenu &menu, CPoint point);

    void setCurrentControl(CurrentObjectControl control);
    void setSelectedObject(  D3SceneObject *obj);
    bool moveLastMouseToFocusPoint();

    String getSelectedString() const;

    D3SceneObject        *getSelectedVisualObject();
    D3AnimatedSurface    *getSelectedAnimatedobject();
          D3LightControl *getSelectedLightControl();
    const D3LightControl *getSelectedLightControl() const;

    void OnSaveSceneParameters();
    void OnLoadSceneParameters();
    void OnControlObjectMoveRotate();
    void OnControlObjectPos();
    void OnControlObjectScale();
    void OnControlObjectKeepFocus();
    void OnControlObjectScaleKeepFocus();

    void setLightControlsVisible(bool visible);
    void addLight(D3DLIGHTTYPE type);
    void setSpotToPointAt(CPoint point);


    void OnMouseMoveCameraWalk(           UINT nFlags, CPoint pt);
    void OnMouseMoveObjectPos(            UINT nFlags, CPoint pt);
    void OnMouseWheelObjectPos(           UINT nFlags, short zDelta, CPoint pt);
    void OnMouseWheelObjectScale(         UINT nFlags, short zDelta, CPoint pt);
    void OnMouseWheelObjectPosKeepFocus(  UINT nFlags, short zDelta, CPoint pt);
    void OnMouseWheelObjectScaleKeepFocus(UINT nFlags, short zDelta, CPoint pt);
    void OnMouseWheelAnimationSpeed(      UINT nFlags, short zDelta, CPoint pt);
    void OnMouseWheelCameraWalk(          UINT nFlags, short zDelta, CPoint pt);
    void OnMouseWheelCameraKeepFocus(     UINT nFlags, short zDelta, CPoint pt);

    void OnMouseMoveLight(                UINT nFlags, CPoint pt);
    void OnMouseMoveLightPoint(           D3LightControlPoint       &ctrl, UINT nFlags, CPoint pt);
    void OnMouseMoveLightSpot(            D3LightControlSpot        &ctrl, UINT nFlags, CPoint pt);
    void OnMouseWheelLight(               UINT nFlags, short zDelta, CPoint pt);
    void OnMouseWheelLightPoint(          D3LightControlPoint       &ctrl, UINT nFlags, short zDelta, CPoint pt);
    void OnMouseWheelLightDirectional(    D3LightControlDirectional &ctrl, UINT nFlags, short zDelta, CPoint pt);
    void OnMouseWheelLightSpot(           D3LightControlSpot        &ctrl, UINT nFlags, short zDelta, CPoint pt);
    void OnMouseWheelLightSpotAngle(      UINT nFlags, short zDelta, CPoint pt);

    void OnContextMenuBackground(  CPoint point);
    void OnContextMenuSceneObject( CPoint point);
    void OnContextMenuVisualObject(CPoint point);
    void OnContextMenuLightControl(CPoint point);
    void OnControlCameraWalk();
    void OnControlCameraKeepFocus();
    void OnObjectStartAnimation();
    void OnObjectStartBckAnimation();
    void OnObjectStartAltAnimation();
    void OnObjectReverseAnimation();
    void OnObjectStopAnimation();
    void OnObjectControlSpeed();
    void OnObjectEditMaterial();
    void OnObjectResetScale();
    void OnObjectRemove();
    void OnLightAdjustColors();
    void OnLightAdjustAngles();
    void OnLightControlSpotAt();
    void OnLightControlHide();
    void OnAddLightDirectional();
    void OnAddLightPoint();
    void OnAddLightSpot();
    void OnEditAmbientLight();
    void OnEditBackgroundColor();
    void setLightEnabled(bool enabled);
    void OnLightRemove();
    void OnLButtonDown(  UINT nFlags, CPoint point);
    void OnLButtonUp(    UINT nFlags, CPoint point);
    void OnMouseMove(    UINT nFlags, CPoint point);
    void OnLButtonDblClk(UINT nFlags, CPoint point);
    BOOL OnMouseWheel(   UINT nFlags, short zDelta, CPoint pt);
    void OnContextMenu(  CWnd *pwnd, CPoint point);
public:
    SceneEditor();
    ~SceneEditor();
    void init(D3SceneContainer *sceneContainer);
    void close();
    void handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue);
    void setEnabled(bool enabled);
    inline bool isEnabled() const {
      return m_editorEnabled;
    }
    BOOL PreTranslateMessage(MSG *pMsg);
    String toString() const;
};

