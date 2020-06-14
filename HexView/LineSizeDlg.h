#pragma once

#include "resource.h"

class CLineSizeDlg : public CDialog, SettingsAccessor {
private:
  void enableLineSize();
  void gotoLineSize();
public:
  CLineSizeDlg(CWnd *pParent = NULL);

  enum { IDD = IDD_DIALOGLINESIZE };
  UINT  m_lineSize;
  BOOL  m_fitLines;

  virtual void DoDataExchange(CDataExchange *pDX);
  virtual void OnOK();
  BOOL OnInitDialog();
  afx_msg void OnCheckFitLines();
  DECLARE_MESSAGE_MAP()
};

