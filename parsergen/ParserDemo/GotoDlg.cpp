#include "stdafx.h"
#include <WinTools.h>
#include "ParserDemo.h"
#include "GotoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

GotoDlg::GotoDlg(CWnd* pParent) : CDialog(GotoDlg::IDD, pParent) {
  //{{AFX_DATA_INIT(GotoDlg)
  m_line = 0;
  //}}AFX_DATA_INIT
}


void GotoDlg::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(GotoDlg)
  DDX_Text(pDX, IDC_EDITLINENUMBER, m_line);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(GotoDlg, CDialog)
  //{{AFX_MSG_MAP(GotoDlg)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL GotoDlg::OnInitDialog() {
  CDialog::OnInitDialog();
	
  gotoEditBox(this, IDC_EDITLINENUMBER);
  return FALSE;
}

void GotoDlg::OnOK() {
  UpdateData();	
  if(m_line < 1) {
    MessageBox("Please enter a linenumber > 0");
  } else {
    CDialog::OnOK();
  }
}
