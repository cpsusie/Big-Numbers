#pragma once

#include <MFCUtil/OBMButton.h>

class CTestOBMButtonDlg : public CDialog {
private:
  OBMButton m_dnArrayButton, m_lfArrowButton, m_rgArrowbutton, m_upArrowButton;
  OBMButton m_zoomButton, m_reduceButton, m_restoreButton;

public:
    CTestOBMButtonDlg(CWnd* pParent = NULL);

    //{{AFX_DATA(CTestOBMButtonDlg)
	enum { IDD = IDD_TEST_OBMBUTTON_DIALOG };
	BOOL	m_buttonsEnabled;
	//}}AFX_DATA


    //{{AFX_VIRTUAL(CTestOBMButtonDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    //}}AFX_VIRTUAL

protected:

    //{{AFX_MSG(CTestOBMButtonDlg)
    virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckEnableButtons();
    afx_msg void OnButtonDNARROW();
    afx_msg void OnButtonLFARROW();
    afx_msg void OnButtonRGARROW();
    afx_msg void OnButtonUPARROW();
    afx_msg void OnButtonZOOM();
    afx_msg void OnButtonREDUCE();
    afx_msg void OnButtonRESTORE();
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
