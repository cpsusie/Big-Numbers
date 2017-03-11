#pragma once

class CApproximateFillDlg : public CDialog {
public:
  CApproximateFillDlg(UINT tolerance, CWnd *pParent = NULL);

  UINT getTolerance() const {
    return m_tolerance;
  }

private:
  enum { IDD = IDD_APPROXIMATEFILLDIALOG };
  UINT  m_tolerance;

protected:
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual BOOL OnInitDialog();
  DECLARE_MESSAGE_MAP()
};

