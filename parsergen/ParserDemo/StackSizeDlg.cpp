#include "stdafx.h"
#include <WinTools.h>
#include "ParserDemo.h"
#include "StackSizeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

StackSizeDlg::StackSizeDlg(int oldSize, CWnd* pParent) : CDialog(StackSizeDlg::IDD, pParent) {
    //{{AFX_DATA_INIT(StackSizeDlg)
    m_stackSize = oldSize;
    //}}AFX_DATA_INIT
}

void StackSizeDlg::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(StackSizeDlg)
    DDX_Text(pDX, IDC_EDITSTACKSIZE, m_stackSize);
    DDV_MinMaxUInt(pDX, m_stackSize, 256, 10000);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(StackSizeDlg, CDialog)
    //{{AFX_MSG_MAP(StackSizeDlg)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void StackSizeDlg::OnOK() {
  UpdateData();

  CDialog::OnOK();
}

BOOL StackSizeDlg::OnInitDialog() {
  CDialog::OnInitDialog();
  gotoEditBox(this, IDC_EDITSTACKSIZE);
  return false;
}
