#pragma once

#include <MFCUtil/LayoutManager.h>
#include <D3DGraphics/Function2DSurface.h>
#include <D3DGraphics/ParametricSurface.h>
#include <D3DGraphics/IsoSurface.h>
#include "D3DGraphics/D3CoordinateSystem.h"
#include "D3DGraphics/D3SceneEditor.h"

#define INFO_EDIT  0x01
#define INFO_MEM   0x02

#ifdef DEBUG_POLYGONIZER
#define INFO_DEBUG 0x04
#include "Debugger.h"
#else
#define INFO_DEBUG 0
#endif // DEBUG_POLYGONIZER

#define INFO_ALL  (INFO_EDIT|INFO_MEM|INFO_DEBUG)

class CD3FunctionPlotterDlg : public CDialog
                            , public D3SceneContainer
#ifdef DEBUG_POLYGONIZER
                            , public PropertyChangeListener
#endif // DEBUG_POLYGONIZER
{
private:
    HICON                       m_hIcon;
    HACCEL                      m_accelTable;
    SimpleLayoutManager         m_layoutManager;
    D3Scene                     m_scene;
    D3SceneEditor               m_editor;
    String                      m_editorInfo, m_memoryInfo, m_debugInfo;
    bool                        m_timerRunning;
#ifdef DEBUG_POLYGONIZER
    Debugger                   *m_debugger;
    float                       m_currentCamDistance;
    bool                        m_hasCubeCenter;
    D3DXVECTOR3                 m_cubeCenter;
    BYTE                        m_cubeLevel;
    int                         m_debugLightIndex;
    inline void createDebugLight() {
      m_debugLightIndex = m_scene.addLight(m_scene.getDefaultLight());
    }
    inline void destroyDebugLight() {
      if(hasDebugLight()) {
        m_scene.removeLight(m_debugLightIndex);
        m_debugLightIndex = -1;
      }
    }
    inline bool hasDebugLight() const {
      return (m_debugLightIndex >= 0) && m_scene.isLightDefined(m_debugLightIndex);
    }
    void adjustDebugLightDir();
#endif // DEBUG_POLYGONIZER
    Function2DSurfaceParameters m_function2DSurfaceParam;
    ParametricSurfaceParameters m_parametricSurfaceParam;
    IsoSurfaceParameters        m_isoSurfaceParam;
    bool                        m_infoVisible;
    D3CoordinateSystem         *m_coordinateSystem;

    CWnd *getInfoPanel() {
      return GetDlgItem(IDC_STATIC_INFOPANEL);
    }
    void startTimer();
    void stopTimer();
    void setInfoVisible(bool visible);
    void updateEditorInfo();
    void updateMemoryInfo();
    void updateDebugInfo();
    void showInfo(_In_z_ _Printf_format_string_ TCHAR const * const format, ...);
    // flags any combination of INFO_MEM, INFO_EDIT
    void show3DInfo(BYTE flags);

    void createInitialObject();
    CPoint get3DPanelPoint(CPoint point, bool screenRelative) const;
    D3SceneObject *createRotatedProfile();
    void createSaddle();
    void deleteCalculatedObject();
    void setCalculatedObject(D3SceneObject *obj, PersistentData *param = NULL);
    void setCalculatedObject(Function2DSurfaceParameters &param);
    void setCalculatedObject(ParametricSurfaceParameters &param);
    void setCalculatedObject(IsoSurfaceParameters        &param);
    D3SceneObject *getCalculatedObject() const;

  void startDebugging();
  void stopDebugging();
  void ajourDebuggerMenu();

#ifdef DEBUG_POLYGONIZER
  void killDebugger(bool showCreateSurface);
  void asyncKillDebugger();
  inline bool hasDebugger() const {
    return m_debugger != NULL;
  }
  inline bool isDebuggerPaused() const {
    return hasDebugger() && (m_debugger->getState() == DEBUGGER_PAUSED);
  }
  void handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue);

  inline String getDebuggerStateName() const {
    return hasDebugger() ? m_debugger->getStateName() : _T("No debugger");
  }
#endif // DEBUG_POLYGONIZER

public:
    CD3FunctionPlotterDlg(CWnd *pParent = NULL);
    D3Scene &getScene() {
      return m_scene;
    }
    CWnd    *getMessageWindow() {
      return this;
    }
    CWnd    *get3DWindow() {
      return GetDlgItem(IDC_STATIC_3DPANEL);
    }
    void render(BYTE renderFlags) {
      SendMessage(ID_MSG_RENDER, renderFlags, 0);
    }
    void modifyContextMenu(CMenu &menu) {
      appendMenuItem(menu, _T("Add box"), ID_ADDBOXOBJECT);
    }
    bool isBreakOnNextLevelChecked() const;
    bool isAutoFocusCurrentCubeChecked() const;

    const Function2DSurfaceParameters &get2DSurfaceParameters() const {
      return m_function2DSurfaceParam;
    }
    const ParametricSurfaceParameters &getParametricSurfaceParameters() const {
      return m_parametricSurfaceParam;
    }
    const IsoSurfaceParameters        &getIsoSurfaceParameters() const {
      return m_isoSurfaceParam;
    }
    enum { IDD = IDR_MAINFRAME };

protected:
    virtual BOOL PreTranslateMessage(MSG *pMsg);
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    virtual void OnCancel();
    virtual void OnOK();
    afx_msg void OnClose();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnFileSaveState();
    afx_msg void OnFileLoadState();
    afx_msg void OnFileFunctionSurface();
    afx_msg void OnFileParametricSurface();
    afx_msg void OnFileIsoSurface();
    afx_msg void OnFileProfileSurface();
    afx_msg void OnFileRead3DPointsFromFile();
    afx_msg void OnFileReadObjFile();
    afx_msg void OnFileExit();
    afx_msg void OnViewShow3dinfo();
    afx_msg void OnDebugGo();
    afx_msg void OnDebugSinglestep();
    afx_msg void OnDebugStepCube();
    afx_msg void OnDebugBreakOnNextLevel();
    afx_msg void OnDebugStopDebugging();
    afx_msg void OnDebugAutoFocusCurrentCube();
    afx_msg void OnDebugAdjustCam45Up();
    afx_msg void OnDebugAdjustCam45Down();
    afx_msg void OnDebugAdjustCam45Left();
    afx_msg void OnDebugAdjustCam45Right();
    afx_msg void OnResetPositions();
    afx_msg void OnObjectEditFunction();
    afx_msg void OnAddBoxObject();
    afx_msg LRESULT OnMsgRender(              WPARAM wp, LPARAM lp);
    afx_msg LRESULT OnMsgDebuggerStateChanged(WPARAM wp, LPARAM lp);
    afx_msg LRESULT OnMsgKillDebugger(        WPARAM wp, LPARAM lp);

    DECLARE_MESSAGE_MAP()
};
