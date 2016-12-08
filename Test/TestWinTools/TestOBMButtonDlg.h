#pragma once

#include <MFCUtil/OBMButton.h>

class CTestOBMButtonDlg : public CDialog {
private:
  OBMButton m_dnArrayButton, m_lfArrowButton, m_rgArrowbutton, m_upArrowButton;
  OBMButton m_zoomButton, m_reduceButton, m_restoreButton;

public:
  CTestOBMButtonDlg(CWnd* pParent = NULL);

  enum { IDD = IDD_TEST_OBMBUTTON_DIALOG };
  BOOL    m_buttonsEnabled;

protected:
  virtual void DoDataExchange(CDataExchange* pDX);

  virtual void OnOK();
  virtual BOOL OnInitDialog();
  afx_msg void OnCheckEnableButtons();
  afx_msg void OnButtonDNARROW();
  afx_msg void OnButtonLFARROW();
  afx_msg void OnButtonRGARROW();
  afx_msg void OnButtonUPARROW();
  afx_msg void OnButtonZOOM();
  afx_msg void OnButtonREDUCE();
  afx_msg void OnButtonRESTORE();
  DECLARE_MESSAGE_MAP()
};
