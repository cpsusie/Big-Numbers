#include "stdafx.h"
#include "ApproximateFillDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CApproximateFillDlg::CApproximateFillDlg(UINT tolerance, CWnd *pParent) : CDialog(CApproximateFillDlg::IDD, pParent){
  m_tolerance = tolerance;
}

void CApproximateFillDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDITTOLERANCE, m_tolerance);
  DDV_MinMaxUInt(pDX, m_tolerance, 0, 255);
}

BEGIN_MESSAGE_MAP(CApproximateFillDlg, CDialog)
END_MESSAGE_MAP()

BOOL CApproximateFillDlg::OnInitDialog() {
  __super::OnInitDialog();
  gotoEditBox(this, IDC_EDITTOLERANCE);
  return FALSE;
}
