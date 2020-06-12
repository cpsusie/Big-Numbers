#include "stdafx.h"
#include "PartyMaker.h"
#include "CheckFilesDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

CCheckFilesDlg::CCheckFilesDlg(CWnd *pParent) : CDialog(CCheckFilesDlg::IDD, pParent) {
  m_hIcon = theApp.LoadIcon(IDI_CHECKFILESICON);
}

void CCheckFilesDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCheckFilesDlg, CDialog)
  ON_BN_CLICKED(IDC_CHECKSTATUS, OnCheckstatus)
  ON_WM_TIMER()
  ON_BN_CLICKED(IDC_SHOWERRORS, OnShowErrors)
END_MESSAGE_MAP()

BOOL CCheckFilesDlg::OnInitDialog() {
  __super::OnInitDialog();

  SetIcon(m_hIcon, TRUE);
  SetIcon(m_hIcon, FALSE);

  CPartyMakerDlg *w = theApp.GetMainWnd();
  m_accelTable = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_CHECKFILES_ACCELERATOR));
  const MediaArray &list = w->getMediaArray();
  CProgressCtrl  *p = (CProgressCtrl*)GetDlgItem(IDC_PROGRESSCHECK);
  p->SetRange(0,list.size());

  m_workerIsRunning = false;
  m_worker = (CCheckFilesThread*)AfxBeginThread(RUNTIME_CLASS(CCheckFilesThread));
  startTimer();
  return false;
}

void CCheckFilesDlg::OnCheckstatus() {
  if(m_workerIsRunning) {
    stopWorker();
  } else {
    startWorker();
  }
}

void CCheckFilesDlg::startWorker() {
  startTimer();
  m_worker->ResumeThread();
  setWindowText(this, IDC_CHECKSTATUS, _T("&Suspend"));
  Invalidate(false);
}

void CCheckFilesDlg::stopWorker() {
  stopTimer();
  m_worker->SuspendThread();
  setWindowText(this, IDC_CHECKSTATUS, _T("&Resume"));
  Invalidate(false);
}

#define TIMERUPDATERATE 200

void CCheckFilesDlg::startTimer() {
  if(m_workerIsRunning) {
    return;
  }
  int iInstallResult = SetTimer(1,TIMERUPDATERATE,NULL);
  m_workerIsRunning = true;
}

void CCheckFilesDlg::stopTimer() {
  if(m_workerIsRunning) {
    KillTimer(1);
  }
  m_workerIsRunning = false;
}

void CCheckFilesDlg::OnTimer(UINT nIDEvent) {
  const TCHAR *fname = EMPTYSTRING;
  int index = m_worker->m_index;
  if(index < m_worker->m_mediaArray.size()) {
    fname = m_worker->m_mediaArray[index].getFileName();
  } else {
    stopTimer();
    setWindowText(this, IDCANCEL, _T("Ok"));
  }

  CProgressCtrl *p = (CProgressCtrl*)GetDlgItem(IDC_PROGRESSCHECK);
  p->SetPos(m_worker->m_index);
  setWindowText(this, IDC_STATICERRORS , format(_T("%d"),m_worker->m_errorCount));
  setWindowText(this, IDC_STATICMESSAGE, fname);
  __super::OnTimer(nIDEvent);
}

void CCheckFilesDlg::OnCancel() {
  m_worker->PostThreadMessage(WM_QUIT,0,0);
  __super::OnCancel();
}

BOOL CCheckFilesDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }
  return __super::PreTranslateMessage(pMsg);
}

void CCheckFilesDlg::OnShowErrors() {
  showErrors();
}
