#pragma once

class GotoDlg : public CDialog {
private:
  UINT    m_line;
public:
  GotoDlg(CWnd *pParent = nullptr);

  enum { IDD = IDD_DIALOGGOTO };

  inline UINT getLine() const {
    return m_line;
  }
protected:
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  DECLARE_MESSAGE_MAP()
};

