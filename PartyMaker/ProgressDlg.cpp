#include "stdafx.h"
#include "PartyMaker.h"
#include "ProgressDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CProgressDlg::CProgressDlg(LoadableMediaArray *mediaArray, CWnd *pParent) : CDialog(CProgressDlg::IDD, pParent) {
    m_mediaArray = mediaArray;
    m_hIcon = theApp.LoadIcon(IDI_READFILESICON);
}

void CProgressDlg::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CProgressDlg, CDialog)
    ON_WM_TIMER()
END_MESSAGE_MAP()

BOOL CProgressDlg::OnInitDialog() {
  __super::OnInitDialog();

  SetIcon(m_hIcon, TRUE);
  SetIcon(m_hIcon, FALSE);
  SetTimer(2,250,NULL);
  return TRUE;
}

void CProgressDlg::OnCancel() {
  m_mediaArray->cancelScan();
  __super::OnCancel();
}

void CProgressDlg::OnTimer(UINT_PTR nIDEvent) {
  setWindowText(this, IDC_STATICDIRNAME, m_mediaArray->getCurrentFileName());
  setWindowText(this, IDC_STATICOUNT   , format(_T("%lu"), m_mediaArray->size()));
  __super::OnTimer(nIDEvent);
}
