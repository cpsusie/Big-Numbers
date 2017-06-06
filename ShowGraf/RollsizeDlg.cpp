#include "stdafx.h"
#include "showgraf.h"
#include "RollsizeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

RollsizeDlg::RollsizeDlg(int rollSize, CWnd* pParent) : CDialog(RollsizeDlg::IDD, pParent) {
    m_rollSize = rollSize;
}


void RollsizeDlg::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDITROLLSIZE, m_rollSize);
    DDV_MinMaxUInt(pDX, m_rollSize, 2, 10000);
}

BEGIN_MESSAGE_MAP(RollsizeDlg, CDialog)
END_MESSAGE_MAP()

BOOL RollsizeDlg::OnInitDialog() {
  __super::OnInitDialog();
  
  gotoEditBox(this, IDC_EDITROLLSIZE);
  return FALSE;
}
