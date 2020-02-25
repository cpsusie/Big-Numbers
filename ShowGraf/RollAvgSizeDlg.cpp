#include "stdafx.h"
#include "RollAvgSizeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

RollAvgSizeDlg::RollAvgSizeDlg(int rollAvgSize, CWnd *pParent) : CDialog(IDD, pParent) {
  m_rollAvgSize = rollAvgSize;
}

void RollAvgSizeDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDITROLLAVGSIZE, m_rollAvgSize);
  DDV_MinMaxUInt(pDX, m_rollAvgSize, 2, 10000);
}

BEGIN_MESSAGE_MAP(RollAvgSizeDlg, CDialog)
END_MESSAGE_MAP()

BOOL RollAvgSizeDlg::OnInitDialog() {
  __super::OnInitDialog();
  gotoEditBox(this, IDC_EDITROLLAVGSIZE);
  return FALSE;
}
