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
  virtual void DoDataExchange(CDataExchange* pDX);

protected:
  DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD) {
}

void CAboutDlg::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CIRemesDlg::CIRemesDlg(CWnd* pParent /*=NULL*/) : CDialog(CIRemesDlg::IDD, pParent), m_name(_T("")) {
	m_K             = 1;
	m_M             = 1;
	m_xFrom         = 0.0;
	m_xTo           = 1.0;
	m_relativeError = FALSE;
	m_digits        = 15;
  m_hIcon         = theApp.LoadIcon(IDR_MAINFRAME);
  m_remes         = NULL;
  m_debugThread   = NULL;
  m_dbgMenuState  = DBGMENU_EMPTY;
}

void CIRemesDlg::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDITM, m_M);
  DDV_MinMaxUInt(pDX, m_M, 0, 20);
  DDX_Text(pDX, IDC_EDITK, m_K);
  DDV_MinMaxUInt(pDX, m_K, 0, 20);
  DDX_Text(pDX, IDC_EDITXFROM, m_xFrom);
  DDX_Text(pDX, IDC_EDITXTO, m_xTo);
  DDX_Check(pDX, IDC_CHECKRELATIVEERROR, m_relativeError);
  DDX_Text(pDX, IDC_EDITDIGITS, m_digits);
  DDV_MinMaxUInt(pDX, m_digits, 2, 200);
  DDX_Text(pDX, IDC_EDITNAME, m_name);
}

BEGIN_MESSAGE_MAP(CIRemesDlg, CDialog)
  ON_WM_SYSCOMMAND()
  ON_WM_QUERYDRAGICON()
  ON_WM_PAINT()
	ON_WM_SIZE()
  ON_WM_CLOSE()
	ON_COMMAND(ID_FILE_EXIT                    , &CIRemesDlg::OnFileExit                  )
  ON_COMMAND(ID_VIEW_GRID                    , &CIRemesDlg::OnViewGrid                  )
	ON_COMMAND(ID_RUN_GO                       , &CIRemesDlg::OnRunGo                     )
	ON_COMMAND(ID_RUN_DEBUG                    , &CIRemesDlg::OnRunDebug                  )
  ON_COMMAND(ID_RUN_CONTINUE                 , &CIRemesDlg::OnRunContinue               )
  ON_COMMAND(ID_RUN_RESTART                  , &CIRemesDlg::OnRunRestart                )
	ON_COMMAND(ID_RUN_STOP                     , &CIRemesDlg::OnRunStop                   )
  ON_COMMAND(ID_RUN_BREAK                    , &CIRemesDlg::OnRunBreak                  )
	ON_COMMAND(ID_RUN_SINGLEITERATION          , &CIRemesDlg::OnRunSingleIteration        )
	ON_COMMAND(ID_RUN_SINGLESUBITERATION       , &CIRemesDlg::OnRunSingleSubIteration     )
  ON_COMMAND(ID_GOTO_INTERVAL                , &CIRemesDlg::OnGotoInterval              )
  ON_COMMAND(ID_GOTO_M                       , &CIRemesDlg::OnGotoM                     )
  ON_COMMAND(ID_GOTO_K                       , &CIRemesDlg::OnGotoK                     )
  ON_COMMAND(ID_GOTO_DIGITS                  , &CIRemesDlg::OnGotoDigits                )
	ON_COMMAND(ID_HELP_ABOUTIREMES             , &CIRemesDlg::OnHelpAboutIRemes           )
  ON_MESSAGE(ID_MSG_THR_RUNSTATE_CHANGED     , &CIRemesDlg::OnMsgThrRunStateChanged     )
  ON_MESSAGE(ID_MSG_THR_TERMINATED_CHANGED   , &CIRemesDlg::OnMsgThrTerminatedChanged   )
  ON_MESSAGE(ID_MSG_THR_ERROR_CHANGED        , &CIRemesDlg::OnMsgThrErrorChanged        )
  ON_MESSAGE(ID_MSG_STATE_CHANGED            , &CIRemesDlg::OnMsgStateChanged           )
  ON_MESSAGE(ID_MSG_COEFFICIENTS_CHANGED     , &CIRemesDlg::OnMsgCoefficientsChanged    )
  ON_MESSAGE(ID_MSG_SEARCHEITERATION_CHANGED , &CIRemesDlg::OnMsgSearchEIterationChanged)
  ON_MESSAGE(ID_MSG_EXTREMACOUNT_CHANGED     , &CIRemesDlg::OnMsgExtremaCountChanged    )
  ON_MESSAGE(ID_MSG_MAXERROR_CHANGED         , &CIRemesDlg::OnMsgMaxErrorChanged        )
  ON_MESSAGE(ID_MSG_WARNING_CHANGED          , &CIRemesDlg::OnMsgWarningChanged         )
END_MESSAGE_MAP()

void CIRemesDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg dlgAbout;
    dlgAbout.DoModal();
  } else {
    CDialog::OnSysCommand(nID, lParam);
  }
}

HCURSOR CIRemesDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

BOOL CIRemesDlg::OnInitDialog() {
  CDialog::OnInitDialog();

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

  m_coorSystem.substituteControl(this, IDC_FRAME_COORDINATESYSTEM);
  m_coorSystem.setRetainAspectRatio(false);
  m_coorSystem.setDataRange(DataRange(m_xFrom, m_xTo, -1, 1), false);

  m_layoutManager.OnInitDialog(this);
  m_layoutManager.addControl(IDC_STATICTEMPORARY       ,                                                     PCT_RELATIVE_BOTTOM );
  m_layoutManager.addControl(IDC_LISTCOEF              ,                                                     PCT_RELATIVE_BOTTOM );
  m_layoutManager.addControl(IDC_LISTEXTRMA            ,                 RELATIVE_RIGHT |                    PCT_RELATIVE_BOTTOM );
  m_layoutManager.addControl(IDC_FRAME_COORDINATESYSTEM, RELATIVE_WIDTH                 | PCT_RELATIVE_TOP | RELATIVE_BOTTOM     );

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
    CDialog::OnPaint();
    ajourDialogItems();
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
    setWindowText(this, IDC_STATICSTATE, _T(""));
  }
  setWindowText(this, IDC_STATICTHREADSTATE, str);
}

void CIRemesDlg::showState(RemesState state) {
  setWindowText(this, IDC_STATICSTATE, Remes::getStateName(state));
}

void CIRemesDlg::showWarning(const String &str) {
  setWindowText(this, IDC_STATICWARNING, str);
}

void CIRemesDlg::showCoefWindowData(const CoefWindowData &data) {
  CListBox *lb = (CListBox*)GetDlgItem(IDC_LISTCOEF);
  lb->ResetContent();
  setWindowText(this, IDC_STATICSUBM, format(_T("%d"), data.m_M));
  setWindowText(this, IDC_STATICSUBK, format(_T("%d"), data.m_K));
  const StringArray &a = data.m_coefStrings;
  const size_t n = a.size();
  for(size_t i = 0; i < n; i++) {
    lb->AddString(a[i].cstr());
  }
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

class ErrorPlot : public CoordinateSystemObject {
private:
  const Point2DArray m_pa;
  const int          m_key;
  double             m_minY, m_maxY;
public:
  ErrorPlot(const Point2DArray &pa, int key);
  inline const double &getMinY() const {
    return m_minY;
  }
  inline const double &getMaxY() const {
    return m_maxY;
  }
  inline int getKey() const {
    return m_key;
  }
  void paint(Viewport2D &vp);
};

ErrorPlot::ErrorPlot(const Point2DArray &pa, int key) : m_pa(pa), m_key(key) {
  const Rectangle2D r = m_pa.getBoundingBox();
  m_minY = r.getMinY();
  m_maxY = r.getMaxY();
}

void ErrorPlot::paint(Viewport2D &vp) {
  const size_t n = m_pa.size();
  if(n > 1) {
    vp.MoveTo(m_pa[0]);
    for(size_t i = 1; i < n; i++) {
      vp.LineTo(m_pa[i]);
    }
  }
}

void CIRemesDlg::clearErrorPlot() {
  const int n = m_coorSystem.getObjectCount();
  for(int i = 0; i < n; i++) {
    CoordinateSystemObject *obj = m_coorSystem.getObject(i);
    delete obj;
  }
  m_coorSystem.removeAllObjects();
}

bool CIRemesDlg::createErrorPlot(const Remes &r) {
  if(!r.hasErrorPlot()) {
    clearErrorPlot();
    return false;
  }
  const int plotKey = r.getCoefVectorIndex();

  if(plotKey == getLastErrorPlotKey()) {
    return false;
  }

  clearErrorPlot();
  Point2DArray pa;
  r.getErrorPlot(getErrorPlotXPixelCount(), pa);
  ErrorPlot *plot = new ErrorPlot(pa, plotKey);
  m_coorSystem.addObject(plot);
  return true;
}

void CIRemesDlg::showErrorPlot() {
  if (m_coorSystem.getObjectCount() < 1) {
    return;
  }
  const ErrorPlot *plot = (ErrorPlot*)m_coorSystem.getObject(0);
  m_coorSystem.setDataRange(DataRange(m_xFrom, m_xTo, 1.05*plot->getMinY(), 1.05*plot->getMaxY()), false);
  m_coorSystem.Invalidate(FALSE);
}

int CIRemesDlg::getLastErrorPlotKey() {
  const int n = m_coorSystem.getObjectCount();
  if(n == 0) return -1;
  return ((ErrorPlot*)m_coorSystem.getObject(0))->getKey();
}

void CIRemesDlg::updateErrorPlotXRange() {
  const DataRange dr = m_coorSystem.getDataRange();
  m_coorSystem.setDataRange(DataRange(m_xFrom, m_xTo, dr.getMinY(), dr.getMaxY()), false);
}


int CIRemesDlg::getErrorPlotXPixelCount() const {
  const IntervalTransformation &tr = m_coorSystem.getTransformation().getXTransformation();
  const DoubleInterval xToRange = tr.forwardTransform(getXRange());
  const int n = (int)xToRange.getLength()+1;
  return (n <= 0) ? 1 : n;
}

void CIRemesDlg::showSearchE(const String &s) {
  setWindowText(this, IDC_STATICTEMPORARY, s);
}

void CIRemesDlg::OnSize(UINT nType, int cx, int cy) {
  CDialog::OnSize(nType, cx, cy);
  m_layoutManager.OnSize(nType, cx, cy);
}

BOOL CIRemesDlg::PreTranslateMessage(MSG* pMsg) {
  if(TranslateAccelerator(m_hWnd, m_accelTable, pMsg)) {
    return true;
  }
  return CDialog::PreTranslateMessage(pMsg);
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
   ,IDC_EDITXFROM
   ,IDC_EDITXTO
   ,IDC_CHECKRELATIVEERROR
   ,IDC_EDITDIGITS
  };

  showThreadState();
  if(hasDebugThread()) {
    if(m_debugThread->isRunning()) {
      ENABLEFIELDLIST(dialogFields, false);
      setDebugMenuState(DBGMENU_RUNNING);
/*
      enableMenuItem(this, ID_RUN_GO                , false);
	    enableMenuItem(this, ID_RUN_DEBUG             , false);
      enableMenuItem(this, ID_RUN_SINGLEITERATION   , false);
      enableMenuItem(this, ID_RUN_SINGLESUBITERATION, false);
      enableMenuItem(this, ID_RUN_BREAK             , true );
      enableMenuItem(this, ID_RUN_STOP              , true );
*/
    } else if(m_debugThread->isTerminated()) {
      ENABLEFIELDLIST(dialogFields, true );
      setDebugMenuState(DBGMENU_IDLE);
/*
      enableMenuItem(this, ID_RUN_GO                , true );
	    enableMenuItem(this, ID_RUN_DEBUG             , true );
      enableMenuItem(this, ID_RUN_SINGLEITERATION   , false);
      enableMenuItem(this, ID_RUN_SINGLESUBITERATION, false);
      enableMenuItem(this, ID_RUN_BREAK             , false);
      enableMenuItem(this, ID_RUN_STOP              , false);
*/
    } else { // paused
      ENABLEFIELDLIST(dialogFields, false);
      setDebugMenuState(DBGMENU_PAUSED);
/*
	    enableMenuItem(this, ID_RUN_GO                , true );
	    enableMenuItem(this, ID_RUN_DEBUG             , true );
      enableMenuItem(this, ID_RUN_SINGLEITERATION   , true );
      enableMenuItem(this, ID_RUN_SINGLESUBITERATION, true );
      enableMenuItem(this, ID_RUN_BREAK             , false);
      enableMenuItem(this, ID_RUN_STOP              , true );
*/
    }
  } else { // no debug thread
    ENABLEFIELDLIST(dialogFields, true);
    setDebugMenuState(DBGMENU_IDLE);
/*
	  enableMenuItem(this, ID_RUN_GO                , true );
	  enableMenuItem(this, ID_RUN_DEBUG             , true );
    enableMenuItem(this, ID_RUN_SINGLEITERATION   , false);
    enableMenuItem(this, ID_RUN_SINGLESUBITERATION, false);
    enableMenuItem(this, ID_RUN_BREAK             , false);
    enableMenuItem(this, ID_RUN_STOP              , false);
*/
  }
}

typedef struct {
  const TCHAR *text;
  int          id;
} MenuLabel;

#define MLABEL(s, id) _T(s), id

static const MenuLabel runMenuLables[] = {
  MLABEL("&Go\tCtrl+F5"               , ID_RUN_GO                 )
 ,MLABEL("&Debug\tF5"                 , ID_RUN_DEBUG              )
 ,MLABEL("&Continue\tF5"              , ID_RUN_CONTINUE           )
 ,MLABEL("&Restart\tCtrl+Shift+F5"    , ID_RUN_RESTART            )
 ,MLABEL("Stop\tShift+F5"             , ID_RUN_STOP               )
 ,MLABEL("&Break\tF9"                 , ID_RUN_BREAK              )
 ,MLABEL("Single &iteration\tF10"     , ID_RUN_SINGLEITERATION    )
 ,MLABEL("Single s&ub iteration\tF11" , ID_RUN_SINGLESUBITERATION )
};

#define ITEM_START     0
#define ITEM_DEBUG     1
#define ITEM_CONTINUE  2
#define ITEM_RESTART   3
#define ITEM_STOP      4
#define ITEM_BREAK     5
#define ITEM_SINGLEIT  6
#define ITEM_SUBIT     7
#define ITEM_SEPARATOR 8

static const int runMenuIdleItems[] = {
  ITEM_START
 ,ITEM_DEBUG
 ,ITEM_SEPARATOR
 ,ITEM_SINGLEIT
 ,ITEM_SUBIT
 ,-1
};

static const int runMenuRunningItems[] = {
  ITEM_STOP
 ,ITEM_BREAK
 ,-1
};

static const int runMenuPausedItems[] = {
  ITEM_CONTINUE
 ,ITEM_RESTART
 ,ITEM_STOP     
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

void CIRemesDlg::setDebugMenuState(DebugMenuState menuState) {
  if(menuState == m_dbgMenuState) return;
  m_dbgMenuState = menuState;
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

void CIRemesDlg::OnFileExit() {
  EndDialog(IDOK);
}

void CIRemesDlg::OnViewGrid() {
  m_coorSystem.setGrid(toggleMenuItem(this, ID_VIEW_GRID));
  Invalidate(FALSE);
}

void CIRemesDlg::OnRunGo() {
  startThread(false);
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

void CIRemesDlg::OnGotoInterval() {
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
  switch(id) {
  case REMES_STATE        : // *RemesState
    { const RemesState oldState = *(RemesState*)oldValue;
      const RemesState newState = *(RemesState*)newValue;
      PostMessage(ID_MSG_STATE_CHANGED, (WPARAM)oldState, (LPARAM)newState);
      if(oldState == REMES_SEARCH_COEFFICIENTS) {
        m_gate.wait();
        m_coefWinData = r;
        m_gate.signal();
        PostMessage(ID_MSG_COEFFICIENTS_CHANGED, 0, 0);
      }
    }
    break;

  case SEARCHEITERATION: // *int
    m_gate.wait();
    m_searchEString = r.getSearchEString();
    m_gate.signal();
    PostMessage(ID_MSG_SEARCHEITERATION_CHANGED, 0, 0);
    break;

   case EXTREMUMCOUNT      : // *int
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
  RemesState newState = (RemesState)lp;
  showState(newState);
  return 0;
}

LRESULT CIRemesDlg::OnMsgCoefficientsChanged(WPARAM wp, LPARAM lp) {
  m_gate.wait();
  showCoefWindowData(m_coefWinData);
  m_gate.signal();
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
  if (m_name.GetLength() == 0) {
    gotoEditBox(this, IDC_EDITNAME);
    MessageBox(_T("Name cannot be empty"));
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

void CIRemesDlg::createThread() {
  m_targetFunction.setName((LPCTSTR)m_name);
  m_targetFunction.setDigits(m_digits);
  m_targetFunction.setInterval(m_xFrom, m_xTo);
  m_remes       = new Remes(m_targetFunction, m_relativeError?true:false);
  m_debugThread = new DebugThread(m_M, m_K, *m_remes);
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

void DynamicTargetFunction::setInterval(double from, double to) {
  m_interval.setFrom(from);
  m_interval.setTo(  to  );
}

BigReal DynamicTargetFunction::operator()(const BigReal &x) {
  return rLn(x + BIGREAL_1, m_digits);
}
