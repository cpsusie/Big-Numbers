#include "stdafx.h"
#include <ProcessTools.h>
#include <D3DGraphics/MeshCreators.h>
#include "Function2DSurfaceDlg.h"
#include "ParametricSurfaceDlg.h"
#include "IsoSurfaceDlg.h"
#include "MainFrm.h"
#include "EnterOptionsNameDlg.h"
#include "OptionsOrganizerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
  ON_WM_CREATE()
  ON_WM_SIZE()
  ON_WM_SHOWWINDOW()
  ON_WM_TIMER()
  ON_COMMAND(ID_FILE_MRU_FILE1            , OnFileMruFile1             )
  ON_COMMAND(ID_FILE_MRU_FILE2            , OnFileMruFile2             )
  ON_COMMAND(ID_FILE_MRU_FILE3            , OnFileMruFile3             )
  ON_COMMAND(ID_FILE_MRU_FILE4            , OnFileMruFile4             )
  ON_COMMAND(ID_FILE_MRU_FILE5            , OnFileMruFile5             )
  ON_COMMAND(ID_FILE_MRU_FILE6            , OnFileMruFile6             )
  ON_COMMAND(ID_FILE_MRU_FILE7            , OnFileMruFile7             )
  ON_COMMAND(ID_FILE_MRU_FILE8            , OnFileMruFile8             )
  ON_COMMAND(ID_FILE_MRU_FILE9            , OnFileMruFile9             )
  ON_COMMAND(ID_FILE_MRU_FILE10           , OnFileMruFile10            )
  ON_COMMAND(ID_FILE_MRU_FILE11           , OnFileMruFile11            )
  ON_COMMAND(ID_FILE_MRU_FILE12           , OnFileMruFile12            )
  ON_COMMAND(ID_FILE_MRU_FILE13           , OnFileMruFile13            )
  ON_COMMAND(ID_FILE_MRU_FILE14           , OnFileMruFile14            )
  ON_COMMAND(ID_FILE_MRU_FILE15           , OnFileMruFile15            )
  ON_COMMAND(ID_FILE_MRU_FILE16           , OnFileMruFile16            )
  ON_COMMAND(ID_FILE_FUNCTIONSURFACE      , OnFileFunctionSurface      )
  ON_COMMAND(ID_FILE_PARAMETRICSURFACE    , OnFileParametricSurface    )
  ON_COMMAND(ID_FILE_ISOSURFACE           , OnFileIsoSurface           )
  ON_COMMAND(ID_OPTIONS_SAVEOPTIONS       , OnOptionsSaveOptions       )
  ON_COMMAND(ID_OPTIONS_LOADOPTIONS1      , OnOptionsLoadOptions1      )
  ON_COMMAND(ID_OPTIONS_LOADOPTIONS2      , OnOptionsLoadOptions2      )
  ON_COMMAND(ID_OPTIONS_LOADOPTIONS3      , OnOptionsLoadOptions3      )
  ON_COMMAND(ID_OPTIONS_LOADOPTIONS4      , OnOptionsLoadOptions4      )
  ON_COMMAND(ID_OPTIONS_LOADOPTIONS5      , OnOptionsLoadOptions5      )
  ON_COMMAND(ID_OPTIONS_LOADOPTIONS6      , OnOptionsLoadOptions6      )
  ON_COMMAND(ID_OPTIONS_LOADOPTIONS7      , OnOptionsLoadOptions7      )
  ON_COMMAND(ID_OPTIONS_LOADOPTIONS8      , OnOptionsLoadOptions8      )
  ON_COMMAND(ID_OPTIONS_LOADOPTIONS9      , OnOptionsLoadOptions9      )
  ON_COMMAND(ID_OPTIONS_ORGANIZEOPTIONS   , OnOptionsOrganizeOptions   )
  ON_MESSAGE(ID_MSG_RENDER                , OnMsgRender                )
  ON_MESSAGE(ID_MSG_DEBUGGERSTATECHANGED  , OnMsgDebuggerStateChanged  )
  ON_MESSAGE(ID_MSG_KILLDEBUGGER          , OnMsgKillDebugger          )
END_MESSAGE_MAP()

static UINT indicators[] = {
  ID_INDICATOR_POSITION
 ,ID_SEPARATOR
 ,ID_INDICATOR_DEBUG
};

#define INDICATORCOUNT ARRAYSIZE(indicators)

#define REPAINT() Invalidate(FALSE)

CMainFrame::CMainFrame() {
  m_statusPanesVisible = true;
  m_timerRunning       = false;
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
  init3D();
  return 0;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/, CCreateContext *pContext) {
  m_relativeHeight = 0.8;

  VERIFY(m_wndSplitter.CreateStatic(this, 2, 1));
  VERIFY(m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CD3SceneView), CSize(500, 400), pContext));
  VERIFY(m_wndSplitter.CreateView(1, 0, RUNTIME_CLASS(CInfoView   ), CSize(500, 30), pContext));
  return TRUE;
}

void CD3FunctionSplitterWnd::RecalcLayout() {
  getInfoPanel()->enableScroll(false);
  __super::RecalcLayout();
  if(m_splitPointMoved) {
    CMainFrame *mf = theApp.getMainFrame();
    mf->saveRelativeHeight();
    mf->getEditor().setEnabled(true);
    if(mf->isInfoPanelVisible()) {
      mf->startTimer();
    } else {
      mf->stopTimer();
    }
    m_splitPointMoved = false;
  }
  getInfoPanel()->enableScroll(true);
}

void CD3FunctionSplitterWnd::OnInvertTracker(const CRect& rect) {
  __super::OnInvertTracker(rect);
  theApp.getMainFrame()->getEditor().setEnabled(false);
  m_splitPointMoved = true;
}

CD3SceneView *CD3FunctionSplitterWnd::get3DPanel() {
  return (CD3SceneView*)GetPane(0,0);
}

CInfoView *CD3FunctionSplitterWnd::getInfoPanel() {
  return (CInfoView*)GetPane(1, 0);
}

#ifdef _DEBUG
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

  if(loadMenu == NULL) {
    if(optionNames.size() > 0) {
      loadMenu = insertSubMenu(optionsMenu, 12, _T("&Load options"));
    } else {
      return;
    }
  } else {
    removeAllMenuItems(loadMenu);
  }
  const int n = min((int)optionNames.size(), 9);
  for(int i = 0; i < n; i++) {
    insertMenuItem(loadMenu,i, format(_T("%s\tCtrl+%d"), optionNames[i].cstr(), i+1), ID_OPTIONS_LOADOPTIONS+i+1);
  }
}

void CMainFrame::saveRelativeHeight() {
  CRect r = getClientRect(this);
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
    if (SetTimer(1, 5000, NULL) == 1) {
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

void CMainFrame::init3D() {
  m_scene.init(*get3DWindow());
  m_editor.init(this);
  m_editor.setEnabled(true);

  createInitialObject();

  m_scene.setLightDirection(0, rotate(m_scene.getCameraDir(), m_scene.getCameraRight(), 0.2f));
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
  m_function2DSurfaceParam.setName(_T("Saddle"));
  m_function2DSurfaceParam.m_expr = _T("(x*x-y*y)/2");
  m_function2DSurfaceParam.m_xInterval = DoubleInterval(-1, 1);
  m_function2DSurfaceParam.m_yInterval = DoubleInterval(-1, 1);
  m_function2DSurfaceParam.m_pointCount = 20;
  m_function2DSurfaceParam.m_includeTime = false;
  m_function2DSurfaceParam.m_machineCode = true;
  m_function2DSurfaceParam.m_doubleSided = true;

  setCalculatedObject(m_function2DSurfaceParam);
}

class D3AnimatedFunctionSurface : public D3AnimatedSurface {
public:
  D3AnimatedFunctionSurface(D3Scene &scene, const MeshArray &ma) : D3AnimatedSurface(scene, ma) {
  }
  void modifyContextMenu(CMenu &menu) {
    appendMenuItem(menu, _T("Edit function"), ID_OBJECT_EDITFUNCTION);
  }
};

class D3FunctionSurface : public D3SceneObjectWithMesh {
public:
  D3FunctionSurface(D3Scene &scene, LPD3DXMESH mesh) : D3SceneObjectWithMesh(scene, mesh) {
  }
  void modifyContextMenu(CMenu &menu) {
    appendMenuItem(menu, _T("Edit function"), ID_OBJECT_EDITFUNCTION);
  }
};


void CMainFrame::setCalculatedObject(Function2DSurfaceParameters &param) {
  stopDebugging();
  if(param.m_includeTime) {
    D3AnimatedFunctionSurface *obj = new D3AnimatedFunctionSurface(m_scene, createMeshArray(this, m_scene, param)); TRACE_NEW(obj);
    setCalculatedObject(obj, &param);
  } else {
    D3FunctionSurface *obj = new D3FunctionSurface(m_scene, createMesh(m_scene, param)); TRACE_NEW(obj);
    setCalculatedObject(obj, &param);
  }
}

void CMainFrame::setCalculatedObject(ParametricSurfaceParameters &param) {
  stopDebugging();
  if(param.m_includeTime) {
    D3AnimatedFunctionSurface *obj = new D3AnimatedFunctionSurface(m_scene, createMeshArray(this, m_scene, param)); TRACE_NEW(obj);
    setCalculatedObject(obj, &param);
  } else {
    D3FunctionSurface *obj = new D3FunctionSurface(m_scene, createMesh(m_scene, param)); TRACE_NEW(obj);
    setCalculatedObject(obj, &param);
  }
}

void CMainFrame::setCalculatedObject(IsoSurfaceParameters &param) {
  stopDebugging();
  if(param.m_includeTime) {
    D3AnimatedFunctionSurface *obj = new D3AnimatedFunctionSurface(m_scene, createMeshArray(this, m_scene, param)); TRACE_NEW(obj);
    setCalculatedObject(obj, &param);
  } else {
    D3FunctionSurface *obj = new D3FunctionSurface(m_scene, createMesh(m_scene, param)); TRACE_NEW(obj);
    setCalculatedObject(obj, &param);
  }
}

void CMainFrame::deleteCalculatedObject() {
  D3SceneObject *oldObj = getCalculatedObject();
  if (oldObj) {
    m_scene.removeSceneObject(oldObj);
    SAFEDELETE(oldObj);
  }
}

void CMainFrame::setCalculatedObject(D3SceneObject *obj, PersistentData *param) {
  deleteCalculatedObject();
  if(obj) {
    obj->setUserData(param);
    if(param) {
      obj->setName(param->getDisplayName());
    }
    m_scene.addSceneObject(obj);
  }
  m_editor.setCurrentSceneObject(obj);
}

D3SceneObject *CMainFrame::getCalculatedObject() const {
  for(Iterator<D3SceneObject*> it = m_scene.getObjectIterator(); it.hasNext();) {
    D3SceneObject *obj = it.next();
    if(obj->getUserData() != NULL) {
      return obj;
    }
  }
  return NULL;
}

LRESULT CMainFrame::OnMsgRender(WPARAM wp, LPARAM lp) {
  if(wp & RENDER_3D) {
    m_scene.render();
  }
  if(wp & RENDER_INFO) {
    if(m_editor.isEnabled()) {
      show3DInfo(INFO_EDIT);
    }
  }
  return 0;
}

void CMainFrame::ajourDebuggerMenu() {
#ifndef DEBUG_POLYGONIZER
  const bool enable1 = false;
  const bool enable2 = false;
  const bool enable3 = false;
#else
  const bool enable1 = isDebuggerPaused();
  const bool enable2 = hasDebugger();
  const bool enable3 = enable1 && isMenuItemChecked(this, ID_DEBUG_AUTOFOCUSCURRENTCUBE) && m_hasCubeCenter;
#endif // DEBUG_POLYGONIZER

  enableSubMenuContainingId(this, ID_DEBUG_GO, enable1 || enable2);
  enableMenuItem(this, ID_DEBUG_GO           , enable1);
  enableMenuItem(this, ID_DEBUG_STEPLEVEL    , enable1);
  enableMenuItem(this, ID_DEBUG_STEPCUBE     , enable1);
  enableMenuItem(this, ID_DEBUG_STEPTETRA    , enable1);
  enableMenuItem(this, ID_DEBUG_STEPFACE     , enable1);
  enableMenuItem(this, ID_DEBUG_STEPVERTEX   , enable1);
  enableMenuItem(this, ID_DEBUG_AUTOFOCUSCURRENTCUBE, enable1);
  enableMenuItem(this, ID_DEBUG_STOPDEBUGGING, enable2);
  enableSubMenuContainingId(this, ID_DEBUG_ADJUSTCAM_45UP, enable3);
}

#ifndef DEBUG_POLYGONIZER
void CMainFrame::startDebugging() {}
void CMainFrame::stopDebugging() {}
void CMainFrame::OnDebugGo() {}
void CMainFrame::OnDebugStepLevel() {}
void CMainFrame::OnDebugStepCube() {}
void CMainFrame::OnDebugStepTetra() {}
void CMainFrame::OnDebugStepFace() {}
void CMainFrame::OnDebugStepVertex() {}
void CMainFrame::OnDebugStopDebugging() {}
void CMainFrame::OnDebugAutoFocusCurrentCube() {}
void CMainFrame::OnDebugAdjustCam45Up() {}
void CMainFrame::OnDebugAdjustCam45Down() {}
void CMainFrame::OnDebugAdjustCam45Left() {}
void CMainFrame::OnDebugAdjustCam45Right() {}
void CMainFrame::OnDebugMarkCube() {}

LRESULT CMainFrame::OnMsgKillDebugger(WPARAM wp, LPARAM lp) { return 0; }
LRESULT CMainFrame::OnMsgDebuggerStateChanged(WPARAM wp, LPARAM lp) { return 0; }
#endif // DEBUG_POLYGONIZER

void CMainFrame::updateDebugInfo() {
#ifdef DEBUG_POLYGONIZER
  m_debugInfo = format(_T("Debugger State:%-8s"), getDebuggerStateName().cstr());
  if (isDebuggerPaused()) {
    m_debugInfo += format(_T(" Flags:%s"), m_debugger->getFlagNames().cstr());
    if (!m_debugger->isOK()) {
      m_debugInfo += format(_T("\nError:%s"), m_debugger->getErrorMsg().cstr());
    }
    if (m_hasCubeCenter) {
      m_debugInfo += format(_T("\nCubeCenter:(%s), level:%u"), toString(m_cubeCenter, 6).cstr(), m_cubeLevel);
    }
    const DebugIsoSurface       &surf = m_debugger->getDebugSurface();
    const IsoSurfacePolygonizer *poly = surf.getPolygonizer();
    if (poly) {
      const PolygonizerStatistics &stat = poly->getStatistics();
      m_debugInfo += format(_T("\n%s\nCubeCalls:%5u, tetraCals:%5u, level:%u")
        , surf.toString().cstr()
        , stat.m_doCubeCalls, stat.m_doTetraCalls, poly->getCurrentLevel());
    }
  }
#endif
}

void CMainFrame::updateEditorInfo() {
  m_editorInfo = m_editor.toString();
}

void CMainFrame::updateMemoryInfo() {
  const PROCESS_MEMORY_COUNTERS mem = getProcessMemoryUsage();
  const ResourceCounters        res = getProcessResources();
  m_memoryInfo = format(_T("Time:%s Memory:%13s User-obj:%4d GDI-obj:%4d")
                       ,Timestamp().toString(hhmmss).cstr()
                       ,format1000(mem.WorkingSetSize).cstr()
                       ,res.m_userObjectCount
                       ,res.m_gdiObjectCount);
}

void CMainFrame::show3DInfo(BYTE flags) {
  if(!isInfoPanelVisible()) return;
  if(flags & INFO_DEBUG) updateDebugInfo();
  if(flags & INFO_MEM  ) updateMemoryInfo();
  if(flags & INFO_EDIT ) updateEditorInfo();
  showInfo(_T("%s\n%s\n%s"), m_debugInfo.cstr(), m_memoryInfo.cstr(), m_editorInfo.cstr());
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
    CFunction2DSurfaceDlg dlg(m_function2DSurfaceParam);
    if(dlg.DoModal() != IDOK) {
      return;
    }
    m_function2DSurfaceParam = dlg.getData();
    setCalculatedObject(m_function2DSurfaceParam);
    REPAINT();
  } catch(Exception e) {
    showException(e);
  }
}

void CMainFrame::OnFileParametricSurface() {
  try {
    CParametricSurfaceDlg dlg(m_parametricSurfaceParam);
    if(dlg.DoModal() != IDOK) {
      return;
    }
    m_parametricSurfaceParam = dlg.getData();
    setCalculatedObject(m_parametricSurfaceParam);
    REPAINT();
  } catch(Exception e) {
    showException(e);
  }
}

void CMainFrame::OnFileIsoSurface() {
  try {
    CIsoSurfaceDlg dlg(m_isoSurfaceParam);
    if(dlg.DoModal() != IDOK) {
      return;
    }
    m_isoSurfaceParam = dlg.getData();
#ifndef DEBUG_POLYGONIZER
    setCalculatedObject(m_isoSurfaceParam);
    REPAINT();
#else
    if(dlg.getDebugPolygonizer()) {
      startDebugging();
    } else {
      setCalculatedObject(m_isoSurfaceParam);
      REPAINT();
    }
#endif // DEBUG_POLYGONIZER
  } catch(Exception e) {
    showException(e);
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
    //view->refreshBoth();
  } catch(Exception e) {
    showException(e);
  }
}

void CMainFrame::activateOptions() {
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

void CMainFrame::loadOptions(int id) {
  const int index = id - 1;
  Array<Options> optionArray = Options::getExisting();
  if(index >= (int)optionArray.size()) {
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


BOOL CMainFrame::PreTranslateMessage(MSG * pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }
  if(m_editor.PreTranslateMessage(pMsg)) {
    return true;
  }
  return __super::PreTranslateMessage(pMsg);
}
