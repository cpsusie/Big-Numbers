#pragma once

#include "GameTypes.h"

class IdentifyDialog : public CDialog, public OptionsAccessor {
private:
  HACCEL  m_accelTable;

public:
  IdentifyDialog(CWnd *pParent = nullptr);

  enum { IDD = IDD_IDENTIFY_DIALOG };
  CString   m_myName;
  CString   m_dealerName;
  int       m_connected;

public:
  virtual BOOL PreTranslateMessage(MSG *pMsg);
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual BOOL OnInitDialog();
  afx_msg void OnDealerRadio();
  afx_msg void OnConnectRadio();
  afx_msg void OnQuitButton();
  afx_msg void OnGotoName();
  afx_msg void OnGotoDealerName();
  virtual void OnOK();
  DECLARE_MESSAGE_MAP()
};

