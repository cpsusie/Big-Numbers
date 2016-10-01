#include "stdafx.h"
#include <MFCUtil/WinTools.h>
#include <FileVersion.h>
#include "ChessDlg.h"
#include "AboutDlg.h"

CAboutDlg::CAboutDlg(CChessDlg *parent) : CDialog(CAboutDlg::IDD), m_parent(parent) {
}

BOOL CAboutDlg::OnInitDialog() {
  CDialog::OnInitDialog();
  setControlText(IDD, this);
  const String fileName = getModuleFileName();
  const String name     = FileNameSplitter(fileName).getFileName();
  String compileYear    = __DATE__;
  compileYear           = substr(compileYear, compileYear.length() - 4, 4);

  setWindowText(this, IDC_STATIC_COPYRIGHT, getWindowText(this, IDC_STATIC_COPYRIGHT) + compileYear);
  FileVersion version(fileName);
  const String text = getWindowText(this, IDC_STATIC_INFO);
  String newText = format(_T("%s® Version %s\n\n%s"), name.cstr(), version.getFileVersion().cstr(), text.cstr());
  setWindowText(this, IDC_STATIC_INFO, newText);
  return TRUE;
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
    ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

void CAboutDlg::OnLButtonDown(UINT nFlags, CPoint point) {
  CRect iconRect = getRelativeClientRect(this, IDC_ICON_CHESS);
  if(iconRect.PtInRect(point) && (nFlags & MK_CONTROL)) {
    m_parent->toggleEnableTestMenu();
  }
  CDialog::OnLButtonDown(nFlags, point);
}
