#pragma once

#include <MFCUtil/PixRect.h>

class CtestlockrectDlg : public CDialogEx {
private:
  PixRectDevice m_device;
  HICON m_hIcon;
  void log(TCHAR *format, ...);
public:
  CtestlockrectDlg(CWnd* pParent = NULL);	// standard constructor

#ifdef AFX_DESIGN_TIME
  enum { IDD = IDD_TESTLOCKRECT_DIALOG };
#endif

  protected:
  virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

protected:

  // Generated message map functions
  virtual BOOL OnInitDialog();
  afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
  afx_msg void OnPaint();
  afx_msg HCURSOR OnQueryDragIcon();
  DECLARE_MESSAGE_MAP()
  virtual void OnCancel();
public:
  afx_msg void OnClose();
  afx_msg void OnClickedButtonstart();
};
