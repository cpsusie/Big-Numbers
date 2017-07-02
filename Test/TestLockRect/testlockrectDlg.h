#pragma once

#include <MFCUtil/PixRect.h>

class CtestlockrectDlg : public CDialogEx {
private:
  HICON         m_hIcon;
  PixRectDevice m_device;
  void log(TCHAR *format, ...);
public:
  CtestlockrectDlg(CWnd *pParent = NULL);   // standard constructor

#ifdef AFX_DESIGN_TIME
  enum { IDD = IDD_TESTLOCKRECT_DIALOG };
#endif

  virtual void DoDataExchange(CDataExchange *pDX);  // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
  afx_msg void OnPaint();
  afx_msg HCURSOR OnQueryDragIcon();
  virtual void OnCancel();
  afx_msg void OnClose();
  afx_msg void OnClickedButtonstart();
  DECLARE_MESSAGE_MAP()
};
