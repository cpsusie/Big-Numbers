#include "stdafx.h"
#include "LixDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CLixDlg::CLixDlg(int lix, BOOL lixfilter, CWnd *pParent) : CDialog(CLixDlg::IDD, pParent) {
    m_lix = lix;
    m_lixfilter = lixfilter;
}


void CLixDlg::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDITLIX, m_lix);
    DDV_MinMaxInt(pDX, m_lix, 50, 400);
    DDX_Check(pDX, IDC_CHECKLIXFILTER, m_lixfilter);
}


BEGIN_MESSAGE_MAP(CLixDlg, CDialog)
END_MESSAGE_MAP()

BOOL CLixDlg::OnInitDialog() {
    CDialog::OnInitDialog();
    gotoEditBox(this, IDC_EDITLIX);
    return FALSE;
}
