#include "stdafx.h"
#include "DegreeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DegreeDlg::DegreeDlg(int degree, CWnd *pParent) : CDialog(DegreeDlg::IDD, pParent) {
  m_degree = degree;
}

void DegreeDlg::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_DEGREE, m_degree);
}

BEGIN_MESSAGE_MAP(DegreeDlg, CDialog)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, OnDeltaposSpin1)
END_MESSAGE_MAP()

BOOL DegreeDlg::OnInitDialog() {
  __super::OnInitDialog();
  gotoEditBox(this,IDC_EDIT_DEGREE);
  return false;
}

void DegreeDlg::OnOK() {
  UpdateData(TRUE);
  if(m_degree > MAXDEGREE) {
    showWarning(_T("Maxdegree = %d"),MAXDEGREE);
    return;
  }
  __super::OnOK();
}

void DegreeDlg::OnDeltaposSpin1(NMHDR *pNMHDR, LRESULT *pResult) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

    UpdateData();
    int ng = m_degree - pNMUpDown->iDelta;
    if(ng < 0 || ng > MAXDEGREE) return;
    m_degree = ng;

    UpdateData(false);
    *pResult = 0;
}
