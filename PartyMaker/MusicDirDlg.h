#pragma once

#include "Options.h"

class CMusicDirDlg : public CDialog {
public:
  CMusicDirDlg(Options &options, CWnd *pParent = NULL);

private:
  Options &m_options;
  HACCEL   m_accelTable;

  CListBox *getListBox();
  enum { IDD = IDD_MUSICDIRDIALOG };

public:
  virtual BOOL PreTranslateMessage(MSG *pMsg);
protected:
  virtual void DoDataExchange(CDataExchange *pDX);

protected:

  virtual BOOL OnInitDialog();
  virtual void OnOK();
  afx_msg void OnDblclkDirlist();
  afx_msg void OnNewButton();
  afx_msg void OnDeleteButton();
  DECLARE_MESSAGE_MAP()
};

