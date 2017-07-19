#include "stdafx.h"
#include "IRemesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialog {
public:
  CAboutDlg();
  enum { IDD = IDD_ABOUTBOX };
protected:
  virtual void DoDataExchange(CDataExchange *pDX);
  DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD) {
}

void CAboutDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CIRemesDlg::CIRemesDlg(CWnd *pParent /*=NULL*/) : CDialog(CIRemesDlg::IDD, pParent), m_name(EMPTYSTRING), m_maxSearchEIterations(0)
{
  m_M                    = 1;
  m_K                    = 1;
  m_MTo                  = m_M;
  m_KTo                  = m_K;
  m_xFrom                = 0.0;
  m_xTo                  = 1.0;
  m_digits               = 19;
  m_maxSearchEIterations = 700;
  m_maxMKSum             = 0;
  m_relativeError        = FALSE;
  m_skipExisting         = FALSE;

  m_reduceToInterpolate  = false;
  m_lastErrorPlotKey     = -1;
  m_hIcon                = theApp.LoadIcon(IDR_MAINFRAME);
  m_remes                = NULL;
  m_debugThread          = NULL;
  m_runMenuState         = RUNMENU_EMPTY;
  m_subM = m_subK        = -1;
}

void CIRemesDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDITNAME, m_name);
  DDX_Text(pDX, IDC_EDITM, m_M);
  DDV_MinMaxUInt(pDX, m_M, 1, 100);
  DDX_Text(pDX, IDC_EDITMTO, m_MTo);
  DDX_Text(pDX, IDC_EDITK, m_K);
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
  ON_COMMAND(ID_FILE_SHOWMAXERRORS           , &CIRemesDlg::OnFileShowMaxErrors         )
  ON_COMMAND(ID_FILE_EXIT                    , &CIRemesDlg::OnFileExit                  )
  ON_COMMAND(ID_VIEW_GRID                    , &CIRemesDlg::OnViewGrid                  )
  ON_COMMAND(ID_VIEW_SHOW_ERRORFUNCTION      , &CIRemesDlg::OnViewShowErrorFunction     )
  ON_COMMAND(ID_VIEW_SHOW_SPLINE             , &CIRemesDlg::OnViewShowSpline            )
  ON_COMMAND(ID_RUN_GO                       , &CIRemesDlg::OnRunGo                     )
  ON_COMMAND(ID_RUN_F5                       , &CIRemesDlg::OnRunF5                     )
  ON_COMMAND(ID_RUN_RESTART                  , &CIRemesDlg::OnRunRestart                )
  ON_COMMAND(ID_RUN_STOP                     , &CIRemesDlg::OnRunStop                   )
  ON_COMMAND(ID_RUN_BREAK                    , &CIRemesDlg::OnRunBreak                  )
  ON_COMMAND(ID_RUN_SINGLEITERATION          , &CIRemesDlg::OnRunSingleIteration        )
  ON_COMMAND(ID_RUN_SINGLESUBITERATION       , &CIRemesDlg::OnRunSingleSubIteration     )
  ON_COMMAND(ID_RUN_REDUCETOINTERPOLATE      , &CIRemesDlg::OnRunReduceToInterpolate    )
  ON_COMMAND(ID_GOTO_DOMAIN                  , &CIRemesDlg::OnGotoDomain                )
  ON_COMMAND(ID_GOTO_M                       , &CIRemesDlg::OnGotoM                     )
  ON_COMMAND(ID_GOTO_K                       , &CIRemesDlg::OnGotoK                     )
  ON_COMMAND(ID_GOTO_DIGITS                  , &CIRemesDlg::OnGotoDigits                )
  ON_COMMAND(ID_GOTO_MAXSEARCHEITERATIONS    , &CIRemesDlg::OnGotoMaxSearchEIterations  )
  ON_COMMAND(ID_HELP_ABOUTIREMES             , &CIRemesDlg::OnHelpAboutIRemes           )
  ON_MESSAGE(ID_MSG_THR_RUNSTATE_CHANGED     , &CIRemesDlg::OnMsgThrRunStateChanged     )
  ON_MESSAGE(ID_MSG_THR_TERMINATED_CHANGED   , &CIRemesDlg::OnMsgThrTerminatedChanged   )
  ON_MESSAGE(ID_MSG_THR_ERROR_CHANGED        , &CIRemesDlg::OnMsgThrErrorChanged        )
  ON_MESSAGE(ID_MSG_STATE_CHANGED            , &CIRemesDlg::OnMsgStateChanged           )
  ON_MESSAGE(ID_MSG_COEFFICIENTS_CHANGED     , &CIRemesDlg::OnMsgCoefficientsChanged    )
  ON_MESSAGE(ID_MSG_SEARCHEITERATION_CHANGED , &CIRemesDlg::OnMsgSearchEIterationChanged)
  ON_MESSAGE(ID_MSG_EXTREMACOUNT_CHANGED     , &CIRemesDlg::OnMsgExtremaCountChanged    )
  ON_MESSAGE(ID_MSG_MAXERROR_CHANGED         , &CIRemesDlg::OnMsgMaxErrorChanged        )
  ON_MESSAGE(IS_MSG_UPDATEINTERPOLATION      , &CIRemesDlg::OnMsgUpdateInterpolation    )
  ON_MESSAGE(ID_MSG_WARNING_CHANGED          , &CIRemesDlg::OnMsgWarningChanged         )
  ON_EN_KILLFOCUS(IDC_EDITM                  , &CIRemesDlg::OnEnKillfocusEditm          )
  ON_EN_KILLFOCUS(IDC_EDITK                  , &CIRemesDlg::OnEnKillfocusEditk          )
  ON_EN_KILLFOCUS(IDC_EDITMTO                , &CIRemesDlg::OnEnKillfocusEditmto        )
  ON_EN_KILLFOCUS(IDC_EDITKTO                , &CIRemesDlg::OnEnKillfocusEditkto        )
  ON_EN_UPDATE(IDC_EDITKTO                   , &CIRemesDlg::OnEnUpdateEditkto           )
  ON_EN_UPDATE(IDC_EDITMTO                   , &CIRemesDlg::OnEnUpdateEditmto           )
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

  setCoorSystemSplineVisible(isMenuItemChecked(this, ID_VIEW_SHOW_SPLINE));

  m_layoutManager.OnInitDialog(this);
  m_layoutManager.addControl(IDC_STATICTEMPORARY        ,                                                         PCT_RELATIVE_BOTTOM );
  m_layoutManager.addControl(IDC_LISTCOEF               ,                                                         PCT_RELATIVE_BOTTOM );
  m_layoutManager.addControl(IDC_LISTEXTRMA             ,                     RELATIVE_RIGHT |                    PCT_RELATIVE_BOTTOM );
  m_layoutManager.addControl(IDC_FRAME_COORSYSTEM_ERROR , PCT_RELATIVE_RIGHT                 | PCT_RELATIVE_TOP | RELATIVE_BOTTOM     );
  m_layoutManager.addControl(IDC_FRAME_COORSYSTEM_SPLINE, PCT_RELATIVE_LEFT | RELATIVE_RIGHT | PCT_RELATIVE_TOP | RELATIVE_BOTTOM     );

  gotoEditBox(this, IDC_EDITNAME);
  return TRUE;
}

void CIRemesDlg::OnPaint() {
  if (IsIconic())  {
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
    ajourDialogItems();
  }
}

void CIRemesDlg::setCoorSystemSplineVisible(bool visible) {
  CRect errorRect  = getWindowRect(this, IDC_FRAME_COORSYSTEM_ERROR);
  if (visible) {
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


void CIRemesDlg::showThreadState() {
  const TCHAR *str;
  bool resetRemesState = false;
  if(hasDebugThread()) {
    str = m_debugThread->getStateName();
  } else {
    str = _T("No thread");
//    resetRemesState = true;
  }
  if(resetRemesState) {
    setWindowText(this, IDC_STATICSTATE, EMPTYSTRING);
  }
  setWindowText(this, IDC_STATICTHREADSTATE, str);
}

void CIRemesDlg::showState(const String &str) {
  setWindowText(this, IDC_STATICSTATE, str);
}

void CIRemesDlg::showWarning(const String &str) {
  setWindowText(this, IDC_STATICWARNING, str);
}

void CIRemesDlg::showCoefWindowData(const CoefWindowData &data) {
  m_coefListBox.setLines(data.m_coefStrings);
  m_coefListBox.Invalidate();
}

void CIRemesDlg::showExtremaStringArray() {
  const ExtremaStringArray &a = m_extrStrArray;
  if(a != m_extrStrArrayOld) {
    CListBox *lb = (CListBox*)GetDlgItem(IDC_LISTEXTRMA);
    const size_t n = a.size();
    if(a.size() != m_extrStrArrayOld.size()) {
      lb->ResetContent();
      for(size_t i = 0; i < n; i++) {
        lb->AddString(a.getString(i).cstr());
      }
    } else {
      for (size_t i = 0; i < n; i++) {
        const String ai = a.getString(i);
        if (ai != m_extrStrArrayOld.getString(i)) {
          lb->DeleteString((UINT)i);
          lb->InsertString((UINT)i, ai.cstr());
        }
      }
    }
    m_extrStrArrayOld = m_extrStrArray;
  }
}

void CIRemesDlg::clearErrorPlot() {
  m_coorSystemError.deleteAllObjects();
}

bool CIRemesDlg::createErrorPlot(const Remes &r) {
  if(!r.hasErrorPlot()) {
    clearErrorPlot();
    return false;
  }
  const int plotKey = r.getCoefVectorIndex();

  if(plotKey == m_lastErrorPlotKey) {
    return false;
  }

  clearErrorPlot();
  Point2DArray pa;
  r.getErrorPlot(getErrorPlotXPixelCount(), pa);
  m_coorSystemError.addPointObject(pa);
  m_lastErrorPlotKey = plotKey;
  return true;
}

void CIRemesDlg::showErrorPlot() {
  if (m_coorSystemError.getObjectCount() == 0) {
    return;
  }
  m_coorSystemError.Invalidate(FALSE);
}

void CIRemesDlg::updateErrorPlotXRange() {
  const DataRange dr = m_coorSystemError.getDataRange();
  m_coorSystemError.setDataRange(DataRange(m_xFrom, m_xTo, dr.getMinY(), dr.getMaxY()), false);
  m_lastErrorPlotKey = -1;
}

int CIRemesDlg::getErrorPlotXPixelCount() const {
  const IntervalTransformation &tr = m_coorSystemError.getTransformation().getXTransformation();
  const DoubleInterval xToRange = tr.forwardTransform(getXRange());
  const int n = (int)xToRange.getLength()+1;
  return (n <= 0) ? 1 : n;
}

void CIRemesDlg::showSearchE(const String &s) {
  setWindowText(this, IDC_STATICTEMPORARY, s);
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


void CIRemesDlg::ajourDialogItems() {
  static const int dialogFields[] = {
    IDC_EDITNAME
   ,IDC_EDITM
   ,IDC_EDITK
   ,IDC_EDITMTO
   ,IDC_EDITKTO
   ,IDC_EDITMAXMKSUM
   ,IDC_EDITXFROM
   ,IDC_EDITXTO
   ,IDC_EDITDIGITS
   ,IDC_EDITMAXSEARCHEITERATIONS
   ,IDC_CHECKRELATIVEERROR
  };

  showThreadState();
  if(hasDebugThread()) {
    if(m_debugThread->isRunning()) {
      ENABLEFIELDLIST(dialogFields, false);
      setRunMenuState(RUNMENU_RUNNING);
    } else if(m_debugThread->isTerminated()) {
      ENABLEFIELDLIST(dialogFields, true );
      setRunMenuState(RUNMENU_IDLE);
    } else { // paused
      ENABLEFIELDLIST(dialogFields, false);
      setRunMenuState(RUNMENU_PAUSED);
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
 ,MLABEL("Stop\tShift+F5"                     , ID_RUN_STOP                ) // 4
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
  HMENU mainMenu = findMenuByString(*GetMenu(), _T("Run"), index);

  HMENU runMenu = GetSubMenu(mainMenu, index);
  if(runMenu == 0) return;
  removeAllMenuItems(runMenu);
  const int *menuItems = runMenuItems[menuState];
  if(menuItems == NULL) return;
  int count = 0;
  for (const int *item = menuItems; *item >= 0; item++,count++) {
    if (*item == ITEM_SEPARATOR) {
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

void CIRemesDlg::OnFileShowMaxErrors() {
  OnRunDebug();
  const ExtremaMap &map = m_remes->getExtremaMap();
  const String tmpFileName = _T("c:\\temp\\RemesErrors.txt");
  FILE *f = MKFOPEN(tmpFileName, _T("w"));
  for (Iterator<ExtremaMapEntry> it = map.getIerator(); it.hasNext();) {
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

void CIRemesDlg::OnFileExit() {
  EndDialog(IDOK);
}

void CIRemesDlg::OnViewGrid() {
  const bool showGrid = toggleMenuItem(this, ID_VIEW_GRID);
  m_coorSystemError.setGrid(showGrid);
  m_coorSystemSpline.setGrid(showGrid);
  Invalidate(FALSE);
}


void CIRemesDlg::OnViewShowErrorFunction() {
  // TODO: Add your command handler code here
}

void CIRemesDlg::OnViewShowSpline() {
  setCoorSystemSplineVisible(toggleMenuItem(this, ID_VIEW_SHOW_SPLINE));
}

void CIRemesDlg::OnRunGo() {
  startThread(false);
}

void CIRemesDlg::OnRunF5() {
  switch(m_runMenuState) {
  case RUNMENU_IDLE  : OnRunDebug();    break;
  case RUNMENU_PAUSED: OnRunContinue(); break;
  }
}

void CIRemesDlg::OnRunDebug() {
  startThread(true);
}

void CIRemesDlg::OnRunContinue() {
  if(isThreadPaused()) {
    m_debugThread->go();
  }
}

void CIRemesDlg::OnRunRestart() {
  OnRunStop();
  if(!hasDebugThread() || isThreadTerminated()) {
    OnRunGo();
  }
}

void CIRemesDlg::OnRunStop() {
  try {
    if(hasDebugThread()) {
      m_debugThread->kill();
    }
  } catch(Exception e) {
    showError(e);
  }
}

void CIRemesDlg::OnRunBreak() {
  if(hasDebugThread()) {
    m_debugThread->stopASAP();
  }
}

void CIRemesDlg::OnRunSingleIteration() {
  if(isThreadPaused()) m_debugThread->singleStep();
}

void CIRemesDlg::OnRunSingleSubIteration() {
  if(isThreadPaused()) m_debugThread->singleSubStep();
}

void CIRemesDlg::OnRunReduceToInterpolate() {
  if(isThreadRunning()) {
    m_reduceToInterpolate = true;
  }
}

void CIRemesDlg::OnGotoDomain() {
  gotoEditBox(this, IDC_EDITXFROM);
}

void CIRemesDlg::OnGotoM() {
  gotoEditBox(this, IDC_EDITM);
}
void CIRemesDlg::OnGotoK() {
  gotoEditBox(this, IDC_EDITK);
}
void CIRemesDlg::OnGotoDigits() {
  gotoEditBox(this, IDC_EDITDIGITS);
}
void CIRemesDlg::OnGotoMaxSearchEIterations() {
  gotoEditBox(this, IDC_EDITMAXSEARCHEITERATIONS);
}


void CIRemesDlg::OnEnKillfocusEditm() {
  String mStr   = getWindowText(this, IDC_EDITM);
  String mToStr = getWindowText(this, IDC_EDITMTO);
  UINT m, mto;
  if(_stscanf(mStr.cstr(), _T("%u"), &m) != 1) {
    return;
  }
  if (_stscanf(mToStr.cstr(), _T("%u"), &mto) != 1) {
    mto = 0;
  }
  if (m > mto) {
    setWindowText(this, IDC_EDITMTO, format(_T("%u"), m));
  }
  adjustMaxMKSum();
}

void CIRemesDlg::OnEnKillfocusEditk() {
  String kStr   = getWindowText(this, IDC_EDITK);
  String kToStr = getWindowText(this, IDC_EDITKTO);
  UINT k, kto;
  if(_stscanf(kStr.cstr(), _T("%u"), &k) != 1) {
    return;
  }
  if (_stscanf(kToStr.cstr(), _T("%u"), &kto) != 1) {
    kto = 0;
  }
  if (k > kto) {
    setWindowText(this, IDC_EDITKTO, format(_T("%u"), k));
  }
  adjustMaxMKSum();
}
void CIRemesDlg::OnEnKillfocusEditmto() {
  adjustMaxMKSum();
}
void CIRemesDlg::OnEnKillfocusEditkto() {
  adjustMaxMKSum();
}
void CIRemesDlg::OnEnUpdateEditmto() {
  adjustMaxMKSum();
}
void CIRemesDlg::OnEnUpdateEditkto() {
  adjustMaxMKSum();
}

void CIRemesDlg::adjustMaxMKSum() {
  const UINT maxM = _ttoi(getWindowText(this, IDC_EDITMTO).cstr());
  const UINT maxK = _ttoi(getWindowText(this, IDC_EDITKTO).cstr());
  const UINT MKsum = maxM + maxK;

  String maxMKSumStr   = getWindowText(this, IDC_EDITMAXMKSUM);
  UINT maxMKSum;
  if (_stscanf(maxMKSumStr.cstr(), _T("%u"), &maxMKSum) != 1) {
    maxMKSum = 0;
  }
  if (maxMKSum < MKsum) {
    setWindowText(this, IDC_EDITMAXMKSUM, format(_T("%u"), MKsum));
  }
}

void CIRemesDlg::handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) {
  if(source == m_debugThread) {
    switch(id) {
    case THREAD_RUNNING:
      PostMessage(ID_MSG_THR_RUNSTATE_CHANGED, *(bool*)oldValue, *(bool*)newValue);
      break;
    case THREAD_TERMINATED:
      PostMessage(ID_MSG_THR_TERMINATED_CHANGED, *(bool*)oldValue, *(bool*)newValue);
      break;
    case THREAD_ERROR:
      m_gate.wait();
      m_error = *(const String*)newValue;
      m_gate.signal();
      PostMessage(ID_MSG_THR_ERROR_CHANGED, 0, 0);
      break;
    case REMES_PROPERTY:
      { const RemesPropertyData &data = *(RemesPropertyData*)newValue; // oldValue = NULL
        handleRemesProperty(data.m_src, data.m_id, data.m_oldValue, data.m_newValue);
        break;
      }
    default:
      MessageBox(format(_T("Unknown property id:%d"), id).cstr(), _T("Error"), MB_ICONEXCLAMATION);
      break;
    }
  }
}

void CIRemesDlg::handleRemesProperty(const Remes &r, int id, const void *oldValue, const void *newValue) {
  if (m_reduceToInterpolate) {
    r.reduceToInterpolate();
    m_reduceToInterpolate = false;
  }

  switch(id) {
  case REMES_STATE        : // *RemesState
    { m_gate.wait();
      const RemesState oldState = *(RemesState*)oldValue;
      const RemesState newState = *(RemesState*)newValue;
      m_stateString = r.getTotalStateString();
      PostMessage(ID_MSG_STATE_CHANGED, r.getM(), r.getK());
      if(oldState == REMES_SEARCH_COEFFICIENTS) {
        m_coefWinData   = r;
        m_searchEString = r.getSearchEString();
        PostMessage(ID_MSG_COEFFICIENTS_CHANGED, 0, 0);
      }
      m_gate.signal();
    }
    break;

  case SEARCHEITERATION   : // *int
    m_gate.wait();
    m_searchEString = r.getSearchEString();
    m_gate.signal();
    PostMessage(ID_MSG_SEARCHEITERATION_CHANGED, 0, 0);
    break;

  case EXTREMACOUNT       : // *int
    m_gate.wait();
    m_extrStrArray = r.getExtremaStringArray();
    m_gate.signal();
    PostMessage(ID_MSG_EXTREMACOUNT_CHANGED, 0, 0);
    break;

  case MAINITERATION      : // *int
  case Q                  : // *BigReal
  case E                  : // *BigReal
  case COEFFICIENTVECTOR  : // *BigRealVector
  case MMQUOT             : // *BigReal
    break;
  case INTERPOLATIONSPLINE: // *dim, *Function
    if (isMenuItemChecked(this, ID_VIEW_SHOW_SPLINE)) {
      const UINT           dim = *(UINT*)oldValue;
      Function            &f   = *(Function*)newValue;
      const DoubleInterval xRange(0, dim-1);
      CClientDC            dc(&m_coorSystemSpline);

      m_gate.wait();
      m_coorSystemSpline.deleteAllObjects();
      m_coorSystemSpline.addFunctionObject(f, &xRange);
      m_gate.signal();
      PostMessage(IS_MSG_UPDATEINTERPOLATION, 0, 0);
    }
    break;
  case MAXERROR           : // *BigReal
    if(r.hasErrorPlot()) {
      m_gate.wait();
      const bool paint = createErrorPlot(r);
      m_gate.signal();
      if(paint) {
        PostMessage(ID_MSG_MAXERROR_CHANGED, 0, 0);
      }
    }
    break;
  case WARNING            : // *String
    m_gate.wait();
    m_warning = *(String*)newValue;
    m_gate.signal();
    PostMessage(ID_MSG_WARNING_CHANGED, 0, 0);
    break;
  default:
    MessageBox(format(_T("Unknown remes property id:%d"), id).cstr(), _T("Error"), MB_ICONEXCLAMATION);
    break;
  }
}

LRESULT CIRemesDlg::OnMsgThrRunStateChanged(WPARAM wp, LPARAM lp) {
  ajourDialogItems();
  return 0;
}

LRESULT CIRemesDlg::OnMsgThrTerminatedChanged(WPARAM wp, LPARAM lp) {
  const bool terminated = lp ? true : false;
  if(terminated) {
    ajourDialogItems();
  }
  return 0;
}

LRESULT CIRemesDlg::OnMsgThrErrorChanged(WPARAM wp, LPARAM lp) {
  m_gate.wait();
  const String error = m_error;
  m_gate.signal();
  MessageBox(error.cstr(), _T("Error"), MB_ICONEXCLAMATION);
  return 0;
}

LRESULT CIRemesDlg::OnMsgStateChanged(WPARAM wp, LPARAM lp) {
  m_gate.wait();
  const UINT M = (UINT)wp, K = (UINT)lp;
  setSubMK(M, K);
  const String str = m_stateString;
  m_gate.signal();
  showState(str);
  return 0;
}

void CIRemesDlg::setSubMK(int subM, int subK) {
  if (subM != m_subM) {
    String str = ((subM>=0) && ((subM!=m_M) || (m_M!=m_MTo)))
               ? format(_T("%u"), subM)
               : EMPTYSTRING;
    setWindowText(this, IDC_STATICSUBM, str);
    m_subM = subM;
  }
  if (subK != m_subK) {
    String str = ((subK>=0) && ((subK!=m_K) || (m_K!=m_KTo)))
               ? format(_T("%u"), subK)
               : EMPTYSTRING;
    setWindowText(this, IDC_STATICSUBK, str);
    m_subK = subK;
  }
}

LRESULT CIRemesDlg::OnMsgCoefficientsChanged(WPARAM wp, LPARAM lp) {
  m_gate.wait();
  const String str = m_searchEString;
  showCoefWindowData(m_coefWinData);
  m_gate.signal();
  showSearchE(str);
  return 0;
}

LRESULT CIRemesDlg::OnMsgSearchEIterationChanged(WPARAM wp, LPARAM lp) {
  m_gate.wait();
  const String str = m_searchEString;
  m_gate.signal();
  showSearchE(str);
  return 0;
}

LRESULT CIRemesDlg::OnMsgExtremaCountChanged(WPARAM wp, LPARAM lp) {
  m_gate.wait();
  showExtremaStringArray();
  m_gate.signal();
  return 0;
}

LRESULT CIRemesDlg::OnMsgUpdateInterpolation(WPARAM wp, LPARAM lp) {
  m_gate.wait();
  if (m_coorSystemSpline.getObjectCount() == 0) {
    return 0;
  }
  m_coorSystemSpline.Invalidate(FALSE);
  m_gate.signal();
  return 0;
}

LRESULT CIRemesDlg::OnMsgMaxErrorChanged(WPARAM wp, LPARAM lp) {
  m_gate.wait();
  showErrorPlot();
  m_gate.signal();
  return 0;
}

LRESULT CIRemesDlg::OnMsgWarningChanged(WPARAM wp, LPARAM lp) {
  m_gate.wait();
  showWarning(m_warning);
  m_gate.signal();
  return 0;
}

void CIRemesDlg::OnHelpAboutIRemes() {
  CAboutDlg dlg;
  dlg.DoModal();
}

void CIRemesDlg::startThread(bool singleStep) {
  if(!UpdateData()) {
    return;
  }
  if (!validateInput()) {
    return;
  }
  updateErrorPlotXRange();
  try {
    destroyThread();
    createThread();
    if(singleStep) {
      m_debugThread->singleStep();
    } else {
      m_debugThread->go();
    }
  } catch(Exception e) {
    showError(e);
  }
}

bool CIRemesDlg::validateInput() {
  const TCHAR *title = _T("Error");
  if (m_name.GetLength() == 0) {
    gotoEditBox(this, IDC_EDITNAME);
    MessageBox(_T("Name cannot be empty"), title, MB_ICONWARNING);
    return false;
  }
  if (m_xFrom > 0 || m_xTo < 0) {
    gotoEditBox(this, IDC_EDITXFROM);
    MessageBox(_T("Domain must contain 0"), title, MB_ICONWARNING);
    return false;
  }
  if (m_xFrom >= m_xTo) {
    gotoEditBox(this, IDC_EDITXFROM);
    MessageBox(_T("Invalid interval. from must be < to"), title, MB_ICONWARNING);
    return false;
  }
  if (m_M < 1) {
    gotoEditBox(this, IDC_EDITM);
    MessageBox(_T("M must be >= 1"), title, MB_ICONWARNING);
    return false;
  }
  if (m_K < 0) {
    gotoEditBox(this, IDC_EDITK);
    MessageBox(_T("K must be >= 0"), title, MB_ICONWARNING);
    return false;
  }
  if (m_M > m_MTo) {
    gotoEditBox(this, IDC_EDITM);
    MessageBox(_T("M start > M end"), title, MB_ICONWARNING);
    return false;
  }
  if (m_K > m_KTo) {
    gotoEditBox(this, IDC_EDITK);
    MessageBox(_T("K start > K end"), title, MB_ICONWARNING);
    return false;
  }
  return true;
}

void CIRemesDlg::createThread() {
  m_targetFunction.setName((LPCTSTR)m_name);
  m_targetFunction.setDigits(m_digits);
  m_targetFunction.setDomain(m_xFrom, m_xTo);
  setSubMK(-1,-1);
  m_reduceToInterpolate = false;
  m_remes               = new Remes(m_targetFunction, m_relativeError?true:false);
  m_remes->setSearchEMaxIterations(m_maxSearchEIterations);
  const IntInterval mInterval(m_M, m_MTo);
  const IntInterval kInterval(m_K, m_KTo);
  m_debugThread         = new DebugThread(*m_remes, mInterval, kInterval, m_maxMKSum, m_skipExisting?true:false);
  m_debugThread->addPropertyChangeListener(this);
}

void CIRemesDlg::destroyThread() {
  if(hasDebugThread()) {
    m_debugThread->kill();
    delete m_debugThread;
    m_debugThread = NULL;
    delete m_remes;
    m_remes = NULL;
  }
}

String CIRemesDlg::getThreadStateName() const {
  if (!hasDebugThread()) {
    return _T("No Thread");
  } else {
    return m_debugThread->getStateName();
  }
}

void CIRemesDlg::showError(const Exception &e) {
  MessageBox(format(_T("Exception:%s"), e.what()).cstr(), _T("Error"), MB_ICONWARNING | MB_OK);
}

BigReal DynamicTargetFunction::operator()(const BigReal &x) {
  return rLn(x + BIGREAL_1, m_digits);
}

void DynamicTargetFunction::setDomain(double from, double to) {
  m_domain.setFrom(from).setTo(to);
}
