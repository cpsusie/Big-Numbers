#pragma once

class ErrorsDlg : public CDialog {
private:
  UINT   m_maxErrorCount;
  UINT   m_cascadeCount;
public:
  ErrorsDlg(int maxErrorCount, int cascadeCount, CWnd *pParent = nullptr);

  enum { IDD = IDD_DIALOGERRORS };

  inline UINT getMaxErrorCount() const {
    return m_maxErrorCount;
  }
  inline UINT getCascadeCount() const {
    return m_cascadeCount;
  }
protected:
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual void OnOK();
  virtual BOOL OnInitDialog();
  DECLARE_MESSAGE_MAP()
};
