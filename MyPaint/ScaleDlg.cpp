#include "stdafx.h"
#include <math.h>
#include "ScaleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CScaleDlg::CScaleDlg(const ScaleParameters &scale, CWnd *pParent) : CDialog(CScaleDlg::IDD, pParent), m_scale(scale) {
  m_toSize      = m_scale.m_toSize;
  m_scaleWidth  = m_scale.m_scale.x;
  m_scaleHeight = m_scale.m_scale.y;
}

void CScaleDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Radio(pDX, IDC_RADIOPERCENT, m_toSize);
  DDX_Text(pDX, IDC_SCALEWIDTHEDIT, m_scaleWidth);
  DDX_Text(pDX, IDC_SCALEHEIGHTEDIT, m_scaleHeight);
}

BEGIN_MESSAGE_MAP(CScaleDlg, CDialog)
  ON_BN_CLICKED(IDC_RADIOPERCENT, OnRadiopercent)
  ON_BN_CLICKED(IDC_RADIOPIXELS, OnRadiopixels)
  ON_COMMAND(ID_GOTO_WIDTH,  OnGotoWidth)
  ON_COMMAND(ID_GOTO_HEIGHT, OnGotoHeight)
END_MESSAGE_MAP()

BOOL CScaleDlg::OnInitDialog() {
  __super::OnInitDialog();

  m_accelTable = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_ACCELERATOR_SCALEDIALOG));
  if(!m_toSize) {
    m_scaleWidth *= 100;
    m_scaleHeight *= 100;
  }
  UpdateData(false);

  setLabels();
  GetDlgItem(IDC_RADIOPERCENT)->SetFocus();

  return FALSE;
}

bool isInteger(double x) {
  return x == floor(x);
}

void CScaleDlg::OnOK() {
  if(!UpdateData()) {
    return;
  }
  if(m_toSize) {
    if(!isInteger(m_scaleWidth)) {
      MessageBox(_T("Must be integer"));
      gotoField(IDC_SCALEWIDTHEDIT);
      return;
    }
    if(!isInteger(m_scaleHeight)) {
      MessageBox(_T("Must be integer"));
      gotoField(IDC_SCALEHEIGHTEDIT);
      return;
    }
  }
  if(m_scaleWidth <= 0) {
    MessageBox(_T("Must be positive"));
    gotoField(IDC_SCALEWIDTHEDIT);
    return;
  }
  if(m_scaleHeight <= 0) {
    MessageBox(_T("Must be positive"));
    gotoField(IDC_SCALEHEIGHTEDIT);
    return;
  }

  m_scale.m_toSize  = m_toSize ? true : false;
  m_scale.m_scale.x = m_scaleWidth;
  m_scale.m_scale.y = m_scaleHeight;
  if(!m_toSize) {
    m_scale.m_scale.x /= 100;
    m_scale.m_scale.y /= 100;
  }

  __super::OnOK();
}

void CScaleDlg::setLabels() {
  if(IsDlgButtonChecked(IDC_RADIOPERCENT)) {
    setWindowText(this, IDC_STATICXLABEL2, _T("%"));
    setWindowText(this, IDC_STATICYLABEL2, _T("%"));
  } else {
    setWindowText(this, IDC_STATICXLABEL2, _T("pixels"));
    setWindowText(this, IDC_STATICYLABEL2, _T("pixels"));
  }
}

void CScaleDlg::OnRadiopercent() {
  setLabels();
}

void CScaleDlg::OnRadiopixels() {
  setLabels();
}

BOOL CScaleDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }
  return __super::PreTranslateMessage(pMsg);
}

void CScaleDlg::gotoField(int id) {
  gotoEditBox(this, id);
}

void CScaleDlg::OnGotoWidth() {
  gotoField(IDC_SCALEWIDTHEDIT);
}

void CScaleDlg::OnGotoHeight() {
  gotoField(IDC_SCALEHEIGHTEDIT);
}
