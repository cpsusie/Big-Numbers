#pragma once

#include <MFCUtil/LayoutManager.h>
#include "D3LightControl.h"
#include "D3CoordinateSystem.h"
#include "MemoryLogThread.h"
#include "PropertyDlgThread.h"

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

class CD3FunctionPlotterDlg : public CDialog, public PropertyChangeListener {
private:
    HACCEL                     m_accelTable;
    SimpleLayoutManager        m_layoutManager;
    MemoryLogThread            m_memlogThread;

    CPropertyDlgThread         *m_lightDlgThread, *m_materialDlgThread;
    D3Scene                     m_scene;
    Function2DSurfaceParameters m_function2DSurfaceParam;
    IsoSurfaceParameters        m_isoSurfaceParam;
    CurrentObjectControl        m_currentControl;
    CPoint                      m_lastMouse;
    bool                        m_propertyChangeHandlerEnabled;
    bool                        m_infoVisible;
    D3SceneObject              *m_calculatedObject;
    D3SceneObject              *m_selectedSceneObject;
    PropertyContainer          *m_currentEditor;
    bool                        m_mouseVisible;
    D3DXVECTOR3                 m_focusPoint;
    D3DXVECTOR3                 m_pickedPoint;
    D3Ray                       m_pickedRay;
    D3CoordinateSystem         *m_coordinateSystem;


    inline bool hasFocusPoint() const {
      return m_currentControl == CONTROL_OBJECT_POS_KEEPFOCUS
          || m_currentControl == CONTROL_OBJECT_SCALE_KEEPFOCUS
          || m_currentControl == CONTROL_CAMERA_KEEPFOCUS
          ;
    }
    void setFillMode( D3DFILLMODE  fillMode );
    void setShadeMode(D3DSHADEMODE shadeMode);
    void setLightParameters(   unsigned int index, const LIGHT &param);
    CWnd *get3DPanel() const {
      return GetDlgItem(IDC_STATIC_3DPANEL);
    }
    CWnd *getInfoPanel() {
      return GetDlgItem(IDC_STATIC_INFOPANEL);
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

    void setInfoVisible(bool visible);
    void showInfo(const TCHAR *format, ...);
    void show3DInfo();
    void setMouseVisible(bool visible);
    CMenu &loadMenu(CMenu &menu, int id);
    void showContextMenu(CMenu &menu, CPoint point);
    void createInitialObject();
    CPoint get3DPanelPoint(CPoint point, bool screenRelative) const;
    D3SceneObject *createRotatedProfile();
    void createSaddle();

    void setCurrentControl(CurrentObjectControl control);
    void setSelectedObject(  D3SceneObject *obj);
    void setCalculatedObject(D3SceneObject *obj, PersistentParameter *param = NULL);
    void setCalculatedObject(Function2DSurfaceParameters *param);
    void setCalculatedObject(IsoSurfaceParameters        *param);
    bool moveLastMouseToFocusPoint();

    String getSelectedString() const;


    D3SceneObject        *getSelectedVisualObject();
    D3AnimatedSurface    *getSelectedAnimatedobject();
          D3LightControl *getSelectedLightControl();
    const D3LightControl *getSelectedLightControl() const;
    void setLightControlsVisible(bool visible);
    void addLightControl(   int lightIndex, bool redraw=true);
	void addLight(D3DLIGHTTYPE type);
    void setSpotToPointAt(CPoint point);
    void removeLightControl(int lightIndex, bool redraw=true);
    void removeAllLightControls();

    void onMouseMoveCameraWalk(           UINT nFlags, CPoint pt);
    void onMouseMoveObjectPos(            UINT nFlags, CPoint pt);
    void onMouseWheelObjectPos(           UINT nFlags, short zDelta, CPoint pt);
    void onMouseWheelObjectScale(         UINT nFlags, short zDelta, CPoint pt);
    void onMouseWheelObjectPosKeepFocus(  UINT nFlags, short zDelta, CPoint pt);
    void onMouseWheelObjectScaleKeepFocus(UINT nFlags, short zDelta, CPoint pt);
    void onMouseWheelAnimationSpeed(      UINT nFlags, short zDelta, CPoint pt);
    void onMouseWheelCameraWalk(          UINT nFlags, short zDelta, CPoint pt);
    void onMouseWheelCameraKeepFocus(     UINT nFlags, short zDelta, CPoint pt);

    void onMouseMoveLight(                UINT nFlags, CPoint pt);
    void onMouseMoveLightPoint(           D3LightControlPoint       &ctrl, UINT nFlags, CPoint pt);
    void onMouseMoveLightSpot(            D3LightControlSpot        &ctrl, UINT nFlags, CPoint pt);
    void onMouseWheelLight(               UINT nFlags, short zDelta, CPoint pt);
    void onMouseWheelLightPoint(          D3LightControlPoint       &ctrl, UINT nFlags, short zDelta, CPoint pt);
    void onMouseWheelLightDirectional(    D3LightControlDirectional &ctrl, UINT nFlags, short zDelta, CPoint pt);
    void onMouseWheelLightSpot(           D3LightControlSpot        &ctrl, UINT nFlags, short zDelta, CPoint pt);
    void onMouseWheelLightSpotAngle(      UINT nFlags, short zDelta, CPoint pt);

    void onContextMenuBackground(  CPoint point);
    void onContextMenuSceneObject( CPoint point);
    void onContextMenuVisualObject(CPoint point);
    void onContextMenuLightControl(CPoint point);
public:
    CD3FunctionPlotterDlg(CWnd* pParent = NULL);
    void handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue);
    inline D3Scene &getScene() {
      return m_scene;
    }
    enum { IDD = IDR_MAINFRAME };

    public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);

protected:
    HICON m_hIcon;

    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    virtual void OnCancel();
    virtual void OnOK();
    afx_msg void OnClose();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(  UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(  UINT nFlags, CPoint point);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	  afx_msg void OnFileSaveState();
	  afx_msg void OnFileLoadState();
    afx_msg void OnFilePlotFunction();
    afx_msg void OnFileIsoSurface();
	  afx_msg void OnFileProfileSurface();
    afx_msg void OnFileRead3DPointsFromFile();
    afx_msg void OnFileExit();
	  afx_msg void OnFileNexttry();
    afx_msg void OnViewFillmodePoint();
    afx_msg void OnViewFillmodeWireframe();
    afx_msg void OnViewFillmodeSolid();
    afx_msg void OnViewShadingFlat();
    afx_msg void OnViewShadingGouraud();
    afx_msg void OnViewShadingPhong();
	  afx_msg void OnViewSpecular();
	  afx_msg void OnViewShowCoordinateSystem();
    afx_msg void OnViewShow3dinfo();
    afx_msg void OnControlCameraWalk();
	  afx_msg void OnControlMoveRotateObject();
	  afx_msg void OnShowLightControls();
	  afx_msg void OnHideLightControls();
	  afx_msg void OnAddLightDirectional();
	  afx_msg void OnAddLightPoint();
	  afx_msg void OnAddLightSpot();
	  afx_msg void OnEditAmbientLight();
    afx_msg void OnEditBackgroundColor();
    afx_msg void OnResetPositions();
    afx_msg void OnControlObjectPos();
    afx_msg void OnControlObjectScale();
	  afx_msg void OnControlObjectKeepFocus();
	  afx_msg void OnControlObjectScaleKeepFocus();
    afx_msg void OnControlCameraKeepFocus();
	  afx_msg void OnObjectStartAnimation();
	  afx_msg void OnObjectStartBckAnimation();
	  afx_msg void OnObjectStartAltAnimation();
	  afx_msg void OnObjectReverseAnimation();
	  afx_msg void OnObjectStopAnimation();
	  afx_msg void OnObjectControlSpeed();
	  afx_msg void OnObjectEditFunction();
    afx_msg void OnObjectEditMaterial();
	  afx_msg void OnObjectResetScale();
	  afx_msg void OnObjectRemove();
	  afx_msg void OnObjectInfo();
	  afx_msg void OnLightEnable();
	  afx_msg void OnLightDisable();
	  afx_msg void OnLightAdjustColors();
	  afx_msg void OnLightAdjustAngles();
	  afx_msg void OnLightControlSpotAt();
	  afx_msg void OnLightControlHide();
	  afx_msg void OnLightRemove();
    afx_msg LRESULT OnMsgRender(WPARAM wp, LPARAM lp);
    DECLARE_MESSAGE_MAP()
};

