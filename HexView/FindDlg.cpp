#include "stdafx.h"
#include <ThreadPool.h>
#include <InterruptableRunnable.h>
#include "FindDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

CFindDlg::CFindDlg(CMainFrame *pParent)
: CDialog(IDD, pParent)
, m_searchParameters(pParent->getSearchParameters())
{
  m_findWhat       = m_searchParameters.m_findWhat.cstr();
  m_timerIsRunning = false;
  m_waitCursorOn   = false;
}

CFindDlg::~CFindDlg() {
}

CMainFrame &CFindDlg::getMainWin() {
  return *(CMainFrame*)GetParent();
}

void CFindDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_CBString(pDX, IDC_COMBOFINDWHAT, m_findWhat);
  if(!pDX->m_bSaveAndValidate) {
    OnEditChangeComboFindWhat();
  }
}

BEGIN_MESSAGE_MAP(CFindDlg, CDialog)
  ON_WM_TIMER()
  ON_BN_CLICKED(IDC_FINDNEXT                  , OnFindNext                    )
  ON_BN_CLICKED(IDC_BUTTONSPECIALCHAR         , OnButtonSpecialChar           )
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

BOOL CFindDlg::OnInitDialog() {
  __super::OnInitDialog();

  m_findWhatCombo.substituteControl( this, IDC_COMBOFINDWHAT, _T("FindHistory"));
  const CRect cbRect = getWindowRect(m_findWhatCombo);
  const CPoint buttonPos(cbRect.right+1, cbRect.top);
  m_specialCharButton.Create(this, OBMIMAGE(RGARROW), buttonPos, IDC_BUTTONSPECIALCHAR, true);
  LoadDynamicLayoutResource(m_lpszTemplateName);

  GetDlgItem(IDC_STATICRADIXNAME)->SetWindowText(getSettings().getDataRadixShortName());
  m_currentControl = 0;
  m_selStart       = 0;
  m_selEnd         = m_findWhat.GetLength();

  if(m_searchParameters.isForwardSearch()) {
    ((CButton*)GetDlgItem(IDC_RADIODOWN))->SetCheck(1);
  } else {
    ((CButton*)GetDlgItem(IDC_RADIOUP))->SetCheck(1);
  }
  m_accelTable = LoadAccelerators(theApp.m_hInstance, MAKEINTRESOURCE(IDR_ACCELERATORFIND));
  UpdateData(false);
  gotoFindWhat();
  return FALSE;
}

void CFindDlg::OnFindNext() {
  UpdateData();
  if(m_findWhat.GetLength() == 0) {
    gotoFindWhat();
    return;
  }

  try {
    m_searchParameters.m_forwardSearch = ((CButton*)GetDlgItem(IDC_RADIODOWN))->GetCheck() ? true : false;
    m_searchParameters.m_findWhat      = (LPCTSTR)m_findWhat;
    ThreadPool::executeNoWait(*getMainWin().createSearchMachine());
    CProgressCtrl *p = (CProgressCtrl*)GetDlgItem(IDC_PROGRESSFIND);
    p->SetRange(0, 100);
    p->SetPos(0);
    startTimer();
  } catch(Exception e) {
    gotoFindWhat();
    showException(e);
  }
}

void CFindDlg::OnTimer(UINT_PTR nIDEvent) {
  __super::OnTimer(nIDEvent);
  SearchMachine *sm = getSearchMachine();
  if(sm->isJobDone()) {
    stopTimer();
    m_result = sm->getResult();
    if(m_result.isEmpty()) {
      gotoFindWhat();
      if(!sm->isInterrupted()) {
        showWarning(_T("%s"), sm->getErrorMsg().cstr());
      }
    } else {
      m_findWhatCombo.updateList();
      __super::OnOK();
    }
  } else {
    CProgressCtrl *p = (CProgressCtrl*)GetDlgItem(IDC_PROGRESSFIND);
    p->SetPos((int)sm->getPercentDone());
  }
}

void CFindDlg::OnCancel() {
  if(m_timerIsRunning) {
    getSearchMachine()->setInterrupted();
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
    DWORD w = getFindWhatCombo()->GetEditSel();
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
  gotoFindWhat();
}

void CFindDlg::gotoFindWhat() {
  getFindWhatCombo()->SetFocus();
}

void CFindDlg::OnSetFocusComboFindWhat() {
  getFindWhatCombo()->SetEditSel(m_selStart, m_selEnd);
  m_currentControl = IDC_COMBOFINDWHAT;
}

void CFindDlg::OnKillFocusComboFindWhat() {
  m_currentControl = 0;
}

void CFindDlg::updateByteSequence(const CString &findWhat) {
  try {
    GetDlgItem(IDC_STATICBYTESEQUENCE)->SetWindowText(SearchPattern((LPCTSTR)findWhat).toString().cstr());
  } catch(Exception e) {
    showException(e);
  }
}

CComboBox *CFindDlg::getFindWhatCombo() {
  return (CComboBox*)GetDlgItem(IDC_COMBOFINDWHAT);
}
void CFindDlg::OnEditChangeComboFindWhat() {
  CString findWhat;
  getFindWhatCombo()->GetWindowText(findWhat);
  updateByteSequence(findWhat);
}

void CFindDlg::OnEditUpdateComboFindWhat() {
  OnEditChangeComboFindWhat();
}

void CFindDlg::OnSelendOkComboFindWhat() {
  OnSelChangeComboFindWhat();
}

void CFindDlg::OnSelChangeComboFindWhat() {
  int index = getFindWhatCombo()->GetCurSel();
  CString str;
  getFindWhatCombo()->GetLBText(index, str);
  updateByteSequence(str);
}

void CFindDlg::OnSelendCancelComboFindWhat() {
  OnEditChangeComboFindWhat();
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
