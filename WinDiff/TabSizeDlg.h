#pragma once

class CTabSizeDlg: public CDialog {
private:
public:
  CTabSizeDlg(int tabSize, CWnd *pParent = nullptr);

  enum { IDD = IDD_DIALOGTABSIZE };
  UINT  m_tabSize;

protected:
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual void OnOK();
  virtual BOOL OnInitDialog();
  DECLARE_MESSAGE_MAP()
};

