#include "stdafx.h"
#include "IRemes.h"
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

void CAboutDlg::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CIRemesDlg::CIRemesDlg(CWnd* pParent /*=NULL*/) : CDialog(CIRemesDlg::IDD, pParent) {
	m_K             = 1;
	m_M             = 1;
	m_xFrom         = 0.0;
	m_xTo           = 1.0;
	m_relativeError = FALSE;
	m_digits        = 15;
  m_hIcon         = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
  m_remes         = NULL;
  m_debugThread   = NULL;
}

void CIRemesDlg::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
	DDX_Text( pDX, IDC_EDITM             , m_M            );
	DDV_MinMaxUInt(pDX, m_M, 0, 20                        );
	DDX_Text( pDX, IDC_EDITK             , m_K            );
	DDV_MinMaxUInt(pDX, m_K, 0, 20                        );
	DDX_Text( pDX, IDC_EDITXFROM         , m_xFrom        );
	DDX_Text( pDX, IDC_EDITXTO           , m_xTo          );
	DDX_Check(pDX, IDC_CHECKRELATIVEERROR, m_relativeError);
	DDX_Text( pDX, IDC_EDITDIGITS        , m_digits       );
	DDV_MinMaxUInt(pDX, m_digits, 2, 100);
}

BEGIN_MESSAGE_MAP(CIRemesDlg, CDialog)
  ON_WM_SYSCOMMAND()
  ON_WM_QUERYDRAGICON()
  ON_WM_PAINT()
	ON_WM_SIZE()
  ON_WM_CLOSE()
	ON_COMMAND(ID_FILE_EXIT                    , OnFileExit                )
	ON_COMMAND(ID_RUN_GO                       , OnRunGo                   )
	ON_COMMAND(ID_RUN_DEBUG                    , OnRunDebug                )
	ON_COMMAND(ID_RUN_SINGLEITERATION          , OnRunSingleIteration      )
	ON_COMMAND(ID_RUN_SINGLESUBITERATION       , OnRunSingleSubIteration   )
	ON_COMMAND(ID_RUN_STOP                     , OnRunStop                 )
  ON_COMMAND(ID_GOTO_INTERVAL                , OnGotoInterval            )
  ON_COMMAND(ID_GOTO_M                       , OnGotoM                   )
  ON_COMMAND(ID_GOTO_K                       , OnGotoK                   )
  ON_COMMAND(ID_GOTO_DIGITS                  , OnGotoDigits              )
	ON_COMMAND(ID_HELP_ABOUTIREMES             , OnHelpAboutIRemes         )
  ON_MESSAGE(ID_MSG_RUNSTATE_CHANGED         , OnMsgRunStateChanged      )
  ON_MESSAGE(ID_MSG_STATE_CHANGED            , OnMsgStateChanged         )
  ON_MESSAGE(ID_MSG_MAINITERATION_CHANGED    , OnMsgMainIterationChanged )
  ON_MESSAGE(ID_MSG_SEARCHEITERATION_CHANGED , OnMsgSearchEItChanged     )
  ON_MESSAGE(ID_MSG_EXTREMACOUNT_CHANGED     , OnMsgExtremaCountChanged  )
  ON_MESSAGE(ID_MSG_WARNING_CHANGED          , OnMsgWarningChanged       )
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

  m_accelTable = LoadAccelerators(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDR_MAINFRAME));

  m_coorSystem.substituteControl(this, IDC_FRAME_COORDINATESYSTEM);
  m_coorSystem.setRetainAspectRatio(false);
  m_coorSystem.setDataRange(DataRange(m_xFrom, m_xTo, -1, 1), true);

  m_layoutManager.OnInitDialog(this);
  m_layoutManager.addControl(IDC_STATICTEMPORARY       ,                                                     PCT_RELATIVE_BOTTOM );
  m_layoutManager.addControl(IDC_LISTCOEF              ,                                                     PCT_RELATIVE_BOTTOM );
  m_layoutManager.addControl(IDC_LISTEXTRMA            ,                 RELATIVE_RIGHT |                    PCT_RELATIVE_BOTTOM );
  m_layoutManager.addControl(IDC_FRAME_COORDINATESYSTEM, RELATIVE_WIDTH                 | PCT_RELATIVE_TOP | RELATIVE_BOTTOM     );

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


void CIRemesDlg::showState(RemesState state) {
  setWindowText(this, IDC_STATICSTATE
               ,format(_T("%-10s %s")
                      ,getThreadStateName().cstr()
                      , Remes::getStateName(state)
                      )
               );
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

void CIRemesDlg::showExtremaWindowData(const ExtremaWindowData &data) {
  CListBox *lb = (CListBox*)GetDlgItem(IDC_LISTEXTRMA);
  lb->ResetContent();
  const StringArray &a = data.m_extremaStrings;
  const size_t n = a.size();
  for(int i = 0; i < n; i++) {
    lb->AddString(a[i].cstr());
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

void CIRemesDlg::showErrorPlot(const Remes &r) {
  if(!r.hasErrorPlot()) {
    clearErrorPlot();
    return;
  }
  const int plotKey = r.getCoefVectorIndex();

  if(plotKey == getLastErrorPlotKey()) {
    return;
  }

  clearErrorPlot();
  Point2DArray pa;
  const DoubleInterval xToRange = m_coorSystem.getTransformation().getToRectangle().getXInterval();
  const int n = (int)xToRange.getLength()+1;
  r.getErrorPlot(n, pa);
  ErrorPlot *plot = new ErrorPlot(pa, plotKey);
  m_coorSystem.addObject(plot);
  m_coorSystem.setDataRange(DataRange(m_xFrom, m_xTo, plot->getMinY(), plot->getMaxY()), true);
  m_coorSystem.Invalidate(FALSE);
}

int CIRemesDlg::getLastErrorPlotKey() {
  const int n = m_coorSystem.getObjectCount();
  if(n == 0) return -1;
  return ((ErrorPlot*)m_coorSystem.getObject(0))->getKey();
}

void CIRemesDlg::updateErrorPlotXRange() {
  const DataRange dr = m_coorSystem.getDataRange();
  m_coorSystem.setDataRange(DataRange(m_xFrom, m_xTo, dr.getMinY(), dr.getMaxY()), true);
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
    IDC_EDITM
   ,IDC_EDITK
   ,IDC_EDITXFROM
   ,IDC_EDITXTO
   ,IDC_CHECKRELATIVEERROR
   ,IDC_EDITDIGITS
  };

  if(hasDebugThread()) {
    if(m_debugThread->isRunning()) {
      ENABLEFIELDLIST(dialogFields, false);
	    enableMenuItem(this, ID_RUN_GO                , false);
	    enableMenuItem(this, ID_RUN_DEBUG             , false);
      enableMenuItem(this, ID_RUN_SINGLEITERATION   , false);
      enableMenuItem(this, ID_RUN_SINGLESUBITERATION, false);
      enableMenuItem(this, ID_RUN_STOP              , true );
    } else if(m_debugThread->isTerminated()) {
      ENABLEFIELDLIST(dialogFields, true );
	    enableMenuItem(this, ID_RUN_GO                , true );
	    enableMenuItem(this, ID_RUN_DEBUG             , true );
      enableMenuItem(this, ID_RUN_SINGLEITERATION   , false);
      enableMenuItem(this, ID_RUN_SINGLESUBITERATION, false);
      enableMenuItem(this, ID_RUN_STOP              , false);
    } else { // paused
      ENABLEFIELDLIST(dialogFields, false);
	    enableMenuItem(this, ID_RUN_GO                , false);
	    enableMenuItem(this, ID_RUN_DEBUG             , false);
      enableMenuItem(this, ID_RUN_SINGLEITERATION   , true );
      enableMenuItem(this, ID_RUN_SINGLESUBITERATION, true );
      enableMenuItem(this, ID_RUN_STOP              , true );
    }
  } else {
    ENABLEFIELDLIST(dialogFields, true);
	  enableMenuItem(this, ID_RUN_GO                , true );
	  enableMenuItem(this, ID_RUN_DEBUG             , true );
    enableMenuItem(this, ID_RUN_SINGLEITERATION   , false);
    enableMenuItem(this, ID_RUN_SINGLESUBITERATION, false);
    enableMenuItem(this, ID_RUN_STOP              , false);
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

void CIRemesDlg::OnRunGo() {
  if(isThreadPaused()) {
    m_debugThread->go();
  } else {
    startThread(false);
  }
}

void CIRemesDlg::OnRunDebug() {
  startThread(true);
}

void CIRemesDlg::OnRunSingleIteration() {
  if(isThreadPaused()) m_debugThread->singleStep();
}

void CIRemesDlg::OnRunSingleSubIteration() {
  if(isThreadPaused()) m_debugThread->singleSubStep();
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
      PostMessage(ID_MSG_RUNSTATE_CHANGED, (WPARAM)oldValue, (LPARAM)newValue);
      break;
    case REMES_PROPERTY:
      { const RemesPropertyData &data = *(RemesPropertyData*)newValue; // oldValue = NULL
        switch(data.m_id) {
        case REMES_STATE        : // *RemesState
          { const RemesState oldState = *(RemesState*)data.m_oldValue;
            const RemesState newState = *(RemesState*)data.m_newValue;
            PostMessage(ID_MSG_STATE_CHANGED, (WPARAM)oldState, (LPARAM)newState);
          }
          break;
        case MAINITERATION      : // *int
          m_coefWinData = data.m_src;
          PostMessage(ID_MSG_MAINITERATION_CHANGED, 0, (LPARAM)&m_coefWinData);
          break;
        case Q                  : // *BigReal
          m_searchEString = data.m_src.getSearchEString();
          PostMessage(ID_MSG_SEARCHEITERATION_CHANGED, 0, (LPARAM)&m_searchEString);
          break;
        case EXTREMUMCOUNT      : // *int
          m_extrWinData = data.m_src;
          PostMessage(ID_MSG_EXTREMACOUNT_CHANGED, 0, (LPARAM)&m_extrWinData);
          break;
        case COEFFICIENTVECTOR  : // *BigRealVector
        case MMQUOT             : // *BigReal
          break;
        case MAXERROR           : // *BigReal
          if(data.m_src.hasErrorPlot()) {
            showErrorPlot(data.m_src);
          }
          break;
        case WARNING            : // *String
          m_warning = *(String*)data.m_newValue;
          PostMessage(ID_MSG_WARNING_CHANGED, (WPARAM)0, (LPARAM)&m_warning);
          break;
        }
      }
    }
  }
}

LRESULT CIRemesDlg::OnMsgRunStateChanged(WPARAM wp, LPARAM lp) {
  if(!m_debugThread->isRunning()) {
    const String str = m_debugThread->getStateName();
    setWindowText(this, IDC_STATICSTATE, str);
  }
  ajourDialogItems();
  return 0;
}

LRESULT CIRemesDlg::OnMsgStateChanged(WPARAM wp, LPARAM lp) {
  RemesState newState = (RemesState)lp;
  showState(newState);
  return 0;
}

LRESULT CIRemesDlg::OnMsgMainIterationChanged(WPARAM wp, LPARAM lp) {
  const CoefWindowData &data = *(CoefWindowData*)lp;
  showCoefWindowData(data);
  return 0;
}

LRESULT CIRemesDlg::OnMsgSearchEItChanged(WPARAM wp, LPARAM lp) {
  const String &str = *(String*)lp;
  showSearchE(str);
  return 0;
}

LRESULT CIRemesDlg::OnMsgExtremaCountChanged(WPARAM wp, LPARAM lp) {
  const ExtremaWindowData &data = *(ExtremaWindowData*)lp;
  showExtremaWindowData(data);
  return 0;
}

LRESULT CIRemesDlg::OnMsgWarningChanged(WPARAM wp, LPARAM lp) {
  const String &warning = *(String*)lp;
  showWarning(warning);
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
