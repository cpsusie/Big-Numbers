#pragma once

class CTestProgressWindowDlg : public CDialog {
private:
public:
	CTestProgressWindowDlg(CWnd* pParent = NULL);

	//{{AFX_DATA(CTestProgressWindowDlg)
	enum { IDD = IDD_TESTPROGRESSWINDOW_DIALOG };
	BOOL	m_hasMessageBox;
	BOOL	m_hasTimeEstimate;
	BOOL	m_interruptable;
	BOOL	m_hasProgressBar;
	BOOL	m_showPercent;
	BOOL	m_hasSubProgressBar;
	BOOL	m_suspendable;
	UINT	m_delayMSec;
	CString	m_title;
	UINT	m_jobTime;
	UINT	m_updateRate;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CTestProgressWindowDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CTestProgressWindowDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonStartJob();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
