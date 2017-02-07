#pragma once

#include <MFCUtil/SliderCtrlWithTransformation.h>

class CTestSliderWithTransformationDlg : public CDialog {
	DECLARE_DYNAMIC(CTestSliderWithTransformationDlg)

private:
  HACCEL                        m_accelTable;
  CSliderCtrlWithTransformation m_slider;
  double                        m_from;
  double                        m_to;

  DoubleInterval getRange();
  bool validateLinearRange();
  bool validateLogarithmicRange();
  bool validateNormalDistRange();
  void setSliderType(IntervalScale type);
  void sliderTypeToRadiobutton();
  void showSliderValue();
public:
	CTestSliderWithTransformationDlg(CWnd *pParent = NULL);   // standard constructor
	virtual ~CTestSliderWithTransformationDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TESTSLIDER_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
  virtual BOOL OnInitDialog();
  afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  afx_msg void OnBnClickedRadioLinear();
  afx_msg void OnBnClickedRadioLogarithmic();
  afx_msg void OnBnClickedRadioNormaldistribution();
  afx_msg void OnBnClickedButtonUpdateSlider();
  afx_msg void OnGotoRange();
virtual BOOL PreTranslateMessage(MSG* pMsg);
};
