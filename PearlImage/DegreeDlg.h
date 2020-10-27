#pragma once

class CDegreeDlg : public CDialog {
public:
  CDegreeDlg(double degree, CWnd *pParent = nullptr);

  double getDegree() const {
    return m_degree;
  }

private:
  enum { IDD = IDD_DEGREEDIALOG };
  double    m_degree;

protected:
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual void OnOK();
  virtual BOOL OnInitDialog();
  DECLARE_MESSAGE_MAP()
};
