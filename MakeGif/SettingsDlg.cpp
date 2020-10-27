#include "stdafx.h"
#include "SettingsDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

CSettingsDlg::CSettingsDlg(const ImageSettings &settings, CWnd *pParent /*=nullptr*/) : CDialog(CSettingsDlg::IDD, pParent) {
  m_colorCount  = settings.m_colorCount;
  m_scaleFactor = (UINT)(settings.m_imageScaleFactor * 100);
}


void CSettingsDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDIT_SCALEFACTOR, m_scaleFactor);
  DDV_MinMaxUInt(pDX, m_scaleFactor, 1, 200);
}


BEGIN_MESSAGE_MAP(CSettingsDlg, CDialog)
END_MESSAGE_MAP()

ImageSettings CSettingsDlg::getImageSettings() const {
  ImageSettings result;
  result.m_colorCount       = m_colorCount;
  result.m_imageScaleFactor = (double)m_scaleFactor / 100.0;
  return result;
}

void CSettingsDlg::OnOK() {
  if(UpdateData()) {
    m_colorCount = getColorCount();
    __super::OnOK();
  }
}

BOOL CSettingsDlg::OnInitDialog() {
  __super::OnInitDialog();

  setColorCount(m_colorCount);
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

CComboBox *CSettingsDlg::getColorCountCombo() {
  return (CComboBox*)GetDlgItem(IDC_COMBO_COLORCOUNT);
}

static bool isPowerOfTwo (unsigned int n) {
  return (n != 0) && !(n & (n - 1));
}

int CSettingsDlg::getColorCount() {
  CComboBox *cb = getColorCountCombo();
  const int index = cb->GetCurSel();
  if(index < 0) {
    return 32;
  }
  CString str;
  cb->GetLBText(index, str);
  int result;
  if(_stscanf((LPCTSTR)str, _T("%d"), &result) != 1) {
    return 32;
  }
  return result;
}

void CSettingsDlg::setColorCount(int colorCount) {
  CComboBox *cb = getColorCountCombo();
  if(!isPowerOfTwo(colorCount)) {
    colorCount = colorCount ^ (colorCount & (colorCount-1));
  }
  const int itemCount = cb->GetCount();
  for(int i = 0; i < itemCount; i++) {
    CString str;
    int itemValue;
    cb->GetLBText(i, str);
    _stscanf((LPCTSTR)str, _T("%d"), &itemValue);
    if(itemValue == colorCount) {
      cb->SetCurSel(i);
      return;
    }
  }
  cb->SetCurSel(5);
}
