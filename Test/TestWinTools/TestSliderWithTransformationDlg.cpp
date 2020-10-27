#include "stdafx.h"
#include "TestSliderWithTransformationDlg.h"
#include "afxdialogex.h"

IMPLEMENT_DYNAMIC(CTestSliderWithTransformationDlg, CDialog)

CTestSliderWithTransformationDlg::CTestSliderWithTransformationDlg(CWnd *pParent /*=nullptr*/)
: CDialog(IDD_TESTSLIDER_DIALOG, pParent)
, m_from(0)
, m_to(100)
{
}

CTestSliderWithTransformationDlg::~CTestSliderWithTransformationDlg() {
}

void CTestSliderWithTransformationDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDIT_TO, m_to);
  DDX_Text(pDX, IDC_EDIT_FROM, m_from);
}


BEGIN_MESSAGE_MAP(CTestSliderWithTransformationDlg, CDialog)
  ON_WM_HSCROLL()
  ON_WM_VSCROLL()
  ON_BN_CLICKED(IDC_RADIO_LINEAR            , OnBnClickedRadioLinear            )
  ON_BN_CLICKED(IDC_RADIO_LOGARITHMIC       , OnBnClickedRadioLogarithmic       )
  ON_BN_CLICKED(IDC_RADIO_NORMALDISTRIBUTION, OnBnClickedRadioNormaldistribution)
  ON_BN_CLICKED(IDC_BUTTON_UPDATESLIDER     , OnBnClickedButtonUpdateSlider     )
  ON_COMMAND(   ID_GOTO_RANGE               , OnGotoRange                       )
END_MESSAGE_MAP()

BOOL CTestSliderWithTransformationDlg::OnInitDialog() {
  __super::OnInitDialog();

  m_slider.substituteControl(this, IDC_TESTSLIDER, DoubleInterval(0,100));
  m_accelTable = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_TESTSLIDER_ACCELERATOR));

  sliderTypeToRadiobutton();
  showSliderValue();

  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CTestSliderWithTransformationDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd, m_accelTable, pMsg)) {
    return true;
  }
  return __super::PreTranslateMessage(pMsg);
}

DoubleInterval CTestSliderWithTransformationDlg::getRange() {
  UpdateData();
  return DoubleInterval(m_from, m_to);
}

void CTestSliderWithTransformationDlg::OnGotoRange() {
  gotoEditBox(this, IDC_EDIT_FROM);
}

void CTestSliderWithTransformationDlg::OnBnClickedRadioLinear() {
  if(!validateLinearRange()) {
    sliderTypeToRadiobutton();
    return;
  }
  setSliderType(IntervalScale::LINEAR);
}


void CTestSliderWithTransformationDlg::OnBnClickedRadioLogarithmic() {
  if(!validateLogarithmicRange()) {
    sliderTypeToRadiobutton();
    return;
  }
  setSliderType(IntervalScale::LOGARITHMIC);
}

void CTestSliderWithTransformationDlg::OnBnClickedRadioNormaldistribution() {
  if(!validateNormalDistRange()) {
    sliderTypeToRadiobutton();
    return;
  }
  setSliderType(IntervalScale::NORMAL_DISTRIBUTION);
}

bool CTestSliderWithTransformationDlg::validateLinearRange() {
  const DoubleInterval range = getRange();
  if(range.getLength() == 0) {
    MessageBox(_T("Range-length = 0"));
    gotoEditBox(this, IDC_EDIT_FROM);
    return false;
  }
  return true;
}

bool CTestSliderWithTransformationDlg::validateLogarithmicRange() {
  const DoubleInterval range = getRange();
  if(range.getLength() == 0) {
    MessageBox(_T("Range-length = 0"));
    gotoEditBox(this, IDC_EDIT_FROM);
    return false;
  }
  if(range.getFrom() <= 0) {
    MessageBox(_T("From must be > 0 for logarithmic transformation"));
    gotoEditBox(this, IDC_EDIT_FROM);
    return false;
  }
  if(range.getTo() <= 0) {
    MessageBox(_T("To must be > 0 for logarithmic transformation"));
    gotoEditBox(this, IDC_EDIT_TO);
    return false;
  }
  return true;
}

bool CTestSliderWithTransformationDlg::validateNormalDistRange() {
  const DoubleInterval range = getRange();
  if(range.getLength() == 0) {
    MessageBox(_T("Range-length = 0"));
    gotoEditBox(this, IDC_EDIT_FROM);
    return false;
  }
  if(range.getFrom() <= 0 || range.getFrom() >= 1) {
    MessageBox(_T("From must be in range ]0..1{ for nomal.dist transformation"));
    gotoEditBox(this, IDC_EDIT_FROM);
    return false;
  }
  if(range.getTo() <= 0 || range.getTo() >= 1) {
    MessageBox(_T("To must be in range ]0..1{ for nomal.dist transformation"));
    gotoEditBox(this, IDC_EDIT_TO);
    return false;
  }
  return true;
}

void CTestSliderWithTransformationDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) {
  showSliderValue();
  __super::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CTestSliderWithTransformationDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) {
  showSliderValue();
  __super::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CTestSliderWithTransformationDlg::showSliderValue() {
  const double v = m_slider.getPos();
  setWindowText(this, IDC_STATIC_SLIDERVALUE, format(_T("%le"),v));
}

void CTestSliderWithTransformationDlg::setSliderType(IntervalScale type) {
  m_slider.setTransformation(getRange(), 1000, type);
  showSliderValue();
}

void CTestSliderWithTransformationDlg::sliderTypeToRadiobutton() {
  switch (m_slider.getTransformationType()) {
  case IntervalScale::LINEAR             :
    CheckRadioButton(IDC_RADIO_LINEAR, IDC_RADIO_NORMALDISTRIBUTION, IDC_RADIO_LINEAR            );
    break;
  case IntervalScale::LOGARITHMIC        :
    CheckRadioButton(IDC_RADIO_LINEAR, IDC_RADIO_NORMALDISTRIBUTION, IDC_RADIO_LOGARITHMIC       );
    break;
  case IntervalScale::NORMAL_DISTRIBUTION:
    CheckRadioButton(IDC_RADIO_LINEAR, IDC_RADIO_NORMALDISTRIBUTION, IDC_RADIO_NORMALDISTRIBUTION);
    break;
  }
}

void CTestSliderWithTransformationDlg::OnBnClickedButtonUpdateSlider() {
  switch (m_slider.getTransformationType()) {
  case IntervalScale::LINEAR             :
    OnBnClickedRadioLinear();
    break;
  case IntervalScale::LOGARITHMIC        :
    OnBnClickedRadioLogarithmic();
    break;
  case IntervalScale::NORMAL_DISTRIBUTION:
    OnBnClickedRadioNormaldistribution();
    break;
  }
}
