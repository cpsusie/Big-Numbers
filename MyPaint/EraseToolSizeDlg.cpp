#include "stdafx.h"
#include "EraseToolSizeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CEraseToolSizeDlg::CEraseToolSizeDlg(const CSize &size, CWnd *pParent) : CDialog(CEraseToolSizeDlg::IDD, pParent) {
  m_height = size.cy;
  m_width = size.cx;
}


void CEraseToolSizeDlg::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDITHEIGHT, m_height);
  DDV_MinMaxUInt(pDX, m_height, 1, 100);
  DDX_Text(pDX, IDC_EDITWIDTH, m_width);
  DDV_MinMaxUInt(pDX, m_width, 1, 100);
}


BEGIN_MESSAGE_MAP(CEraseToolSizeDlg, CDialog)
END_MESSAGE_MAP()

BOOL CEraseToolSizeDlg::OnInitDialog() {
  CDialog::OnInitDialog();
  gotoEditBox(this, IDC_EDITWIDTH);
  return FALSE;
}
