#pragma once

class CPrndlgDlg : public CDialog {
private:
  HICON m_hIcon;
public:
  CPrndlgDlg(CWnd *pParent = NULL); // standard constructor

  enum { IDD = IDD_PRNDLG_DIALOG };
protected:
  virtual void DoDataExchange(CDataExchange *pDX);  // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
  afx_msg void OnPaint();
  afx_msg HCURSOR OnQueryDragIcon();
  afx_msg void OnButton1();
  afx_msg void OnButton2();
  afx_msg void OnButton3();
  DECLARE_MESSAGE_MAP()
};

