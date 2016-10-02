#pragma once

class CChessDlg;

class CAboutDlg : public CDialog {
private:
  CChessDlg *m_parent;
public:
  CAboutDlg(CChessDlg *parent);

  enum { IDD = IDD_ABOUTBOX };

public:
protected:
    virtual void DoDataExchange(CDataExchange *pDX);

protected:
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  virtual BOOL OnInitDialog();
  DECLARE_MESSAGE_MAP()
};
