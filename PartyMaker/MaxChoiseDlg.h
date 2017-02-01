#pragma once

class MaxChoiseDlg : public CDialog {
public:
  MaxChoiseDlg(int maxChoise, CWnd *pParent = NULL);

  enum { IDD = IDD_MAXCHOISEDIALOG };
  UINT  m_maxChoise;

protected:
  virtual void DoDataExchange(CDataExchange *pDX);

protected:
  BOOL OnInitDialog();

  DECLARE_MESSAGE_MAP()
};

