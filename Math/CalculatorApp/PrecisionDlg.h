#pragma once

class PrecisionDlg : public CDialog {
private:
    HACCEL    m_accelTabel;

public:
    PrecisionDlg(int prec, CWnd *pParent = NULL);

    enum { IDD = IDD_DIALOGPREC };
    long    m_precision;

	virtual BOOL PreTranslateMessage(MSG* pMsg);
  virtual void DoDataExchange(CDataExchange* pDX);
  virtual void OnOK();
  virtual BOOL OnInitDialog();
  afx_msg void OnGotoPrecision();
  DECLARE_MESSAGE_MAP()
};
