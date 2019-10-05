#pragma once

class CPrecisionDlg : public CDialog {
  DECLARE_DYNAMIC(CPrecisionDlg)
  UINT m_coefDigits;
  UINT m_extremaDigits;
  UINT m_errorDigits;

public:
  CPrecisionDlg(const VisiblePrecisions &values, CWnd* pParent = nullptr);
  virtual ~CPrecisionDlg();

  enum { IDD = IDD_PRECISION_DIALOG };

  inline VisiblePrecisions getVisiblePrecisions() const {
    return VisiblePrecisions(m_coefDigits, m_extremaDigits, m_errorDigits);
  }
protected:
  virtual void DoDataExchange(CDataExchange* pDX);

  DECLARE_MESSAGE_MAP()
public:
  virtual BOOL OnInitDialog();
  virtual void OnOK();
};
