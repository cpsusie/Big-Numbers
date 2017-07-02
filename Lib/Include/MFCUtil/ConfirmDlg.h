#pragma once

#include <afxcmn.h>         // MFC support for Windows Common Controls
#include "resource.h"
#include "ColoredStatic.h"

class CConfirmDlg : public CDialog {
private:
  HICON            m_icon;
  HACCEL           m_accelTable;
  const TCHAR     *m_message, *m_caption;
  const UINT       m_nType;
  int              m_buttonResult[3];
  bool             m_hasCancelButton;
  CColoredStatic   m_msgCtrl;
  CStatic          m_iconCtrl;
  void disableCloseButton();

public:
    CConfirmDlg(const TCHAR *message, const TCHAR *caption, UINT nType, CWnd *pParent = NULL);

    bool getShowAgain() const {
      return !m_dontShowAgain;
    }

    enum { IDD = _IDD_CONFIRM_DIALOG };
    BOOL    m_dontShowAgain;

public:
    virtual BOOL PreTranslateMessage(MSG *pMsg);
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnButton0();
    afx_msg void OnButton1();
    afx_msg void OnButton2();
    virtual void OnCancel();
    afx_msg void OnPaint();
    DECLARE_MESSAGE_MAP()
};
