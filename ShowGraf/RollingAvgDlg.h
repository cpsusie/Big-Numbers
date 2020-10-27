#pragma once

class CRollingAvgDlg : public CDialog {
private:
  BOOL       m_enabled;
  RollingAvg m_rollingAvg;

  void enableFields(bool enable);
  void paramToWin(const RollingAvg &param);
  void winToParam(      RollingAvg &param);
  bool validate();
public:
  CRollingAvgDlg(const RollingAvg &rollingAvg, CWnd *pParent = nullptr);

  enum { IDD = IDD_ROLLINGAVG_DIALOG };
  UINT    m_queueSize;

  inline const RollingAvg &getRollingAvg() const {
    return m_rollingAvg;
  }
protected:
  DECLARE_MESSAGE_MAP()
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  virtual void OnCancel();
  afx_msg void OnDeltaposEditRollingQueueSizeSpin(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnClickedCheckEnableRollingAvg();
};
