#pragma once

#include <MyString.h>

// CTestWinToolsDlg dialog
class CTestWinToolsDlg : public CDialogEx {
private:
  HACCEL  m_accelTable;
  HICON   m_hIcon;
  String  m_currentDir;
  void showInfo();

// Construction
public:
	CTestWinToolsDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TESTWINTOOLS_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
  virtual BOOL PreTranslateMessage(MSG* pMsg);


// Implementation
protected:

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
  virtual void OnCancel();
  afx_msg void OnFileExit();
  afx_msg void OnTestProgressWindow();
  afx_msg void OnTestConfirmDialog();
  afx_msg void OnTestLayoutManager();
  afx_msg void OnTestMouse();
  afx_msg void OnTestOBMBitmaps();
  afx_msg void OnTestGif();
  afx_msg void OnTestColorControl();
  afx_msg void OnTestOBMButton();
  afx_msg void OnTestSelectFolder();
  
  DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnClose();
};
