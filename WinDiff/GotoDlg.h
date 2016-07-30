#pragma once

class CGotoDlg: public CDialog {
public:
  CGotoDlg(CWnd *pParent = NULL);

  enum { IDD = IDD_DIALOGGOTO };
  UINT  m_line;


protected:
  virtual void DoDataExchange(CDataExchange* pDX);

protected:

  virtual BOOL OnInitDialog();
  virtual void OnOK();
  DECLARE_MESSAGE_MAP()
};

