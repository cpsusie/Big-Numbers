#include "stdafx.h"
#include "PartyMaker.h"
#include "MaxChoiseDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

MaxChoiseDlg::MaxChoiseDlg(int maxChoise, CWnd *pParent) : CDialog(MaxChoiseDlg::IDD, pParent) {
  m_maxChoise = maxChoise;
}


void MaxChoiseDlg::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_MAXCHOISEEDIT, m_maxChoise);
  DDV_MinMaxUInt(pDX, m_maxChoise, 1, 50);
}

BOOL MaxChoiseDlg::OnInitDialog() {
  CDialog::OnInitDialog();
  CEdit *e = (CEdit*)GetDlgItem(IDC_MAXCHOISEEDIT);
  e->SetFocus();
  e->SetSel(0,2);
  return false;
}

BEGIN_MESSAGE_MAP(MaxChoiseDlg, CDialog)
END_MESSAGE_MAP()
