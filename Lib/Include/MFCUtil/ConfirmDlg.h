#pragma once

#include <afxcmn.h>			// MFC support for Windows Common Controls
#include "resource.h"
#include "ColoredStatic.h"

class CConfirmDlg : public CDialog {
private:
  const TCHAR *m_message, *m_caption;
  const UINT       m_nType;
  HICON            m_icon;
  HACCEL           m_accelTable;
  int              m_buttonResult[3];
  bool             m_hasCancelButton;
  CColoredStatic   m_msgCtrl;
  CStatic          m_iconCtrl;
  void disableCloseButton();

public:
    CConfirmDlg(const TCHAR *message, const TCHAR *caption, UINT nType, CWnd* pParent = NULL);

    bool getShowAgain() const {
      return !m_dontShowAgain;
    }

    //{{AFX_DATA(CConfirmDlg)
    enum { IDD = _IDD_CONFIRM_DIALOG };
    BOOL    m_dontShowAgain;
    //}}AFX_DATA


    //{{AFX_VIRTUAL(CConfirmDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
    virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:

    //{{AFX_MSG(CConfirmDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnButton0();
    afx_msg void OnButton1();
    afx_msg void OnButton2();
    virtual void OnCancel();
	afx_msg void OnPaint();
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
