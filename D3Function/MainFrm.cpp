#include "stdafx.h"
#include <Date.h>
#include <ProcessTools.h>
#if defined(_DEBUG)
#include <DebugLog.h>
#endif
#if defined(ISODEBUGGER)
#include <ThreadPool.h>
#endif // ISODEBUGGER
#include <D3DGraphics/MeshCreators.h>
#include <D3DGraphics/D3SceneEditor.h>
#include <D3DGraphics/D3Camera.h>
#include "FunctionR2R1SurfaceParametersDlg.h"
#include "ParametricR2R3SurfaceParametersDlg.h"
#include "IsoSurfaceParametersDlg.h"
#include "ProfileDlg.h"
#include "D3FunctionSurface.h"
#include "SplitView.h"
#include "EnterOptionsNameDlg.h"
#include "OptionsOrganizerDlg.h"
#include "MainFrm.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
  ON_WM_CREATE()
  ON_WM_DESTROY()
  ON_WM_SIZE()
  ON_WM_SHOWWINDOW()
  ON_WM_TIMER()
  ON_COMMAND(ID_FILE_MRU_FILE1             , OnFileMruFile1              )
  ON_COMMAND(ID_FILE_MRU_FILE2             , OnFileMruFile2              )
  ON_COMMAND(ID_FILE_MRU_FILE3             , OnFileMruFile3              )
  ON_COMMAND(ID_FILE_MRU_FILE4             , OnFileMruFile4              )
  ON_COMMAND(ID_FILE_MRU_FILE5             , OnFileMruFile5              )
  ON_COMMAND(ID_FILE_MRU_FILE6             , OnFileMruFile6              )
  ON_COMMAND(ID_FILE_MRU_FILE7             , OnFileMruFile7              )
  ON_COMMAND(ID_FILE_MRU_FILE8             , OnFileMruFile8              )
  ON_COMMAND(ID_FILE_MRU_FILE9             , OnFileMruFile9              )
  ON_COMMAND(ID_FILE_MRU_FILE10            , OnFileMruFile10             )
  ON_COMMAND(ID_FILE_MRU_FILE11            , OnFileMruFile11             )
  ON_COMMAND(ID_FILE_MRU_FILE12            , OnFileMruFile12             )
  ON_COMMAND(ID_FILE_MRU_FILE13            , OnFileMruFile13             )
  ON_COMMAND(ID_FILE_MRU_FILE14            , OnFileMruFile14             )
  ON_COMMAND(ID_FILE_MRU_FILE15            , OnFileMruFile15             )
  ON_COMMAND(ID_FILE_MRU_FILE16            , OnFileMruFile16             )
  ON_COMMAND(ID_FILE_FUNCTIONSURFACE       , OnFileFunctionSurface       )
  ON_COMMAND(ID_FILE_PARAMETRICSURFACE     , OnFileParametricSurface     )
  ON_COMMAND(ID_FILE_ISOSURFACE            , OnFileIsoSurface            )
  ON_COMMAND(ID_FILE_PROFILESURFACE        , OnFileProfileSurface        )
  ON_COMMAND(ID_OBJECT_EDITFUNCTION        , OnObjectEditFunction        )
  ON_COMMAND(ID_DEBUG_GO                   , OnDebugGo                   )
  ON_COMMAND(ID_DEBUG_STEPCUBE             , OnDebugStepCube             )
  ON_COMMAND(ID_DEBUG_STEPTETRA            , OnDebugStepTetra            )
  ON_COMMAND(ID_DEBUG_STEPFACE             , OnDebugStepFace             )
  ON_COMMAND(ID_DEBUG_STEPVERTEX           , OnDebugStepVertex           )
  ON_COMMAND(ID_DEBUG_STOPDEBUGGING        , OnDebugStopDebugging        )
  ON_COMMAND(ID_DEBUG_TOGGLEBREAKONPREVCUBE, OnDebugToggleBreakOnPrevCube)
  ON_COMMAND(ID_DEBUG_DISABLEALLBREAKPOINTS, OnDebugDisableAllBreakPoints)
  ON_COMMAND(ID_DEBUG_CLEARALLBREAKPOINTS  , OnDebugClearAllBreakPoints  )
  ON_COMMAND(ID_RESETPOSITIONS             , OnResetPositions            )
  ON_COMMAND(ID_OPTIONS_SAVEOPTIONS        , OnOptionsSaveOptions        )
  ON_COMMAND(ID_OPTIONS_LOADOPTIONS1       , OnOptionsLoadOptions1       )
  ON_COMMAND(ID_OPTIONS_LOADOPTIONS2       , OnOptionsLoadOptions2       )
  ON_COMMAND(ID_OPTIONS_LOADOPTIONS3       , OnOptionsLoadOptions3       )
  ON_COMMAND(ID_OPTIONS_LOADOPTIONS4       , OnOptionsLoadOptions4       )
  ON_COMMAND(ID_OPTIONS_LOADOPTIONS5       , OnOptionsLoadOptions5       )
  ON_COMMAND(ID_OPTIONS_LOADOPTIONS6       , OnOptionsLoadOptions6       )
  ON_COMMAND(ID_OPTIONS_LOADOPTIONS7       , OnOptionsLoadOptions7       )
  ON_COMMAND(ID_OPTIONS_LOADOPTIONS8       , OnOptionsLoadOptions8       )
  ON_COMMAND(ID_OPTIONS_LOADOPTIONS9       , OnOptionsLoadOptions9       )
  ON_COMMAND(ID_OPTIONS_ORGANIZEOPTIONS    , OnOptionsOrganizeOptions    )
  ON_MESSAGE(ID_MSG_RENDER                 , OnMsgRender                 )
  ON_MESSAGE(ID_MSG_KILLDEBUGGER           , OnMsgKillDebugger           )
END_MESSAGE_MAP()

static UINT indicators[] = {
  ID_INDICATOR_POSITION
 ,ID_SEPARATOR
 ,ID_INDICATOR_DEBUG
};

#define INDICATORCOUNT ARRAYSIZE(indicators)

#define REPAINT() Invalidate(FALSE)

CMainFrame::CMainFrame()
#if defined(ISODEBUGGER)
: m_debugger(               NULL )
, m_hasIsoSurfaceParam(     false)
, m_hasFinalDebugIsoSurface(false)
, m_octaBreakPoints(        100  )
, m_breakPointsEnabled(     true )
#endif // ISODEBUGGER
{
  m_statusPanesVisible     = true;
  m_timerRunning           = false;
  m_destroyCalled          = false;
}

CMainFrame::~CMainFrame() {
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  if(__super::OnCreate(lpCreateStruct) == -1) {
    return -1;
  }
  if(!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
      | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
      !m_wndToolBar.LoadToolBar(IDR_MAINFRAME)) {
    TRACE0("Failed to create toolbar\n");
    return -1;      // fail to create
  }

  if(!m_wndStatusBar.Create(this) || !m_wndStatusBar.SetIndicators(indicators, INDICATORCOUNT)) {
    TRACE0("Failed to create status bar\n");
    return -1;      // fail to create
  }

  m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);

  m_wndStatusBar.Invalidate();
  m_bAutoMenuEnable = false;

  EnableDocking(CBRS_ALIGN_ANY);
  DockControlBar(&m_wndToolBar);
  m_accelTable = LoadAccelerators(theApp.m_hInstance, MAKEINTRESOURCE(IDR_MAINFRAME));
  ajourDebugMenu();

  init3D();
  return 0;
}

void CMainFrame::OnDestroy() {
  m_destroyCalled = true;
  stopDebugging();
  deleteCalculatedObject();
  __super::OnDestroy();
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/, CCreateContext *pContext) {
  m_relativeHeight = 0.8;
  VERIFY(m_wndSplitter.CreateStatic(this, 2, 1));
  VERIFY(m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(C3DSceneView), CSize(700, 500), pContext));
  VERIFY(m_wndSplitter.CreateView(1, 0, RUNTIME_CLASS(CInfoView   ), CSize(700, 100), pContext));
  return TRUE;
}

D3SceneEditor &CD3FunctionSplitterWnd::getEditor() const {
  if(m_editor == NULL) {
    m_editor = &theApp.getMainFrame()->getEditor();
  }
  return *m_editor;
}

void CD3FunctionSplitterWnd::RecalcLayout() {
  CInfoView *infoView = (CInfoView*)GetPane(1, 0);
  infoView->enableScroll(false);
  __super::RecalcLayout();
  if(m_splitPointMoved) {
    CMainFrame *mf = theApp.getMainFrame();
    mf->saveRelativeHeight();
    D3SceneEditor &editor = getEditor();
    mf->getEditor().popAllStateFlags(); // setEnabled(true);
    if(mf->isInfoPanelVisible()) {
      mf->startTimer();
    } else {
      mf->stopTimer();
    }
    m_splitPointMoved = false;
  }
  infoView->enableScroll(true);
}

void CD3FunctionSplitterWnd::OnInvertTracker(const CRect &rect) {
  __super::OnInvertTracker(rect);
  D3SceneEditor &editor = getEditor();
  if(editor.isFlagsStackEmpty()) {
    editor.pushStateFlags(false, SE_ENABLED | SE_PROPCHANGES | SE_RENDER);
  }
  m_splitPointMoved = true;
}

#if defined(_DEBUG)
void CMainFrame::AssertValid() const {
  __super::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const {
  __super::Dump(dc);
}

#endif //_DEBUG

void CMainFrame::updateLoadOptionsMenu() {
  const StringArray optionNames = Options::getExistingNames();
  const String saveOptionsMenuPath = _T("3/11");
  int index;
  HMENU optionsMenu = findMenuByString(m_hMenuDefault, saveOptionsMenuPath, &index);
  HMENU loadMenu    = (optionsMenu && (index >= 0)) ? getSubMenu(optionsMenu, _T("load options")) : NULL;

  if(loadMenu != NULL) {
    removeAllMenuItems(loadMenu);
  } else if(optionNames.size() > 0) {
    loadMenu = insertSubMenu(optionsMenu, 12, _T("&Load options"));
  } else {
    return;
  }
  const int n = min((int)optionNames.size(), 9);
  for(int i = 0; i < n; i++) {
    insertMenuItem(loadMenu,i, format(_T("%s\tCtrl+%d"), optionNames[i].cstr(), i+1), ID_OPTIONS_LOADOPTIONS+i+1);
  }
}

void CMainFrame::loadOptions(int id) {
  const int index = id - 1;
  Array<Options> optionArray = Options::getExisting();
  if (index >= (int)optionArray.size()) {
    return;
  }
  getOptions() = optionArray[index];
  activateOptions();
}

void CMainFrame::OnOptionsOrganizeOptions() {
  COptionsOrganizerDlg dlg;
  if(dlg.DoModal() == IDOK) {
    updateLoadOptionsMenu();
  }
}

void CMainFrame::saveRelativeHeight() {
  const CRect r = getClientRect(this);
  if(getPanelCount() == 2) {
    int h1,cyMin;
    m_wndSplitter.GetRowInfo(0,h1,cyMin);
    m_relativeHeight = (double)h1 / r.Height();
  }
}

void CMainFrame::setRelativeHeight(int cy) {
  if(getPanelCount() == 2) {
    m_wndSplitter.SetRowInfo(0,(int)(m_relativeHeight * cy),10);
  }
}

// ---------------------------------------------- non menu commands ----------------------------------------

void CMainFrame::OnSize(UINT nType, int cx, int cy) {
  __super::OnSize(nType, cx, cy);
  if(IsIconic())  {
    return;
  }

  if(m_wndSplitter.IsWindowVisible()) {
    setRelativeHeight(cy);
    m_wndSplitter.RecalcLayout();
    getOptions().m_windowSize = getWindowSize(this);
  }
}

void CMainFrame::OnShowWindow(BOOL bShow, UINT nStatus) {
  __super::OnShowWindow(bShow, nStatus);
  updateLoadOptionsMenu();
  RECT r = getClientRect(this);
  int cy = r.bottom;
  if(getPanelCount() == 2) {
    m_wndSplitter.SetRowInfo(0,(int)(m_relativeHeight * cy),10);
  } else {
    m_wndSplitter.SplitRow((int)(m_relativeHeight * cy));
  }
}

void CMainFrame::enableToolbarButtonAndMenuItem(int id, bool enable) {
  CToolBarCtrl &ctrlBar = m_wndToolBar.GetToolBarCtrl();
  if(ctrlBar.m_hWnd) {
    if(enable != isToolbarButtonEnabled(id)) {
      if(enable) {
        ctrlBar.SetState(id, TBSTATE_ENABLED);
        ctrlBar.EnableButton(id, TRUE);
      } else {
        ctrlBar.SetState(id, TBSTATE_INDETERMINATE);
        ctrlBar.EnableButton(id, FALSE);
      }
    }
  }
  enableMenuItem(this, id, enable);
}

bool CMainFrame::isToolbarButtonEnabled(int id) {
  return !m_wndToolBar.GetToolBarCtrl().IsButtonIndeterminate(id);
}

void CMainFrame::showStatusBarPanes(bool show) {
  if(show != m_statusPanesVisible) {
    if(show) {
      for(size_t i = 0; i < m_paneInfo.size(); i++) {
        const StatusBarPaneInfo &info = m_paneInfo[i];
        m_wndStatusBar.SetPaneInfo(info.m_index, info.m_id, info.m_style, info.m_width);
      }
    } else {
      if(m_paneInfo.isEmpty()) {
        for(int i = 1; i < INDICATORCOUNT; i++) {
          StatusBarPaneInfo info;
          m_wndStatusBar.GetPaneInfo(i, info.m_id, info.m_style, info.m_width);
          info.m_index = i;
          m_paneInfo.add(info);
        }
      }
      for(int i = 1; i < INDICATORCOUNT; i++) {
        m_wndStatusBar.SetPaneInfo(i, indicators[i], SBPS_NOBORDERS | SBPS_DISABLED ,0);
      }
    }
    m_statusPanesVisible = show;
  }
}

void CMainFrame::ajourMenuItems() {
}

void CMainFrame::startTimer() {
  if(!m_timerRunning) {
    if(SetTimer(1, 5000, NULL) == 1) {
      m_timerRunning = true;
    }
  }
}

void CMainFrame::stopTimer() {
  if(m_timerRunning) {
    KillTimer(1);
    m_timerRunning = false;
  }
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent) {
  show3DInfo(INFO_MEM);
  __super::OnTimer(nIDEvent);
}

// -------------------------------------------- 3D --------------------------------------------

#define sCAM() m_editor.getSelectedCAM()
#define CHECKHASCAM() { if(!hasCamera()) { showWarning(_T("No camera selected")); return; } }

void CMainFrame::init3D() {
  m_scene.initDevice(*this);
  m_editor.init(this);
  m_editor.setEnabled(true);

  createInitialObject();
  D3Camera *cam = sCAM();
  if(cam) {
    m_scene.setLightDirection(0, rotate(cam->getDir(), cam->getRight(), 0.2f));
  }
  startTimer();
}

void CMainFrame::createInitialObject() {
  try {
//  D3LineArray *tt = new D3LineArray(m_scene, tetraEder, ARRAYSIZE(tetraEder)); TRACE_NEW(tt);
//  setCalculatedObject(tt);

  createSaddle();
//    m_isoSurfaceParam.load("c:\\mytools\\D3FunctionPlotter\\samples\\jacktool.imp");
//  m_isoSurfaceParam.load("c:\\mytools\\D3FunctionPlotter\\samples\\wifflecube.imp");
//    setCalculatedObject(&m_isoSurfaceParam);
  } catch(Exception e) {
    showException(e);
  }
}

void CMainFrame::createSaddle() {
  m_functionR2R1SurfaceParam.setName(_T("Saddle"));
  m_functionR2R1SurfaceParam.m_expr        = _T("(x*x-y*y)/2");
  m_functionR2R1SurfaceParam.m_xInterval   = DoubleInterval(-1, 1);
  m_functionR2R1SurfaceParam.m_yInterval   = DoubleInterval(-1, 1);
  m_functionR2R1SurfaceParam.m_pointCount  = 20;
  m_functionR2R1SurfaceParam.m_machineCode = true;
  m_functionR2R1SurfaceParam.m_doubleSided = true;
  m_functionR2R1SurfaceParam.m_animation.reset();

  setCalculatedObject(m_functionR2R1SurfaceParam);
}

void CMainFrame::deleteCalculatedObject() {
  D3SceneObjectVisual *oldObj = getCalculatedObject();
  if(oldObj) {
    m_scene.removeVisual(oldObj);
    SAFEDELETE(oldObj);
#if defined(ISODEBUGGER)
    m_hasFinalDebugIsoSurface = false;
#endif // ISODEBUGGER
  }
}

void CMainFrame::setCalculatedObject(D3SceneObjectVisual *obj, FunctionImageParamPersistentData *param) {
  deleteCalculatedObject();
  if(obj) {
    obj->setUserData(param);
    if(param) {
      obj->setName(param->getDisplayName());
    }
    m_scene.addVisual(obj);
  }
}

D3SceneObjectVisual *CMainFrame::getCalculatedObject() const {
  for(D3VisualIterator it = m_scene.getVisualIterator(); it.hasNext();) {
    D3SceneObjectVisual *obj = it.next();
    if(obj->getUserData() != NULL) {
      return obj;
    }
  }
  return NULL;
}

bool CMainFrame::is3DWindow(HWND hwnd) const {
  return C3DSceneView::is3DWindow(hwnd);
}

bool CMainFrame::canSplit3DWindow(HWND hwnd) const {
  return is3DWindow(hwnd);
}

bool CMainFrame::canDelete3DWindow(HWND hwnd) const {
  return is3DWindow(hwnd) && (get3DWindowCount() > 1);
}


WindowPair CMainFrame::split3DWindow(HWND hwnd, bool vertical) {
  WindowPair    result;
  C3DSceneView *viewToSplit = C3DSceneView::findViewByHwnd(hwnd);
  if(viewToSplit == NULL) {
    return result;
  }
  bool timerRuns = getTimerRunning();
  stopTimer();
  const SplitDirection splitDirection = vertical ? SPLIT_VERTICAL : SPLIT_HORIZONTAL;
  const CRect          rect           = getWindowRect(viewToSplit);
  CSplitterWnd        *parentSplitter = (CSplitterWnd*)viewToSplit->GetParent();
  MatrixIndex          viewPos        = findPosition(parentSplitter, viewToSplit);
  CSplitView          *newSplitView   = CSplitView::create(parentSplitter
                                                          ,RUNTIME_CLASS(C3DSceneView), RUNTIME_CLASS(C3DSceneView)
                                                          ,splitDirection
                                                          ,rect);

  CD3FunctionDoc *activeDoc           = (CD3FunctionDoc*)GetActiveDocument();
  activeDoc->m_bAutoDelete            = false;
  activeDoc->RemoveView(viewToSplit);
  activeDoc->m_bAutoDelete = true;
  activeDoc->AddView((C3DSceneView*)newSplitView->getChild(0));
  activeDoc->AddView((C3DSceneView*)newSplitView->getChild(1));

  bool ok = viewToSplit->DestroyWindow();
  SetLastError(0);
  LONG_PTR rr = ::SetWindowLongPtr(newSplitView->m_hWnd, GWL_ID, parentSplitter->IdFromRowCol((int)viewPos.r, (int)viewPos.c));

  RecalcLayout();
  newSplitView->UpdateWindow();
  SetActiveView((C3DSceneView*)newSplitView->getChild(0));
  result = WindowPair(*newSplitView->getChild(0), *newSplitView->getChild(1));
  if(timerRuns) {
    startTimer();
  }
  return result;
}

CWnd *getParentAndCheckType(CWnd *wnd, CRuntimeClass *expectedClass) {
  if(wnd == NULL) return NULL;
  CWnd *parent = wnd->GetParent();
  if((parent == NULL) || !parent->IsKindOf(expectedClass)) {
    return NULL;
  }
  return parent;
}
#define GETPARENT(wnd,Class) ((Class*)getParentAndCheckType(wnd, RUNTIME_CLASS(Class)))

bool CMainFrame::delete3DWindow(HWND hwnd) {
  C3DSceneView      *viewToDelete      = C3DSceneView::findViewByHwnd(hwnd);
  SplitViewSplitter *parentSplitter    = GETPARENT(viewToDelete     , SplitViewSplitter);
  CSplitView        *splitViewToDelete = GETPARENT(parentSplitter   , CSplitView       );
  CSplitterWnd      *gparentSplitter   = GETPARENT(splitViewToDelete, CSplitterWnd     );
  CWnd              *partner           = parentSplitter->findNeighbor(viewToDelete); // either C3DSceneView/CSplitView
  if((partner == NULL) || (gparentSplitter == NULL)) { // something went wrong...cannot go further
    return false;
  }
  const MatrixIndex   viewPos           = findPosition(gparentSplitter, splitViewToDelete);
  const CRect         rect              = getWindowRect(splitViewToDelete);

  bool               result             = false;
  const bool         timerRuns          = getTimerRunning();
  stopTimer();

  CD3FunctionDoc *activeDoc = (CD3FunctionDoc*)GetActiveDocument();
  activeDoc->m_bAutoDelete = false;
  activeDoc->RemoveView(viewToDelete);
  activeDoc->m_bAutoDelete = true;
  partner->SetParent(gparentSplitter);

//    bool ok = viewToSplit->DestroyWindow();
    SetLastError(0);
  LONG_PTR rr = ::SetWindowLongPtr(splitViewToDelete->m_hWnd, GWL_ID, 0);
  rr = ::SetWindowLongPtr(partner->m_hWnd, GWL_ID, gparentSplitter->IdFromRowCol((int)viewPos.r, (int)viewPos.c));

  BOOL ok = splitViewToDelete->DestroyWindow();
  setWindowRect(partner, rect);
  RecalcLayout();
  SetActiveView(NULL, FALSE);
  if(timerRuns) {
    startTimer();
  }
  return true;
}

void CMainFrame::doRender(BYTE renderFlags, CameraSet cameraSet) {
  if(renderFlags & SC_RENDERNOW) {
    OnMsgRender(renderFlags, cameraSet);
  } else {
    PostMessage(ID_MSG_RENDER, renderFlags, cameraSet);
  }
}

static UINT renderSceneCount = 0, renderInfoCount = 0;
LRESULT CMainFrame::OnMsgRender(WPARAM wp, LPARAM lp) {
  if(wp & SC_RENDER3D) {
    CameraSet cameraSet(lp);
    __super::doRender((BYTE)wp, cameraSet);
    renderSceneCount++;
  }
  if(wp & SC_RENDERINFO) {
    if(m_editor.isEnabled()) {
      renderInfoCount++;
      show3DInfo(INFO_EDIT);
    }
  }
  ajourDebugMenu();
  return 0;
}

typedef struct {
  UINT m_enableStart       : 1;
  UINT m_hasDebugger       : 1;
  UINT m_debuggerPaused    : 1;
  UINT m_enableGo          : 1;
  UINT m_enableBreak       : 1;
  UINT m_enableToggleBreak : 1;
} _DebugMenuFlags;

class DebugMenuFlags {
public:
  union {
    _DebugMenuFlags m_flags;
    UINT            m_allFlags;
  };
  DebugMenuFlags() : m_allFlags(0) {
  }
  inline bool enableGo() const {
    return m_flags.m_enableStart || m_flags.m_debuggerPaused;
  }
  inline bool enableDebugMenu() const {
    return enableGo() || enableBreakpoints()  || m_flags.m_hasDebugger;
  }
  inline bool enableBreakpoints() const {
    return m_flags.m_enableToggleBreak || m_flags.m_enableBreak;
  }
  inline bool enableStep() const {
    return m_flags.m_debuggerPaused;
  }
  inline bool enableStopDebugger() const {
    return m_flags.m_hasDebugger;
  }
};

void CMainFrame::ajourDebugMenu() {
  DebugMenuFlags menuFlags;
#if defined(ISODEBUGGER)
#define FLAGS menuFlags.m_flags
  FLAGS.m_enableStart       = !hasDebugger() && m_hasIsoSurfaceParam;
  FLAGS.m_hasDebugger       = hasDebugger();
  FLAGS.m_debuggerPaused    = isDebuggerPaused();
  FLAGS.m_enableBreak       = FLAGS.m_debuggerPaused || hasFinalDebugIsoSurface();
  FLAGS.m_enableToggleBreak = FLAGS.m_debuggerPaused || (hasFinalDebugIsoSurface() && getFinalDebugIsoSurface()->hasSelectedCube());
#endif // ISODEBUGGER

  enableSubMenuContainingId(this, ID_DEBUG_GO, menuFlags.enableDebugMenu());
#if defined(ISODEBUGGER)
  if(FLAGS.m_enableStart) {
    setMenuItemText(this, ID_DEBUG_GO, _T("Start debugging\tF5"));
  } else if(FLAGS.m_debuggerPaused) {
    setMenuItemText(this, ID_DEBUG_GO, _T("Go\tF5"));
  }
  enableMenuItem(           this, ID_DEBUG_GO                   , menuFlags.enableGo()          );
  enableMenuItem(           this, ID_DEBUG_STEPCUBE             , menuFlags.enableStep()        );
  enableMenuItem(           this, ID_DEBUG_STEPTETRA            , menuFlags.enableStep()        );
  enableMenuItem(           this, ID_DEBUG_STEPFACE             , menuFlags.enableStep()        );
  enableMenuItem(           this, ID_DEBUG_STEPVERTEX           , menuFlags.enableStep()        );
  enableMenuItem(           this, ID_DEBUG_TOGGLEBREAKONPREVCUBE, FLAGS.m_enableToggleBreak     );
  enableMenuItem(           this, ID_DEBUG_DISABLEALLBREAKPOINTS, FLAGS.m_enableBreak           );
  enableMenuItem(           this, ID_DEBUG_CLEARALLBREAKPOINTS  , FLAGS.m_enableBreak           );
  enableMenuItem(           this, ID_DEBUG_STOPDEBUGGING        , menuFlags.enableStopDebugger());
#endif // ISODEBUGGER
}

#if !defined(ISODEBUGGER)
void CMainFrame::startDebugging() {}
void CMainFrame::stopDebugging() {}
void CMainFrame::OnDebugGo() {}
void CMainFrame::OnDebugStepCube() {}
void CMainFrame::OnDebugStepTetra() {}
void CMainFrame::OnDebugStepFace() {}
void CMainFrame::OnDebugStepVertex() {}
void CMainFrame::OnDebugStopDebugging() {}
void CMainFrame::OnDebugToggleBreakOnPrevCube() {}
void CMainFrame::OnDebugDisableAllBreakPoints() {}
void CMainFrame::OnDebugClearAllBreakPoints() {}

LRESULT CMainFrame::OnMsgKillDebugger(WPARAM wp, LPARAM lp) { return 0; }
#else

D3Camera *CMainFrame::dbgCAM() {
  D3Camera *cam = sCAM();
  if(cam) {
    return cam;
  } else {
    return m_scene.getCameraArray()[0];
  }
}

void CMainFrame::startDebugging() {
  setCalculatedObject(NULL);
  try {
    killDebugger(false);
    m_debugger = new Debugger(this, m_isoSurfaceParam);
    m_editor.OnControlCameraWalk();
    ThreadPool::executeNoWait(*m_debugger);
    show3DInfo(INFO_MEM);
  } catch (Exception e) {
    showException(e);
  }
}

void CMainFrame::stopDebugging() {
  killDebugger(false);
}

void CMainFrame::killDebugger(bool showCreateSurface) {
  if(!hasDebugger()) return;
  m_scene.removeAllVisuals();
  const bool allOk = m_debugger->isOk();
  if(allOk && showCreateSurface) {
    const DebugIsoSurface &surface = m_debugger->getDebugSurface();
    if(surface.getFaceCount()) {
      FinalDebugIsoSurface *obj = surface.createFinalDebugIsoSurface(m_editor);
      setCalculatedObject(obj, &m_isoSurfaceParam);
      m_hasFinalDebugIsoSurface = true;
    }
  }
  SAFEDELETE(m_debugger);
  if(!m_destroyCalled) {
    show3DInfo(INFO_ALL);
    if(allOk) {
      showInformation(_T("%s"), _T("Polygonizing surface done"));
    } else {
      showError(_T("%s"), m_debugger->getErrorMsg().cstr());
    }
  }
}

void CMainFrame::OnDebugGo() {
  if(!hasDebugger()) {
    if(m_hasIsoSurfaceParam) {
      startDebugging();
    }
  } else if(isDebuggerPaused()) {
    m_debugger->go(m_octaBreakPoints);
  }
}

void CMainFrame::OnDebugStepCube()   { OnDebugStep(FL_BREAKONNEXTOCTA  ); }
void CMainFrame::OnDebugStepTetra()  { OnDebugStep(FL_BREAKONNEXTTETRA ); }
void CMainFrame::OnDebugStepFace()   { OnDebugStep(FL_BREAKONNEXTFACE  ); }
void CMainFrame::OnDebugStepVertex() { OnDebugStep(FL_BREAKONNEXTVERTEX); }

void CMainFrame::OnDebugStopDebugging() {
  if(hasDebugger()) {
    killDebugger(false);
  }
}

void CMainFrame::OnDebugToggleBreakOnPrevCube() {
  if(isDebuggerPaused()) {
    const int octaIndex = m_debugger->getOctaIndex();
    if(octaIndex <= 0) return;
    const UINT breakPoint = octaIndex - 1;
    toggleOctaBreakpoint(breakPoint);
  } else if(hasFinalDebugIsoSurface() && getFinalDebugIsoSurface()->hasSelectedCube()) {
    toggleOctaBreakpoint(getFinalDebugIsoSurface()->getSelectedCubeIndex());
  }
}

void CMainFrame::OnDebugDisableAllBreakPoints() {
  enableOctaBreakpoints(!isOctaBreakpointsEnabled());
}

void CMainFrame::OnDebugClearAllBreakPoints() {
  clearAllOctaBreakpoints();
}

void CMainFrame::setOctaBreakpoint(size_t index, bool on) {
  if(!canUpdateBreakpoints()) return;
  if(m_octaBreakPoints.getCapacity() <= index) {
    m_octaBreakPoints.setCapacity(2 * index);
  }
  const bool updateInfo = on != hasOctaBreakPoint(index);
  if(on) {
    m_octaBreakPoints.add(index);
  } else {
    m_octaBreakPoints.remove(index);
  }
  if(updateInfo) {
    show3DInfo(INFO_DEBUG);
  }
}

void CMainFrame::toggleOctaBreakpoint(size_t index) {
  setOctaBreakpoint(index, !hasOctaBreakPoint(index));
}

bool CMainFrame::hasOctaBreakPoint(size_t index) const {
  return m_octaBreakPoints.contains(index);
}

void CMainFrame::clearAllOctaBreakpoints() {
  if(!canUpdateBreakpoints()) return;
  const bool updateInfo = !m_octaBreakPoints.isEmpty();
  m_octaBreakPoints.clear();
  if(updateInfo) {
    show3DInfo(INFO_DEBUG);
  }
}

void CMainFrame::enableOctaBreakpoints(bool enable) {
  if(!canUpdateBreakpoints()) return;
  const bool updateInfo = enable != m_breakPointsEnabled;
  m_breakPointsEnabled = enable;
  if(updateInfo) {
    show3DInfo(INFO_DEBUG);
  }
}

LRESULT CMainFrame::OnMsgKillDebugger(WPARAM wp, LPARAM lp) {
  killDebugger(true);
  return 0;
}

void CMainFrame::updateDebugInfo() {
  m_debugInfo = format(_T("Debugger State:%-8s"), getDebuggerStateName().cstr());
  if(!m_octaBreakPoints.isEmpty()) {
    m_debugInfo += format(_T("\nBreakpoints:%s"), m_octaBreakPoints.toString().cstr());
    m_debugInfo += format(_T("\nBreakpoints %s"), m_breakPointsEnabled ? _T("enabled") : _T("disabled"));
  }

  if(isDebuggerPaused()) {
    m_debugInfo += format(_T(" Flags:%s"), m_debugger->getFlagNames().cstr());
    if(!m_debugger->isOk()) {
      m_debugInfo += format(_T("\nError:%s"), m_debugger->getErrorMsg().cstr());
    }
    m_debugInfo += m_debugger->getDebugSurface().getInfoString();
  }
}
#endif // ISODEBUGGER

void CMainFrame::updateMemoryInfo() {
  const PROCESS_MEMORY_COUNTERS mem = getProcessMemoryUsage();
  const ResourceCounters        res = getProcessResources();
  m_memoryInfo = format(_T("Time:%s Memory:%13s User-obj:%4d GDI-obj:%4d %s")
                       ,Timestamp().toString(hhmmss).cstr()
                       ,format1000(mem.WorkingSetSize).cstr()
                       ,res.m_userObjectCount
                       ,res.m_gdiObjectCount
                       ,m_editor.stateFlagsToString().cstr()
                       );
}

void CMainFrame::updateEditorInfo() {
  m_editorInfo = m_editor.toString();
}

void CMainFrame::show3DInfo(BYTE flags) {
  if(!isInfoPanelVisible()) return;
  if(flags & INFO_MEM  ) updateMemoryInfo();
  if(flags & INFO_EDIT ) updateEditorInfo();
#if !defined(ISODEBUGGER)
  showInfo(_T("%s\n%s"), m_memoryInfo.cstr(), m_editorInfo.cstr());
#else
  if(flags & INFO_DEBUG) updateDebugInfo();
  showInfo(_T("%s, RenderCount:(%5u,%5u)\n%s\n%s")
          ,m_memoryInfo.cstr()
          ,renderSceneCount, renderInfoCount
          ,m_editorInfo.cstr()
          ,m_debugInfo.cstr());
#endif //  ISODEBUGGER
}

void CMainFrame::showInfo(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  va_list argptr;
  va_start(argptr, format);
  getInfoPanel()->vprintf(format, argptr);
  va_end(argptr);
}

// -------------------------------------------- File menu ----------------------------------------

void CMainFrame::OnFileFunctionSurface() {
  try {
    CExprFunctionR2R1SurfaceParametersDlg dlg(m_functionR2R1SurfaceParam, m_scene.getDevice());
    if(dlg.DoModal() != IDOK) {
      return;
    }
    m_functionR2R1SurfaceParam = dlg.getData();
    setCalculatedObject(m_functionR2R1SurfaceParam);
    REPAINT();
  } catch(Exception e) {
    showException(e);
  }
}

void CMainFrame::OnFileParametricSurface() {
  try {
    CParametricR2R3SurfaceParametersDlg dlg(m_parametricR2R3SurfaceParam, m_scene.getDevice());
    if(dlg.DoModal() != IDOK) {
      return;
    }
    m_parametricR2R3SurfaceParam = dlg.getData();
    setCalculatedObject(m_parametricR2R3SurfaceParam);
    REPAINT();
  } catch(Exception e) {
    showException(e);
  }
}

void CMainFrame::OnFileIsoSurface() {
  try {
    CIsoSurfaceParametersDlg dlg(m_isoSurfaceParam, m_scene.getDevice());
    if(dlg.DoModal() != IDOK) {
      return;
    }
    m_isoSurfaceParam = dlg.getData();
#if !defined(ISODEBUGGER)
    setCalculatedObject(m_isoSurfaceParam);
    REPAINT();
#else
    m_hasIsoSurfaceParam = true;
    if(dlg.getDebugPolygonizer()) {
      startDebugging();
    } else {
      setCalculatedObject(m_isoSurfaceParam);
      REPAINT();
    }
#endif // ISODEBUGGER
  } catch(Exception e) {
    showException(e);
  }
}

void CMainFrame::OnFileProfileSurface() {
  try {
    CProfileDlg dlg;
    dlg.DoModal();
  } catch(Exception e) {
    showException(e);
  }
}

void CMainFrame::OnObjectEditFunction() {
  D3SceneObjectVisual *calcObj = getCalculatedObject();
  if(!calcObj) return;
  FunctionImageParamPersistentData *param = (FunctionImageParamPersistentData*)calcObj->getUserData();
  switch(param->getType()) {
  case TYPE_FUNCTIONR2R1SURFACE:
    OnFileFunctionSurface();
    break;
  case TYPE_PARAMETRICR2R3SURFACE:
    OnFileParametricSurface();
    break;
  case TYPE_ISOSURFACE:
    OnFileIsoSurface();
    break;
  default:
    showWarning(_T("Unknown PersistentDataType:%d"), param->getType());
    break;
  }
}

void CMainFrame::OnFileMruFile1()  { onFileMruFile( 0);}
void CMainFrame::OnFileMruFile2()  { onFileMruFile( 1);}
void CMainFrame::OnFileMruFile3()  { onFileMruFile( 2);}
void CMainFrame::OnFileMruFile4()  { onFileMruFile( 3);}
void CMainFrame::OnFileMruFile5()  { onFileMruFile( 4);}
void CMainFrame::OnFileMruFile6()  { onFileMruFile( 5);}
void CMainFrame::OnFileMruFile7()  { onFileMruFile( 6);}
void CMainFrame::OnFileMruFile8()  { onFileMruFile( 7);}
void CMainFrame::OnFileMruFile9()  { onFileMruFile( 8);}
void CMainFrame::OnFileMruFile10() { onFileMruFile( 9);}
void CMainFrame::OnFileMruFile11() { onFileMruFile(10);}
void CMainFrame::OnFileMruFile12() { onFileMruFile(11);}
void CMainFrame::OnFileMruFile13() { onFileMruFile(12);}
void CMainFrame::OnFileMruFile14() { onFileMruFile(13);}
void CMainFrame::OnFileMruFile15() { onFileMruFile(14);}
void CMainFrame::OnFileMruFile16() { onFileMruFile(15);}

void CMainFrame::onFileMruFile(int index) {
  try {
    const String fname = theApp.getRecentFile(index);

    if(ACCESS(fname, 4) < 0) {
      const int errorCode = errno;
      showWarning(getErrnoText());
      if(errorCode == ENOENT) {
        theApp.removeFromRecentFiles(index);
      }
      return;
    }
    Invalidate(FALSE);
  } catch(Exception e) {
    showException(e);
  }
}

void CMainFrame::activateOptions() {
}

void CMainFrame::OnResetPositions() {
  D3Camera *cam = sCAM();
  if(cam) {
    cam->resetPos();
  }
}

void CMainFrame::OnOptionsSaveOptions() {
  Options &options = getOptions();
  CEnterOptionsNameDlg dlg(options.m_name);
  if(dlg.DoModal() == IDOK) {
    options.m_name = dlg.m_name;
    if(options.save()) {
      updateLoadOptionsMenu();
    }
  }
}

void CMainFrame::OnOptionsLoadOptions1() { loadOptions(1); }
void CMainFrame::OnOptionsLoadOptions2() { loadOptions(2); }
void CMainFrame::OnOptionsLoadOptions3() { loadOptions(3); }
void CMainFrame::OnOptionsLoadOptions4() { loadOptions(4); }
void CMainFrame::OnOptionsLoadOptions5() { loadOptions(5); }
void CMainFrame::OnOptionsLoadOptions6() { loadOptions(6); }
void CMainFrame::OnOptionsLoadOptions7() { loadOptions(7); }
void CMainFrame::OnOptionsLoadOptions8() { loadOptions(8); }
void CMainFrame::OnOptionsLoadOptions9() { loadOptions(9); }

BOOL CMainFrame::PreTranslateMessage(MSG *pMsg) {
  if(GetActiveWindow() != this) {
    return false;
  }
  D3Camera *cam;
  if((pMsg->message == WM_MOUSEMOVE) && ((cam = m_scene.getPickedCamera(pMsg->pt)) != NULL)) {
    m_wndStatusBar.SetPaneText(0, toString(cam->screenToWin(pMsg->pt)).cstr());
  }
  const bool levelIncremented = pMsg->message != ID_MSG_RENDER;
  if(levelIncremented) incrLevel();
  BOOL result;
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    result = true;
  } else if(m_editor.PreTranslateMessage(pMsg)) {
    result = true;
  } else {
    result = __super::PreTranslateMessage(pMsg);
  }
  if(levelIncremented) decrLevel();
  return result;
}
