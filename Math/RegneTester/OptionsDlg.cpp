#include "stdafx.h"
#include "regnetester.h"
#include "OptionsDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

COptionsDlg::COptionsDlg(Options &options, CWnd *pParent /*=nullptr*/)
: m_options(options)
, CDialog(COptionsDlg::IDD, pParent)
{
    m_confirmPassword = EMPTYSTRING;
    m_highscorePath   = options.m_highScoreFileName.cstr();
    m_oldPassword     = EMPTYSTRING;
    m_password        = EMPTYSTRING;
}


void COptionsDlg::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDITCONFIRMPASSWORD, m_confirmPassword);
    DDX_Text(pDX, IDC_EDITHIGSCOREPATH, m_highscorePath);
    DDX_Text(pDX, IDC_EDITOLDPASSWORD, m_oldPassword);
    DDX_Text(pDX, IDC_EDITPASSWORD, m_password);
}

BEGIN_MESSAGE_MAP(COptionsDlg, CDialog)
    ON_BN_CLICKED(IDC_BUTTONFILE, OnButtonFile)
END_MESSAGE_MAP()

BOOL COptionsDlg::OnInitDialog() {
  __super::OnInitDialog();

  gotoEditBox(this, IDC_EDITOLDPASSWORD);
  return FALSE;
}

void COptionsDlg::OnOK() {
  UpdateData();
  if((LPCTSTR)m_oldPassword != m_options.m_password) {
    gotoEditBox(this, IDC_EDITPASSWORD);
    showWarning(_T("Forkert kodeord"));
    return;
  }
  if(m_password != m_confirmPassword) {
    gotoEditBox(this, IDC_EDITCONFIRMPASSWORD);
    showWarning(_T("De 2 kodeord er ikke ens"));
    return;
  }

  if((LPCTSTR)m_password != m_options.m_password || (LPCTSTR)m_highscorePath != m_options.m_highScoreFileName) {
    m_options.m_password          = m_password;
    m_options.m_highScoreFileName = m_highscorePath;
    if(!m_options.save()) {
      return;
    }
  }
  __super::OnOK();
}

void COptionsDlg::OnButtonFile() {
  UpdateData();
  TCHAR *fileName = nullptr;
  TCHAR  fileNameBuffer[256];
  if(m_highscorePath.GetLength() > 0) {
    _tcscpy(fileNameBuffer, (LPCTSTR)m_highscorePath);
    fileName = fileNameBuffer;
  }

  CFileDialog dlg(FALSE, _T("dat"), fileName, OFN_ENABLESIZING);
  if(dlg.DoModal() == IDOK) {
    m_highscorePath = dlg.m_ofn.lpstrFile;
    UpdateData(FALSE);
  }
}
