#include "stdafx.h"
#include "TestAVIFile.h"
#include "FrameCountDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

CFrameCountDlg::CFrameCountDlg(UINT frameCount, CWnd *pParent /*=nullptr*/) : CDialog(CFrameCountDlg::IDD, pParent) {
    m_frameCount = frameCount;
}


void CFrameCountDlg::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_FRAMECOUNT, m_frameCount);
    DDV_MinMaxUInt(pDX, m_frameCount, 1, 30000);
}


BEGIN_MESSAGE_MAP(CFrameCountDlg, CDialog)
END_MESSAGE_MAP()

BOOL CFrameCountDlg::OnInitDialog() {
  __super::OnInitDialog();

  gotoEditBox(this, IDC_EDIT_FRAMECOUNT);
  return FALSE;
}

void CFrameCountDlg::OnOK() {
  if(UpdateData()) {
    __super::OnOK();
  }
}

void CFrameCountDlg::OnCancel() {
  __super::OnCancel();
}

