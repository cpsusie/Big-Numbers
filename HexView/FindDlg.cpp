#include "stdafx.h"
#include "FindDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CFindDlg::CFindDlg(SearchMachine &searchMachine, CWnd *pParent)
: m_searchMachine(searchMachine)
, CDialog(CFindDlg::IDD, pParent) {
  m_findWhat       = searchMachine.getFindWhat().cstr();
  m_timerIsRunning = false;
  m_waitCursorOn   = false;
  m_searchThread   = NULL;
}

CFindDlg::~CFindDlg() {
  if(m_searchThread) {
    delete m_searchThread;
  }
}

void CFindDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
    DDX_CBString(pDX, IDC_COMBOFINDWHAT, m_findWhat);
  if(!pDX->m_bSaveAndValidate) {
    OnEditChangeComboFindWhat();
  }
}

BEGIN_MESSAGE_MAP(CFindDlg, CDialog)
    ON_WM_SIZE()
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_FINDNEXT                  , OnFindNext                    )
    ON_BN_CLICKED(IDC_BUTTONSPECIALCHAR         , OnButtonSpecialChar           )
    ON_COMMAND(ID_GOTO_FINDWHAT                 , OnGotoFindWhat                )
    ON_COMMAND(ID_SPECIALCHAR_CARRIAGERETURN    , OnSpecialCharCarriageReturn   )
    ON_COMMAND(ID_SPECIALCHAR_NEWLINE           , OnSpecialCharNewline          )
    ON_COMMAND(ID_SPECIALCHAR_BACKSPACE         , OnSpecialCharBackspace        )
    ON_COMMAND(ID_SPECIALCHAR_TAB               , OnSpecialCharTab              )
    ON_COMMAND(ID_SPECIALCHAR_FORMFEED          , OnSpecialCharFormfeed         )
    ON_COMMAND(ID_SPECIALCHAR_ESCCHARACTER      , OnSpecialCharEscapeCharacter  )
    ON_COMMAND(ID_SPECIALCHAR_OCTNUMBER         , OnSpecialCharOctalNumber      )
    ON_COMMAND(ID_SPECIALCHAR_DECNUMBER         , OnSpecialCharDecimalNumber    )
    ON_COMMAND(ID_SPECIALCHAR_HEXNUMBER         , OnSpecialCharHexadecimalNumber)
    ON_COMMAND(ID_SPECIALCHAR_BACKSLASH         , OnSpecialCharBackslash        )
    ON_CBN_SETFOCUS(    IDC_COMBOFINDWHAT       , OnSetFocusComboFindWhat       )
    ON_CBN_KILLFOCUS(   IDC_COMBOFINDWHAT       , OnKillFocusComboFindWhat      )
    ON_CBN_EDITCHANGE(  IDC_COMBOFINDWHAT       , OnEditChangeComboFindWhat     )
    ON_CBN_EDITUPDATE(  IDC_COMBOFINDWHAT       , OnEditUpdateComboFindWhat     )
    ON_CBN_SELCHANGE(   IDC_COMBOFINDWHAT       , OnSelChangeComboFindWhat      )
    ON_CBN_SELENDOK(    IDC_COMBOFINDWHAT       , OnSelendOkComboFindWhat       )
    ON_CBN_SELENDCANCEL(IDC_COMBOFINDWHAT       , OnSelendCancelComboFindWhat   )
END_MESSAGE_MAP()

class SearchThread : public Thread {
private:
  SearchMachine  &m_job;
  bool            m_quit, m_suspended;
public:
  SearchThread(SearchMachine &target);
  ~SearchThread();
  UINT run();
  bool isSuspended() const {
    return m_suspended;
  }
};

SearchThread::SearchThread(SearchMachine &job) : Thread(job, _T("SearchThread")), m_job(job) {
  m_quit = m_suspended = false;
}

SearchThread::~SearchThread() {
  m_quit = true;
  m_job.setInterrupted();
  if (isSuspended()) {
    resume();
  }
  while(stillActive()) {
    Sleep(20);
  }
}

UINT SearchThread::run() {
  while(!m_quit) {
    Thread::run();
    m_suspended = true;
    suspend();
    m_suspended = false;
  }
  return 0;
}

BOOL CFindDlg::OnInitDialog() {
  __super::OnInitDialog();

  CComboBox *combo = getComboFindWhat();
  combo->AddString(m_findWhat.GetBuffer(m_findWhat.GetLength()));
  for(size_t i = 0; i < m_history.size(); i++) {
    combo->AddString(m_history[i].cstr());
  }
  const CRect cbRect = getWindowRect(combo);
  const CPoint buttonPos(cbRect.right+1, cbRect.top);
  m_specialCharButton.Create(this, OBMIMAGE(RGARROW), buttonPos, IDC_BUTTONSPECIALCHAR, true);

  GetDlgItem(IDC_STATICRADIXNAME)->SetWindowText(getSettings().getDataRadixShortName());
  m_currentControl = 0;
  m_selStart       = 0;
  m_selEnd         = m_findWhat.GetLength();

  if(m_searchMachine.isForwardSearch()) {
    ((CButton*)GetDlgItem(IDC_RADIODOWN))->SetCheck(1);
  } else {
    ((CButton*)GetDlgItem(IDC_RADIOUP))->SetCheck(1);
  }
  m_accelTable = LoadAccelerators(theApp.m_hInstance, MAKEINTRESOURCE(IDR_ACCELERATORFIND));

  m_layoutManager.OnInitDialog(this);

  m_layoutManager.addControl(IDC_COMBOFINDWHAT,         RELATIVE_WIDTH        );
  m_layoutManager.addControl(IDC_BUTTONSPECIALCHAR    , RELATIVE_X_POS        );
  m_layoutManager.addControl(IDC_FINDNEXT             , RELATIVE_X_POS        );
  m_layoutManager.addControl(IDCANCEL                 , RELATIVE_X_POS        );
  m_layoutManager.addControl(IDC_STATICRADIXNAME      , PCT_RELATIVE_Y_CENTER );
  m_layoutManager.addControl(IDC_STATICBYTESEQUENCE   , RELATIVE_SIZE         );
  m_layoutManager.addControl(IDC_STATICDIRECTIONFRAME , RELATIVE_POSITION     );
  m_layoutManager.addControl(IDC_RADIOUP              , RELATIVE_POSITION     );
  m_layoutManager.addControl(IDC_RADIODOWN            , RELATIVE_POSITION     );
  m_layoutManager.addControl(IDC_PROGRESSFIND         , RELATIVE_WIDTH | RELATIVE_Y_POS);

  UpdateData(false);

  OnGotoFindWhat();
  return FALSE;
}

void CFindDlg::OnFindNext() {
  UpdateData();
  if(m_findWhat.GetLength() == 0) {
    OnGotoFindWhat();
    return;
  }

  try {
    if(m_searchThread == NULL) {
      m_searchThread = new SearchThread(m_searchMachine);
    }
    const bool forwardSearch = ((CButton*)GetDlgItem(IDC_RADIODOWN))->GetCheck() ? true : false;
    m_searchMachine.prepareSearch(forwardSearch, -1, (LPCTSTR)m_findWhat);
    m_searchThread->resume();
    CProgressCtrl *p = (CProgressCtrl*)GetDlgItem(IDC_PROGRESSFIND);
    p->SetRange(0, 100);
    p->SetPos(0);
    startTimer();
  } catch(Exception e) {
    OnGotoFindWhat();
    showException(e);
  }
}

void CFindDlg::OnTimer(UINT_PTR nIDEvent) {
  __super::OnTimer(nIDEvent);
  if(m_searchMachine.isFinished()) {
    stopTimer();
    m_result = m_searchMachine.getResult();
    if(m_result.isEmpty()) {
      OnGotoFindWhat();
      if(!m_searchMachine.isInterrupted()) {
        showWarning(_T("%s"), m_searchMachine.getResultMessage().cstr());
      }
    } else {
      m_history.add(m_searchMachine.getFindWhat());
      __super::OnOK();
    }
  } else {
    CProgressCtrl *p = (CProgressCtrl*)GetDlgItem(IDC_PROGRESSFIND);
    p->SetPos((int)m_searchMachine.getPercentDone());
  }
}

void CFindDlg::OnCancel() {
  if(m_timerIsRunning) {
    m_searchMachine.setInterrupted();
  } else {
    __super::OnCancel();
  }
}

BOOL CFindDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd, m_accelTable, pMsg)) {
    return TRUE;
  }

  const BOOL ret = __super::PreTranslateMessage(pMsg);

  if(m_currentControl == IDC_COMBOFINDWHAT) {
    DWORD w = getComboFindWhat()->GetEditSel();
    m_selStart = w & 0xff;
    m_selEnd   = w >> 16;
  }
  return ret;
}

void CFindDlg::OnButtonSpecialChar() {
  CMenu menu;
  int ret = menu.LoadMenu(IDR_MENUSPECIALCHAR);
  if(!ret) {
    showWarning(_T("Loadmenu failed"));
    return;
  }
  CRect r;
  GetDlgItem(IDC_BUTTONSPECIALCHAR)->GetWindowRect(&r);
  menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,r.right,r.top, this );
}

void CFindDlg::OnSpecialCharCarriageReturn()    { addSpecialChar(_T("\\r")   , 2); }
void CFindDlg::OnSpecialCharNewline()           { addSpecialChar(_T("\\n")   , 2); }
void CFindDlg::OnSpecialCharBackspace()         { addSpecialChar(_T("\\b")   , 2); }
void CFindDlg::OnSpecialCharEscapeCharacter()   { addSpecialChar(_T("\\e")   , 2); }
void CFindDlg::OnSpecialCharOctalNumber()       { addSpecialChar(_T("\\0ddd"), 2); }
void CFindDlg::OnSpecialCharDecimalNumber()     { addSpecialChar(_T("\\ddd") , 1); }
void CFindDlg::OnSpecialCharHexadecimalNumber() { addSpecialChar(_T("\\xdd") , 2); }
void CFindDlg::OnSpecialCharBackslash()         { addSpecialChar(_T("\\\\")  , 2); }
void CFindDlg::OnSpecialCharTab()               { addSpecialChar(_T("\\t")   , 2); }
void CFindDlg::OnSpecialCharFormfeed()          { addSpecialChar(_T("\\f")   , 2); }

void CFindDlg::addSpecialChar(const String &s, int cursorPos) {
  UpdateData();

  String str = (LPCTSTR)m_findWhat;

  str = substr(str, 0, m_selStart) + s + substr(str, m_selEnd, str.length());

  m_findWhat  = str.cstr();

  m_selStart += cursorPos;
  m_selEnd   = m_selStart + (int)s.length() - cursorPos;

  UpdateData(false);
  OnGotoFindWhat();
}

void CFindDlg::OnGotoFindWhat() {
  getComboFindWhat()->SetFocus();
}

void CFindDlg::OnSetFocusComboFindWhat() {
  getComboFindWhat()->SetEditSel(m_selStart, m_selEnd);
  m_currentControl = IDC_COMBOFINDWHAT;
}

void CFindDlg::OnKillFocusComboFindWhat() {
  m_currentControl = 0;
}

CComboBox *CFindDlg::getComboFindWhat() {
  return (CComboBox*)GetDlgItem(IDC_COMBOFINDWHAT);
}

void CFindDlg::updateByteSequence(const CString &findWhat) {
  try {
    GetDlgItem(IDC_STATICBYTESEQUENCE)->SetWindowText(SearchPattern((LPCTSTR)findWhat).toString().cstr());
  } catch(Exception e) {
    showException(e);
  }
}

void CFindDlg::OnEditChangeComboFindWhat() {
  CString findWhat;
  getComboFindWhat()->GetWindowText(findWhat);
  updateByteSequence(findWhat);
}

void CFindDlg::OnEditUpdateComboFindWhat() {
  OnEditChangeComboFindWhat();
}

void CFindDlg::OnSelendOkComboFindWhat() {
  OnSelChangeComboFindWhat();
}

void CFindDlg::OnSelChangeComboFindWhat() {
  CComboBox *cb = getComboFindWhat();
  int index = cb->GetCurSel();
  CString str;
  cb->GetLBText(index, str);
  updateByteSequence(str);
}

void CFindDlg::OnSelendCancelComboFindWhat() {
  OnEditChangeComboFindWhat();
}

void CFindDlg::OnSize(UINT nType, int cx, int cy) {
  m_layoutManager.OnSize(nType,cx,cy);
  __super::OnSize(nType, cx, cy);
}

#define TIMERUPDATERATE 200

static const int controlIds[] = {
  IDC_COMBOFINDWHAT
 ,IDC_FINDNEXT
 ,IDC_RADIOUP
 ,IDC_RADIODOWN
 ,IDC_BUTTONSPECIALCHAR
};

void CFindDlg::enableControls(bool enable) {
  for(int i = 0; i < ARRAYSIZE(controlIds); i++) {
    GetDlgItem(controlIds[i])->EnableWindow(enable ? TRUE : FALSE);
  }
}

void CFindDlg::startTimer() {
  if(!m_timerIsRunning && SetTimer(1, TIMERUPDATERATE, NULL)) {
    m_timerIsRunning = true;
    enableControls(false);
    GetDlgItem(IDC_PROGRESSFIND)->ShowWindow(SW_SHOW);
    waitCursor(true);
  }
}

void CFindDlg::stopTimer() {
  if(m_timerIsRunning) {
    KillTimer(1);
    m_timerIsRunning = false;
    enableControls(true);
    GetDlgItem(IDC_PROGRESSFIND)->ShowWindow(SW_HIDE);
    waitCursor(false);
  }
}

void CFindDlg::waitCursor(bool on) {
  if(on == m_waitCursorOn) {
    return;
  }
  LPTSTR cursorId = on ? IDC_WAIT : IDC_ARROW;
  setWindowCursor(this, cursorId);
  for(int i = 0; i < ARRAYSIZE(controlIds); i++) {
    setWindowCursor(GetDlgItem(controlIds[i]), cursorId);
  }
  setWindowCursor(GetDlgItem(IDC_PROGRESSFIND), cursorId);
  m_waitCursorOn = on;
}
