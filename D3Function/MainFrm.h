#pragma once

#include <D3DGraphics/Function2DSurface.h>
#include <D3DGraphics/ParametricSurface.h>
#include <D3DGraphics/IsoSurface.h>
#include "D3DGraphics/D3SceneObjectCoordinateSystem.h"
#include "D3DGraphics/D3SceneEditor.h"
#include "D3FunctionDoc.h"
#include "D3SceneView.h"
#include "InfoView.h"

class StatusBarPaneInfo {
public:
  int  m_index;
  UINT m_id;
  UINT m_style;
  int  m_width;
};

class SplitViewSplitter;

class CD3FunctionSplitterWnd : public CSplitterWnd {
private:
  bool m_splitPointMoved;
public:
  CD3FunctionSplitterWnd() : m_splitPointMoved(false) {
  }
  void RecalcLayout();
  void OnInvertTracker(const CRect& rect);
  int           getPanelCount() const {
    return GetRowCount();
  }
};

#define INFO_EDIT  0x01
#define INFO_MEM   0x02

#ifdef DEBUG_POLYGONIZER
#define INFO_DEBUG 0x04
#include "Debugger.h"
#else
#define INFO_DEBUG 0
#endif // DEBUG_POLYGONIZER

#define INFO_ALL  (INFO_EDIT|INFO_MEM|INFO_DEBUG)

class CMainFrame : public CFrameWnd
                 , public OptionsUpdater
                 , public D3SceneContainer
#ifdef DEBUG_POLYGONIZER
                 , public PropertyChangeListener
#endif // DEBUG_POLYGONIZER
{
private:
  friend class C3DSceneView;
  bool                            m_statusPanesVisible;
  CStatusBar                      m_wndStatusBar;
  CToolBar                        m_wndToolBar;
  HACCEL                          m_accelTable;
  CD3FunctionSplitterWnd          m_wndSplitter;
  CompactArray<StatusBarPaneInfo> m_paneInfo;
  bool                            m_timerRunning;
  bool                            m_destroyCalled;
  BYTE                            m_renderLevel;
  CameraSet                       m_accumulatedCameraSet;
  BYTE                            m_accumulatedRenderFlags;
  double                          m_relativeHeight;
  D3Scene                         m_scene;
  D3SceneEditor                   m_editor;
  Function2DSurfaceParameters     m_function2DSurfaceParam;
  ParametricSurfaceParameters     m_parametricSurfaceParam;
  IsoSurfaceParameters            m_isoSurfaceParam;
  String                          m_memoryInfo, m_editorInfo;

#ifdef DEBUG_POLYGONIZER
  Debugger                   *m_debugger;
  float                       m_currentCamDistance;
  bool                        m_hasIsoSurfaceParam;
  bool                        m_hasCubeCenter;
  D3DXVECTOR3                 m_cubeCenter;
  BitSet                      m_octaBreakPoints;
  bool                        m_breakPointsEnabled;
  int                         m_debugLightIndex;
  String                      m_debugInfo;

  void killDebugger(bool showCreateSurface);
  void asyncKillDebugger();
  inline bool hasDebugger() const {
    return m_debugger != NULL;
  }
  inline bool isDebuggerState(DebuggerState state) const {
    return hasDebugger() && (m_debugger->getState() == state);
  }
  inline bool isDebuggerPaused() const {
    return isDebuggerState(DEBUGGER_PAUSED);
  }
  inline void OnDebugStep(BYTE breakFlags) {
    if(isDebuggerPaused()) m_debugger->singleStep(breakFlags, m_octaBreakPoints);
  }
  bool isAutoFocusCurrentCubeChecked() const;

  void handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue);

  inline String getDebuggerStateName() const {
    return hasDebugger() ? m_debugger->getStateName() : _T("No debugger");
  }
  void debugAdjustCamDir(const D3DXVECTOR3 &newDir, const D3DXVECTOR3 &newUp);

  inline void createDebugLight() {
    m_debugLightIndex = m_scene.addLight(D3Light::createDefaultLight());
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
  void updateDebugInfo();
#endif // DEBUG_POLYGONIZER

  void OnFileOpen(int id);
  void onFileMruFile(int index);
  void updateLoadOptionsMenu();
  void updateCheckedMenuItems();
  void loadOptions(int id);
  void saveRelativeHeight();
  void setRelativeHeight(int cy);
  inline int getPanelCount() const {
    return m_wndSplitter.getPanelCount();
  }
  inline CInfoView *getInfoPanel() {
    return (CInfoView*)m_wndSplitter.GetPane(1,0);
  }
  friend class CD3FunctionSplitterWnd;

  void init3D();
  void createInitialObject();
  void createSaddle();
  void deleteCalculatedObject();
  void setCalculatedObject(Function2DSurfaceParameters &param);
  void setCalculatedObject(ParametricSurfaceParameters &param);
  void setCalculatedObject(IsoSurfaceParameters        &param);
  void setCalculatedObject(D3SceneObjectVisual *obj, PersistentData *param = NULL);
  D3SceneObjectVisual *getCalculatedObject() const;

  void startDebugging();
  void stopDebugging();
  void ajourDebuggerMenu();

  void incrLevel();
  void decrLevel();
public:

  D3Scene &getScene() {
    return m_scene;
  }
  D3SceneEditor &getEditor() {
    return m_editor;
  }
  HWND getMessageWindow() const {
    return *this;
  }
  HWND get3DWindow(UINT index) const {
    return C3DSceneView::get3DWindow(index);
  }
  UINT get3DWindowCount() const {
    return C3DSceneView::get3DWindowCount();
  }
  bool canSplit3DWindow(HWND hwnd) const;
  WindowPair split3DWindow(HWND hwnd, bool vertical);
  bool canDelete3DWindow(HWND hwnd) const;
  bool delete3DWindow(HWND hwnd);

  void render(BYTE renderFlags, CameraSet cameraSet);

  bool is3DWindow(HWND hwnd) const;
  void startTimer();
  void stopTimer();
  inline bool getTimerRunning() const {
    return m_timerRunning;
  }
  inline bool isInfoPanelVisible() const {
    return getPanelCount() > 1;
  }
  void updateMemoryInfo();
  void updateEditorInfo();
  void showInfo(_In_z_ _Printf_format_string_ TCHAR const * const format, ...);
  // flags any combination of INFO_MEM, INFO_EDIT, INFO_DEBUG
  void show3DInfo(BYTE flags);

  void activateOptions();
  void ajourMenuItems();

  virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext *pContext);
  virtual BOOL PreTranslateMessage(MSG* pMsg);

  virtual ~CMainFrame();
  void enableToolbarButtonAndMenuItem(int id, bool enable);
  bool isToolbarButtonEnabled(int id);
  void showStatusBarPanes(bool show);

  CD3FunctionDoc *getDoc() {
    return (CD3FunctionDoc*)GetActiveDocument();
  }

#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

public:
    CMainFrame();
    DECLARE_DYNCREATE(CMainFrame)
    DECLARE_MESSAGE_MAP()
    afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    afx_msg void OnFileFunctionSurface();
    afx_msg void OnFileParametricSurface();
    afx_msg void OnFileIsoSurface();
    afx_msg void OnObjectEditFunction();
    afx_msg void OnFilePrintPreview();
    afx_msg void OnFileMruFile1();
    afx_msg void OnFileMruFile2();
    afx_msg void OnFileMruFile3();
    afx_msg void OnFileMruFile4();
    afx_msg void OnFileMruFile5();
    afx_msg void OnFileMruFile6();
    afx_msg void OnFileMruFile7();
    afx_msg void OnFileMruFile8();
    afx_msg void OnFileMruFile9();
    afx_msg void OnFileMruFile10();
    afx_msg void OnFileMruFile11();
    afx_msg void OnFileMruFile12();
    afx_msg void OnFileMruFile13();
    afx_msg void OnFileMruFile14();
    afx_msg void OnFileMruFile15();
    afx_msg void OnFileMruFile16();
    afx_msg void OnDebugGo();
    afx_msg void OnDebugStepCube();
    afx_msg void OnDebugStepTetra();
    afx_msg void OnDebugStepFace();
    afx_msg void OnDebugStepVertex();
    afx_msg void OnDebugStopDebugging();
    afx_msg void OnDebugToggleBreakOnPrevCube();
    afx_msg void OnDebugDisableAllBreakPoints();
    afx_msg void OnDebugClearAllBreakPoints();
    afx_msg void OnDebugAutoFocusCurrentCube();
    afx_msg void OnDebugAdjustCam45Up();
    afx_msg void OnDebugAdjustCam45Down();
    afx_msg void OnDebugAdjustCam45Left();
    afx_msg void OnDebugAdjustCam45Right();
    afx_msg void OnDebugMarkCube();
    afx_msg void OnResetPositions();
    afx_msg void OnOptionsSaveOptions();
    afx_msg void OnOptionsLoadOptions1();
    afx_msg void OnOptionsLoadOptions2();
    afx_msg void OnOptionsLoadOptions3();
    afx_msg void OnOptionsLoadOptions4();
    afx_msg void OnOptionsLoadOptions5();
    afx_msg void OnOptionsLoadOptions6();
    afx_msg void OnOptionsLoadOptions7();
    afx_msg void OnOptionsLoadOptions8();
    afx_msg void OnOptionsLoadOptions9();
    afx_msg void OnOptionsOrganizeOptions();
    afx_msg LRESULT OnMsgRender(              WPARAM wp, LPARAM lp);
    afx_msg LRESULT OnMsgDebuggerStateChanged(WPARAM wp, LPARAM lp);
    afx_msg LRESULT OnMsgKillDebugger(        WPARAM wp, LPARAM lp);
};
