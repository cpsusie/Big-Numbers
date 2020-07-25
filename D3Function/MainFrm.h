#pragma once

#include <D3DGraphics/ExprFunctionR2R1SurfaceParameters.h>
#include <D3DGraphics/ExprParametricR1R3SurfaceParameters.h>
#include <D3DGraphics/ExprParametricR2R3SurfaceParameters.h>
#include <D3DGraphics/ExprIsoSurfaceParameters.h>
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
  bool                   m_splitPointMoved;
  mutable D3SceneEditor *m_editor;
  D3SceneEditor &getEditor() const;
public:
  CD3FunctionSplitterWnd() : m_splitPointMoved(false), m_editor(NULL) {
  }
  void RecalcLayout();
  void OnInvertTracker(const CRect& rect);
  int           getPanelCount() const {
    return GetRowCount();
  }
};

#define INFO_EDIT  0x01
#define INFO_MEM   0x02

#if defined(ISODEBUGGER)
#define INFO_DEBUG 0x04
#include "Debugger.h"
#else
#define INFO_DEBUG 0
#endif // ISODEBUGGER

#define INFO_ALL  (INFO_EDIT|INFO_MEM|INFO_DEBUG)

class CMainFrame : public CFrameWnd
                 , public OptionsUpdater
                 , public D3SceneContainer
{
private:
  friend class C3DSceneView;
  bool                                m_statusPanesVisible;
  CStatusBar                          m_wndStatusBar;
  CToolBar                            m_wndToolBar;
  HACCEL                              m_accelTable;
  CD3FunctionSplitterWnd              m_wndSplitter;
  CompactArray<StatusBarPaneInfo>     m_paneInfo;
  bool                                m_timerRunning;
  bool                                m_destroyCalled;
  double                              m_relativeHeight;
  D3Scene                             m_scene;
  D3SceneEditor                       m_editor;
  ExprFunctionR2R1SurfaceParameters   m_functionR2R1SurfaceParam;
  ExprParametricR1R3SurfaceParameters m_parametricR1R3SurfaceParam;
  ExprParametricR2R3SurfaceParameters m_parametricR2R3SurfaceParam;
  ExprIsoSurfaceParameters            m_isoSurfaceParam;
  String                              m_memoryInfo, m_editorInfo;

#if defined(ISODEBUGGER)
  Debugger                       *m_debugger;
  bool                            m_hasIsoSurfaceParam;
  bool                            m_hasFinalDebugIsoSurface;
  BitSet                          m_octaBreakPoints;
  bool                            m_breakPointsEnabled;
  String                          m_debugInfo;

  void killDebugger(bool showCreateSurface);
  inline bool hasDebugger() const {
    return m_debugger != NULL;
  }
  inline bool hasCamera() const {
    return m_editor.getSelectedCAM() != NULL;
  }
  D3Camera *dbgCAM();
  inline bool isDebuggerState(DebuggerState state) const {
    return hasDebugger() && (m_debugger->getState() == state);
  }
  inline bool isDebuggerPaused() const {
    return isDebuggerState(DEBUGGER_PAUSED);
  }
  inline void OnDebugStep(BYTE breakFlags) {
    if(isDebuggerPaused()) m_debugger->singleStep(breakFlags, m_octaBreakPoints);
  }
  inline bool canUpdateBreakpoints() const {
    return isDebuggerPaused() || hasFinalDebugIsoSurface();
  }
  void setOctaBreakpoint(   size_t index, bool on);
  void toggleOctaBreakpoint(size_t index);
  bool hasOctaBreakPoint(   size_t index) const;
  void clearAllOctaBreakpoints();
  void enableOctaBreakpoints(bool enable);
  bool isOctaBreakpointsEnabled() const {
    return m_breakPointsEnabled;
  }
public:
  bool isSlidingCamera() const;
private:
  inline String getDebuggerStateName() const {
    return hasDebugger() ? m_debugger->getStateName() : _T("No debugger");
  }
  void updateDebugInfo();
  inline bool hasFinalDebugIsoSurface() const {
    return m_hasFinalDebugIsoSurface;
  }
  inline FinalDebugIsoSurface *getFinalDebugIsoSurface() const {
    return hasFinalDebugIsoSurface() ? (FinalDebugIsoSurface*)getCalculatedObject() : NULL;
  }
#endif // ISODEBUGGER

  void OnFileOpen(int id);
  void onFileMruFile(int index);
  void updateLoadOptionsMenu();
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
  void setCalculatedObject(D3SceneObjectVisual *obj, FunctionImageParamPersistentData *param = NULL);

  template<typename T> void setCalculatedObject(T &param) {
    stopDebugging();
    if(param.isAnimated()) {
      D3AnimatedFunctionSurface *obj = createAnimatedSurface(this, m_scene, param);
      setCalculatedObject(obj, &param);
    } else {
      D3FunctionSurface *obj = createSurface(m_scene, param);
      setCalculatedObject(obj, &param);
    }
  }

  D3SceneObjectVisual *getCalculatedObject() const;

  void startDebugging();
  void stopDebugging();
  void ajourDebugMenu();
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

  void doRender(BYTE renderFlags, CameraSet cameraSet);

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

#if defined(_DEBUG)
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
    afx_msg void OnFileParametricCurve();
    afx_msg void OnFileParametricSurface();
    afx_msg void OnFileIsoSurface();
    afx_msg void OnFileProfileSurface();
    afx_msg void OnFileConvertAll();
    afx_msg void OnObjectEditFunction();
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
    afx_msg void OnDebugSlidingCamera();
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
    afx_msg LRESULT OnMsgKillDebugger(        WPARAM wp, LPARAM lp);
};
