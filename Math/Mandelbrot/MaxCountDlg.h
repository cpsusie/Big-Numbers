#pragma once

class CMaxCountDlg : public CDialog {
  DECLARE_DYNAMIC(CMaxCountDlg)
private:
  HACCEL  m_accelTable;
  UINT    m_maxCount;
public:
  CMaxCountDlg(UINT maxCount, CWnd* pParent = nullptr); 
  virtual ~CMaxCountDlg();

  inline UINT getMaxCount() const {
    return m_maxCount;
  }
  enum { IDD = IDD_MAXCOUNT_DIALOG };

public:
  virtual BOOL PreTranslateMessage(MSG *pMsg);
protected:
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  afx_msg void OnGotoMaxCount();
  DECLARE_MESSAGE_MAP()
};

