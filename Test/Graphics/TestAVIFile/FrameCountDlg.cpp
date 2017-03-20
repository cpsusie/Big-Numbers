#include "stdafx.h"
#include "TestAVIFile.h"
#include "FrameCountDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CFrameCountDlg::CFrameCountDlg(UINT frameCount, CWnd *pParent /*=NULL*/) : CDialog(CFrameCountDlg::IDD, pParent) {
    m_frameCount = frameCount;
}


void CFrameCountDlg::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_FRAMECOUNT, m_frameCount);
    DDV_MinMaxUInt(pDX, m_frameCount, 1, 30000);
}


BEGIN_MESSAGE_MAP(CFrameCountDlg, CDialog)
END_MESSAGE_MAP()

BOOL CFrameCountDlg::OnInitDialog() {
  CDialog::OnInitDialog();
    
  gotoEditBox(this, IDC_EDIT_FRAMECOUNT);
  return FALSE;
}

void CFrameCountDlg::OnOK() {
  if(UpdateData()) {
    CDialog::OnOK();
  }
}

void CFrameCountDlg::OnCancel() {
  CDialog::OnCancel();
}

