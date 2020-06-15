#include "stdafx.h"
#include "ParserDemoDlg.h"
#include "ShowStateDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

ShowStateDlg::ShowStateDlg(CDialog *mainDialog, CWnd *pParent): CDialog(ShowStateDlg::IDD, pParent), m_mainDialog(mainDialog) {
  m_data = EMPTYSTRING;
  m_hIcon = theApp.LoadIcon(IDR_MAINFRAME);
}

void ShowStateDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDITSTATE, m_data);
}

BEGIN_MESSAGE_MAP(ShowStateDlg, CDialog)
  ON_MESSAGE(      ID_SHOWSTATE_UPDATE, OnShowStateUpdate)
END_MESSAGE_MAP()

void ShowStateDlg::ajourState() {
  TestParser &parser = ((CParserDemoDlg*)m_mainDialog)->m_parser;
  m_data = parser.getStateItems(parser.state()).cstr();
  UpdateData(false);
}

LRESULT ShowStateDlg::OnShowStateUpdate(WPARAM wp, LPARAM lp) {
  ajourState();
  return 0;
}

BOOL ShowStateDlg::OnInitDialog() {
  __super::OnInitDialog();

  SetIcon(m_hIcon, TRUE);  // Set big icon
  SetIcon(m_hIcon, FALSE); // Set small icon

  ajourState();

  m_accelTable = LoadAccelerators(theApp.m_hInstance, MAKEINTRESOURCE(IDR_ACCELERATORSHOWSTATE));
  return TRUE;  // return TRUE unless you set the focus to a control
	            // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL ShowStateDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd, m_accelTable, pMsg)) {
    theApp.m_pMainWnd->PostMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
    return true;
  }
  return __super::PreTranslateMessage(pMsg);
}
