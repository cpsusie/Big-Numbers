#include "stdafx.h"
#include "sudoku.h"
#include "SelectGameDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CSelectGameDlg::CSelectGameDlg(CWnd *pParent) : CDialog(CSelectGameDlg::IDD, pParent) {
    m_seed = 0;
}


void CSelectGameDlg::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDITSEED, m_seed);
}


BEGIN_MESSAGE_MAP(CSelectGameDlg, CDialog)
END_MESSAGE_MAP()

void CSelectGameDlg::OnOK() {
  UpdateData(); 
  __super::OnOK();
}

BOOL CSelectGameDlg::OnInitDialog() {
  __super::OnInitDialog();

  CEdit *e = (CEdit*)GetDlgItem(IDC_EDITSEED);
  e->SetSel(0,1);
  e->SetFocus();
  return FALSE;
}
