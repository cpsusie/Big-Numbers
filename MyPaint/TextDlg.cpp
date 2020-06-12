#include "stdafx.h"
#include "TextDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

CTextDlg::CTextDlg(FontParameters &fontParameters, String &text, CWnd *pParent) : CDialog(CTextDlg::IDD, pParent) {
  m_fontParameters = fontParameters;
  m_text   = text.cstr();
  m_degree = m_fontParameters.m_orientation;
}

void CTextDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDITTEXT, m_text);
  DDX_Text(pDX, IDC_EDITORIENTATION, m_degree);
}


BEGIN_MESSAGE_MAP(CTextDlg, CDialog)
  ON_BN_CLICKED(IDC_BUTTONFONT, OnButtonfont)
END_MESSAGE_MAP()

BOOL CTextDlg::OnInitDialog() {
  __super::OnInitDialog();

  gotoEditBox(this, IDC_EDITTEXT);
  return FALSE;
}

void CTextDlg::OnButtonfont() {
  CFontDialog dlg(&m_fontParameters.m_logFont);
  if(dlg.DoModal() == IDOK) {
    dlg.GetCurrentFont(&m_fontParameters.m_logFont);
  }
}

void CTextDlg::OnOK() {
  UpdateData();
  m_fontParameters.m_orientation = m_degree;
  __super::OnOK();
}
