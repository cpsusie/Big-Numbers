#pragma once

class CProgressDlg : public CDialog {
private:
  HICON               m_hIcon;
  LoadableMediaArray *m_mediaArray;
public:
  CProgressDlg(LoadableMediaArray *mediaArray, CWnd *pParent = NULL);

  enum { IDD = IDD_PROGRESSDIALOG };

protected:
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual BOOL OnInitDialog();
  virtual void OnCancel();
  afx_msg void OnTimer(UINT_PTR nIDEvent);
  DECLARE_MESSAGE_MAP()
};

