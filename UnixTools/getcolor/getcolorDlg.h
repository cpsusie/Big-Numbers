#pragma once

class CGetcolorDlg : public CDialog {
public:
  CGetcolorDlg(CWnd *pParent = NULL);

  HBITMAP hbmScreen;
  HICON   m_hIcon;
  HDC     hdcCompatible;
  HDC     hdcScreen;
  int scrw,scrh;
  void displayColor(const POINT &pt);

  enum { IDD = IDD_GETCOLOR_DIALOG };

public:
  virtual BOOL PreTranslateMessage(MSG *pMsg);
protected:
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual BOOL OnInitDialog();
  afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
  afx_msg void OnPaint();
  afx_msg HCURSOR OnQueryDragIcon();
  afx_msg void OnCancelMode();
  DECLARE_MESSAGE_MAP()
};

