#include "stdafx.h"
#include "PartyMaker.h"
#include "CheckFilesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CCheckFilesDlg::CCheckFilesDlg(CWnd *pParent) : CDialog(CCheckFilesDlg::IDD, pParent) {

  m_hIcon = AfxGetApp()->LoadIcon(IDI_CHECKFILESICON);
}

void CCheckFilesDlg::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCheckFilesDlg, CDialog)
  ON_BN_CLICKED(IDC_CHECKSTATUS, OnCheckstatus)
  ON_WM_TIMER()
  ON_BN_CLICKED(IDC_SHOWERRORS, OnShowErrors)
END_MESSAGE_MAP()

BOOL CCheckFilesDlg::OnInitDialog() {
  CDialog::OnInitDialog();
  
  SetIcon(m_hIcon, TRUE);
  SetIcon(m_hIcon, FALSE);

  CPartyMakerDlg *w = (CPartyMakerDlg*)AfxGetApp()->GetMainWnd();
  m_accelTable = LoadAccelerators(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDR_CHECKFILES_ACCELERATOR));
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
  GetDlgItem(IDC_CHECKSTATUS)->SetWindowText(_T("&Suspend"));
  Invalidate(false);
}

void CCheckFilesDlg::stopWorker() {
  stopTimer();
  m_worker->SuspendThread();
  GetDlgItem(IDC_CHECKSTATUS)->SetWindowText(_T("&Resume"));
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
  const TCHAR *fname = _T("");
  int index = m_worker->m_index;
  if(index < m_worker->m_mediaArray.size()) {
    fname = m_worker->m_mediaArray[index].getFileName();
  } else {
    stopTimer();
    GetDlgItem(IDCANCEL)->SetWindowText(_T("Ok"));
  }

  CProgressCtrl *p = (CProgressCtrl*)GetDlgItem(IDC_PROGRESSCHECK);
  p->SetPos(m_worker->m_index);
  const String tmp = format(_T("%d"),m_worker->m_errorCount);
  GetDlgItem(IDC_STATICERRORS)->SetWindowText(tmp);
  GetDlgItem(IDC_STATICMESSAGE)->SetWindowText(fname);
  CDialog::OnTimer(nIDEvent);
}

void CCheckFilesDlg::OnCancel() {
  m_worker->PostThreadMessage(WM_QUIT,0,0);
  CDialog::OnCancel();
}

BOOL CCheckFilesDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }
    
  return CDialog::PreTranslateMessage(pMsg);
}

void CCheckFilesDlg::OnShowErrors() {
  showErrors();
}
