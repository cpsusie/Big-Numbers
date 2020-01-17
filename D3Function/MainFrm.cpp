#include "stdafx.h"
#include <ProcessTools.h>
#include "MainFrm.h"
#include "EnterOptionsNameDlg.h"
#include "OptionsOrganizerDlg.h"
#include "InfoView.h"

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
  ON_COMMAND(ID_VIEW_STARTTIMER           , OnViewStartTimer           )
  ON_COMMAND(ID_VIEW_STOPTIMER            , OnViewStopTimer            )
END_MESSAGE_MAP()

static UINT indicators[] = {
  ID_INDICATOR_POSITION
 ,ID_SEPARATOR
 ,ID_INDICATOR_DEBUG
};

#define INDICATORCOUNT ARRAYSIZE(indicators)

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
  return 0;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/, CCreateContext *pContext) {
  m_relativeHeight = 0.8;

  VERIFY(m_wndSplitter.CreateStatic(this, 2, 1));
  VERIFY(m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CD3SceneView), CSize(100, 100), pContext));
  VERIFY(m_wndSplitter.CreateView(1, 0, RUNTIME_CLASS(CInfoView   ), CSize(100, 10), pContext));
  return TRUE;
}

void CD3FunctionSplitterWnd::RecalcLayout() {
  getInfoPanel()->enableScroll(false);
  __super::RecalcLayout();
  if(m_splitPointMoved) {
    ((CMainFrame*)GetParent())->saveRelativeHeight();
    m_splitPointMoved = false;
  }
  getInfoPanel()->enableScroll(true);
}

void CD3FunctionSplitterWnd::OnInvertTracker(const CRect& rect) {
  __super::OnInvertTracker(rect);
  m_splitPointMoved = true;
}

CD3SceneView *CD3FunctionSplitterWnd::get3DPanel() {
  return (CD3SceneView*)GetPane(0,0);
}

CInfoView *CD3FunctionSplitterWnd::getInfoPanel() {
  return (CInfoView*)GetPane(1, 0);
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs) {
  if(!__super::PreCreateWindow(cs)) {
    return FALSE;
  }
  return TRUE;
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
  if (m_timerRunning) {
    KillTimer(1);
    m_timerRunning = false;
  }
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent) {
  show3DInfo(INFO_MEM);
  __super::OnTimer(nIDEvent);
}

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
//  m_editorInfo = m_editor.toString(); TODO
}

void CMainFrame::updateMemoryInfo() {
  const PROCESS_MEMORY_COUNTERS mem = getProcessMemoryUsage();
  const ResourceCounters        res = getProcessResources();
  m_memoryInfo += format(_T("Time:%s Memory:%13s User-obj:%4d GDI-obj:%4d\n")
                       ,Timestamp().toString(hhmmss).cstr()
                       ,format1000(mem.WorkingSetSize).cstr()
                       ,res.m_userObjectCount
                       ,res.m_gdiObjectCount);
}

void CMainFrame::show3DInfo(BYTE flags) {
  if(!m_infoPanelVisible) return;
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


void CMainFrame::OnViewStartTimer() {
  startTimer();
}


void CMainFrame::OnViewStopTimer() {
  stopTimer();
}
