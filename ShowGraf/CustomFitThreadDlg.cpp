#include "stdafx.h"
#define APSTUDIO_INVOKED
#include <Random.h>
#include "resource.h"
#include "Showgraf.h"
#include "CustomFitThreadDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CCustomFitThreadDlg::CCustomFitThreadDlg(const CString &expr, const DoubleInterval &range, const Point2DArray &pointArray, FunctionPlotter &fp, CWnd* pParent) 
: m_pointArray(pointArray)
, m_fp(fp)
, CDialog(IDD, pParent) {

  m_expr  = expr;
  setXInterval(range);
  m_name  = EMPTYSTRING;
  m_functionFitter = NULL;
  m_worker         = NULL;
}

void CCustomFitThreadDlg::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_DATALIST, m_dataList);
    DDX_Text(pDX, IDC_EDITEXPR , m_expr );
    DDX_Text(pDX, IDC_EDITXFROM, m_xFrom);
    DDX_Text(pDX, IDC_EDITXTO  , m_xTo  );
    DDX_Text(pDX, IDC_EDITNAME , m_name );
}


BEGIN_MESSAGE_MAP(CCustomFitThreadDlg, CDialog)
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_BUTTONSTEP   , OnButtonStep   )
    ON_BN_CLICKED(IDC_BUTTONSOLVE  , OnButtonSolve  )
    ON_BN_CLICKED(IDC_BUTTONSTOP   , OnButtonStop   )
    ON_BN_CLICKED(IDC_BUTTONPLOT   , OnButtonPlot   )
    ON_BN_CLICKED(IDC_BUTTONRESTART, OnButtonRestart)
    ON_COMMAND(ID_GOTO_NAME        , OnGotoName     )
    ON_COMMAND(ID_GOTO_EXPR        , OnGotoFunction )
    ON_COMMAND(ID_GOTO_XINTERVAL   , OnGotoXInterval)
END_MESSAGE_MAP()

BOOL CCustomFitThreadDlg::OnInitDialog() {
  __super::OnInitDialog();

  m_accelTable = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_CUSTOMFIT));
  m_running = false;
  showInfo();
  updateButtons();
  randomize();

  setColor(randomColor());
  const int dataWidth = getWindowRect(&m_dataList).Width()/2-12;
  m_dataList.InsertColumn( 0,_T("X"), LVCFMT_LEFT, dataWidth);
  m_dataList.InsertColumn( 1,_T("Y"), LVCFMT_LEFT, dataWidth);
  m_dataList.SetExtendedStyle(LVS_EX_TRACKSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP);

  for(size_t i = 0; i < m_pointArray.size(); i++) {
    const Point2D &p = m_pointArray[i];
    addData(m_dataList, (int)i, 0, format(_T("%lg"), p.x),true);
    addData(m_dataList, (int)i, 1, format(_T("%lg"), p.y));
  }

  return TRUE;
}

BOOL CCustomFitThreadDlg::PreTranslateMessage(MSG* pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }
  return __super::PreTranslateMessage(pMsg);
}

void CCustomFitThreadDlg::updateButtons() {
  if(m_functionFitter == NULL) {
    GetDlgItem(IDC_BUTTONRESTART)->EnableWindow(TRUE );
    GetDlgItem(IDC_BUTTONSTEP   )->EnableWindow(FALSE);
    GetDlgItem(IDC_BUTTONSOLVE  )->EnableWindow(FALSE);
    GetDlgItem(IDC_BUTTONSTOP   )->EnableWindow(FALSE);
    GetDlgItem(IDC_BUTTONPLOT   )->EnableWindow(FALSE);
    GetDlgItem(IDOK             )->EnableWindow(FALSE);
    GetDlgItem(IDCANCEL         )->EnableWindow(TRUE );
  } else if(m_running) {
    GetDlgItem(IDC_BUTTONRESTART)->EnableWindow(FALSE);
    GetDlgItem(IDC_BUTTONSTEP   )->EnableWindow(FALSE);
    GetDlgItem(IDC_BUTTONSOLVE  )->EnableWindow(FALSE);
    GetDlgItem(IDC_BUTTONSTOP   )->EnableWindow(TRUE );
    GetDlgItem(IDC_BUTTONPLOT   )->EnableWindow(FALSE);
    GetDlgItem(IDOK             )->EnableWindow(FALSE);
    GetDlgItem(IDCANCEL         )->EnableWindow(FALSE);
  } else {
    GetDlgItem(IDC_BUTTONRESTART)->EnableWindow(TRUE );
    GetDlgItem(IDC_BUTTONSTEP   )->EnableWindow(TRUE );
    GetDlgItem(IDC_BUTTONSOLVE  )->EnableWindow(TRUE );
    GetDlgItem(IDC_BUTTONSTOP   )->EnableWindow(FALSE);
    GetDlgItem(IDC_BUTTONPLOT   )->EnableWindow(TRUE );
    GetDlgItem(IDOK             )->EnableWindow(TRUE );
    GetDlgItem(IDCANCEL         )->EnableWindow(TRUE );
  }
}

Point2DArray selectPointsInInterval(const DoubleInterval &interval, const Point2DArray &data) {
  Point2DArray result;
  for(size_t i = 0; i < data.size(); i++) {
    if(interval.contains(data[i].x)) {
      result.add(data[i]);
    }
  }
  return result;
}

void CCustomFitThreadDlg::allocateFunctionFitter() {
  deallocateFunctionFitter();

  m_functionFitter  = new FunctionFitter((LPCTSTR)m_expr,selectPointsInInterval(getXInterval(), m_pointArray));
  if(!m_functionFitter->isOk()) {
    MessageBox(m_functionFitter->getErrors()[0].cstr());
    delete m_functionFitter;
    m_functionFitter = NULL;
    return;
  }
  m_worker = new FitThread(this,*m_functionFitter);

  allocateInfoFields(m_functionFitter->getParamCount() + 5);
}

void CCustomFitThreadDlg::deallocateFunctionFitter() {
  if(m_worker) {
    m_worker->kill();
    delete m_worker;
    m_worker = NULL;
  }
  if(m_functionFitter) {
    delete m_functionFitter;
    m_functionFitter  = NULL;
  }
  deallocateInfoFields();
}

void CCustomFitThreadDlg::allocateInfoFields(int n) {
  deallocateInfoFields();
  for(int i = 0; i < n; i++) {
    m_infoField.add(infofield(i));
  }
}

void CCustomFitThreadDlg::deallocateInfoFields() {
  for(size_t i = 0; i < m_infoField.size(); i++) {
    CStatic *st = m_infoField[i];
    st->DestroyWindow();
    delete st;
  }
  m_infoField.clear();
}

#define FIELDID(i) (_APS_NEXT_CONTROL_VALUE + 1 + i)

CStatic *CCustomFitThreadDlg::infofield(int i) {
  CRect rect;
  rect.top    = i * 20    + 30;
  rect.bottom = rect.top  + 18;
  rect.left   = 10;
  rect.right  = rect.left + 200;
  CStatic *st = new CStatic;
  st->Create(EMPTYSTRING,WS_VISIBLE | WS_GROUP | WS_TABSTOP,rect,this,FIELDID(i));
  return st;
}

void CCustomFitThreadDlg::startTimer() {
  intptr_t iInstallResult = SetTimer(1,500,NULL);
}

void CCustomFitThreadDlg::stopTimer() {
  KillTimer(1);
}

void CCustomFitThreadDlg::printf(int field, const TCHAR *format, ...) {
  va_list argptr;
  va_start(argptr,format);
  const String tmp = vformat(format,argptr);
  va_end(argptr);
  setWindowText(m_infoField[field], tmp);
}

void CCustomFitThreadDlg::showInfo() {
  if (m_functionFitter == NULL) {
    return;
  }
  int i;
  for(i = 0; i < m_functionFitter->getParamCount(); i++) {
    printf(i,_T("%-10s:%.12lf"),m_functionFitter->getParamName(i).cstr(),m_functionFitter->getParamValue(i));
  }
  printf(i++,_T("Iteration :%d")    ,m_functionFitter->getIterationCount());
  printf(i++,_T("Stepsize  :%.12lf"),m_functionFitter->getStepSize());
  printf(i++,_T("SSD       :%.12lf"),m_functionFitter->getSSD());
  printf(i++,_T("Descent   :%.12lf"),m_functionFitter->getSSDDescent());
  if(m_functionFitter->done()) printf(i++,_T("Done"));
  UpdateData(false);
}

void CCustomFitThreadDlg::OnTimer(UINT_PTR nIDEvent) {
  showInfo();
  if(!m_running) {
    updateButtons();
    stopTimer();
  }
  __super::OnTimer(nIDEvent);
}

void CCustomFitThreadDlg::OnOK() {
  UpdateData();
  if(m_name.GetLength() == 0) {
    gotoEditBox(this, IDC_EDITNAME);
    MessageBox(_T("Must specify name"));
    return;
  }
  if(m_functionFitter == NULL) {
    MessageBox(_T("Illegal state:called OnOk with no functionfitter defined"), _T("Error"), MB_ICONEXCLAMATION);
    return;
  }

  m_param            = FunctionGraphParameters((LPCTSTR)m_name,getColor(),0,GSCURVE);
  m_param.m_interval = getXInterval();
  m_param.m_expr     = m_functionFitter->toString().cstr();

  m_fp.addFunctionGraph(m_param);

  stopTimer();
  deallocateFunctionFitter();
  __super::OnOK();
}

void CCustomFitThreadDlg::OnCancel() {
  stopTimer();
  deallocateFunctionFitter();
  __super::OnCancel();
}

void CCustomFitThreadDlg::startWorker(int loopCounter) {
  if(m_functionFitter == NULL || m_worker == NULL) {
    MessageBox(_T("Illegal state:called startWorker with no functionfitter defined"), _T("Error"),MB_ICONEXCLAMATION);
    return;
  }
  m_running = true;
  updateButtons();
  startTimer();
  m_worker->m_loopCount = loopCounter;
  m_worker->resume();
}

void CCustomFitThreadDlg::OnButtonRestart() {
  UpdateData();
  allocateFunctionFitter();
  updateButtons();
  if(m_functionFitter != NULL) {
    m_functionFitter->init();
    showInfo();
  }
}

void CCustomFitThreadDlg::OnButtonPlot() {
  m_fp.plotFunction(*m_functionFitter, getXInterval(), getColor());
}

void CCustomFitThreadDlg::OnButtonStep() {
  startWorker(1);
  showInfo();
}

void CCustomFitThreadDlg::OnButtonSolve() {
  startWorker(-1);
}

void CCustomFitThreadDlg::OnButtonStop() {
  m_worker->m_loopCount = 1;
}

void CCustomFitThreadDlg::OnGotoName() {
  gotoEditBox(this, IDC_EDITNAME);
}

void CCustomFitThreadDlg::OnGotoFunction() {
  gotoEditBox(this, IDC_EDITEXPR);
}

void CCustomFitThreadDlg::OnGotoXInterval() {
  gotoEditBox(this, IDC_EDITXFROM);
}

FitThread::FitThread(CCustomFitThreadDlg *dlg, FunctionFitter &functionFitter) : m_dlg(*dlg), m_functionFitter(functionFitter) {
  setDeamon(true);
  m_killed = false;
}

UINT FitThread::run() {
  while(!m_killed) {
    while(m_loopCount-- != 0 && !m_functionFitter.done() && !m_killed) {
      m_functionFitter.stepIteration();
    }
    m_dlg.m_running = false;
    if(m_killed) {
      break;
    }
    suspend();
  }
  return 0;
}

void FitThread::kill() {
  m_killed = true;
  if(!m_dlg.m_running) {
    resume();
  }
  while(stillActive()) {
    Sleep(20);
  }
}
