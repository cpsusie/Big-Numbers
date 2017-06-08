#pragma once

class CTesttooltipDlg : public CDialog {
public:
	CTesttooltipDlg(CWnd* pParent = NULL);

	//{{AFX_DATA(CTesttooltipDlg)
	enum { IDD = IDD_TESTTOOLTIP_DIALOG };
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CTesttooltipDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:
	HICON m_hIcon;

	//{{AFX_MSG(CTesttooltipDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
    afx_msg BOOL OnToolTipNotify( UINT id, NMHDR* pNMHDR, LRESULT* pResult );
    afx_msg void OnButton1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
