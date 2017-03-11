#include "stdafx.h"
#include "DegreeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CDegreeDlg::CDegreeDlg(double degree, CWnd *pParent) : CDialog(CDegreeDlg::IDD, pParent) {
  m_degree = degree;
}

void CDegreeDlg::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDITDEGREE, m_degree);
  DDV_MinMaxDouble(pDX, m_degree, -180., 180.);
}

BEGIN_MESSAGE_MAP(CDegreeDlg, CDialog)
END_MESSAGE_MAP()

void CDegreeDlg::OnOK() {
  UpdateData();
  CDialog::OnOK();
}

BOOL CDegreeDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  gotoEditBox(this, IDC_EDITDEGREE);
  return FALSE;
}
