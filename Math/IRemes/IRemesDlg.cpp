#include "stdafx.h"
#include <ExternProcess.h>
#include <ThreadPool.h>
#include <Thread.h>
#include <Math/BigReal/BigRealResourcePool.h>
#include "PrecisionDlg.h"
#include "IRemesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialog {
public:
  CAboutDlg() : CDialog(IDD) {
  }
  enum { IDD = IDD_ABOUTBOX };
protected:
  DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CIRemesDlg::CIRemesDlg(CWnd *pParent /*=NULL*/)
: CDialog(IDD, pParent)
, m_name(EMPTYSTRING)
, m_maxSearchEIterations(0)
{
  m_M                      = 6;
  m_K                      = 6;
  m_MTo                    = m_M;
  m_KTo                    = m_K;
  m_xFrom                  = -0.5;
  m_xTo                    = 0.5;
  m_digits                 = 35;
  m_maxSearchEIterations   = 700;
  m_maxMKSum               = 0;
  m_relativeError          = TRUE;
  m_skipExisting           = TRUE;
/*
  m_M                      = ;
  m_K                      = 1;
  m_MTo                    = m_M;
  m_KTo                    = m_K;
  m_xFrom                  = 0.0;
  m_xTo                    = 1.0;
  m_digits                 = 19;
  m_maxSearchEIterations   = 700;
  m_maxMKSum               = 0;
  m_relativeError          = FALSE;
  m_skipExisting           = FALSE;
*/
  m_hIcon                  = theApp.LoadIcon(IDR_MAINFRAME);
  m_remes                  = NULL;
  m_allowRemesProperties   = false;
  m_debugger               = NULL;
  m_errorPlotter           = NULL;
  m_errorPlotTimerRunning  = false;
  m_reduceToInterpolate    = false;
  m_runMenuState           = RUNMENU_EMPTY;
  m_debuggerState          = DEBUGGER_TERMINATED;
  m_subM = m_subK          = -1;
}

void CIRemesDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDITNAME, m_name);
  DDX_Text(pDX, IDC_EDITMFROM, m_M);
  DDV_MinMaxUInt(pDX, m_M, 1, 100);
  DDX_Text(pDX, IDC_EDITMTO, m_MTo);
  DDX_Text(pDX, IDC_EDITKFROM, m_K);
  DDV_MinMaxUInt(pDX, m_K, 0, 100);
  DDX_Text(pDX, IDC_EDITKTO, m_KTo);
  DDX_Text(pDX, IDC_EDITXFROM, m_xFrom);
  DDX_Text(pDX, IDC_EDITXTO, m_xTo);
  DDX_Text(pDX, IDC_EDITDIGITS, m_digits);
  DDV_MinMaxUInt(pDX, m_digits, 2, 200);
  DDX_Text(pDX, IDC_EDITMAXSEARCHEITERATIONS, m_maxSearchEIterations);
  DDV_MinMaxUInt(pDX, m_maxSearchEIterations, 2, 20000);
  DDX_Check(pDX, IDC_CHECKRELATIVEERROR, m_relativeError);
  DDX_Text(pDX, IDC_EDITMAXMKSUM, m_maxMKSum);
  DDX_Check(pDX, IDC_CHECKSKIPEXISTING, m_skipExisting);
}

BEGIN_MESSAGE_MAP(CIRemesDlg, CDialog)
  ON_WM_SYSCOMMAND()
  ON_WM_QUERYDRAGICON()
  ON_WM_PAINT()
  ON_WM_SIZE()
  ON_WM_CLOSE()
  ON_COMMAND(ID_FILE_SHOWMAXERRORS           , OnFileShowMaxErrors         )
  ON_COMMAND(ID_FILE_EXIT                    , OnFileExit                  )
  ON_COMMAND(ID_VIEW_GRID                    , OnViewGrid                  )
  ON_COMMAND(ID_VIEW_SHOW_ERRORFUNCTION      , OnViewShowErrorFunction     )
  ON_COMMAND(ID_VIEW_SHOW_SPLINE             , OnViewShowSpline            )
  ON_COMMAND(ID_VIEW_DISPLAYEDPRECISION      , OnViewDisplayedPrecision    )
  ON_COMMAND(ID_GENERATECODE_CCODED64        , OnGenerateCcodeD64          )
  ON_COMMAND(ID_GENERATECODE_CCODED80        , OnGenerateCcodeD80          )
  ON_COMMAND(ID_GENERATECODE_JAVAD64         , OnGenerateJavacodeD64       )
  ON_COMMAND(ID_RUN_GO                       , OnRunGo                     )
  ON_COMMAND(ID_RUN_F5                       , OnRunF5                     )
  ON_COMMAND(ID_RUN_RESTART                  , OnRunRestart                )
  ON_COMMAND(ID_RUN_STOPDEBUGGING            , OnRunStopDebugging          )
  ON_COMMAND(ID_RUN_BREAK                    , OnRunBreak                  )
  ON_COMMAND(ID_RUN_SINGLEITERATION          , OnRunSingleIteration        )
  ON_COMMAND(ID_RUN_SINGLESUBITERATION       , OnRunSingleSubIteration     )
  ON_COMMAND(ID_RUN_REDUCETOINTERPOLATE      , OnRunReduceToInterpolate    )
  ON_COMMAND(ID_HELP_ABOUTIREMES             , OnHelpAboutIRemes           )
  ON_COMMAND(ID_GOTO_DOMAIN                  , OnGotoDomain                )
  ON_COMMAND(ID_GOTO_M                       , OnGotoM                     )
  ON_COMMAND(ID_GOTO_K                       , OnGotoK                     )
  ON_COMMAND(ID_GOTO_DIGITS                  , OnGotoDigits                )
  ON_COMMAND(ID_GOTO_MAXSEARCHEITERATIONS    , OnGotoMaxSearchEIterations  )
  ON_MESSAGE(ID_MSG_DEBUGGERSTATE_CHANGED    , OnMsgDebuggerStateChanged   )
  ON_MESSAGE(ID_MSG_REMES_STATE_CHANGED      , OnMsgRemesStateChanged      )
  ON_MESSAGE(ID_MSG_COEFFICIENTS_CHANGED     , OnMsgCoefficientsChanged    )
  ON_MESSAGE(ID_MSG_SEARCHEITERATION_CHANGED , OnMsgSearchEIterationChanged)
  ON_MESSAGE(ID_MSG_EXTREMACOUNT_CHANGED     , OnMsgExtremaCountChanged    )
  ON_MESSAGE(ID_MSG_SHOWERRORFUNCTION        , OnMsgShowErrorFunction      )
  ON_MESSAGE(ID_MSG_CLEARERRORFUNCTION       , OnMsgClearErrorFunction     )
  ON_MESSAGE(ID_MSG_UPDATEINTERPOLATION      , OnMsgUpdateInterpolation    )
  ON_MESSAGE(ID_MSG_WARNING_CHANGED          , OnMsgWarningChanged         )
  ON_MESSAGE(ID_MSG_APPROXIMATION_CHANGED    , OnMsgApproximationChanged   )
  ON_MESSAGE(ID_MSG_ERRORPOINTARRAY_CHANGED  , OnMsgErrorPointArrayChanged )
  ON_MESSAGE(ID_MSG_MAXERROR_CHANGED         , OnMsgMaxErrorChanged        )
  ON_EN_KILLFOCUS(IDC_EDITMFROM              , OnEnKillfocusEditmFrom      )
  ON_EN_KILLFOCUS(IDC_EDITKFROM              , OnEnKillfocusEditkFrom      )
  ON_EN_KILLFOCUS(IDC_EDITMTO                , OnEnKillfocusEditmTo        )
  ON_EN_KILLFOCUS(IDC_EDITKTO                , OnEnKillfocusEditkTo        )
  ON_EN_UPDATE(IDC_EDITKTO                   , OnEnUpdateEditkTo           )
  ON_EN_UPDATE(IDC_EDITMTO                   , OnEnUpdateEditmTo           )
  ON_WM_TIMER()
END_MESSAGE_MAP()

void CIRemesDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg().DoModal();
  } else {
    __super::OnSysCommand(nID, lParam);
  }
}

HCURSOR CIRemesDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

BOOL CIRemesDlg::OnInitDialog() {
  __super::OnInitDialog();

  ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
  ASSERT(IDM_ABOUTBOX < 0xF000);

  CMenu *pSysMenu = GetSystemMenu(FALSE);
  if(pSysMenu != NULL) {
    CString strAboutMenu;
    strAboutMenu.LoadString(IDS_ABOUTBOX);
    if(!strAboutMenu.IsEmpty()) {
      pSysMenu->AppendMenu(MF_SEPARATOR);
      pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
    }
  }

  SetIcon(m_hIcon, TRUE);
  SetIcon(m_hIcon, FALSE);

  m_accelTable = LoadAccelerators(theApp.m_hInstance, MAKEINTRESOURCE(IDR_MAINFRAME));

  m_coefListBox.substituteControl(this, IDC_LISTCOEF);
  m_coorSystemError.substituteControl(   this, IDC_FRAME_COORSYSTEM_ERROR);
  m_coorSystemError.setRetainAspectRatio(false);
  m_coorSystemError.setDataRange(DataRange(m_xFrom, m_xTo, -1, 1), false);
  m_coorSystemError.setAutoScale(true, false);

  m_coorSystemSpline.substituteControl(   this, IDC_FRAME_COORSYSTEM_SPLINE);
  m_coorSystemSpline.setRetainAspectRatio(false);
  m_coorSystemSpline.setDataRange(DataRange(0, 1, 0, 1), false);
  m_coorSystemSpline.setAutoScale(true, false);

  setErrorFunctionVisible(isMenuItemChecked(this, ID_VIEW_SHOW_ERRORFUNCTION));
  setSplineVisible(       isMenuItemChecked(this, ID_VIEW_SHOW_SPLINE));

  m_layoutManager.OnInitDialog(this);
  m_layoutManager.addControl(IDC_STATICSEARCHE          ,                                                          PCT_RELATIVE_BOTTOM );
  m_layoutManager.addControl(IDC_LISTCOEF               , PCT_RELATIVE_RIGHT                  |                    PCT_RELATIVE_BOTTOM );
  m_layoutManager.addControl(IDC_LISTEXTRMA             , PCT_RELATIVE_LEFT  | RELATIVE_RIGHT |                    PCT_RELATIVE_BOTTOM );
  m_layoutManager.addControl(IDC_FRAME_COORSYSTEM_ERROR , PCT_RELATIVE_RIGHT                  | PCT_RELATIVE_TOP | RELATIVE_BOTTOM     );
  m_layoutManager.addControl(IDC_FRAME_COORSYSTEM_SPLINE, PCT_RELATIVE_LEFT  | RELATIVE_RIGHT | PCT_RELATIVE_TOP | RELATIVE_BOTTOM     );

  if(BigReal::pow2CacheHasFile()) {
    BigReal::pow2CacheLoad();
  }
  m_name  = m_targetFunction.getDefaultName().cstr();
  m_xFrom = m_targetFunction.getDefaultDomain().getFrom();
  m_xTo   = m_targetFunction.getDefaultDomain().getTo();
  UpdateData(0);
  adjustMaxMKSum();
  gotoEditBox(this, IDC_EDITNAME);
  SETTHREADDESCRIPTION("WinThread");
  return TRUE;
}

void CIRemesDlg::OnPaint() {
  if(IsIconic())  {
    CPaintDC dc(this); // device context for painting

    SendMessage(WM_ICONERASEBKGND, (WPARAM)dc.GetSafeHdc(), 0);

    // Center icon in client rectangle
    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width() - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;

    dc.DrawIcon(x, y, m_hIcon);
  } else {
    __super::OnPaint();
    ajourDialogItems(m_debuggerState);
  }
}

void CIRemesDlg::setErrorFunctionVisible(bool visible) {
  if(visible && !m_debugInfo.isApproxEmpty()) {
    PostMessage(ID_MSG_SHOWERRORFUNCTION, 0, 0);
  } else {
    PostMessage(ID_MSG_CLEARERRORFUNCTION, 0, 0);
  }
}

void CIRemesDlg::setSplineVisible(bool visible) {
  CRect errorRect  = getWindowRect(this, IDC_FRAME_COORSYSTEM_ERROR);
  if(visible) {
    CRect splineRect = getWindowRect(this, IDC_FRAME_COORSYSTEM_SPLINE);
    errorRect.right  = splineRect.left-1;
    setWindowRect(this, IDC_FRAME_COORSYSTEM_ERROR, errorRect);
    GetDlgItem(IDC_FRAME_COORSYSTEM_SPLINE)->ShowWindow(SW_SHOW);
  } else {
    GetDlgItem(IDC_FRAME_COORSYSTEM_SPLINE)->ShowWindow(SW_HIDE);
    const CRect cr = getClientRect(this);
    errorRect.right = cr.right;
    setWindowRect(this, IDC_FRAME_COORSYSTEM_ERROR, errorRect);
  }
}

bool CIRemesDlg::isErrorFunctionVisible() {
  return isMenuItemChecked(this, ID_VIEW_SHOW_ERRORFUNCTION);
}
bool CIRemesDlg::isSplineVisible() {
  return isMenuItemChecked(this, ID_VIEW_SHOW_SPLINE);
}

void CIRemesDlg::showDebuggerState(DebuggerState state) {
  String str;
  bool resetRemesState = false;
  if(hasDebugger()) {
    str = Debugger::getStateName(state);
  } else {
    str = _T("No thread");
  }
  m_debuggerState = state;

  if(resetRemesState) {
    showRemesState(EMPTYSTRING);
  }
  setWindowText(this, IDC_STATICDEBUGGERSTATE, str);
}

void CIRemesDlg::showRemesState(const String &str) {
  setWindowText(this, IDC_STATICREMESSTATE, str);
}

void CIRemesDlg::showWarning(const String &str) {
  setWindowText(this, IDC_STATICWARNING, str);
}

void CIRemesDlg::showCoefWindowData(const CoefWindowData &data) {
  m_coefListBox.setLines(data.m_coefStrings);
  m_coefListBox.Invalidate();
}

void CIRemesDlg::showExtremaStringArray(const ExtremaStringArray &a) {
  if(a != m_extrStrArrayOld) {
    CListBox *lb = (CListBox*)GetDlgItem(IDC_LISTEXTRMA);
    const size_t n = a.size();
    if(a.size() != m_extrStrArrayOld.size()) {
      lb->ResetContent();
      for(size_t i = 0; i < n; i++) {
        lb->AddString(a.getString(i).cstr());
      }
    } else {
      for(size_t i = 0; i < n; i++) {
        const String ai = a.getString(i);
        if(ai != m_extrStrArrayOld.getString(i)) {
          lb->DeleteString((UINT)i);
          lb->InsertString((UINT)i, ai.cstr());
        }
      }
    }
    m_extrStrArrayOld = a;
  }
}

void CIRemesDlg::clearErrorPointArray() {
  m_debugInfo.clearPointArray();
}

class ErrorPlotter : public SafeRunnable {
private:
  CIRemesDlg       &m_dlg;
  bool              m_requestTerminate;
  DigitPool        *m_digitPool;
  float             m_progress; // in pct
public:
  ErrorPlotter(CIRemesDlg *dlg) : m_dlg(*dlg), m_requestTerminate(false) {
    m_digitPool = BigRealResourcePool::fetchDigitPool();
    m_progress  = 0;
  }
  ~ErrorPlotter() {
    requestTerminate();
    waitUntilJobDone();
    BigRealResourcePool::releaseDigitPool(m_digitPool);
  }
  void requestTerminate() {
    if(!m_requestTerminate) {
      m_requestTerminate = true;
      m_digitPool->terminatePoolCalculation();
    }
  }
  UINT safeRun();
  float getProgress() const {
    return m_progress;
  }
};

UINT ErrorPlotter::safeRun() {
  SETTHREADDESCRIPTION("ErrorPlotter");
  m_progress = 0;
  try {
    DebugInfo       &info = m_dlg.m_debugInfo;
    RationalFunction f;
    Remes           *remes = m_dlg.m_remes;
    info.getApproximation(f);
    Point2DArray pa;
    m_dlg.startErrorPlotTimer();
    remes->getErrorPlot(f, m_dlg.getErrorPlotXPixelCount(), pa, m_digitPool, &m_progress);
    info.setPointArray(pa, f.getCoefVectorIndex());
    m_dlg.PostMessage(ID_MSG_ERRORPOINTARRAY_CHANGED, 0, 0);
    m_dlg.stopErrorPlotTimer();
  } catch (...) {
    m_dlg.stopErrorPlotTimer();
    throw;
  }
  return 0;
}

void CIRemesDlg::startErrorPlotter() {
  stopErrorPlotter();
  if(!hasRemes()) return;
  m_errorPlotter = new ErrorPlotter(this);
  ThreadPool::executeNoWait(*m_errorPlotter);
}
void CIRemesDlg::stopErrorPlotter() {
  if(hasErrorPlotter()) {
    stopErrorPlotTimer();
    m_errorPlotter->requestTerminate();
    SAFEDELETE(m_errorPlotter);
  }
}
void CIRemesDlg::OnTimer(UINT_PTR nIDEvent) {
  if(!hasErrorPlotter()) return;
  float progressPct = m_errorPlotter->getProgress();
  CClientDC dc(&m_coorSystemError);
  textOut(dc, 10, 10, format(_T("Progress:%.1f  "), progressPct));
  __super::OnTimer(nIDEvent);
}

void CIRemesDlg::createErrorPointArray() {
  RationalFunction f;
  m_debugInfo.getApproximation(f);
  if(f.isEmpty()) {
    clearErrorPointArray();
    removeErrorPlot();
    return;
  }
  if(f.getCoefVectorIndex() != m_debugInfo.getPointArrayKey()) {
    startErrorPlotter();
  }
}

void CIRemesDlg::showErrorPointArray() {
  m_coorSystemError.deleteAllObjects();
  Point2DArray pa;
  m_debugInfo.getPointArray(pa);
  if(!pa.isEmpty()) {
    m_coorSystemError.addPointObject(pa);
    m_coorSystemError.Invalidate(FALSE);
  }
}

void CIRemesDlg::removeErrorPlot() {
  const int count = m_coorSystemError.getObjectCount();
  if(count > 0) {
    m_coorSystemError.deleteAllObjects();
    m_coorSystemError.Invalidate(FALSE);
  }
}

void CIRemesDlg::startErrorPlotTimer() {
  if(!m_errorPlotTimerRunning && SetTimer(1, 1000, NULL)) {
    m_errorPlotTimerRunning = true;
  }
}

void CIRemesDlg::stopErrorPlotTimer() {
  if(m_errorPlotTimerRunning) {
    KillTimer(1);
    m_errorPlotTimerRunning = false;
  }
}

void CIRemesDlg::updateErrorPlotXRange() {
  const DataRange dr = m_coorSystemError.getDataRange();
  m_coorSystemError.setDataRange(DataRange(m_xFrom, m_xTo, dr.getMinY(), dr.getMaxY()), false);
  clearErrorPointArray();
}

int CIRemesDlg::getErrorPlotXPixelCount() const {
  const IntervalTransformation &tr = m_coorSystemError.getTransformation().getXTransformation();
  const DoubleInterval xToRange = tr.forwardTransform(getXRange());
  const int n = (int)xToRange.getLength()+1;
  return (n <= 0) ? 1 : n;
}

void CIRemesDlg::showSearchEString(const String &s) {
  setWindowText(this, IDC_STATICSEARCHE, s);
}

void CIRemesDlg::OnSize(UINT nType, int cx, int cy) {
  __super::OnSize(nType, cx, cy);
  m_layoutManager.OnSize(nType, cx, cy);
}

BOOL CIRemesDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd, m_accelTable, pMsg)) {
    return true;
  }
  return __super::PreTranslateMessage(pMsg);
}

void CIRemesDlg::enableFieldList(const int *ids, int n, bool enabled) {
  while(n--) {
    GetDlgItem(*(ids++))->EnableWindow(enabled ? TRUE : FALSE);
  }
}

#define ENABLEFIELDLIST(a,enabled) enableFieldList(a, ARRAYSIZE(a), enabled)

void CIRemesDlg::ajourDialogItems(DebuggerState state) {
  static const int dialogFields[] = {
    IDC_EDITNAME
   ,IDC_EDITMFROM
   ,IDC_EDITKFROM
   ,IDC_EDITMTO
   ,IDC_EDITKTO
   ,IDC_EDITMAXMKSUM
   ,IDC_EDITXFROM
   ,IDC_EDITXTO
   ,IDC_EDITDIGITS
   ,IDC_EDITMAXSEARCHEITERATIONS
   ,IDC_CHECKRELATIVEERROR
  };

  showDebuggerState(state);
  if(hasDebugger()) {
    switch(state) {
    case DEBUGGER_RUNNING    :
      ENABLEFIELDLIST(dialogFields, false);
      setRunMenuState(RUNMENU_RUNNING);
      break;

    case DEBUGGER_CREATED    :
    case DEBUGGER_PAUSED     :
      ENABLEFIELDLIST(dialogFields, false);
      setRunMenuState(RUNMENU_PAUSED);
      break;

    case DEBUGGER_TERMINATED :
      ENABLEFIELDLIST(dialogFields, true);
      setRunMenuState(RUNMENU_IDLE);
      break;
    }
  } else { // no debug thread
    ENABLEFIELDLIST(dialogFields, true);
    setRunMenuState(RUNMENU_IDLE);
  }
}

typedef struct {
  const TCHAR *text;
  int          id;
} MenuLabel;

#define MLABEL(s, id) _T(s), id

static const MenuLabel runMenuLables[] = {
  MLABEL("&Go\tCtrl+F5"                       , ID_RUN_GO                  ) // 0
 ,MLABEL("&Debug\tF5"                         , ID_RUN_F5                  ) // 1
 ,MLABEL("&Continue\tF5"                      , ID_RUN_F5                  ) // 2
 ,MLABEL("&Restart\tCtrl+Shift+F5"            , ID_RUN_RESTART             ) // 3
 ,MLABEL("Stop Debugging\tShift+F5"           , ID_RUN_STOPDEBUGGING       ) // 4
 ,MLABEL("&Break\tF9"                         , ID_RUN_BREAK               ) // 5
 ,MLABEL("Single &iteration\tF10"             , ID_RUN_SINGLEITERATION     ) // 6
 ,MLABEL("Single s&ub iteration\tF11"         , ID_RUN_SINGLESUBITERATION  ) // 7
 ,MLABEL("Reduce to interpolate extrema\tNum-", ID_RUN_REDUCETOINTERPOLATE ) // 8
};

#define ITEM_START     0
#define ITEM_DEBUG     1
#define ITEM_CONTINUE  2
#define ITEM_RESTART   3
#define ITEM_STOP      4
#define ITEM_BREAK     5
#define ITEM_SINGLEIT  6
#define ITEM_SUBIT     7
#define ITEM_REDUCE    8
#define ITEM_SEPARATOR 9

static const int runMenuIdleItems[] = {
  ITEM_START
 ,ITEM_DEBUG
 ,-1
};

static const int runMenuRunningItems[] = {
  ITEM_BREAK
 ,ITEM_STOP
 ,ITEM_SEPARATOR
 ,ITEM_REDUCE
 ,-1
};

static const int runMenuPausedItems[] = {
  ITEM_CONTINUE
 ,ITEM_STOP
 ,ITEM_SEPARATOR
 ,ITEM_RESTART
 ,ITEM_SEPARATOR
 ,ITEM_SINGLEIT
 ,ITEM_SUBIT
 ,-1
};

static const int * const runMenuItems[] = {
  NULL
 ,runMenuIdleItems
 ,runMenuRunningItems
 ,runMenuPausedItems
};

void CIRemesDlg::setRunMenuState(RunMenuState menuState) {
  if(menuState == m_runMenuState) return;
  m_runMenuState = menuState;
  int index;
  HMENU mainMenu = findMenuByString(*GetMenu(), _T("Run"), &index);

  HMENU runMenu = GetSubMenu(mainMenu, index);
  if(runMenu == 0) return;
  removeAllMenuItems(runMenu);
  const int *menuItems = runMenuItems[menuState];
  if(menuItems == NULL) return;
  int count = 0;
  for(const int *item = menuItems; *item >= 0; item++,count++) {
    if(*item == ITEM_SEPARATOR) {
      insertMenuSeparator(runMenu, count);
    } else {
      const MenuLabel &label = runMenuLables[*item];
      insertMenuItem(runMenu, count, label.text, label.id);
    }
  }
}

void CIRemesDlg::OnOK() {
}

void CIRemesDlg::OnCancel() {
}

void CIRemesDlg::OnClose() {
  OnFileExit();
}

void CIRemesDlg::OnFileExit() {
  deallocateAll();
  EndDialog(IDOK);
}

void CIRemesDlg::deallocateAll() {
  destroyDebugger();
  removeErrorPlot();
  m_debugInfo.clear();
  Remes::releaaseExtremaMap();
}

void CIRemesDlg::OnFileShowMaxErrors() {
  OnRunDebug();
  const ExtremaMap &map = m_remes->getExtremaMap();
  const String tmpFileName = _T("c:\\temp\\RemesErrors.txt");
  FILE *f = MKFOPEN(tmpFileName, _T("w"));
  for(Iterator<ExtremaMapEntry> it = map.getIerator(); it.hasNext();) {
    ExtremaMapEntry &e = it.next();
    const ExtremaKey           &key = e.getKey();
    const Array<ExtremaVector> &v   = e.getValue();
    const int    x = key.getM();
    const int    y = key.getK();
    const double z = fabs(getDouble(v[0].getE()));
    _ftprintf(f, _T("%2d %2d %lf\n"), x, y, log(z));
  }
  fclose(f);
//  system("c:\\windows\\System32\\notepad %s")
}

static void setGrid(CCoordinateSystem &s, bool on) {
  AxisAttribute &xattr = s.getAxisAttr(XAXIS_INDEX);
  AxisAttribute &yattr = s.getAxisAttr(YAXIS_INDEX);
  if(on) xattr.setFlags(AXIS_SHOW_GRIDLINES, 0); else xattr.setFlags(0, AXIS_SHOW_GRIDLINES);
  if(on) yattr.setFlags(AXIS_SHOW_GRIDLINES, 0); else yattr.setFlags(0, AXIS_SHOW_GRIDLINES);
}

void CIRemesDlg::OnViewGrid() {
  const bool showGrid = toggleMenuItem(this, ID_VIEW_GRID);
  setGrid(m_coorSystemError , showGrid);
  setGrid(m_coorSystemSpline, showGrid);
  Invalidate(FALSE);
}

void CIRemesDlg::OnViewShowErrorFunction() {
  setErrorFunctionVisible(toggleMenuItem(this, ID_VIEW_SHOW_ERRORFUNCTION));
}

void CIRemesDlg::OnViewShowSpline() {
  setSplineVisible(toggleMenuItem(this, ID_VIEW_SHOW_SPLINE));
}

void CIRemesDlg::OnViewDisplayedPrecision() {
  CPrecisionDlg dlg(m_visiblePrecisions);
  if(dlg.DoModal() == IDOK) {
    m_visiblePrecisions = dlg.getVisiblePrecisions();
    if(hasDebugger()) {
      m_remes->setVisiblePrecisions(m_visiblePrecisions);
    }
  }
}

static void showText(const String &str) {
  const String fileName = _T("c:\\temp\\fisk.txt");
  FILE *f = MKFOPEN(fileName, _T("w"));
  _ftprintf(f, _T("%s"), str.cstr());
  fclose(f);
  ExternProcess::run(false, _T("c:\\windows\\system32\\notepad.exe"), fileName.cstr(), NULL);
}

void CIRemesDlg::OnGenerateCcodeD64() {
  if(hasSolution()) showText(m_remes->getCFunctionString(false));
}

void CIRemesDlg::OnGenerateCcodeD80() {
  if(hasSolution()) showText(m_remes->getCFunctionString(true));
}

void CIRemesDlg::OnGenerateJavacodeD64() {
  if(hasSolution()) showText(m_remes->getJavaFunctionString());
}

void CIRemesDlg::OnRunGo() {
  startDebugger(false);
}

void CIRemesDlg::OnRunF5() {
  switch(m_runMenuState) {
  case RUNMENU_IDLE  : OnRunDebug();    break;
  case RUNMENU_PAUSED: OnRunContinue(); break;
  }
}

void CIRemesDlg::OnRunDebug()    { startDebugger(true);                      }
void CIRemesDlg::OnRunContinue() { if(isDebuggerPaused()) m_debugger->go();  }

void CIRemesDlg::OnRunRestart() {
  OnRunStopDebugging();
  if(!hasDebugger() || isDebuggerTerminated()) {
    OnRunGo();
  }
}

void CIRemesDlg::OnRunStopDebugging() {
  try {
    if(hasDebugger()) {
      m_debugger->kill();
    }
  } catch(Exception e) {
    showException(e);
  }
}

void CIRemesDlg::OnRunBreak()                 { if(hasDebugger())       m_debugger->breakASAP();                 }
void CIRemesDlg::OnRunSingleIteration()       { if(isDebuggerPaused())  m_debugger->singleStep(FL_BREAKSTEP);    }
void CIRemesDlg::OnRunSingleSubIteration()    { if(isDebuggerPaused())  m_debugger->singleStep(FL_BREAKSUBSTEP); }
void CIRemesDlg::OnRunReduceToInterpolate()   { if(isDebuggerRunning()) m_reduceToInterpolate = true; }
void CIRemesDlg::OnGotoDomain()               { gotoEditBox(this, IDC_EDITXFROM               );      }
void CIRemesDlg::OnGotoM()                    { gotoEditBox(this, IDC_EDITMFROM               );      }
void CIRemesDlg::OnGotoK()                    { gotoEditBox(this, IDC_EDITKFROM               );      }
void CIRemesDlg::OnGotoDigits()               { gotoEditBox(this, IDC_EDITDIGITS              );      }
void CIRemesDlg::OnGotoMaxSearchEIterations() { gotoEditBox(this, IDC_EDITMAXSEARCHEITERATIONS);      }

void CIRemesDlg::OnEnKillfocusEditmFrom() {
  UINT mFrom, mTo;
  if(!getEditValue(this, IDC_EDITMFROM, mFrom)) return;
  if(!getEditValue(this, IDC_EDITMTO  , mTo  )) return;
  mTo = max(mFrom, mTo);
  setEditValue(this, IDC_EDITMTO, mTo);
  adjustMaxMKSum();
}

void CIRemesDlg::OnEnKillfocusEditkFrom() {
  UINT kFrom, kTo;
  if(!getEditValue(this,IDC_EDITKFROM, kFrom)) return;
  if(!getEditValue(this,IDC_EDITKTO  , kTo  )) return;
  kTo = max(kFrom, kTo);
  setEditValue(this, IDC_EDITKTO, kTo);
  adjustMaxMKSum();
}

void CIRemesDlg::OnEnKillfocusEditmTo() { adjustMaxMKSum(); }
void CIRemesDlg::OnEnKillfocusEditkTo() { adjustMaxMKSum(); }
void CIRemesDlg::OnEnUpdateEditmTo()    { adjustMaxMKSum(); }
void CIRemesDlg::OnEnUpdateEditkTo()    { adjustMaxMKSum(); }

void CIRemesDlg::adjustMaxMKSum() {
  UINT maxM, maxK;
  if(!getEditValue(this, IDC_EDITMTO, maxM)) return;
  if(!getEditValue(this, IDC_EDITKTO, maxK)) return;
  const UINT MKsum = maxM + maxK;
  setEditValue(this, IDC_EDITMAXMKSUM, MKsum);
}

void CIRemesDlg::handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) {
  if(source == m_debugger) {
    switch(id) {
    case DEBUGGER_STATE:
      PostMessage(ID_MSG_DEBUGGERSTATE_CHANGED, *(DebuggerState*)oldValue, *(DebuggerState*)newValue);
      break;
    case REMES_PROPERTY:
      if(!m_allowRemesProperties) break;
      { const RemesPropertyData &data = *(RemesPropertyData*)newValue; // oldValue = NULL
        handleRemesProperty(data.m_src, data.m_id, data.m_oldValue, data.m_newValue);
        break;
      }
    default:
      showError(_T("Unknown property id:%d"), id);
      break;
    }
  }
}

void CIRemesDlg::handleRemesProperty(const Remes &r, int id, const void *oldValue, const void *newValue) {
  if(m_reduceToInterpolate) {
    r.reduceToInterpolate();
    m_reduceToInterpolate = false;
  }

  switch(id) {
  case REMES_STATE        : // *RemesState
    { const RemesState oldState = *(RemesState*)oldValue;
      const RemesState newState = *(RemesState*)newValue;
      String str = r.getTotalStateString();
      m_debugInfo.setRemesStateString(str);
      PostMessage(ID_MSG_REMES_STATE_CHANGED, r.getM(), r.getK());
      if(oldState == REMES_SEARCH_COEFFICIENTS) {
        CoefWindowData cwd = r;
        str = r.getSearchEString();
        m_debugInfo.setCoefWinData(  cwd);
        m_debugInfo.setSearchEString(str);
        PostMessage(ID_MSG_COEFFICIENTS_CHANGED    , 0, 0);
        PostMessage(ID_MSG_SEARCHEITERATION_CHANGED, 0, 0);
      }
    }
    break;

  case SEARCHEITERATION   : // *int
    { const String str = r.getSearchEString();
      m_debugInfo.setSearchEString(str);
      PostMessage(ID_MSG_SEARCHEITERATION_CHANGED, 0, 0);
    }
    break;

  case EXTREMACOUNT       : // *int
    { const ExtremaStringArray a = r.getExtremaStringArray();
      m_debugInfo.setExtremaStringArray(a);
      PostMessage(ID_MSG_EXTREMACOUNT_CHANGED, 0, 0);
    }
    break;

  case MAINITERATION      : // *int
  case Q                  : // *BigReal
  case E                  : // *BigReal
  case COEFFICIENTVECTOR  : // *BigRealVector
  case MMQUOT             : // *BigReal
    break;
  case INTERPOLATIONSPLINE: // *dim, *Function
#ifdef __TODO__
    if(isSplineVisible()) {
      const UINT           dim = *(UINT*)oldValue;
      Function            &f   = *(Function*)newValue;
      const DoubleInterval xRange(0, dim-1);

      m_gate.wait();
      m_coorSystemSpline.deleteAllObjects();
      m_coorSystemSpline.addFunctionObject(f, &xRange);
      m_gate.notify();
      SendMessage(ID_MSG_UPDATEINTERPOLATION, 0, 0);
    }
#endif
    break;
  case CURRENTAPPROX      : // *RationalFunction
    { RationalFunction rf;
      r.getCurrentApproximation(rf);
      m_debugInfo.setApproximation(rf);
      PostMessage(ID_MSG_APPROXIMATION_CHANGED, 0, 0);
    }
    break;
  case MAXERROR           : // *BigReal
    { const double maxError = getDouble(r.getMaxError(), false);
      m_debugInfo.setMaxError(maxError);
      PostMessage(ID_MSG_MAXERROR_CHANGED, 0, 0);
    }
    break;
  case WARNING            : // *String
    { const String &str = *(String*)newValue;
      m_debugInfo.setWarningString(str);
      PostMessage(ID_MSG_WARNING_CHANGED, 0, 0);
    }
    break;
  default:
    showError(_T("Unknown remes property id:%d"), id);
    break;
  }
}

LRESULT CIRemesDlg::OnMsgDebuggerStateChanged(WPARAM wp, LPARAM lp) {
  const DebuggerState state = (DebuggerState)lp;
  ajourDialogItems(state);
  if(state == DEBUGGER_TERMINATED) {
    BigRealResourcePool::resetAllPoolCalculations();
    if(!m_debugger->isOk()) {
      showWarning(m_debugger->getErrorMsg());
    }
  }
  return 0;
}

LRESULT CIRemesDlg::OnMsgWarningChanged(WPARAM wp, LPARAM lp) {
  String s;
  m_debugInfo.getWarningString(s);
  showWarning(s);
  return 0;
}

LRESULT CIRemesDlg::OnMsgRemesStateChanged(WPARAM wp, LPARAM lp) {
  const UINT M = (UINT)wp, K = (UINT)lp;
  String s;
  m_debugInfo.getRemesStateString(s);
  setSubMK(M, K);
  showRemesState(s);
  return 0;
}

void CIRemesDlg::setSubMK(int subM, int subK) {
  if(subM != m_subM) {
    String str = ((subM>=0) && ((subM!=m_M) || (m_M!=m_MTo)))
               ? format(_T("%u"), subM)
               : EMPTYSTRING;
    setWindowText(this, IDC_STATICSUBM, str);
    m_subM = subM;
  }
  if(subK != m_subK) {
    String str = ((subK>=0) && ((subK!=m_K) || (m_K!=m_KTo)))
               ? format(_T("%u"), subK)
               : EMPTYSTRING;
    setWindowText(this, IDC_STATICSUBK, str);
    m_subK = subK;
  }
}

LRESULT CIRemesDlg::OnMsgCoefficientsChanged(WPARAM wp, LPARAM lp) {
  CoefWindowData cwd;
  m_debugInfo.getCoefWinData(cwd);
  showCoefWindowData(cwd);
  return 0;
}

LRESULT CIRemesDlg::OnMsgSearchEIterationChanged(WPARAM wp, LPARAM lp) {
  String str;
  m_debugInfo.getSearchEString(str);
  showSearchEString(str);
  return 0;
}

LRESULT CIRemesDlg::OnMsgExtremaCountChanged(WPARAM wp, LPARAM lp) {
  ExtremaStringArray a;
  m_debugInfo.getExtremaStringArray(a);
  showExtremaStringArray(a);
  return 0;
}

LRESULT CIRemesDlg::OnMsgMaxErrorChanged(WPARAM wp, LPARAM lp) {
  return 0;
}

LRESULT CIRemesDlg::OnMsgShowErrorFunction(WPARAM wp, LPARAM lp) {
  createErrorPointArray();
  return 0;
}

LRESULT CIRemesDlg::OnMsgClearErrorFunction(WPARAM wp, LPARAM lp) {
  removeErrorPlot();
  return 0;
}

LRESULT CIRemesDlg::OnMsgUpdateInterpolation(WPARAM wp, LPARAM lp) {
#ifdef __TODO__
  m_gate.wait();
  if (m_coorSystemSpline.getObjectCount() == 0) {
    return 0;
  }
  m_coorSystemSpline.Invalidate(FALSE);
  m_gate.notify();
#endif
  return 0;
}

LRESULT CIRemesDlg::OnMsgApproximationChanged(WPARAM wp, LPARAM lp) {
  createErrorPointArray();
  return 0;
}

LRESULT CIRemesDlg::OnMsgErrorPointArrayChanged(WPARAM wp, LPARAM lp) {
  showErrorPointArray();
  return 0;
}

void CIRemesDlg::OnHelpAboutIRemes() {
  CAboutDlg().DoModal();
}

void CIRemesDlg::startDebugger(bool singleStep) {
  if(!UpdateData()) {
    return;
  }
  if(!validateInput()) {
    return;
  }
  updateErrorPlotXRange();
  try {
    destroyDebugger();
    createDebugger();
    if(singleStep) {
      m_debugger->singleStep(FL_BREAKSUBSTEP);
    } else {
      m_debugger->go();
    }
  } catch(Exception e) {
    showException(e);
  }
}

bool CIRemesDlg::validateInput() {
  if(m_name.GetLength() == 0) {
    gotoEditBox(this, IDC_EDITNAME);
    showWarning(_T("Name cannot be empty"));
    return false;
  }
  if(m_xFrom > 0 || m_xTo < 0) {
    gotoEditBox(this, IDC_EDITXFROM);
    showWarning(_T("Domain must contain 0"));
    return false;
  }
  if(m_xFrom >= m_xTo) {
    gotoEditBox(this, IDC_EDITXFROM);
    showWarning(_T("Invalid interval. from must be < to"));
    return false;
  }
  if(m_M < 1) {
    gotoEditBox(this, IDC_EDITMFROM);
    showWarning(_T("M must be >= 1"));
    return false;
  }
  if(m_K < 0) {
    gotoEditBox(this, IDC_EDITKFROM);
    showWarning(_T("K must be >= 0"));
    return false;
  }
  if(m_M > m_MTo) {
    gotoEditBox(this, IDC_EDITMFROM);
    showWarning(_T("M start > M end"));
    return false;
  }
  if(m_K > m_KTo) {
    gotoEditBox(this, IDC_EDITKFROM);
    showWarning(_T("K start > K end"));
    return false;
  }
  return true;
}


void CIRemesDlg::createDebugger() {
  m_targetFunction.setName((LPCTSTR)m_name);
  m_targetFunction.setDigits(m_digits);
  m_targetFunction.setDomain(m_xFrom, m_xTo);
  setSubMK(-1,-1);
  m_reduceToInterpolate = false;

  m_remes               = new Remes(m_targetFunction, m_relativeError?true:false); TRACE_NEW(m_remes);
  m_allowRemesProperties = true;
  m_remes->setSearchEMaxIterations(m_maxSearchEIterations);
  m_remes->setVisiblePrecisions(   m_visiblePrecisions   );
  const IntInterval mInterval(m_M, m_MTo);
  const IntInterval kInterval(m_K, m_KTo);
  m_debugger = new Debugger(*m_remes, mInterval, kInterval, m_maxMKSum, m_skipExisting?true:false); TRACE_NEW(m_debugger);
  m_debugger->addPropertyChangeListener(this);
}

void CIRemesDlg::destroyDebugger() {
  stopErrorPlotter();
  if(hasDebugger()) {
    m_allowRemesProperties = false;
    SAFEDELETE(m_debugger);
    SAFEDELETE(m_remes);
  }
  BigRealResourcePool::resetAllPoolCalculations();
}

String CIRemesDlg::getThreadStateName() const {
  if(!hasDebugger()) {
    return _T("No Debugger");
  } else {
    return m_debugger->getStateName();
  }
}

/*
BigReal DynamicTargetFunction::operator()(const BigReal &x) {
  return rLn(x + BIGREAL_1, m_digits);
}
*/

static const ConstBigReal _3over2(1.5);

BigReal DynamicTargetFunction::operator()(const BigReal &x) {
  return rGamma(x + _3over2, m_digits);
}


void DynamicTargetFunction::setDomain(double from, double to) {
  m_domain.setFrom(from).setTo(to);
}

String DynamicTargetFunction::getDefaultName() const {
  return _T("gamma");
}

DoubleInterval DynamicTargetFunction::getDefaultDomain() const {
  return DoubleInterval(-0.5, 0.5);
}
