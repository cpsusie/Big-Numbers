#include "stdafx.h"
#include "Spectrum.h"
#include "FrequenceDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CFrequenceDlg::CFrequenceDlg(unsigned int frequence, CWnd* pParent)	: CDialog(CFrequenceDlg::IDD, pParent) {
  m_frequence = frequence;
}

void CFrequenceDlg::DoDataExchange(CDataExchange* pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDITFREQUENCE, m_frequence);
  DDV_MinMaxUInt(pDX, m_frequence, 37, 0x7fff);
}

BEGIN_MESSAGE_MAP(CFrequenceDlg, CDialog)
END_MESSAGE_MAP()

BOOL CFrequenceDlg::OnInitDialog() {
  __super::OnInitDialog();

  gotoEditBox(this, IDC_EDITFREQUENCE);

  return FALSE;
}
