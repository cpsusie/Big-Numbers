#pragma once

class ErrorsDlg : public CDialog {
private:
  HACCEL m_accelTable;
  UINT   m_maxErrorCount;
  UINT   m_cascadeCount;
public:
  ErrorsDlg(int maxErrorCount, int cascadeCount, CWnd *pParent = NULL);

  enum { IDD = IDD_DIALOGERRORS };

  inline UINT getMaxErrorCount() const {
    return m_maxErrorCount;
  }
  inline UINT getCascadeCount() const {
    return m_cascadeCount;
  }
protected:
  virtual BOOL PreTranslateMessage(MSG *pMsg);
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual void OnOK();
  virtual BOOL OnInitDialog();
  afx_msg void OnGotoErrorCascadeCount();
  afx_msg void OnGotoMaxErrorCount();
  DECLARE_MESSAGE_MAP()
};
