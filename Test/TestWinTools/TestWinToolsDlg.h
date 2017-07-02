#pragma once

class CTestWinToolsDlg : public CDialogEx {
private:
  HACCEL  m_accelTable;
  HICON   m_hIcon;
  String  m_currentDir;
  void showInfo();

public:
    CTestWinToolsDlg(CWnd *pParent = NULL); // standard constructor

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_TESTWINTOOLS_DIALOG };
#endif

protected:
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual BOOL PreTranslateMessage(MSG *pMsg);

protected:
  virtual BOOL OnInitDialog();
  afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
  afx_msg void OnPaint();
  afx_msg HCURSOR OnQueryDragIcon();
  afx_msg void OnClose();
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
  afx_msg void OnTestTestSliderWithTransformation();

  DECLARE_MESSAGE_MAP()
};
