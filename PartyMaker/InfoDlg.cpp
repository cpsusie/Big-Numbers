#include "stdafx.h"
#include "partymaker.h"
#include "InfoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CInfoDlg::CInfoDlg(const MediaFile &mediaFile, CWnd *pParent)
: m_attributes(mediaFile.getAttributes())
, CDialog(CInfoDlg::IDD, pParent) {

}


void CInfoDlg::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CInfoDlg, CDialog)
    ON_WM_SIZE()
END_MESSAGE_MAP()


BOOL CInfoDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  const int maxNameLength = m_attributes.getMaxNameLength();
  const int maxValueLength = m_attributes.getMaxValueLength();

  CListBox *lb = (CListBox*)GetDlgItem(IDC_ATTRIBUTELIST);
  for(size_t i = 0; i < m_attributes.size(); i++) {
    const MediaAttribute &attr = m_attributes[i];
    String line = format(_T("%c %-*s %s"), attr.isReadOnly()?'r':' ', maxNameLength, attr.getName(), attr.getValue());
    lb->AddString(line.cstr());
  }

  m_layoutManager.OnInitDialog(this);
  m_layoutManager.addControl(IDC_ATTRIBUTELIST, RELATIVE_SIZE    );
  m_layoutManager.addControl(IDOK             , RELATIVE_POSITION);

  return TRUE;
}

void CInfoDlg::OnSize(UINT nType, int cx, int cy) {
  m_layoutManager.OnSize(nType,cx,cy);
  CDialog::OnSize(nType, cx, cy);
}
