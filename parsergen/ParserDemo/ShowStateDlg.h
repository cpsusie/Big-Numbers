#pragma once

class ShowStateDlg : public CDialog {
private:
  HICON               m_hIcon;
  HACCEL              m_accelTable;
  CDialog            *m_mainDialog;
  void ajourState();
public:
  ShowStateDlg(CDialog *mainDialog, CWnd *pParent = nullptr);

  enum { IDD = IDD_DIALOGSHOWSTATE };

  CString m_data;

protected:
  virtual BOOL OnInitDialog();
  virtual BOOL PreTranslateMessage(MSG *pMsg);
  virtual void DoDataExchange(CDataExchange *pDX);
  afx_msg LRESULT OnShowStateUpdate(WPARAM wp, LPARAM lp);
  DECLARE_MESSAGE_MAP()
};
