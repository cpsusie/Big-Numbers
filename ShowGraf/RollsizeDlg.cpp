#include "stdafx.h"
#include "showgraf.h"
#include "RollsizeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

RollsizeDlg::RollsizeDlg(int rollSize, CWnd* pParent) : CDialog(RollsizeDlg::IDD, pParent) {
    //{{AFX_DATA_INIT(RollsizeDlg)
    m_rollSize = rollSize;
    //}}AFX_DATA_INIT
}


void RollsizeDlg::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(RollsizeDlg)
    DDX_Text(pDX, IDC_EDITROLLSIZE, m_rollSize);
    DDV_MinMaxUInt(pDX, m_rollSize, 2, 10000);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(RollsizeDlg, CDialog)
    //{{AFX_MSG_MAP(RollsizeDlg)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL RollsizeDlg::OnInitDialog() {
  CDialog::OnInitDialog();
  
  gotoEditBox(this, IDC_EDITROLLSIZE);
  return FALSE;
}
