#pragma once

class CProgressDlg : public CDialog {
private:
  LoadableMediaArray *m_mediaArray;
public:
  CProgressDlg(LoadableMediaArray *mediaArray, CWnd *pParent = NULL);

  enum { IDD = IDD_PROGRESSDIALOG };

protected:
  virtual void DoDataExchange(CDataExchange *pDX);

protected:
  HICON m_hIcon;

  virtual void OnCancel();
  afx_msg void OnTimer(UINT_PTR nIDEvent);
  virtual BOOL OnInitDialog();
  DECLARE_MESSAGE_MAP()
};

