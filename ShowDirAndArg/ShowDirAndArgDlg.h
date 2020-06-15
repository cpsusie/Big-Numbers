#pragma once


class CShowDirAndArgDlg : public CDialog {
private:
  HICON               m_hIcon;
public:
  CShowDirAndArgDlg(CWnd *pParent = NULL);

  enum { IDD = IDD_SHOWDIRANDARG_DIALOG };
protected:
  virtual void    DoDataExchange(CDataExchange *pDX);
  virtual BOOL    OnInitDialog();
  afx_msg void    OnPaint();
  afx_msg HCURSOR OnQueryDragIcon();
  DECLARE_MESSAGE_MAP()
};

